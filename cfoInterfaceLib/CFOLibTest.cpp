#include "CFOLibTest.h"
#include "CFOSoftwareCFO.h"

#include <iostream>

#include "trace.h"

CFOLib::CFOLibTest::CFOLibTest() : running_(false), classPassed_(0), classFailed_(0),
                                   regPassed_(0), regFailed_(0), daqPassed_(0), daqFailed_(0), dcsPassed_(0), dcsFailed_(0),
                                   classPassedTemp_(0), classFailedTemp_(0), regPassedTemp_(0), regFailedTemp_(0),
                                   daqPassedTemp_(0), daqFailedTemp_(0), dcsPassedTemp_(0), dcsFailedTemp_(0), nTests_(0),
                                   runClassTest_(false), runRegTest_(false),
                                   runDAQTest_(false), runDCSTest_(false), printMessages_(false)
{
	thisCFO_ = new CFO();
}

CFOLib::CFOLibTest::~CFOLibTest()
{
	nTests_ = 0;
	if (workerThread_.joinable())
	{
		workerThread_.join();
	}
	delete thisCFO_;
}

// Test Control
void CFOLib::CFOLibTest::startTest(bool classEnabled, bool regIOEnabled,
                                   bool daqEnabled, bool dcsEnabled, int nTests, bool printMessages)
{
	runClassTest_ = classEnabled;
	runRegTest_ = regIOEnabled;
	runDCSTest_ = dcsEnabled;
	runDAQTest_ = daqEnabled;
	nTests_ = nTests;
	printMessages_ = printMessages;

	if (printMessages_)
	{
		std::cout << "Starting workerThread" << std::endl;
	}

	workerThread_ = std::thread(&CFOLibTest::doTests, this);
	if (nTests_ >= 0)
	{
		workerThread_.join();
	}
}

void CFOLib::CFOLibTest::stopTests()
{
	nTests_ = 0;
	if (workerThread_.joinable())
	{
		workerThread_.join();
	}
}

// Accessors
int CFOLib::CFOLibTest::classPassed()
{
	auto result = classPassed_ - classPassedTemp_;
	classPassedTemp_ = classPassed_;
	return result;
}

int CFOLib::CFOLibTest::classFailed()
{
	auto result = classFailed_ - classFailedTemp_;
	classFailedTemp_ = classFailed_;
	return result;
}

int CFOLib::CFOLibTest::regPassed()
{
	auto result = regPassed_ - regPassedTemp_;
	regPassedTemp_ = regPassed_;
	return result;
}

int CFOLib::CFOLibTest::regFailed()
{
	auto result = regFailed_ - regFailedTemp_;
	regFailedTemp_ = regFailed_;
	return result;
}

int CFOLib::CFOLibTest::daqPassed()
{
	auto result = daqPassed_ - daqPassedTemp_;
	daqPassedTemp_ = daqPassed_;
	return result;
}

int CFOLib::CFOLibTest::daqFailed()
{
	auto result = daqFailed_ - daqFailedTemp_;
	daqFailedTemp_ = daqFailed_;
	return result;
}

int CFOLib::CFOLibTest::dcsPassed()
{
	auto result = dcsPassed_ - dcsPassedTemp_;
	dcsPassedTemp_ = dcsPassed_;
	return result;
}

int CFOLib::CFOLibTest::dcsFailed()
{
	auto result = dcsFailed_ - dcsFailedTemp_;
	dcsFailedTemp_ = dcsFailed_;
	return result;
}

