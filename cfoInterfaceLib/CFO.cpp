#include "CFO.h"

#include <sstream> // Convert uint to hex string


#include <iostream>
#include <fstream>

#include <unistd.h>
#include "trace.h"

CFOLib::CFO::CFO(std::string expectedDesignVersion, CFO_SimMode mode, unsigned rocMask) : CFO_Registers(expectedDesignVersion, mode, rocMask),
daqbuffer_(), dcsbuffer_(), lastDAQBufferActive_(false), lastDCSBufferActive_(false),
bufferIndex_(0), first_read_(true), daqDMAByteCount_(0), dcsDMAByteCount_(0),
lastReadPtr_(nullptr), nextReadPtr_(nullptr), dcsReadPtr_(nullptr)
{
	//ELF, 05/18/2016: Rick reports that 3.125 Gbp
	//SetSERDESOscillatorClock(CFO_SerdesClockSpeed_25Gbps); // We're going to 2.5Gbps for now
}

CFOLib::CFO::~CFO()
{
	ReleaseAllBuffers();
	daqbuffer_.clear();
	dcsbuffer_.clear();
	lastReadPtr_ = nullptr;
	nextReadPtr_ = nullptr;
	dcsReadPtr_ = nullptr;
}

//
// DMA Functions
//
std::vector<CFOLib::CFO_DataBlock> CFOLib::CFO::GetData(CFO_Timestamp when)
{
	TRACE(19, "CFO::GetData begin");
	std::vector<CFO_DataBlock> output;

	first_read_ = true;
	TRACE(19, "CFO::GetData: Releasing %i buffers", (int)daqbuffer_.size() + (lastDAQBufferActive_ ? -1 : 0));
	device_.read_release(CFO_DMA_Engine_DAQ, static_cast<unsigned>(daqbuffer_.size() + (lastDAQBufferActive_ ? -1 : 0)));

	cfo_databuff_t* last = daqbuffer_.back();
	daqbuffer_.clear();
	if (lastDAQBufferActive_)
	{
		daqbuffer_.push_back(last);
	}
	last = nullptr;
	lastDAQBufferActive_ = false;

	CFO_DataHeaderPacket* packet = nullptr;

	try
	{
		// Read the header packet
		auto tries = 0;
		size_t sz;
		while (packet == nullptr && tries < 3)
		{
			TRACE(19, "CFO::GetData before ReadNextDAQPacket, tries=%i", tries);
			packet = ReadNextDAQPacket(first_read_ ? 100 : 1);
			if (packet != nullptr)
			{
				TRACE(19, "CFO::GetData after ReadDMADAQPacket, ts=0x%llx", (unsigned long long)packet->GetTimestamp().GetTimestamp(true));
			}
			tries++;
			//if (packet == nullptr) usleep(5000);
		}
		if (packet == nullptr)
		{
			TRACE(19, "CFO::GetData: Timeout Occurred! (Lead packet is nullptr after retries)");
			return output;
		}

		if (packet->GetTimestamp() != when && when.GetTimestamp(true) != 0)
		{
			TRACE(0, "CFO::GetData: Error: Lead packet has wrong timestamp! 0x%llX(expected) != 0x%llX"
				  , (unsigned long long)when.GetTimestamp(true), (unsigned long long)packet->GetTimestamp().GetTimestamp(true));
			delete packet;
			lastDAQBufferActive_ = true;
			return output;
		}

		sz = packet->GetByteCount();
		when = packet->GetTimestamp();

		delete packet;
		packet = nullptr;

		TRACE(19, "CFO::GetData: Adding pointer %p to the list (first)", (void*)lastReadPtr_);
		output.push_back(CFO_DataBlock(reinterpret_cast<CFO_DataBlock::pointer_t*>(lastReadPtr_), sz));

		auto done = false;
		while (!done)
		{
			size_t sz2 = 0;
			TRACE(19, "CFO::GetData: Reading next DAQ Packet");
			packet = ReadNextDAQPacket();
			if (packet == nullptr) // End of Data
			{
				TRACE(19, "CFO::GetData: Next packet is nullptr; we're done");
				done = true;
				nextReadPtr_ = nullptr;
			}
			else if (packet->GetTimestamp() != when)
			{
				TRACE(19, "CFO::GetData: Next packet has ts=0x%llx, not 0x%llx; we're done", (unsigned long long)packet->GetTimestamp().GetTimestamp(true), (unsigned long long)when.GetTimestamp(true));
				done = true;
				nextReadPtr_ = lastReadPtr_;
				lastDAQBufferActive_ = true;
			}
			else
			{
				TRACE(19, "CFO::GetData: Next packet has same ts=0x%llx, continuing (bc=0x%llx)", (unsigned long long)packet->GetTimestamp().GetTimestamp(true), (unsigned long long)packet->GetByteCount());
				sz2 = packet->GetByteCount();
			}
			delete packet;
			packet = nullptr;

			if (!done)
			{
				TRACE(19, "CFO::GetData: Adding pointer %p to the list", (void*)lastReadPtr_);
				output.push_back(CFO_DataBlock(reinterpret_cast<CFO_DataBlock::pointer_t*>(lastReadPtr_), sz2));
			}
		}
	}
	catch (CFO_WrongPacketTypeException& ex)
	{
		TRACE(19, "CFO::GetData: Bad omen: Wrong packet type at the current read position");
		nextReadPtr_ = nullptr;
	}
	catch (CFO_IOErrorException& ex)
	{
		nextReadPtr_ = nullptr;
		TRACE(19, "CFO::GetData: IO Exception Occurred!");
	}
	catch (CFO_DataCorruptionException& ex)
	{
		nextReadPtr_ = nullptr;
		TRACE(19, "CFO::GetData: Data Corruption Exception Occurred!");
	}

	TRACE(19, "CFO::GetData RETURN");
	delete packet;
	return output;
} // GetData

