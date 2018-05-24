#ifndef CFO_REGISTERS_H
#define CFO_REGISTERS_H

//#include <bitset> // std::bitset
//#include <cstdint> // uint8_t, uint16_t
#include <functional> // std::bind, std::function
#include <vector> // std::vector

#include "cfoInterfaceLib/CFO_Types.h"
#include "cfoInterfaceLib/cfodev.h"

namespace CFOLib
{
	enum CFO_Register : uint16_t
	{
		CFO_Register_DesignVersion = 0x9000,
		CFO_Register_DesignDate = 0x9004,
		CFO_Register_DesignStatus = 0x9008,
		CFO_Register_VivadoVersion = 0x9080,
		CFO_Register_CFOControl = 0x9100,
		CFO_Register_DMATransferLength = 0x9104,
		CFO_Register_SERDESLoopbackEnable = 0x9108,
		CFO_Register_ClockOscillatorStatus = 0x910C,
		CFO_Register_LinkEnable = 0x9114,
		CFO_Register_SERDESReset = 0x9118,
		CFO_Register_SERDESRXDisparityError = 0x911C,
		CFO_Register_SERDESRXCharacterNotInTableError = 0x9120,
		CFO_Register_SERDESUnlockError = 0x9124,
		CFO_Register_SERDESPLLLocked = 0x9128,
		CFO_Register_SERDESRXStatus = 0x9134,
		CFO_Register_SERDESResetDone = 0x9138,
		CFO_Register_SFPSERDESStatus = 0x9140,
		CFO_Register_BeamOnTimerPreset = 0x9144,
		CFO_Register_EnableBeamOnMode = 0x9148,
		CFO_Register_EnableBeamOffMode = 0x914C,
		CFO_Register_ClockMarkerIntervalCount = 0x9154,
		CFO_Register_SERDESOscillatorFrequency = 0x9160,
		CFO_Register_SERDESOscillatorControl = 0x9164,
		CFO_Register_SERDESOscillatorParameterLow = 0x9168,
		CFO_Register_SERDESOscillatorParameterHigh = 0x916C,
		CFO_Register_TimestampPreset0 = 0x9180,
		CFO_Register_TimestampPreset1 = 0x9184,
		CFO_Register_NUMDTCs = 0x918C,
		CFO_Register_FIFOFullErrorFlag0 = 0x9190,
		CFO_Register_ReceivePacketError = 0x919C,
		CFO_Register_EventWindowEmulatorIntervalTime = 0x91A0,
		CFO_Register_EventWindowHoldoffTime = 0x91A4,
		CFO_Register_EventWindowTimeoutError = 0x91A8,
		CFO_Register_EventWindowTimeoutValue = 0x91AC,
		CFO_Register_ReceiveByteCountDataLink0 = 0x9200,
		CFO_Register_ReceiveByteCountDataLink1 = 0x9204,
		CFO_Register_ReceiveByteCountDataLink2 = 0x9208,
		CFO_Register_ReceiveByteCountDataLink3 = 0x920C,
		CFO_Register_ReceiveByteCountDataLink4 = 0x9210,
		CFO_Register_ReceiveByteCountDataLink5 = 0x9214,
		CFO_Register_ReceiveByteCountDataLink6 = 0x9218,
		CFO_Register_ReceiveByteCountDataLink7 = 0x921C,
		CFO_Register_ReceivePacketCountDataLink0 = 0x9220,
		CFO_Register_ReceivePacketCountDataLink1 = 0x9224,
		CFO_Register_ReceivePacketCountDataLink2 = 0x9228,
		CFO_Register_ReceivePacketCountDataLink3 = 0x922C,
		CFO_Register_ReceivePacketCountDataLink4 = 0x9230,
		CFO_Register_ReceivePacketCountDataLink5 = 0x9234,
		CFO_Register_ReceivePacketCountDataLink6 = 0x9238,
		CFO_Register_ReceivePacketCountDataLink7 = 0x923C,
		CFO_Register_TransmitByteCountDataLink0 = 0x9240,
		CFO_Register_TransmitByteCountDataLink1 = 0x9244,
		CFO_Register_TransmitByteCountDataLink2 = 0x9248,
		CFO_Register_TransmitByteCountDataLink3 = 0x924C,
		CFO_Register_TransmitByteCountDataLink4 = 0x9250,
		CFO_Register_TransmitByteCountDataLink5 = 0x9254,
		CFO_Register_TransmitByteCountDataLink6 = 0x9258,
		CFO_Register_TransmitByteCountDataLink7 = 0x925C,
		CFO_Register_TransmitPacketCountDataLink0 = 0x9260,
		CFO_Register_TransmitPacketCountDataLink1 = 0x9264,
		CFO_Register_TransmitPacketCountDataLink2 = 0x9268,
		CFO_Register_TransmitPacketCountDataLink3 = 0x926C,
		CFO_Register_TransmitPacketCountDataLink4 = 0x9270,
		CFO_Register_TransmitPacketCountDataLink5 = 0x9274,
		CFO_Register_TransmitPacketCountDataLink6 = 0x9278,
		CFO_Register_TransmitPacketCountDataLink7 = 0x927C,
		CFO_Register_DDRMemoryDMAWriteStartAddress = 0x9300,
		CFO_Register_DDRMemoryDMAReadStartAddress = 0x9304,
		CFO_Register_DDRMemoryDMAReadByteCount = 0x9308,
		CFO_Register_DDRBeamOnBaseAddress = 0x930C,
		CFO_Register_DDRBeamOffBaseAddress = 0x9310,
		CFO_Register_FireflyCSRRegister = 0x9320,
		CFO_Register_SERDESPRBSControlLink0 = 0x9330,
		CFO_Register_SERDESPRBSControlLink1 = 0x9334,
		CFO_Register_SERDESPRBSControlLink2 = 0x9338,
		CFO_Register_SERDESPRBSControlLink3 = 0x933C,
		CFO_Register_SERDESPRBSControlLink4 = 0x9340,
		CFO_Register_SERDESPRBSControlLink5 = 0x9344,
		CFO_Register_SERDESPRBSControlLink6 = 0x9348,
		CFO_Register_SERDESPRBSControlLink7 = 0x934C,
		CFO_Register_CableDelayValueLink0 = 0x9360,
		CFO_Register_CableDelayValueLink1 = 0x9364,
		CFO_Register_CableDelayValueLink2 = 0x9368,
		CFO_Register_CableDelayValueLink3 = 0x936C,
		CFO_Register_CableDelayValueLink4 = 0x9370,
		CFO_Register_CableDelayValueLink5 = 0x9374,
		CFO_Register_CableDelayValueLink6 = 0x9378,
		CFO_Register_CableDelayValueLink7 = 0x937C,
		CFO_Register_CableDelayControlStatus = 0x9380,
		CFO_Register_FPGAProgramData = 0x9400,
		CFO_Register_FPGAPROMProgramStatus = 0x9404,
		CFO_Register_FPGACoreAccess = 0x9408,
		CFO_Register_Invalid,
	};