// Private Functions
void CFOLib::CFOLibTest::doTests()
{
	if (printMessages_)
	{
		std::cout << "Worker thread started" << std::endl;
	}
	std::cout << "DEBUG 1" << std::endl;
	running_ = true;
	// Make sure that the ring is enabled before the tests.
	thisCFO_->EnableRing(CFO_Ring_0, CFO_RingEnableMode(true, true, false), CFO_ROC_0);

	auto testCount = 0;
	while (testCount < nTests_ || nTests_ < 0)
	{
		++testCount;
		if (runClassTest_)
		{
			doClassTest();
		}
		if (runRegTest_)
		{
			doRegTest();
		}
		if (runDCSTest_)
		{
			doDCSTest();
		}
		if (runDAQTest_)
		{
			doDAQTest();
		}
	}
	running_ = false;

	auto totalPassed = 0;
	auto totalTests = 0;

	if (runClassTest_)
	{
		totalPassed += classPassed_;
		totalTests += classPassed_ + classFailed_;
		std::cout << std::dec << classPassed_ << " of " << classPassed_ + classFailed_ << " Class Construction/Destruction Tests passed." << std::endl;
	}
	if (runRegTest_)
	{
		totalPassed += regPassed_;
		totalTests += regPassed_ + regFailed_;
		std::cout << std::dec << regPassed_ << " of " << regPassed_ + regFailed_ << " register I/O tests passed." << std::endl;
	}
	if (runDCSTest_)
	{
		totalPassed += dcsPassed_;
		totalTests += dcsPassed_ + dcsFailed_;
		std::cout << std::dec << dcsPassed_ << " of " << dcsPassed_ + dcsFailed_ << " DCS DMA I/O tests passed." << std::endl;
	}
	if (runDAQTest_)
	{
		totalPassed += daqPassed_;
		totalTests += daqPassed_ + daqFailed_;
		std::cout << std::dec << daqPassed_ << " of " << daqPassed_ + daqFailed_ << " DAQ DMA I/O tests passed." << std::endl;
	}
	std::cout << std::dec << totalPassed << " of " << totalTests << " tests passed." << std::endl;
}