std::string CFOLib::CFO::GetJSONData(CFO_Timestamp when)
{
	TRACE(19, "CFO::GetJSONData BEGIN");
	std::stringstream ss;
	TRACE(19, "CFO::GetJSONData before call to GetData");
	auto data = GetData(when);
	TRACE(19, "CFO::GetJSONData after call to GetData, data size %llu", (unsigned long long)data.size());

	for (size_t i = 0; i < data.size(); ++i)
	{
		TRACE(19, "CFO::GetJSONData constructing DataPacket:");
		auto test = CFO_DataPacket(data[i].blockPointer);
		TRACE(19, test.toJSON().c_str());
		TRACE(19, "CFO::GetJSONData constructing DataHeaderPacket");
		auto theHeader = CFO_DataHeaderPacket(test);
		ss << "DataBlock: {";
		ss << theHeader.toJSON() << ",";
		ss << "DataPackets: [";
		for (auto packet = 0; packet < theHeader.GetPacketCount(); ++packet)
		{
			auto packetPtr = static_cast<void*>(reinterpret_cast<char*>(data[i].blockPointer) + 16 * (1 + packet));
			ss << CFO_DataPacket(packetPtr).toJSON() << ",";
		}
		ss << "]";
		ss << "}";
		if (i + 1 < data.size())
		{
			ss << ",";
		}
	}

	TRACE(19, "CFO::GetJSONData RETURN");
	return ss.str();
}