	/// <summary>
	/// The CFO_Registers class represents the CFO Register space, and all the methods necessary to read and write those registers.
	/// Each register has, at the very least, a read method, a write method, and a CFO_RegisterFormatter method which
	/// formats the register value in a human-readable way.
	/// </summary>
	class CFO_Registers
	{
	public:
		/// <summary>
		/// Construct an instance of the CFO register map
		/// </summary>
		/// <param name="mode">Default: CFO_SimMode_Disabled; The simulation mode of the CFO</param>
		/// <param name="CFO">CFO/DTC card instance to use</param>
		/// <param name="dtcMask">The initially-enabled DTCs. Each digit corresponds to a Link, so all DTCs = 0xFFFFFFFF</param>
		/// <param name="skipInit">Default: false; Whether to skip initializing the CFO using the SimMode. Used to read state.</param>
		/// <param name="expectedDesignVersion">Expected CFO Firmware Design Version. If set, will throw an exception if the CFO firmware does not match (Default: "")</param>
		explicit CFO_Registers(CFO_SimMode mode, int CFO, unsigned dtcMask = 0, std::string expectedDesignVersion = "", bool skipInit = false);
		/// <summary>
		/// CFO_Registers destructor
		/// </summary>
		virtual ~CFO_Registers();

		/// <summary>
		/// Get a pointer to the device handle
		/// </summary>
		/// <returns>cfodev* pointer</returns>
		cfodev* GetDevice()
		{
			return &device_;
		}

		//
		// CFO Sim Mode Virtual Register
		//
		/// <summary>
		/// Get the current CFO_SimMode of this CFO_Registers object
		/// </summary>
		/// <returns></returns>
		CFO_SimMode ReadSimMode() const
		{
			return simMode_;
		}

		/// <summary>
		/// Initialize the CFO in the given SimMode.
		/// </summary>
		/// <param name="expectedDesignVersion">Expected CFO Firmware Design Version. If set, will throw an exception if the CFO firmware does not match</param>
		/// <param name="mode">Mode to set</param>
		/// <param name="CFO">CFO/DTC card instance to use</param>
		/// <param name="skipInit">Whether to skip initializing the CFO using the SimMode. Used to read state.</param>
		/// <returns></returns>
		CFO_SimMode SetSimMode(std::string expectedDesignVersion, CFO_SimMode mode, int CFO, bool skipInit = false);

		//
		// CFO Register Dumps
		//
		/// <summary>
		/// Perform a register dump
		/// </summary>
		/// <param name="width">Printable width of description fields</param>
		/// <returns>StLink containing all registers, with their human-readable representations</returns>
		std::string FormattedRegDump(int width);
		/// <summary>
		/// Dump the Link byte/packet counters
		/// </summary>
		/// <param name="width">Printable width of description fields</param>
		/// <returns>StLink containing the Link counter registers, with their human-readable representations</returns>
		std::string LinkCountersRegDump(int width);

		/// <summary>
		/// Initializes a CFO_RegisterFormatter for the given CFO_Register
		/// </summary>
		/// <param name="address">Address of register to format</param>
		/// <returns>CFO_RegisterFormatter with address and raw value set</returns>
		CFO_RegisterFormatter CreateFormatter(const CFO_Register& address)
		{
			CFO_RegisterFormatter form;
			form.descWidth = formatterWidth_;
			form.address = address;
			form.value = ReadRegister_(address);
			return form;
		}

		//
		// Register IO Functions
		//

		// Desgin Version/Date Registers
		std::string ReadDesignVersion();
		CFO_SerdesClockSpeed ReadSERDESVersion();
		CFO_RegisterFormatter FormatDesignVersion();

		/// <summary>
		/// Read the modification date of the CFO firmware
		/// </summary>
		/// <returns>Design date in 20YY-MM-DD-HH format</returns>
		std::string ReadDesignDate();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatDesignDate();

		// Design Status Register
		bool ReadDDRFIFOEmpty();
		bool ReadDDRClockCalibrationDone();
		CFO_RegisterFormatter FormatDesignStatus();

		// Vivado Version Register
		std::string ReadVivadoVersion();
		CFO_RegisterFormatter FormatVivadoVersion();

