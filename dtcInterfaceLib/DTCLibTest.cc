#include "DTCLibTest.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

DTC::DTCLibTest::DTCLibTest() : running_(false), regPassed_(0),
regFailed_(0), pciePassed_(0), pcieFailed_(0), dmaStatePassed_(0), dmaStateFailed_(0),
daqPassed_(0), daqFailed_(0), dcsPassed_(0), dcsFailed_(0), regPassedTemp_(0),
regFailedTemp_(0), pciePassedTemp_(0), pcieFailedTemp_(0), dmaStatePassedTemp_(0),
dmaStateFailedTemp_(0), daqPassedTemp_(0), daqFailedTemp_(0), dcsPassedTemp_(0),
dcsFailedTemp_(0), nTests_(0), runRegTest_(false), runPCIeTest_(false),
runDMAStateTest_(false), runDAQTest_(false), runDCSTest_(false)
{
	thisDTC_ = new DTC();
}

DTC::DTCLibTest::~DTCLibTest()
{
	nTests_ = 0;
	if (workerThread_->joinable()) {
		workerThread_->join();
	}
	delete workerThread_;
	delete thisDTC_;
}

// Test Control
void DTC::DTCLibTest::startTest(bool regIOEnabled, bool pcieEnabled, bool dmaStateEnabled,
	bool daqEnabled, bool dcsEnabled, int nTests, bool printMessages)
{
	runRegTest_ = regIOEnabled;
	runPCIeTest_ = pcieEnabled;
	runDMAStateTest_ = dmaStateEnabled;
	runDCSTest_ = dcsEnabled;
	runDAQTest_ = daqEnabled;
	nTests_ = nTests;
	printMessages_ = printMessages;

	workerThread_ = new std::thread(&DTCLibTest::doTests, this);
}

void DTC::DTCLibTest::stopTests()
{
	nTests_ = 0;
	if (workerThread_->joinable()) {
		workerThread_->join();
	}
}

// Accessors
int DTC::DTCLibTest::regPassed()
{
	int result = regPassed_ - regPassedTemp_;
	regPassedTemp_ = regPassed_;
	return result;
}

int DTC::DTCLibTest::regFailed()
{
	int result = regFailed_ - regFailedTemp_;
	regFailedTemp_ = regFailed_;
	return result;
}

int DTC::DTCLibTest::pciePassed()
{
	int result = pciePassed_ - pciePassedTemp_;
	pciePassedTemp_ = pciePassed_;
	return result;
}

int DTC::DTCLibTest::pcieFailed()
{
	int result = pcieFailed_ - pcieFailedTemp_;
	pcieFailedTemp_ = pcieFailed_;
	return result;
}

int DTC::DTCLibTest::dmaStatePassed()
{
	int result = dmaStatePassed_ - dmaStatePassedTemp_;
	dmaStatePassedTemp_ = dmaStatePassed_;
	return result;
}

int DTC::DTCLibTest::dmaStateFailed()
{
	int result = dmaStateFailed_ - dmaStateFailedTemp_;
	dmaStateFailedTemp_ = dmaStateFailed_;
	return result;
}

int DTC::DTCLibTest::daqPassed()
{
	int result = daqPassed_ - daqPassedTemp_;
	daqPassedTemp_ = daqPassed_;
	return result;
}

int DTC::DTCLibTest::daqFailed()
{
	int result = daqFailed_ - daqFailedTemp_;
	daqFailedTemp_ = daqFailed_;
	return result;
}

int DTC::DTCLibTest::dcsPassed()
{
	int result = dcsPassed_ - dcsPassedTemp_;
	dcsPassedTemp_ = dcsPassed_;
	return result;
}

int DTC::DTCLibTest::dcsFailed()
{
	int result = dcsFailed_ - dcsFailedTemp_;
	dcsFailedTemp_ = dcsFailed_;
	return result;
}