void CFOLib::CFO::WriteSimFileToCFO(std::string file, bool /*goForever*/, bool overwriteEnvironment, std::string outputFileName, bool skipVerify)
{
	bool success = false;
	int retryCount = 0;
	while (!success && retryCount < 5)
	{

		TRACE(4, "CFO::WriteSimFileToCFO BEGIN");
		auto writeOutput = outputFileName != "";
		std::ofstream outputStream;
		if (writeOutput)
		{
			outputStream.open(outputFileName, std::ios::out | std::ios::binary);
		}
		auto sim = getenv("CFOLIB_SIM_FILE");
		if (!overwriteEnvironment && sim != nullptr)
		{
			file = std::string(sim);
		}


		TRACE(4, "CFO::WriteSimFileToCFO file is " + file + ", Setting up CFO");
		DisableDetectorEmulator();
		DisableDetectorEmulatorMode();
		//ResetDDR();  // this can take about a second
		ResetDDRWriteAddress();
		ResetDDRReadAddress();
		SetDDRDataLocalStartAddress(0x0);
		SetDDRDataLocalEndAddress(0xFFFFFFFF);
		EnableDetectorEmulatorMode();
		SetDetectorEmulationDMACount(1);
		SetDetectorEmulationDMADelayCount(250); // 1 microseconds
		uint64_t totalSize = 0;
		auto n = 0;

		auto sizeCheck = true;
		TRACE(4, "CFO::WriteSimFileToCFO Opening file");
		std::ifstream is(file, std::ifstream::binary);
		TRACE(4, "CFO::WriteSimFileToCFO Reading file");
		while (is && is.good() && sizeCheck)
		{
			TRACE(5, "CFO::WriteSimFileToCFO Reading a DMA from file...%s", file.c_str());
			auto buf = reinterpret_cast<cfo_databuff_t*>(new char[0x10000]);
			is.read(reinterpret_cast<char*>(buf), sizeof(uint64_t));
			if (is.eof())
			{
				TRACE(5, "CFO::WriteSimFileToCFO End of file reached.");
				delete[] buf;
				break;
			}
			auto sz = *reinterpret_cast<uint64_t*>(buf);
			//TRACE(5, "Size is %llu, writing to device", (long long unsigned)sz);
			is.read(reinterpret_cast<char*>(buf) + 8, sz - sizeof(uint64_t));
			if (sz < 80 && sz > 0)
			{
				auto oldSize = sz;
				sz = 80;
				memcpy(buf, &sz, sizeof(uint64_t));
				uint64_t sixtyFour = 64;
				memcpy(reinterpret_cast<uint64_t*>(buf) + 1, &sixtyFour, sizeof(uint64_t));
				bzero(reinterpret_cast<uint64_t*>(buf) + 2, sz - oldSize);
			}
			//is.read((char*)buf + 8, sz - sizeof(uint64_t));
			if (sz > 0 && (sz + totalSize < 0xFFFFFFFF || simMode_ == CFO_SimMode_LargeFile))
			{
				TRACE(5, "CFO::WriteSimFileToCFO Size is %zu, writing to device", sz);
				if (writeOutput)
				{
					TRACE(11, "CFO::WriteSimFileToCFO: Stripping off DMA header words and writing to binary file");
					outputStream.write(reinterpret_cast<char*>(buf) + 16, sz - 16);
				}

				auto exclusiveByteCount = *(reinterpret_cast<uint64_t*>(buf) + 1);
				TRACE(11, "CFO::WriteSimFileToCFO: Inclusive byte count: %llu, Exclusive byte count: %llu", (long long unsigned)sz, (long long unsigned)exclusiveByteCount);
				if (sz - 16 != exclusiveByteCount)
				{
					TRACE(0, "CFO::WriteSimFileToCFO: ERROR: Inclusive Byte count %llu is inconsistent with exclusive byte count %llu for DMA at 0x%llx (%llu != %llu)",
						(long long unsigned)sz, (long long unsigned)exclusiveByteCount, (long long unsigned) totalSize, (long long unsigned)sz - 16, (long long unsigned)exclusiveByteCount);
					sizeCheck = false;
				}

				totalSize += sz - 8;
				n++;
				TRACE(10, "CFO::WriteSimFileToCFO: totalSize is now %lu, n is now %lu", static_cast<unsigned long>(totalSize), static_cast<unsigned long>(n));
				WriteDetectorEmulatorData(buf, static_cast<size_t>(sz));
			}
			else if (sz > 0)
			{
				TRACE(5, "CFO::WriteSimFileToCFO CFO memory is now full. Closing file.");
				sizeCheck = false;
			}
			delete[] buf;
		}

		TRACE(4, "CFO::WriteSimFileToCFO Closing file. sizecheck=%i, eof=%i, fail=%i, bad=%i", sizeCheck, is.eof(), is.fail(), is.bad());
		is.close();
		if (writeOutput) outputStream.close();
		SetDDRDataLocalEndAddress(static_cast<uint32_t>(totalSize - 1));
		success = skipVerify || VerifySimFileInCFO(file, outputFileName);
		retryCount++;
	}

	if (retryCount == 5)
	{
		TRACE(0, "CFO::WriteSimFileToCFO FAILED after 5 attempts! ABORTING!");
		exit(4);
	}
	else
	{
		TRACE(2, "CFO::WriteSimFileToCFO Took %d attempts to write file", retryCount);
	}

	SetDetectorEmulatorInUse();
	TRACE(4, "CFO::WriteSimFileToCFO END");
}