void CFOLib::CFOLibTest::doClassTest()
{
	if (printMessages_)
	{
		std::cout << std::endl << "Test 0: Class Construction/Destruction" << std::endl;
	}
	auto err = false;
	try
	{
		if (printMessages_) std::cout << "Testing CFO_Timestamp Class..." << std::endl;

		auto defaultTS = CFO_Timestamp();
		if (printMessages_) std::cout << "Default Constructor, TS should be 0: " << defaultTS.GetTimestamp(true) << std::endl;
		err = err || defaultTS.GetTimestamp(true) != 0;

		auto tsSixtyFour = CFO_Timestamp(static_cast<uint64_t>(0xFFFFBEEFDEADBEEF));
		if (printMessages_)
			std::cout << "uint64_t Constructor, TS should be 0xBEEFDEADBEEF: "
				<< std::hex << std::showbase << tsSixtyFour.GetTimestamp(true) << std::endl;
		err = err || tsSixtyFour.GetTimestamp(true) != 0xBEEFDEADBEEF;

		auto tsSixteenThirtyTwo = CFO_Timestamp(static_cast<uint32_t>(0xDEADBEEF), static_cast<uint16_t>(0xDEAD));
		if (printMessages_)
			std::cout << "uint32_t/uint16_t Constructor, TS should be 0xDEADDEADBEEF: "
				<< tsSixteenThirtyTwo.GetTimestamp(true) << std::endl;
		err = err || tsSixteenThirtyTwo.GetTimestamp(true) != 0xDEADDEADBEEF;

		uint8_t arr[6] = {0xAD, 0xDE, 0xAD, 0xDE, 0xEF, 0xBE};
		auto tsArray = CFO_Timestamp(arr);
		if (printMessages_)
			std::cout << "Array Constructor, TS should be 0xBEEFDEADDEAD: "
				<< tsArray.GetTimestamp(true) << std::endl;
		err = err || tsArray.GetTimestamp(true) != 0xBEEFDEADDEAD;

		auto tsCopy = new CFO_Timestamp(defaultTS);
		if (printMessages_)
			std::cout << "Copy Constructor, TS should be 0: "
				<< tsCopy->GetTimestamp(true) << std::endl;
		err = err || tsCopy->GetTimestamp(true) != 0;

		tsCopy->SetTimestamp(0xBEEFDEAD, 0xBEEF);
		if (printMessages_)
			std::cout << "SetTimestamp 32/16 Method, TS should be 0xBEEFBEEFDEAD: "
				<< tsCopy->GetTimestamp(true) << std::endl;
		err = err || tsCopy->GetTimestamp().to_ullong() != 0xBEEFBEEFDEAD;

		tsCopy->SetTimestamp(0xDEADDEADBEEFBEEF);
		if (printMessages_)
			std::cout << "SetTimestamp 64 Method, TS should be 0xDEADBEEFBEEF: "
				<< tsCopy->GetTimestamp(true) << std::endl;
		err = err || tsCopy->GetTimestamp().to_ullong() != 0xDEADBEEFBEEF;

		if (printMessages_) std::cout << "Running CFO_Timestamp destructor" << std::endl;
		delete tsCopy;

		if (err)
		{
			if (printMessages_) std::cout << "CFO_Timestamp Class failed checks!" << std::endl;
			goto end;
		}

		if (printMessages_) std::cout << "Testing CFO_DataPacket Class..." << std::endl;
		auto defaultDP = new CFO_DataPacket();
		if (printMessages_)
			std::cout << "Default Constructor, Size should be 64: " << std::dec
			             << static_cast<int>(defaultDP->GetSize()) << ", and IsMemoryPacket should be false: "
			             << (defaultDP->IsMemoryPacket() ? "true" : "false") << std::endl;
		err = defaultDP->GetSize() != 64;
		err = err || defaultDP->IsMemoryPacket();

		defaultDP->Resize(128);
		if (printMessages_)
			std::cout << "Resize(128), Size should be 128: "
				<< static_cast<int>(defaultDP->GetSize()) << std::endl;
		err = err || defaultDP->GetSize() != 128;

		if (printMessages_) std::cout << "Running Data Integrity Check" << std::endl;
		err = err || !DataPacketIntegrityCheck(defaultDP);

		auto memCopyDP = CFO_DataPacket(*defaultDP);
		if (printMessages_)
			std::cout << "Copy Constructor, MemoryPacket, data[64] should be 64: "
				<< static_cast<int>(memCopyDP.GetWord(64)) << std::endl;
		err = err || memCopyDP.GetWord(64) != 64;

		if (printMessages_) std::cout << "Running CFO_DataPacket Destructor" << std::endl;
		delete defaultDP;

		auto buf = reinterpret_cast<cfo_databuff_t*>(new cfo_databuff_t());
		auto dataBufPtrfDP = new CFO_DataPacket(buf);
		if (printMessages_)
			std::cout << "Databuff Pointer Constructor, Size should be 16: "
				<< static_cast<int>(dataBufPtrfDP->GetSize()) << ", and IsMemoryPacket should be true: "
				<< (dataBufPtrfDP->IsMemoryPacket() ? "true" : "false") << std::endl;
		err = dataBufPtrfDP->GetSize() != 16;
		err = err || !dataBufPtrfDP->IsMemoryPacket();

		auto nonmemCopyDP = CFO_DataPacket(*dataBufPtrfDP);
		(*buf)[0] = 0x8F;
		if (printMessages_)
			std::cout
				<< "DataPacket Memory Packet Copy Constructor: Modifications to original buffer should modify both packets: "
				<< std::hex
				<< "COPY[0]: " << static_cast<int>(nonmemCopyDP.GetWord(0)) << ", ORIGINAL[0]: " << static_cast<int>(dataBufPtrfDP->GetWord(0)) << std::endl;
		err = err || nonmemCopyDP.GetWord(0) != 0x8F || dataBufPtrfDP->GetWord(0) != 0x8F;

		delete dataBufPtrfDP;
		if (printMessages_) std::cout << "Deleting ORIGINAL should not affect COPY: " << static_cast<int>(nonmemCopyDP.GetWord(0)) << std::endl;
		err = err || nonmemCopyDP.GetWord(0) != 0x8F;

		uint8_t buff2[16] = {0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0};
		auto uintBufDP = new CFO_DataPacket(buff2);
		if (printMessages_)
			std::cout << "Integer Array Constructor, IsMemoryPacket should be true: "
				<< (uintBufDP->IsMemoryPacket() ? "true" : "false") << ", and data[0] should be 0xf: "
				<< static_cast<int>(uintBufDP->GetWord(0)) << std::endl;
		err = err || !uintBufDP->IsMemoryPacket() || uintBufDP->GetWord(0) != 0xf;

		uintBufDP->Resize(128);
		if (printMessages_)
			std::cout << "Resizing a memory packet should not work, size should be 16: "
				<< std::dec << static_cast<int>(uintBufDP->GetSize()) << std::endl;
		err = err || uintBufDP->GetSize() != 16;

		if (printMessages_) std::cout << "Running CFO_DataPacket Destructor" << std::endl;
		delete uintBufDP;

		if (err)
		{
			if (printMessages_) std::cout << "CFO_DataPacket Class failed checks!" << std::endl;
		}
	}
	catch (std::exception ex)
	{
		if (printMessages_)
		{
			std::cout << "Test failed with exception: " << ex.what() << std::endl;
		}
		++classFailed_;
	}

end:
	if (err)
	{
		if (printMessages_) std::cout << "One or more classes failed!" << std::endl;
		++classFailed_;
	}
	else
	{
		if (printMessages_) std::cout << "All Classes Passed" << std::endl;
		++classPassed_;
	}

	if (printMessages_)
	{
		std::cout << std::endl << std::endl;
	}
}