// Private Functions
void DTC::DTCLibTest::doTests()
{
	running_ = true;
	// Make sure that the ring is enabled before the tests.
	thisDTC_->EnableRing(DTC_Ring_0, DTC_ROC_0);

	int testCount = 0;
	while (testCount < nTests_ || nTests_ < 0)
	{
		++testCount;
		if (runRegTest_){
			doRegTest();
		}
		if (runPCIeTest_){
			doPCIeTest();
		}
		if (runDMAStateTest_){
			doDMAStateTest();
		}
		if (runDCSTest_){
			doDCSTest();
		}
		if (runDAQTest_){
			doDAQTest();
		}
	}
	running_ = false;

	int totalPassed = 0;
	int totalTests = 0;

	if (runRegTest_) {
		totalPassed += regPassed_;
		totalTests += regPassed_ + regFailed_;
		std::cout << std::dec << regPassed_ << " of " << (regPassed_ + regFailed_) << " register I/O tests passed." << std::endl;
	}
	if (runPCIeTest_){
		totalPassed += pciePassed_;
		totalTests += pciePassed_ + pcieFailed_;
		std::cout << std::dec << pciePassed_ << " of " << (pciePassed_ + pcieFailed_) << " PCIe Status tests passed." << std::endl;
	}
	if (runDMAStateTest_){
		totalPassed += dmaStatePassed_;
		totalTests += dmaStatePassed_ + dmaStateFailed_;
		std::cout << std::dec << dmaStatePassed_ << " of " << (dmaStatePassed_ + dmaStateFailed_) << " DMA State tests passed." << std::endl;
	}
	if (runDCSTest_){
		totalPassed += dcsPassed_;
		totalTests += dcsPassed_ + dcsFailed_;
		std::cout << std::dec << dcsPassed_ << " of " << (dcsPassed_ + dcsFailed_) << " DCS DMA I/O tests passed." << std::endl;
	}
	if (runDAQTest_){
		totalPassed += daqPassed_;
		totalTests += daqPassed_ + daqFailed_;
		std::cout << std::dec << daqPassed_ << " of " << (daqPassed_ + daqFailed_) << " DAQ DMA I/O tests passed." << std::endl;
	}
	std::cout << std::dec << totalPassed << " of " << totalTests << " tests passed." << std::endl;
}

void DTC::DTCLibTest::doRegTest()
{
	if (printMessages_) {
		std::cout << "Test 1: Register R/W" << std::endl;
	}
	try{
		std::string designVersion = thisDTC_->ReadDesignVersion();
		if (printMessages_) {
			std::cout << "Reading Design Version: " << designVersion << std::endl;
			std::cout << "If simulated, result will be 53494D44 (SIMD in ASCII)" << std::endl;
			std::cout << "Attempting to Toggle Ring 0." << std::endl;
		}
		bool ring0Value = thisDTC_->ReadRingEnabled(DTC_Ring_0);
		if (printMessages_) {
			std::cout << "Value before: " << ring0Value << std::endl;
		}
		bool ring0New = thisDTC_->ToggleRingEnabled(DTC_Ring_0);
		if (printMessages_) {
			std::cout << "Value after: " << ring0New << std::endl;
		}
		// Make sure that the ring is enabled after the test.
		thisDTC_->EnableRing(DTC_Ring_0, DTC_ROC_0);
		if (ring0New != ring0Value)
		{
			if (printMessages_) {
				std::cout << "Test Succeeded" << std::endl;
			}
			++regPassed_;
		}
		else
		{
			if (printMessages_) {
				std::cout << "Test Failed" << std::endl;
			}
			++regFailed_;
		}
	}
	catch (std::exception ex)
	{
		if (printMessages_) {
			std::cout << "Test failed with exception: " << ex.what() << std::endl;
		}
		++regFailed_;
	}
	if (printMessages_) {
		std::cout << std::endl << std::endl;
	}
}

void DTC::DTCLibTest::doPCIeTest()
{
	if (printMessages_) {
		std::cout << "Test 2: PCIe State and Stats" << std::endl;
	}
	try {
		DTC_PCIeState state = thisDTC_->ReadPCIeState();
		DTC_PCIeStat stats = thisDTC_->ReadPCIeStats();
		if (printMessages_) {
			std::cout << "PCIe State: " << std::endl
				<< state.toString() << std::endl << std::endl;
			std::cout << "PCIe Stats, RX: " << stats.LRX << ", TX: " << stats.LTX << std::endl;
			std::cout << "Test Passed" << std::endl;
		}
		++pciePassed_;
	}
	catch (std::exception ex)
	{
		if (printMessages_) {
			std::cout << "Test failed with exception: " << ex.what() << std::endl;
		}
		++pcieFailed_;
	}
	if (printMessages_) {
		std::cout << std::endl << std::endl;
	}
}