bool CFOLib::CFO::VerifySimFileInCFO(std::string file, std::string rawOutputFilename)
{
	uint64_t totalSize = 0;
	auto n = 0;
	auto sizeCheck = true;

	auto writeOutput = rawOutputFilename != "";
	std::ofstream outputStream;
	if (writeOutput)
	{
		outputStream.open(rawOutputFilename + ".verify", std::ios::out | std::ios::binary);
	}

	auto sim = getenv("CFOLIB_SIM_FILE");
	if (file.size() == 0 && sim != nullptr)
	{
		file = std::string(sim);
	}

	ResetDDRReadAddress();
	TRACE(4, "CFO::VerifySimFileInCFO Opening file");
	std::ifstream is(file, std::ifstream::binary);
	if (!is || !is.good())
	{
		TRACE(0, "CFO::VerifySimFileInCFO Failed to open file " + file + "!");
	}

	TRACE(4, "CFO::VerifySimFileInCFO Reading file");
	while (is && is.good() && sizeCheck)
	{
		TRACE(5, "CFO::VerifySimFileInCFO Reading a DMA from file...%s", file.c_str());
		auto buf = reinterpret_cast<cfo_databuff_t*>(new char[0x10000]);
		is.read(reinterpret_cast<char*>(buf), sizeof(uint64_t));
		if (is.eof())
		{
			TRACE(5, "CFO::VerifySimFileInCFO End of file reached.");
			delete[] buf;
			break;
		}
		auto sz = *reinterpret_cast<uint64_t*>(buf);
		//TRACE(5, "Size is %llu, writing to device", (long long unsigned)sz);
		is.read(reinterpret_cast<char*>(buf) + 8, sz - sizeof(uint64_t));
		if (sz < 80 && sz > 0)
		{
			auto oldSize = sz;
			sz = 80;
			memcpy(buf, &sz, sizeof(uint64_t));
			uint64_t sixtyFour = 64;
			memcpy(reinterpret_cast<uint64_t*>(buf) + 1, &sixtyFour, sizeof(uint64_t));
			bzero(reinterpret_cast<uint64_t*>(buf) + 2, sz - oldSize);
		}

		if (sz > 0 && (sz + totalSize < 0xFFFFFFFF || simMode_ == CFO_SimMode_LargeFile))
		{
			TRACE(5, "CFO::VerifySimFileInCFO Expected Size is %zu, reading from device", sz);
			auto exclusiveByteCount = *(reinterpret_cast<uint64_t*>(buf) + 1);
			TRACE(11, "CFO::VerifySimFileInCFO: Inclusive byte count: %llu, Exclusive byte count: %llu", (long long unsigned)sz, (long long unsigned)exclusiveByteCount);
			if (sz - 16 != exclusiveByteCount)
			{
				TRACE(0, "CFO::VerifySimFileInCFO: ERROR: Inclusive Byte count %llu is inconsistent with exclusive byte count %llu for DMA at 0x%llx (%llu != %llu)",
					(long long unsigned)sz, (long long unsigned)exclusiveByteCount, (long long unsigned) totalSize, (long long unsigned)sz - 16, (long long unsigned)exclusiveByteCount);
				sizeCheck = false;
			}

			totalSize += sz;
			n++;
			TRACE(10, "CFO::VerifySimFileInCFO: totalSize is now %lu, n is now %lu", static_cast<unsigned long>(totalSize), static_cast<unsigned long>(n));
			//WriteDetectorEmulatorData(buf, static_cast<size_t>(sz));
			DisableDetectorEmulator();
			SetDetectorEmulationDMACount(1);
			EnableDetectorEmulator();

			cfo_databuff_t* buffer;
			auto tmo_ms = 1500;
			TRACE(4, "CFO::VerifySimFileInCFO - before read for DAQ ");
			auto sts = device_.read_data(CFO_DMA_Engine_DAQ, reinterpret_cast<void**>(&buffer), tmo_ms);
			if (writeOutput && sts > 8)
			{
				TRACE(11, "CFO::VerifySimFileInCFO: Writing to binary file");
				outputStream.write(reinterpret_cast<char*>(*buffer + 8), sts - 8);
			}
			size_t readSz = *(reinterpret_cast<uint64_t*>(buffer));
			TRACE(4, "CFO::VerifySimFileInCFO - after read, sz=%zu sts=%d rdSz=%zu", sz, sts, readSz);

			// DMA engine strips off leading 64-bit word
			TRACE(6, "CFO::VerifySimFileInCFO - Checking buffer size");
			if (static_cast<size_t>(sts) != sz - sizeof(uint64_t))
			{
				TRACE(0, "CFO::VerifySimFileInCFO Buffer %d has size 0x%zx but the input file has size 0x%zx for that buffer!", n, static_cast<size_t>(sts), sz - sizeof(uint64_t));

				device_.read_release(CFO_DMA_Engine_DAQ, 1);
				delete[] buf;
				is.close();
				if (writeOutput) outputStream.close();
				return false;
			}

			TRACE(6, "CFO::VerifySimFileInCFO - Checking buffer contents");
			size_t cnt = sts % sizeof(uint64_t) == 0 ? sts / sizeof(uint64_t) : 1 + (sts / sizeof(uint64_t));

			for (size_t ii = 0; ii < cnt; ++ii)
			{
				auto l = *(reinterpret_cast<uint64_t*>(*buffer) + ii);
				auto r = *(reinterpret_cast<uint64_t*>(*buf) + ii + 1);
				if (l != r)
				{
					size_t address = totalSize - sz + ((ii + 1) * sizeof(uint64_t));
					TRACE(0, "CFO::VerifySimFileInCFO Buffer %d word %zu (Address in file 0x%zx): Expected 0x%llx, but got 0x%llx. Returning False!", n, ii, address,
						  static_cast<unsigned long long>(r), static_cast<unsigned long long>(l));
					delete[] buf;
					is.close();
					if (writeOutput) outputStream.close();
					device_.read_release(CFO_DMA_Engine_DAQ, 1);
					return false;
				}
			}
			device_.read_release(CFO_DMA_Engine_DAQ, 1);
		}
		else if (sz > 0)
		{
			TRACE(5, "CFO::VerifySimFileInCFO CFO memory is now full. Closing file.");
			sizeCheck = false;
		}
		delete[] buf;
	}

	TRACE(4, "CFO::VerifySimFileInCFO Closing file. sizecheck=%i, eof=%i, fail=%i, bad=%i", sizeCheck, is.eof(), is.fail(), is.bad());
	TRACE(1, "CFO::VerifySimFileInCFO: The Detector Emulation file was written correctly");
	is.close();
	if (writeOutput) outputStream.close();
	return true;
}