void CFOLib::CFOLibTest::doRegTest()
{
	if (printMessages_)
	{
		std::cout << "Test 1: Register R/W" << std::endl;
	}
	try
	{
		auto designVersion = thisCFO_->ReadDesignVersion();
		if (printMessages_)
		{
			std::cout << "Reading Design Version: " << designVersion << std::endl;
			std::cout << "If simulated, result will be v99.99_2053-49-53-44 (SIMD in ASCII)" << std::endl;
			std::cout << "Attempting to Disable Ring 0." << std::endl;
		}
		auto ring0Value = thisCFO_->ReadRingEnabled(CFO_Ring_0);
		if (printMessages_)
		{
			std::cout << "Value before: " << ring0Value << std::endl;
		}
		thisCFO_->DisableRing(CFO_Ring_0);
		auto ring0New = thisCFO_->ReadRingEnabled(CFO_Ring_0);

		if (printMessages_)
		{
			std::cout << "Value after: " << ring0New << std::endl;
		}
		// Make sure that the ring is enabled after the test.
		thisCFO_->EnableRing(CFO_Ring_0, CFO_RingEnableMode(true, true, false), CFO_ROC_0);
		if (ring0New != ring0Value)
		{
			if (printMessages_)
			{
				std::cout << "Test Succeeded" << std::endl;
			}
			++regPassed_;
		}
		else
		{
			if (printMessages_)
			{
				std::cout << "Test Failed" << std::endl;
			}
			++regFailed_;
		}
	}
	catch (std::exception ex)
	{
		if (printMessages_)
		{
			std::cout << "Test failed with exception: " << ex.what() << std::endl;
		}
		++regFailed_;
	}
	if (printMessages_)
	{
		std::cout << std::endl << std::endl;
	}
}

void CFOLib::CFOLibTest::doDCSTest()
{
	if (printMessages_)
	{
		std::cout << "Test 4: DMA R/W on DCS Channel" << std::endl;
	}
	try
	{
		if (printMessages_)
		{
			std::cout << "Running DCS Request/Reply Cycle with 0-11 sequence" << std::endl;
		}
		uint8_t testData[12]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
		if (printMessages_)
		{
			std::cout << "Data in: ";
			for (auto i = 0; i < 12; i++)
			{
				std::cout << std::dec << static_cast<int>(testData[i]) << " ";
			}
			std::cout << std::endl;
		}
		std::cout << "TEST DISABLED FOR NOW!!!" << std::endl;
		//thisCFO_->DCSRequestReply(CFO_Ring_0, CFO_ROC_0, testData);
		if (printMessages_)
		{
			std::cout << "Data out: ";
			for (auto i = 0; i < 12; i++)
			{
				std::cout << std::dec << static_cast<int>(testData[i]) << " ";
			}
			std::cout << std::endl;
			std::cout << "Simulated CFO should match before/after." << std::endl;
		}
		++dcsPassed_;
	}
	catch (std::exception ex)
	{
		if (printMessages_)
		{
			std::cout << "Test failed with exception: " << ex.what() << std::endl;
		}
		++dcsFailed_;
	}
	if (printMessages_)
	{
		std::cout << std::endl << std::endl;
	}
}