void DTC::DTCLibTest::doDMAStateTest()
{
	if (printMessages_) {
		std::cout << "Test 3: DMA State and Stats" << std::endl;
	}
	try {
		DTC_DMAState eng0 = thisDTC_->ReadDMAState(DTC_DMA_Engine_DAQ, DTC_DMA_Direction_S2C);
		DTC_DMAState eng1 = thisDTC_->ReadDMAState(DTC_DMA_Engine_DAQ, DTC_DMA_Direction_C2S);
		DTC_DMAState eng32 = thisDTC_->ReadDMAState(DTC_DMA_Engine_DCS, DTC_DMA_Direction_S2C);
		DTC_DMAState eng33 = thisDTC_->ReadDMAState(DTC_DMA_Engine_DCS, DTC_DMA_Direction_C2S);

		DTC_DMAStats eng0Stats = thisDTC_->ReadDMAStats(DTC_DMA_Engine_DAQ, DTC_DMA_Direction_S2C);
		DTC_DMAStats eng1Stats = thisDTC_->ReadDMAStats(DTC_DMA_Engine_DAQ, DTC_DMA_Direction_C2S);
		DTC_DMAStats eng32Stats = thisDTC_->ReadDMAStats(DTC_DMA_Engine_DCS, DTC_DMA_Direction_S2C);
		DTC_DMAStats eng33Stats = thisDTC_->ReadDMAStats(DTC_DMA_Engine_DCS, DTC_DMA_Direction_C2S);

		if (printMessages_) {
			std::cout << "DMA State: " << std::endl
				<< "DAQ Channel, S2C: " << eng0.toString() << std::endl
				<< "DAQ Channel, C2S: " << eng1.toString() << std::endl
				<< "DCS Channel, S2C: " << eng32.toString() << std::endl
				<< "DCS Channel, C2S: " << eng33.toString() << std::endl;
			std::cout << "DMA Stats: " << std::endl
				<< "DAQ Channel, S2C: " << eng0Stats.Stats[0].toString() << std::endl
				<< "DAQ Channel, C2S: " << eng1Stats.Stats[0].toString() << std::endl
				<< "DCS Channel, S2C: " << eng32Stats.Stats[0].toString() << std::endl
				<< "DCS Channel, C2S: " << eng33Stats.Stats[0].toString() << std::endl;
			std::cout << "Test Passed." << std::endl;
		}
		++dmaStatePassed_;
	}
	catch (std::exception ex)
	{
		if (printMessages_) {
			std::cout << "Test failed with exception: " << ex.what() << std::endl;
		}
		++dmaStateFailed_;
	}
	if (printMessages_) {
		std::cout << std::endl << std::endl;
	}
}

void DTC::DTCLibTest::doDCSTest()
{
	if (printMessages_) {
		std::cout << "Test 4: DMA R/W on DCS Channel" << std::endl;
	}
	try{
		if (printMessages_) {
			std::cout << "Running DCS Request/Reply Cycle with 0-11 sequence" << std::endl;
		}
		uint8_t testData[12] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
		if (printMessages_) {
			std::cout << "Data in: ";
			for (int i = 0; i < 12; i++)
			{
				std::cout << std::dec << (int)testData[i] << " ";
			}
			std::cout << std::endl;
		}
		thisDTC_->DCSRequestReply(DTC_Ring_0, DTC_ROC_0, testData);
		if (printMessages_) {
			std::cout << "Data out: ";
			for (int i = 0; i < 12; i++)
			{
				std::cout << std::dec << (int)testData[i] << " ";
			}
			std::cout << std::endl;
			std::cout << "Simulated DTC should match before/after." << std::endl;
		}
		++dcsPassed_;
	}
	catch (std::exception ex)
	{
		if (printMessages_) {
			std::cout << "Test failed with exception: " << ex.what() << std::endl;
		}
		++dcsFailed_;
	}
	if (printMessages_) {
		std::cout << std::endl << std::endl;
	}
}

void DTC::DTCLibTest::doDAQTest()
{
	if (printMessages_) {
		std::cout << "Test 5: DMA R/W on DAQ Channel" << std::endl;
	}
	try{
		if (printMessages_) {
			std::cout << "Sending Readout Request Packet on Ring 0" << std::endl;
		}
		thisDTC_->SendReadoutRequestPacket(DTC_Ring_0, DTC_Timestamp((uint64_t)time(0)));
		int length;
		int retry = 3;
		bool err = false;
		do {
			std::vector<void*> data = thisDTC_->GetData(DTC_Ring_0, DTC_ROC_0, DTC_Timestamp((uint64_t)time(0)), &length);
			if (data.size() > 0) {
				if (data.size() > 1) {
					if (printMessages_) {
						std::cout << "Data array is larger than expected! Cowardly refusing to continue the test." << std::endl;
					}
					err = true;
					++daqFailed_;
					break;
				}
				else {
					if (printMessages_) {
						std::cout << "Dumping data..." << std::endl;
					}
					for (int i = 0; i < length; ++i)
					{
						for (int j = 0; j < 8; ++j)
						{
							if (printMessages_) {
								std::cout << "0x" << std::setfill('0') << std::setw(4) << std::hex
									<< ((uint16_t*)data[0])[i * 8 + j] << std::endl;
							}
						}
					}
				}
			}
			retry--;
		} while (retry > 0);
		if (err) {
			if (printMessages_) {
				std::cout << "Test Aborted (fail!)" << std::endl;
			}
			++daqFailed_;
		}
		else {
			if (printMessages_) {
				std::cout << "Test Passed" << std::endl;
			}
			++daqPassed_;
		}
	}
	catch (std::exception ex)
	{
		if (printMessages_) {
			std::cout << "Test failed with exception: " << ex.what() << std::endl;
		}
		++daqFailed_;
	}
	if (printMessages_) {
		std::cout << std::endl << std::endl;
	}
}