// ROC Register Functions
uint16_t CFOLib::CFO::ReadROCRegister(const CFO_Ring_ID& ring, const CFO_ROC_ID& roc, const uint8_t address)
{
	SendDCSRequestPacket(ring, roc, CFO_DCSOperationType_Read, address);
	auto reply = ReadNextDCSPacket();
	if (reply != nullptr)
	{
		return reply->GetData();
	}
	return 0;
}

void CFOLib::CFO::WriteROCRegister(const CFO_Ring_ID& ring, const CFO_ROC_ID& roc, const uint8_t address, const uint16_t data)
{
	SendDCSRequestPacket(ring, roc, CFO_DCSOperationType_Write, address, data);
}

uint16_t CFOLib::CFO::ReadExtROCRegister(const CFO_Ring_ID& ring, const CFO_ROC_ID& roc, const uint8_t block, const uint16_t address)
{
	uint16_t addressT = address & 0x7FFF;
	WriteROCRegister(ring, roc, 12, block);
	WriteROCRegister(ring, roc, 13, addressT);
	WriteROCRegister(ring, roc, 13, addressT | 0x8000);
	return ReadROCRegister(ring, roc, 22);
}

void CFOLib::CFO::WriteExtROCRegister(const CFO_Ring_ID& ring, const CFO_ROC_ID& roc, const uint8_t block, const uint8_t address, const uint16_t data)
{
	uint16_t dataT = data & 0x7FFF;
	WriteROCRegister(ring, roc, 12, block + (address << 8));
	WriteROCRegister(ring, roc, 13, dataT);
	WriteROCRegister(ring, roc, 13, dataT | 0x8000);
}