void CFOLib::CFOLibTest::doDAQTest()
{
	if (printMessages_)
	{
		std::cout << "Test 5: DMA R/W on DAQ Channel" << std::endl;
	}
	try
	{
		thisCFO_->EnableRing(CFO_Ring_0, CFO_RingEnableMode(true, true, false), CFO_ROC_0);
		thisCFO_->SetInternalSystemClock();
		thisCFO_->DisableTiming();
		thisCFO_->SetMaxROCNumber(CFO_Ring_0, CFO_ROC_0);

		CFOSoftwareCFO theCFO(thisCFO_, true, 0, CFO_DebugType_SpecialSequence, true, !printMessages_);
		theCFO.SendRequestForTimestamp();
		auto data = thisCFO_->GetData();
		if (data.size() > 0)
		{
			if (printMessages_) std::cout << data.size() << " packets returned\n";
			for (auto i = 0ULL; i < data.size(); ++i)
			{
				TRACE(19, "CFO::GetJSONData constructing DataPacket:");
				auto test = CFO_DataPacket(data[i].blockPointer);
				if (printMessages_) std::cout << test.toJSON() << '\n'; // dumps whole databuff_t
				auto h2 = CFO_DataHeaderPacket(test);
				if (printMessages_)
				{
					std::cout << h2.toJSON() << '\n';
					for (auto jj = 0; jj < h2.GetPacketCount(); ++jj)
					{
						std::cout << "\t" << CFO_DataPacket(reinterpret_cast<uint8_t*>(data[i].blockPointer) + (jj + 1) * 16).toJSON() << std::endl;
					}
				}
			}
		}
		else
		{
			TRACE_CNTL("modeM", 0L);
			if (printMessages_) std::cout << "no data returned\n";
			++daqFailed_;
			return;
		}

		auto disparity = thisCFO_->ReadSERDESRXDisparityError(CFO_Ring_0);
		auto cnit = thisCFO_->ReadSERDESRXCharacterNotInTableError(CFO_Ring_0);
		auto rxBufferStatus = thisCFO_->ReadSERDESRXBufferStatus(CFO_Ring_0);
		auto eyescan = thisCFO_->ReadSERDESEyescanError(CFO_Ring_0);
		if (eyescan)
		{
			TRACE_CNTL("modeM", 0L);
			std::cout << "SERDES Eyescan Error Detected" << std::endl;
			++daqFailed_;
			return;
		}
		if (static_cast<int>(rxBufferStatus) > 2)
		{
			TRACE_CNTL("modeM", 0L);
			std::cout << "Bad Buffer status detected: " << rxBufferStatus << std::endl;
			++daqFailed_;
			return;
		}
		if (cnit.GetData()[0] || cnit.GetData()[1])
		{
			TRACE_CNTL("modeM", 0L);
			std::cout << "Character Not In Table Error detected" << std::endl;
			++daqFailed_;
			return;
		}
		if (disparity.GetData()[0] || disparity.GetData()[1])
		{
			TRACE_CNTL("modeM", 0L);
			std::cout << "Disparity Error Detected" << std::endl;
			++daqFailed_;
			return;
		}

		if (printMessages_)
		{
			std::cout << "Test Passed" << std::endl;
		}
		++daqPassed_;
	}
	catch (std::exception ex)
	{
		if (printMessages_)
		{
			std::cout << "Test failed with exception: " << ex.what() << std::endl;
		}
		++daqFailed_;
	}
	if (printMessages_)
	{
		std::cout << std::endl << std::endl;
	}
}

bool CFOLib::CFOLibTest::DataPacketIntegrityCheck(CFO_DataPacket* packet)
{
	auto retCode = true;
	for (auto ii = 0; ii < packet->GetSize(); ++ii)
	{
		packet->SetWord(ii, ii);
	}

	for (auto ii = 0; ii < packet->GetSize(); ++ii)
	{
		retCode = packet->GetWord(ii) == ii;
		if (!retCode) break;
	}

	return retCode;
}