		// CFO Control Register
		/// <summary>
		/// Perform a CFO Reset
		/// </summary>
		void ResetCFO();
		/// <summary>
		/// Read the Reset CFO Bit
		/// </summary>
		/// <returns>True if the CFO is currently resetting, false otherwise</returns>
		bool ReadResetCFO();
		/// <summary>
		/// Enable automatically generating Data Request packets from the CFO CFO Emulator
		/// </summary>
		void EnableAutogenDRP();
		/// <summary>
		/// Disable automatically generating Data Request packets from the CFO CFO Emulator
		/// </summary>
		void DisableAutogenDRP();
		/// <summary>
		/// Read whether Data Request packets are generated by the CFO CFO Emulator
		/// </summary>
		/// <returns>True if Data Request packets are generated by the CFO CFO Emulator, false otherwise</returns>
		bool ReadAutogenDRP();

		void EnableEventWindowInput();
		void DisableEventWindowInput();
		bool ReadEventWindowInput();

		/// <summary>
		/// Set the CFO to External timing mode
		/// </summary>
		void SetExternalSystemClock();
		/// <summary>
		/// Set the CFO to Internal timing mode
		/// </summary>
		void SetInternalSystemClock();
		/// <summary>
		/// Read the status of the System timing bit
		/// </summary>
		/// <returns>True if the clock is in External mode, false for Internal mode</returns>
		bool ReadSystemClock();
		/// <summary>
		/// Enable Timing Synchronization for the ROC control packets
		/// </summary>
		void EnableTiming();
		/// <summary>
		/// Disable timing synchronization for ROC control packets
		/// </summary>
		void DisableTiming();
		/// <summary>
		/// Read the status of the timing sync enable bit
		/// </summary>
		/// <returns>The current value of the timing sync enable bit</returns>
		bool ReadTimingEnable();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatCFOControl();

		// DMA Transfer Length Register
		/// <summary>
		/// Set the DMA buffer size which will automatically trigger a DMA
		/// </summary>
		/// <param name="length">Size, in bytes of buffer that will trigger a DMA</param>
		void SetTriggerDMATransferLength(uint16_t length);
		/// <summary>
		/// Read the DMA buffer size which will automatically trigger a DMA
		/// </summary>
		/// <returns>The DMA buffer size which will automatically trigger a DMA, in bytes</returns>
		uint16_t ReadTriggerDMATransferLength();
		/// <summary>
		/// Set the minimum DMA transfer size. Absolute minimum is 64 bytes.
		/// Buffers smaller than this size will be padded to the minimum.
		/// </summary>
		/// <param name="length">Size, in bytes, of the minimum DMA transfer buffer</param>
		void SetMinDMATransferLength(uint16_t length);
		/// <summary>
		/// Read the minimum DMA transfer size.
		/// </summary>
		/// <returns>The minimum DMA size, in bytes</returns>
		uint16_t ReadMinDMATransferLength();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatDMATransferLength();