std::string CFOLib::CFO::ROCRegDump(const CFO_Ring_ID& ring, const CFO_ROC_ID& roc)
{
	std::ostringstream o;
	o.setf(std::ios_base::boolalpha);
	o << "{";
	o << "\"Forward Detector 0 Status\": " << ReadExtROCRegister(ring, roc, 8, 0) << ",\n";
	o << "\"Forward Detector 1 Status\": " << ReadExtROCRegister(ring, roc, 9, 0) << ",\n";
	o << "\"Command Handler Status\": " << ReadExtROCRegister(ring, roc, 10, 0) << ",\n";
	o << "\"Packet Sender 0 Status\": " << ReadExtROCRegister(ring, roc, 11, 0) << ",\n";
	o << "\"Packet Sender 1 Status\": " << ReadExtROCRegister(ring, roc, 12, 0) << ",\n";
	o << "\"Forward Detector 0 Errors\": " << ReadExtROCRegister(ring, roc, 8, 1) << ",\n";
	o << "\"Forward Detector 1 Errors\": " << ReadExtROCRegister(ring, roc, 9, 1) << ",\n";
	o << "\"Command Handler Errors\": " << ReadExtROCRegister(ring, roc, 10, 1) << ",\n";
	o << "\"Packet Sender 0 Errors\": " << ReadExtROCRegister(ring, roc, 11, 1) << ",\n";
	o << "\"Packet Sender 1 Errors\": " << ReadExtROCRegister(ring, roc, 12, 1) << "\n";
	o << "}";

	return o.str();
}

void CFOLib::CFO::SendReadoutRequestPacket(const CFO_Ring_ID& ring, const CFO_Timestamp& when, bool quiet)
{
	CFO_HeartbeatPacket req(ring, when, ReadRingROCCount(static_cast<CFO_Ring_ID>(ring)));
	TRACE(19, "CFO::SendReadoutRequestPacket before WriteDMADAQPacket - CFO_HeartbeatPacket");
	if (!quiet) std::cout << req.toJSON() << std::endl;
	WriteDMAPacket(req);
	TRACE(19, "CFO::SendReadoutRequestPacket after  WriteDMADAQPacket - CFO_HeartbeatPacket");
}

void CFOLib::CFO::SendDCSRequestPacket(const CFO_Ring_ID& ring, const CFO_ROC_ID& roc, const CFO_DCSOperationType type, const uint8_t address, const uint16_t data, bool quiet)
{
	CFO_DCSRequestPacket req(ring, roc, type, address, data);
	TRACE(19, "CFO::SendDCSRequestPacket before WriteDMADCSPacket - CFO_DCSRequestPacket");
	if (!quiet) std::cout << req.toJSON() << std::endl;
	WriteDMAPacket(req);
	TRACE(19, "CFO::SendDCSRequestPacket after  WriteDMADCSPacket - CFO_DCSRequestPacket");
}

CFOLib::CFO_DataHeaderPacket* CFOLib::CFO::ReadNextDAQPacket(int tmo_ms)
{
	TRACE(19, "CFO::ReadNextDAQPacket BEGIN");
	if (nextReadPtr_ != nullptr)
	{
		TRACE(19, "CFO::ReadNextDAQPacket BEFORE BUFFER CHECK nextReadPtr_=%p *nextReadPtr_=0x%08x"
			  , (void*)nextReadPtr_, *(uint16_t*)nextReadPtr_);
	}
	else
	{
		TRACE(19, "CFO::ReadNextDAQPacket BEFORE BUFFER CHECK nextReadPtr_=nullptr");
	}
	auto newBuffer = false;
	// Check if the nextReadPtr has been initialized, and if its pointing to a valid location
	if (nextReadPtr_ == nullptr
		|| nextReadPtr_ >= reinterpret_cast<uint8_t*>(daqbuffer_.back()) + sizeof(cfo_databuff_t)
		|| nextReadPtr_ >= reinterpret_cast<uint8_t*>(daqbuffer_.back()) + daqDMAByteCount_
		|| *static_cast<uint16_t*>(nextReadPtr_) == 0)
	{
		newBuffer = true;
		if (first_read_)
		{
			lastReadPtr_ = nullptr;
		}
		TRACE(19, "CFO::ReadNextDAQPacket Obtaining new DAQ Buffer");
		void* oldBufferPtr = &daqbuffer_.back()[0];
		auto sts = ReadBuffer(CFO_DMA_Engine_DAQ, tmo_ms); // does return code
		if (sts <= 0)
		{
			TRACE(19, "CFO::ReadNextDAQPacket: ReadBuffer returned %i, returning nullptr", sts);
			return nullptr;
		}
		// MUST BE ABLE TO HANDLE daqbuffer_==nullptr OR retry forever?
		nextReadPtr_ = &daqbuffer_.back()[0];
		TRACE(19, "CFO::ReadNextDAQPacket nextReadPtr_=%p *nextReadPtr_=0x%08x lastReadPtr_=%p"
			  , (void*)nextReadPtr_, *(unsigned*)nextReadPtr_, (void*)lastReadPtr_);
		void* bufferIndexPointer = static_cast<uint8_t*>(nextReadPtr_) + 2;
		if (nextReadPtr_ == oldBufferPtr && bufferIndex_ == *static_cast<uint32_t*>(bufferIndexPointer))
		{
			nextReadPtr_ = nullptr;
			//We didn't actually get a new buffer...this probably means there's no more data
			//Try and see if we're merely stuck...hopefully, all the data is out of the buffers...
			device_.read_release(CFO_DMA_Engine_DAQ, 1);
			return nullptr;
		}
		bufferIndex_++;
	}
	//Read the next packet
	TRACE(19, "CFO::ReadNextDAQPacket reading next packet from buffer: nextReadPtr_=%p:", (void*)nextReadPtr_);
	if (newBuffer)
	{
		daqDMAByteCount_ = static_cast<uint16_t>(*static_cast<uint16_t*>(nextReadPtr_));
		nextReadPtr_ = static_cast<uint8_t*>(nextReadPtr_) + 2;
		*static_cast<uint32_t*>(nextReadPtr_) = bufferIndex_;
		nextReadPtr_ = static_cast<uint8_t*>(nextReadPtr_) + 6;
	}
	auto blockByteCount = *static_cast<uint16_t*>(nextReadPtr_);
	TRACE(19, "CFO::ReadNextDAQPacket: blockByteCount=%u, daqDMAByteCount=%u, nextReadPtr_=%p, *nextReadPtr=0x%x", blockByteCount, daqDMAByteCount_, (void*)nextReadPtr_, *((uint8_t*)nextReadPtr_));
	if (blockByteCount == 0 || blockByteCount == 0xcafe)
	{
		TRACE(19, "CFO::ReadNextDAQPacket: blockByteCount is 0, returning NULL!");
		return nullptr;
	}

	auto test = CFO_DataPacket(nextReadPtr_);
	if (reinterpret_cast<uint8_t*>(nextReadPtr_) + blockByteCount >= daqbuffer_.back()[0] + daqDMAByteCount_)
	{
		blockByteCount = static_cast<uint16_t>(daqbuffer_.back()[0] + daqDMAByteCount_ - reinterpret_cast<uint8_t*>(nextReadPtr_));
		test.SetWord(0, blockByteCount & 0xFF);
		test.SetWord(1, (blockByteCount >> 8));
	}

	TRACE(19, test.toJSON().c_str());
	auto output = new CFO_DataHeaderPacket(test);
	TRACE(19, output->toJSON().c_str());
	if (static_cast<uint16_t>((1 + output->GetPacketCount()) * 16) != blockByteCount)
	{
		TRACE(19, "Data Error Detected: PacketCount: %u, ExpectedByteCount: %u, BlockByteCount: %u", output->GetPacketCount(), (1u + output->GetPacketCount()) * 16u, blockByteCount);
		throw CFO_DataCorruptionException();
	}
	first_read_ = false;

	// Update the packet pointers

	// lastReadPtr_ is easy...
	lastReadPtr_ = nextReadPtr_;

	// Increment by the size of the data block
	nextReadPtr_ = static_cast<char*>(nextReadPtr_) + blockByteCount;

	TRACE(19, "CFO::ReadNextDAQPacket RETURN");
	return output;
}