		// SERDES Loopback Enable Register
		/// <summary>
		/// Set the SERDES Loopback mode for the given Link
		/// </summary>
		/// <param name="Link">Link to set for</param>
		/// <param name="mode">CFO_SERDESLoopbackMode to set</param>
		void SetSERDESLoopbackMode(const CFO_Link_ID& Link, const CFO_SERDESLoopbackMode& mode);
		/// <summary>
		/// Read the SERDES Loopback mode for the given Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>CFO_SERDESLoopbackMode of the Link</returns>
		CFO_SERDESLoopbackMode ReadSERDESLoopback(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESLoopbackEnable();

		// Clock Status Register
		/// <summary>
		/// Read the SERDES Oscillator IIC Error Bit
		/// </summary>
		/// <returns>True if the SERDES Oscillator IIC Error is set</returns>
		bool ReadSERDESOscillatorIICError();
		/// <summary>
		/// Read the SERDES Oscillator Initalization Complete flag
		/// </summary>
		/// <returns>Whether the SERDES Oscillator has completed initialization</returns>
		bool ReadSERDESOscillatorInitializationComplete();
		/// <summary>
		/// Wait for the SERDES Oscillator to initialize, up to max_wait seconds
		/// </summary>
		/// <param name="max_wait">Seconds to wait</param>
		/// <returns>Whether the SERDES OScillator Completed initialization in the timeout</returns>
		bool WaitForSERDESOscillatorInitializationComplete(double max_wait = 1.0);

		bool ReadTimingClockPLLLocked();

		CFO_RegisterFormatter FormatClockOscillatorStatus();

		// Link Enable Register
		/// <summary>
		/// Enable a SERDES Link
		/// </summary>
		/// <param name="Link">Link to enable</param>
		/// <param name="mode">Link enable bits to set (Default: All)</param>
		/// <param name="dtcCount">Number of DTCs in the Link (Default: 0)</param>
		void EnableLink(const CFO_Link_ID& Link, const CFO_LinkEnableMode& mode = CFO_LinkEnableMode(), const uint8_t& dtcCount = 0);
		/// <summary>
		/// Disable a SERDES Link
		/// The given mode bits will be UNSET
		/// </summary>
		/// <param name="Link">Link to disable</param>
		/// <param name="mode">Link enable bits to unset (Default: All)</param>
		void DisableLink(const CFO_Link_ID& Link, const CFO_LinkEnableMode& mode = CFO_LinkEnableMode());
		/// <summary>
		/// Read the Link Enable bits for a given SERDES Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>CFO_LinkEnableMode containing TX, RX, and CFO bits</returns>
		CFO_LinkEnableMode ReadLinkEnabled(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatLinkEnable();

		// SERDES Reset Register
		/// <summary>
		/// Reset the SERDES
		/// Will poll the Reset SERDES Done flag until the SERDES reset is complete
		/// </summary>
		/// <param name="Link">Link to reset</param>
		/// <param name="interval">Pollint interval, in microseconds</param>
		void ResetSERDES(const CFO_Link_ID& Link, int interval = 100);
		/// <summary>
		/// Read if a SERDES reset is currently in progress
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>True if a SERDES reset is in progress</returns>
		bool ReadResetSERDES(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESReset();

		// SERDES RX Disparity Error Register
		/// <summary>
		/// Read the SERDES RX Dispatity Error bits
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>CFO_SERDESRXDisparityError object with error bits</returns>
		CFO_SERDESRXDisparityError ReadSERDESRXDisparityError(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESRXDisparityError();

		// SERDES Character Not In Table Error Register
		/// <summary>
		/// Read the SERDES Character Not In Table Error bits
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>CFO_CharacterNotInTableError object with error bits</returns>
		CFO_CharacterNotInTableError ReadSERDESRXCharacterNotInTableError(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESRXCharacterNotInTableError();

		// SERDES Unlock Error Register
		/// <summary>
		/// Read whether the SERDES Unlock Error bit is set
		/// </summary>
		/// <param name="Link">Link to check</param>
		/// <returns>True if the SERDES Unlock Error bit is set on the given Link</returns>
		bool ReadSERDESUnlockError(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESUnlockError();

		// SERDES PLL Locked Register
		/// <summary>
		/// Read if the SERDES PLL is locked for the given SERDES Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>True if the PLL is locked, false otherwise</returns>
		bool ReadSERDESPLLLocked(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESPLLLocked();

		// SERDES RX Status Register
		/// <summary>
		/// Read the SERDES RX Status for the given SERDES Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>CFO_RXStatus object</returns>
		CFO_RXStatus ReadSERDESRXStatus(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESRXStatus();

		// SERDES Reset Done Register
		/// <summary>
		/// Read if the SERDES reset is complete on the given SERDES Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>True if the SERDES Reset is done, false otherwise</returns>
		bool ReadResetSERDESDone(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESResetDone();


		// SERDES RX CDR Lock Register
		/// <summary>
		/// Read the SERDES CDR Lock bit for the given SERDES Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>True if the SERDES CDR Lock bit is set</returns>
		bool ReadSERDESRXCDRLock(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESRXCDRLock();

		// Beam On Timer Preset Regsiter
		/// <summary>
		/// Set the maximum time a DMA buffer may be active before it is sent, in 4ns ticks.
		/// The default value is 0x800
		/// </summary>
		/// <param name="preset">Maximum active time for DMA buffers</param>
		void SetBeamOnTimerPreset(uint32_t preset);
		/// <summary>
		/// Read the maximum time a DMA buffer may be active before it is sent, in 4ns ticks.
		/// The default value is 0x800
		/// </summary>
		/// <returns>Maximum active time for DMA buffers</returns>
		uint32_t ReadBeamOnTimerPreset();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatBeamOnTimerPreset();

		// Enable Beam On Mode Register
		void EnableBeamOnMode(const CFO_Link_ID& link);
		void DisableBeamOnMode(const CFO_Link_ID& link);
		bool ReadBeamOnMode(const CFO_Link_ID& link);
		CFO_RegisterFormatter FormatBeamOnMode();

		// Enable Beam Off Mode Register
		void EnableBeamOffMode(const CFO_Link_ID& link);
		void DisableBeamOffMode(const CFO_Link_ID& link);
		bool ReadBeamOffMode(const CFO_Link_ID& link);
		CFO_RegisterFormatter FormatBeamOffMode();

		// 40 MHz Clock Marker Interval Count Register
		void SetClockMarkerIntervalCount(uint32_t data);
		uint32_t ReadClockMarkerIntervalCount();
		CFO_RegisterFormatter FormatClockMarkerIntervalCount();

		// SERDES Oscillator Registers
		/// <summary>
		/// Read the current SERDES Oscillator frequency, in Hz
		/// </summary>
		/// <returns>Current SERDES Oscillator frequency, in Hz</returns>
		uint32_t ReadSERDESOscillatorFrequency();
		/// <summary>
		/// Set the SERDES Oscillator frequency
		/// </summary>
		/// <param name="freq">New frequency, in Hz</param>
		void SetSERDESOscillatorFrequency(uint32_t freq);
		/// <summary>
		/// Read the SERDES Oscillator IIC FSM bit
		/// </summary>
		/// <returns>True if the Oscillator IIC FSM bit is set</returns>
		bool ReadSERDESOscillaotrIICFSMEnable();
		/// <summary>
		/// Enable the SERDES Oscillator IIC FSM
		/// </summary>
		void EnableSERDESOscillatorIICFSM();
		/// <summary>
		/// Disable the SERDES Oscillator IIC FSM
		/// </summary>
		void DisableSERDESOscillatorIICFSM();
		/// <summary>
		/// Returns the current mode of the SERDES Oscillator Parameter register
		/// </summary>
		/// <returns>True if the Oscillator is in Write mode, false for Read mode</returns>
		bool ReadSERDESOscillatorReadWriteMode();
		/// <summary>
		/// Set the SERDES Oscillator to Write mode
		/// </summary>
		void SetSERDESOscillatorWriteMode();
		/// <summary>
		/// Set the SERDES Oscillator to Read mode
		/// </summary>
		void SetSERDESOscillatorReadMode();
		/// <summary>
		/// Read the current Oscillator program for the SERDES Oscillator
		/// </summary>
		/// <returns>SERDES Oscillator Program</returns>
		uint64_t ReadSERDESOscillatorParameters();
		/// <summary>
		/// Set the SERDES Oscillator program
		/// </summary>
		/// <param name="parameters">New program for the SERDES Oscillator</param>
		void SetSERDESOscillatorParameters(uint64_t parameters);
		/// <summary>
		/// Read the current SERDES Oscillator clock speed
		/// </summary>
		/// <returns>Current SERDES clock speed</returns>
		CFO_SerdesClockSpeed ReadSERDESOscillatorClock();
		/// <summary>
		/// Set the SERDES Oscillator clock speed for the given SERDES transfer rate
		/// </summary>
		/// <param name="speed">Clock speed to set</param>
		void SetSERDESOscillatorClock(CFO_SerdesClockSpeed speed);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESOscillatorFrequency();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESOscillatorControl();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESOscillatorParameterLow();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatSERDESOscillatorParameterHigh();

		// Timestamp Preset Registers
		/// <summary>
		/// Set the Timestamp preset for Timing system emulation mode
		/// </summary>
		/// <param name="preset">Timestamp for Timing emulation</param>
		void SetTimestampPreset(const CFO_Timestamp& preset);
		/// <summary>
		/// Read the Timestamp preset for Timing system emulation mode
		/// </summary>
		/// <returns>Timestamp preset</returns>
		CFO_Timestamp ReadTimestampPreset();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTimestampPreset0();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTimestampPreset1();

		// NUMDTCs Register
		/// <summary>
		/// Set the maximum ROC ID for the given Link
		/// </summary>
		/// <param name="Link">Link to set</param>
		/// <param name="dtcCount">Number of DTCs on the link</param>
		void SetMaxDTCNumber(const CFO_Link_ID& Link, const uint8_t& lastRoc);
		/// <summary>
		/// Read the number of ROCs configured on the given Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <param name="local">Whether to use the NUMDTCs virtual register or perform a register access (Default: true, use virtual register)</param>
		/// <returns>Number of DTCs on the link</returns>
		uint8_t ReadLinkDTCCount(const CFO_Link_ID& Link, bool local = true);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatNUMDTCs();

		// FIFO Full Error Flags Registers
		/// <summary>
		/// Clear all FIFO Full Error Flags for the given Link
		/// </summary>
		/// <param name="Link">Link to clear</param>
		void ClearFIFOFullErrorFlags(const CFO_Link_ID& Link);
		/// <summary>
		/// Read the FIFO Full Error/Status Flags for the given Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>CFO_FIFOFullErrorFlags object</returns>
		CFO_FIFOFullErrorFlags ReadFIFOFullErrorFlags(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatFIFOFullErrorFlag0();

		// Receive Packet Error Register
		/// <summary>
		/// Clear the RX Elastic Buffer Underrun Error Flag for the given Link
		/// </summary>
		/// <param name="Link">Link to clear</param>
		void ClearRXElasticBufferUnderrun(const CFO_Link_ID& Link);
		/// <summary>
		/// Read the RX Elastic Buffer Underrun Error Flag for the given Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>True if the RX Elastic Buffer Underrun Error Flag is set</returns>
		bool ReadRXElasticBufferUnderrun(const CFO_Link_ID& Link);
		/// <summary>
		/// Clear the RX Elastic Buffer Overrun Error Flag for the given Link
		/// </summary>
		/// <param name="Link">Link to clear</param>
		void ClearRXElasticBufferOverrun(const CFO_Link_ID& Link);
		/// <summary>
		/// Read the RX Elastic Buffer Overrun Error Flag for the given Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>True if the RX Elastic Buffer Overrun Error Flag is set</returns>
		bool ReadRXElasticBufferOverrun(const CFO_Link_ID& Link);
		/// <summary>
		/// Clear the Packet Error Flag for the given Link
		/// </summary>
		/// <param name="Link">Link to clear</param>
		void ClearPacketError(const CFO_Link_ID& Link);
		/// <summary>
		/// Read the Packet Error Flag for the given Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>True if the Packet Error Flag is set</returns>
		bool ReadPacketError(const CFO_Link_ID& Link);
		/// <summary>
		/// Clear the Packet CRC Error Flag for the given Link
		/// </summary>
		/// <param name="Link">Link to clear</param>
		void ClearPacketCRCError(const CFO_Link_ID& Link);
		/// <summary>
		/// Read the Packet CRC Error Flag for the given Link
		/// </summary>
		/// <param name="Link">Link to read</param>
		/// <returns>True if the Packet CRC Error Flag is set</returns>
		bool ReadPacketCRCError(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceivePacketError();

		// Event Window Emulator (Beam Off) Interval Time Register
		void SetEventWindowEmulatorInterval(const uint32_t& data);
		uint32_t ReadEventWindowEmulatorInterval();
		CFO_RegisterFormatter FormatEventWindowEmulatorIntervalTime();

		// Event Window Holdoff Time Register
		void SetEventWindowHoldoffTime(const uint32_t& data);
		uint32_t ReadEventWindowHoldoffTime();
		CFO_RegisterFormatter FormatEventWindowHoldoffTime();

		// Event Window Timeout Error Register
		bool ReadEventWindowTimeoutError(const CFO_Link_ID& link);
		void ClearEventWindowTimeoutError(const CFO_Link_ID& link);
		CFO_RegisterFormatter FormatEventWindowTimeoutError();

		// Event Window Timeout Interval Register
		void SetEventWindowTimeoutInterval(const uint32_t& data);
		uint32_t ReadEventWindowTimeoutInterval();
		CFO_RegisterFormatter FormatEventWindowTimeoutInterval();

		/// <summary>
		/// Clear the value of the Receive byte counter
		/// </summary>
		/// <param name="Link">Link to clear counter for</param>
		void ClearReceiveByteCount(const CFO_Link_ID& Link);
		/// <summary>
		/// Read the value of the Receive byte counter
		/// </summary>
		/// <param name="Link">Link to read counter for</param>
		/// <returns>Current value of the Receive byte counter on the given Link</returns>
		uint32_t ReadReceiveByteCount(const CFO_Link_ID& Link);
		/// <summary>
		/// Clear the value of the Receive Packet counter
		/// </summary>
		/// <param name="Link">Link to clear counter for</param>
		void ClearReceivePacketCount(const CFO_Link_ID& Link);
		/// <summary>
		/// Read the value of the Receive Packet counter
		/// </summary>
		/// <param name="Link">Link to read counter for</param>
		/// <returns>Current value of the Receive Packet counter on the given Link</returns>
		uint32_t ReadReceivePacketCount(const CFO_Link_ID& Link);
		/// <summary>
		/// Clear the value of the Transmit byte counter
		/// </summary>
		/// <param name="Link">Link to clear counter for</param>
		void ClearTransmitByteCount(const CFO_Link_ID& Link);
		/// <summary>
		/// Read the value of the Transmit byye counter
		/// </summary>
		/// <param name="Link">Link to read counter for</param>
		/// <returns>Current value of the Transmit byte counter on the given Link</returns>
		uint32_t ReadTransmitByteCount(const CFO_Link_ID& Link);
		/// <summary>
		/// Clear the value of the Transmit Packet counter
		/// </summary>
		/// <param name="Link">Link to clear counter for</param>
		void ClearTransmitPacketCount(const CFO_Link_ID& Link);
		/// <summary>
		/// Read the value of the Transmit Packet counter
		/// </summary>
		/// <param name="Link">Link to read counter for</param>
		/// <returns>Current value of the Transmit Packet counter on the given Link</returns>
		uint32_t ReadTransmitPacketCount(const CFO_Link_ID& Link);
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceiveByteCountLink0();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceiveByteCountLink1();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceiveByteCountLink2();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceiveByteCountLink3();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceiveByteCountLink4();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceiveByteCountLink5();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceiveByteCountLink6();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceiveByteCountLink7();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceivePacketCountLink0();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceivePacketCountLink1();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceivePacketCountLink2();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceivePacketCountLink3();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceivePacketCountLink4();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceivePacketCountLink5();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceivePacketCountLink6();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatReceivePacketCountLink7();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTramsitByteCountLink0();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTramsitByteCountLink1();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTramsitByteCountLink2();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTramsitByteCountLink3();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTramsitByteCountLink4();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTramsitByteCountLink5();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTramsitByteCountLink6();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTramsitByteCountLink7();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTransmitPacketCountLink0();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTransmitPacketCountLink1();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTransmitPacketCountLink2();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTransmitPacketCountLink3();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTransmitPacketCountLink4();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTransmitPacketCountLink5();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTransmitPacketCountLink6();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatTransmitPacketCountLink7();


		// DDR3 Memory DMA Write Start Address Register
		void SetDMAWriteStartAddress(const uint32_t& address);
		uint32_t ReadDMAWriteStartAddress();
		CFO_RegisterFormatter FormatDMAWriteStartAddress();

		// DDR3 Memory DMA Read Start Address Register
		void SetDMAReadStartAddress(const uint32_t& address);
		uint32_t ReadDMAReadStartAddress();
		CFO_RegisterFormatter FormatDMAReadStartAddress();

		// DDR3 Memory DMA Read Byte Count / Start Read DMA Register
		void SetDMAReadByteCount(const uint32_t& bytes);
		uint32_t ReadDMAReadByteCount();
		void StartDMARead() { SetDMAReadByteCount(ReadDMAReadByteCount()); }
		CFO_RegisterFormatter FormatDMAReadByteCount();

		// DDR3 Beam On Base Address Register
		void SetDDRBeamOnBaseAddress(const uint32_t& address);
		uint32_t ReadDDRBeamOnBaseAddress();
		CFO_RegisterFormatter FormatDDRBeamOnBaseAddress();

		// DDR3 Beam Off Base Address Register
		void SetDDRBeamOffBaseAddress(const uint32_t& address);
		uint32_t ReadDDRBeamOffBaseAddress();
		CFO_RegisterFormatter FormatDDRBeamOffBaseAddress();

		// Firefly CSR Register
		bool ReadFireflyTXRXPresent();
		bool ReadFireflyRXPresent();
		bool ReadFireflyTXPresent();
		bool ReadFireflyTXRXInterrupt();
		bool ReadFireflyRXInterrupt();
		bool ReadFireflyTXInterrupt();
		void SetFireflyTXRXSelect(bool select);
		void SetFireflyRXSelect(bool select);
		void SetFireflyTXSelect(bool select);
		bool ReadFireflyTXRXSelect();
		bool ReadFireflyRXSelect();
		bool ReadFireflyTXSelect();
		void SetFireflyTXRXReset(bool reset);
		void SetFireflyRXReset(bool reset);
		void SetFireflyTXReset(bool reset);
		bool ReadFireflyTXRXReset();
		bool ReadFireflyRXReset();
		bool ReadFireflyTXReset();
		CFO_RegisterFormatter FormatFireflyCSR();

		// SERDES PRBS Control Registers
		bool ReadSERDESPRBSErrorFlag(const CFO_Link_ID& link);
		uint8_t ReadSERDESTXPRBSSEL(const CFO_Link_ID& link);
		void SetSERDESTXPRBSSEL(const CFO_Link_ID& link, uint8_t byte);
		uint8_t ReadSERDESRXPRBSSEL(const CFO_Link_ID& link);
		void SetSERDESRXPRBSSEL(const CFO_Link_ID& link, uint8_t byte);
		bool ReadSERDESTXPRBSForceError(const CFO_Link_ID& link);
		void SetSERDESTXPRBSForceError(const CFO_Link_ID& link, bool flag);
		void ToggleSERDESTXPRBSForceError(const CFO_Link_ID& link);
		bool ReadSERDESRXPRBSCountReset(const CFO_Link_ID& link);
		void SetSERDESRXPRBSCountReset(const CFO_Link_ID& link, bool flag);
		void ToggleSERDESRXPRBSCountReset(const CFO_Link_ID& link);
		CFO_RegisterFormatter FormatSERDESPRBSControlLink0();
		CFO_RegisterFormatter FormatSERDESPRBSControlLink1();
		CFO_RegisterFormatter FormatSERDESPRBSControlLink2();
		CFO_RegisterFormatter FormatSERDESPRBSControlLink3();
		CFO_RegisterFormatter FormatSERDESPRBSControlLink4();
		CFO_RegisterFormatter FormatSERDESPRBSControlLink5();
		CFO_RegisterFormatter FormatSERDESPRBSControlLink6();
		CFO_RegisterFormatter FormatSERDESPRBSControlLink7();

		// Cable Delays
		void SetCableDelayValue(const CFO_Link_ID& link, const uint32_t delay);
		uint32_t ReadCableDelayValue(const CFO_Link_ID& link);
		CFO_RegisterFormatter FormatCableDelayValueLink0();
		CFO_RegisterFormatter FormatCableDelayValueLink1();
		CFO_RegisterFormatter FormatCableDelayValueLink2();
		CFO_RegisterFormatter FormatCableDelayValueLink3();
		CFO_RegisterFormatter FormatCableDelayValueLink4();
		CFO_RegisterFormatter FormatCableDelayValueLink5();
		CFO_RegisterFormatter FormatCableDelayValueLink6();
		CFO_RegisterFormatter FormatCableDelayValueLink7();

		// Cable Delay Control And Status Register
		bool ReadDelayMeasureError(const CFO_Link_ID& link);
		bool ReadDelayExternalLoopbackEnable();
		void SetDelayExternalLoopbackEnable(bool value);
		void EnableDelayMeasureMode(const CFO_Link_ID& link);
		void DisableDelayMeasureMode(const CFO_Link_ID& link);
		bool ReadDelayMeasureMode(const CFO_Link_ID& link);
		void EnableDelayMeasureNow(const CFO_Link_ID& link);
		void DisableDelayMeasureNow(const CFO_Link_ID& link);
		bool ReadDelayMeasureNow(const CFO_Link_ID& link);
		CFO_RegisterFormatter FormatCableDelayControl();

		// FPGA PROM Program Data Register

		// FPGA PROM Program Status Register
		/// <summary>
		/// Read the full bit on the FPGA PROM FIFO
		/// </summary>
		/// <returns>the full bit on the FPGA PROM FIFO</returns>
		bool ReadFPGAPROMProgramFIFOFull();
		/// <summary>
		/// Read whether the FPGA PROM is ready for data
		/// </summary>
		/// <returns>whether the FPGA PROM is ready for data</returns>
		bool ReadFPGAPROMReady();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatFPGAPROMProgramStatus();

		// FPGA Core Access Register
		/// <summary>
		/// Performs the chants necessary to reload the CFO firmware
		/// </summary>
		void ReloadFPGAFirmware();
		/// <summary>
		/// Read the FPGA Core Access FIFO Full bit
		/// </summary>
		/// <returns>Whether the FPGA Core Access FIFO is full</returns>
		bool ReadFPGACoreAccessFIFOFull();
		/// <summary>
		/// Read the FPGA Core Access FIFO Empty bit
		/// </summary>
		/// <returns>Whether the FPGA Core Access FIFO is empty</returns>
		bool ReadFPGACoreAccessFIFOEmpty();
		/// <summary>
		/// Formats the register's current value for register dumps
		/// </summary>
		/// <returns>CFO_RegisterFormatter object containing register information</returns>
		CFO_RegisterFormatter FormatFPGACoreAccess();

		// Oscillator Programming (DDR and SERDES)
		/// <summary>
		/// Set the given oscillator to the given frequency, calculating a new program in the process.
		/// </summary>
		/// <param name="targetFrequency">New frequency to program, in Hz</param>
		void SetNewOscillatorFrequency(double targetFrequency);

	private:
		void WriteRegister_(uint32_t data, const CFO_Register& address);
		uint32_t ReadRegister_(const CFO_Register& address);

		static int DecodeHighSpeedDivider_(int input);
		static int DecodeOutputDivider_(int input) { return input + 1; }
		static double DecodeRFREQ_(uint64_t input) { return input / 268435456.0; }
		static int EncodeHighSpeedDivider_(int input);
		static int EncodeOutputDivider_(int input);
		static uint64_t EncodeRFREQ_(double input) { return static_cast<uint64_t>(input * 268435456) & 0x3FFFFFFFFF; }
		static uint64_t CalculateFrequencyForProgramming_(double targetFrequency, double currentFrequency, uint64_t currentProgram);


	protected:
		cfodev device_; ///< Device handle
		CFO_SimMode simMode_; ///< Simulation mode
		uint32_t maxDTCs_; ///< Map of active DTCs
		bool usingDetectorEmulator_; ///< Whether Detector Emulation mode is enabled
		uint16_t dmaSize_; ///< Size of DMAs, in bytes (default 32k)
		int formatterWidth_; ///< Description field width, in characters

		/// <summary>
		/// Functions needed to print regular register map
		/// </summary>
		const std::vector<std::function<CFO_RegisterFormatter()>> formattedDumpFunctions_
		{
			[this]() { return this->FormatDesignVersion(); },
			[this]() { return this->FormatDesignDate(); },
			[this]() { return this->FormatDesignStatus(); },
			[this]() { return this->FormatVivadoVersion(); },
			[this]() { return this->FormatCFOControl(); },
			[this]() { return this->FormatDMATransferLength(); },
			[this]() { return this->FormatSERDESLoopbackEnable(); },
			[this]() { return this->FormatClockOscillatorStatus(); },
			[this]() { return this->FormatLinkEnable(); },
			[this]() { return this->FormatSERDESReset(); },
			[this]() { return this->FormatSERDESRXDisparityError(); },
			[this]() { return this->FormatSERDESRXCharacterNotInTableError(); },
			[this]() { return this->FormatSERDESUnlockError(); },
			[this]() { return this->FormatSERDESPLLLocked(); },
			[this]() { return this->FormatSERDESRXStatus(); },
			[this]() { return this->FormatSERDESResetDone(); },
			[this]() { return this->FormatSERDESRXCDRLock(); },
			[this]() { return this->FormatBeamOnTimerPreset(); },
			[this]() { return this->FormatBeamOnMode(); },
			[this]() { return this->FormatBeamOffMode(); },
			[this]() { return this->FormatClockMarkerIntervalCount(); },
			[this]() { return this->FormatSERDESOscillatorFrequency(); },
			[this]() { return this->FormatSERDESOscillatorControl(); },
			[this]() { return this->FormatSERDESOscillatorParameterLow(); },
			[this]() { return this->FormatSERDESOscillatorParameterHigh(); },
			[this]() { return this->FormatTimestampPreset0(); },
			[this]() { return this->FormatTimestampPreset1(); },
			[this]() { return this->FormatNUMDTCs(); },
			[this]() { return this->FormatFIFOFullErrorFlag0(); },
			[this]() { return this->FormatReceivePacketError(); },
			[this]() { return this->FormatEventWindowEmulatorIntervalTime(); },
			[this]() { return this->FormatEventWindowHoldoffTime(); },
			[this]() { return this->FormatEventWindowTimeoutError(); },
			[this]() { return this->FormatEventWindowTimeoutInterval(); },
			[this]() { return this->FormatDMAWriteStartAddress(); },
			[this]() { return this->FormatDMAReadStartAddress(); },
			[this]() { return this->FormatDMAReadByteCount(); },
			[this]() { return this->FormatDDRBeamOnBaseAddress(); },
			[this]() { return this->FormatDDRBeamOffBaseAddress(); },
			[this]() { return this->FormatFireflyCSR(); },
			[this]() { return this->FormatSERDESPRBSControlLink0(); },
			[this]() { return this->FormatSERDESPRBSControlLink1(); },
			[this]() { return this->FormatSERDESPRBSControlLink2(); },
			[this]() { return this->FormatSERDESPRBSControlLink3(); },
			[this]() { return this->FormatSERDESPRBSControlLink4(); },
			[this]() { return this->FormatSERDESPRBSControlLink5(); },
			[this]() { return this->FormatSERDESPRBSControlLink6(); },
			[this]() { return this->FormatSERDESPRBSControlLink7(); },
			[this]() { return this->FormatCableDelayValueLink0(); },
			[this]() { return this->FormatCableDelayValueLink1(); },
			[this]() { return this->FormatCableDelayValueLink2(); },
			[this]() { return this->FormatCableDelayValueLink3(); },
			[this]() { return this->FormatCableDelayValueLink4(); },
			[this]() { return this->FormatCableDelayValueLink5(); },
			[this]() { return this->FormatCableDelayValueLink6(); },
			[this]() { return this->FormatCableDelayValueLink7(); },
			[this]() { return this->FormatCableDelayControl(); },
			[this]() { return this->FormatFPGAPROMProgramStatus(); },
			[this]() { return this->FormatFPGACoreAccess(); }
		};


		/// <summary>
		/// Dump Byte/Packet Counter Registers
		/// </summary>
		const std::vector<std::function<CFO_RegisterFormatter()>> formattedCounterFunctions_
		{
			[this]() { return this->FormatReceiveByteCountLink0(); },
			[this]() { return this->FormatReceiveByteCountLink1(); },
			[this]() { return this->FormatReceiveByteCountLink2(); },
			[this]() { return this->FormatReceiveByteCountLink3(); },
			[this]() { return this->FormatReceiveByteCountLink4(); },
			[this]() { return this->FormatReceiveByteCountLink5(); },
			[this]() { return this->FormatReceiveByteCountLink6(); },
			[this]() { return this->FormatReceiveByteCountLink7(); },
			[this]() { return this->FormatReceivePacketCountLink0(); },
			[this]() { return this->FormatReceivePacketCountLink1(); },
			[this]() { return this->FormatReceivePacketCountLink2(); },
			[this]() { return this->FormatReceivePacketCountLink3(); },
			[this]() { return this->FormatReceivePacketCountLink4(); },
			[this]() { return this->FormatReceivePacketCountLink5(); },
			[this]() { return this->FormatReceivePacketCountLink6(); },
			[this]() { return this->FormatReceivePacketCountLink7(); },
			[this]() { return this->FormatTramsitByteCountLink0(); },
			[this]() { return this->FormatTramsitByteCountLink1(); },
			[this]() { return this->FormatTramsitByteCountLink2(); },
			[this]() { return this->FormatTramsitByteCountLink3(); },
			[this]() { return this->FormatTramsitByteCountLink4(); },
			[this]() { return this->FormatTramsitByteCountLink5(); },
			[this]() { return this->FormatTramsitByteCountLink6(); },
			[this]() { return this->FormatTramsitByteCountLink7(); },
			[this]() { return this->FormatTransmitPacketCountLink0(); },
			[this]() { return this->FormatTransmitPacketCountLink1(); },
			[this]() { return this->FormatTransmitPacketCountLink2(); },
			[this]() { return this->FormatTransmitPacketCountLink3(); },
			[this]() { return this->FormatTransmitPacketCountLink4(); },
			[this]() { return this->FormatTransmitPacketCountLink5(); },
			[this]() { return this->FormatTransmitPacketCountLink6(); },
			[this]() { return this->FormatTransmitPacketCountLink7(); }
		};
	};
}

#endif //CFO_REGISTERS_H