CFOLib::CFO_DCSReplyPacket* CFOLib::CFO::ReadNextDCSPacket()
{
	TRACE(19, "CFO::ReadNextDCSPacket BEGIN");
	if (dcsReadPtr_ == nullptr || dcsReadPtr_ >= reinterpret_cast<uint8_t*>(dcsbuffer_.back()) + sizeof(cfo_databuff_t) || *static_cast<uint16_t*>(dcsReadPtr_) == 0)
	{
		TRACE(19, "CFO::ReadNextDCSPacket Obtaining new DCS Buffer");
		auto retsts = ReadBuffer(CFO_DMA_Engine_DCS);
		if (retsts > 0)
		{
			dcsReadPtr_ = &dcsbuffer_.back()[0];
			TRACE(19, "CFO::ReadNextDCSPacket dcsReadPtr_=%p dcsBuffer_=%p", (void*)dcsReadPtr_, (void*)dcsbuffer_.back());
		}
		else
		{
			TRACE(19, "CFO::ReadNextDCSPacket ReadBuffer returned %i", retsts);
			return nullptr;
		}
	}

	//Read the next packet
	TRACE(19, "CFO::ReadNextDCSPacket Reading packet from buffer: dcsReadPtr_=%p:", (void*)dcsReadPtr_);
	auto output = new CFO_DCSReplyPacket(CFO_DataPacket(dcsReadPtr_));
	TRACE(19, output->toJSON().c_str());

	// Update the packet pointer

	// Increment by the size of the data block
	dcsReadPtr_ = static_cast<char*>(dcsReadPtr_) + 16;

	TRACE(19, "CFO::ReadNextDCSPacket RETURN");
	return output;
}

void CFOLib::CFO::WriteDetectorEmulatorData(cfo_databuff_t* buf, size_t sz)
{
	if (sz < dmaSize_)
	{
		sz = dmaSize_;
	}
	auto retry = 3;
	int errorCode;
	do
	{
		TRACE(21, "CFO::WriteDetectorEmulatorData: Writing buffer of size %zu", sz);
		errorCode = device_.write_data(CFO_DMA_Engine_DAQ, buf, sz);
		retry--;
	} while (retry > 0 && errorCode != 0);
	if (errorCode != 0)
	{
		throw CFO_IOErrorException();
	}
}

//
// Private Functions.
//
int CFOLib::CFO::ReadBuffer(const CFO_DMA_Engine& channel, int tmo_ms)
{
	cfo_databuff_t* buffer;
	auto retry = 2;
	int errorCode;
	do
	{
		TRACE(19, "CFO::ReadBuffer before device_.read_data");
		errorCode = device_.read_data(channel, reinterpret_cast<void**>(&buffer), tmo_ms);
		retry--;
		//if (errorCode == 0) usleep(1000);
	} while (retry > 0 && errorCode == 0);
	if (errorCode == 0)
	{
		TRACE(16, "CFO::ReadBuffer: Device timeout occurred! ec=%i, rt=%i", errorCode, retry);
	}
	else if (errorCode < 0) throw CFO_IOErrorException();
	else
	{
		TRACE(16, "CFO::ReadDataPacket buffer_=%p errorCode=%d *buffer_=0x%08x"
			  , (void*)buffer, errorCode, *(unsigned*)buffer);
		if (channel == CFO_DMA_Engine_DAQ)
		{
			daqbuffer_.push_back(buffer);
		}
		else if (channel == CFO_DMA_Engine_DCS)
		{
			dcsbuffer_.push_back(buffer);
		}
	}
	return errorCode;
}

void CFOLib::CFO::WriteDataPacket(const CFO_DataPacket& packet)
{
	if (packet.GetSize() < dmaSize_)
	{
		auto thisPacket(packet);
		thisPacket.Resize(dmaSize_);
		auto retry = 3;
		int errorCode;
		do
		{
			auto output = "CFO::WriteDataPacket: Writing packet: " + packet.toJSON();
			TRACE(21, output.c_str());
			errorCode = device_.write_data(CFO_DMA_Engine_DCS, const_cast<uint8_t*>(thisPacket.GetData()), thisPacket.GetSize() * sizeof(uint8_t));
			retry--;
		} while (retry > 0 && errorCode != 0);
		if (errorCode != 0)
		{
			throw CFO_IOErrorException();
		}
	}
	else
	{
		auto retry = 3;
		int errorCode;
		do
		{
			auto output = "CFO::WriteDataPacket: Writing packet: " + packet.toJSON();
			TRACE(21, output.c_str());
			errorCode = device_.write_data(CFO_DMA_Engine_DCS, const_cast<uint8_t*>(packet.GetData()), packet.GetSize() * sizeof(uint8_t));
			retry--;
		} while (retry > 0 && errorCode != 0);
		if (errorCode != 0)
		{
			throw CFO_IOErrorException();
		}
	}
}

void CFOLib::CFO::WriteDMAPacket(const CFO_DMAPacket& packet)
{
	WriteDataPacket(packet.ConvertToDataPacket());
}
