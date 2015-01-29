// DTCDriver.js, v1.0
// Author: Eric Flumerfelt, FNAL/RSI
// Last Modified: January 27, 2015
// 
// This module for serverbase.js performs register I/O on the mu2e DTC board
// Version History
// v1.0: Updated to use new libDTCInterface interface
// v0.4: Working version

var dtc = require('./DTC');
var gmetric = require('./gmetric');
var fs = require('fs');
var emitter = require('events').EventEmitter;
var DTC = new dtc.DTC();
// So that we can send events back to serverbase
var dtcem = new emitter();
// Variables used to store persistent data
var date = new Date();
date.setTime(0);

var sTime = date,
    rTime = date,
    spTime = date,
    rpTime = date,
    rdTime = date,
    ssTime = date;
var send = 0,
    receive = 0,
    sendP = 0,
    receiveP = 0;
var regDump = {};
var systemStatus = {};

//
// Local Functions
// Cannot be called directly by outside code
//
function logMessage(message, method, name) {
    message = "\n" + new Date().toLocaleString() + " " + name + " " + method + " " + message;
    console.log(message);
    fs.appendFileSync("./DTC.log", message);
    console.log("Done logging message");
}

function SendStatistics() {
    dtcem.GET_Receive();
    dtcem.GET_Send();
    gmetric.send_gmetric("/etc/ganglia/gmond.conf", "PCIe Send Rate", send.toString(), "double", "B/s", "both", 15, 0, "DTC_PCIe", "mu2e DAQ", "PCIe Send Rate", "PCIe Send Rate");
    gmetric.send_gmetric("/etc/ganglia/gmond.conf", "PCIe Receive Rate", receive.toString(), "double", "B/s", "both", 15, 0, "DTC_PCIe", "mu2e DAQ", "PCIe Receive Rate", "PCIe Receive Rate");
};

function getRegDump() {
    rdTime = new Date();
    var dtcRegisters = {};
    dtcRegisters.Ring0 = {};
    dtcRegisters.Ring1 = {};
    dtcRegisters.Ring2 = {};
    dtcRegisters.Ring3 = {};
    dtcRegisters.Ring4 = {};
    dtcRegisters.Ring5 = {};
    dtcRegisters.Version = dtcem.RO_readDesignVersion();
    dtcRegisters.ResetDTC = dtcem.RO_readResetDTC();
    dtcRegisters.ClearLatchedErrors = dtcem.RO_readClearLatchedErrors();
    dtcRegisters.Ring0.SERDESLoopback = dtcem.RO_readSERDESLoopback({ ring: 0 });
    dtcRegisters.Ring1.SERDESLoopback = dtcem.RO_readSERDESLoopback({ ring: 1 });
    dtcRegisters.Ring2.SERDESLoopback = dtcem.RO_readSERDESLoopback({ ring: 2 });
    dtcRegisters.Ring3.SERDESLoopback = dtcem.RO_readSERDESLoopback({ ring: 3 });
    dtcRegisters.Ring4.SERDESLoopback = dtcem.RO_readSERDESLoopback({ ring: 4 });
    dtcRegisters.Ring5.SERDESLoopback = dtcem.RO_readSERDESLoopback({ ring: 5 });
    dtcRegisters.ROCEmulator = dtcem.RO_readROCEmulator();
    dtcRegisters.Ring0.Enabled = dtcem.RO_readRingEnabled({ ring: 0 });
    dtcRegisters.Ring1.Enabled = dtcem.RO_readRingEnabled({ ring: 1 });
    dtcRegisters.Ring2.Enabled = dtcem.RO_readRingEnabled({ ring: 2 });
    dtcRegisters.Ring3.Enabled = dtcem.RO_readRingEnabled({ ring: 3 });
    dtcRegisters.Ring4.Enabled = dtcem.RO_readRingEnabled({ ring: 4 });
    dtcRegisters.Ring5.Enabled = dtcem.RO_readRingEnabled({ ring: 5 });
    dtcRegisters.Ring0.ResetSERDES = dtcem.RO_readResetSERDES({ ring: 0 });
    dtcRegisters.Ring1.ResetSERDES = dtcem.RO_readResetSERDES({ ring: 1 });
    dtcRegisters.Ring2.ResetSERDES = dtcem.RO_readResetSERDES({ ring: 2 });
    dtcRegisters.Ring3.ResetSERDES = dtcem.RO_readResetSERDES({ ring: 3 });
    dtcRegisters.Ring4.ResetSERDES = dtcem.RO_readResetSERDES({ ring: 4 });
    dtcRegisters.Ring5.ResetSERDES = dtcem.RO_readResetSERDES({ ring: 5 });
    dtcRegisters.Ring0.SERDESRXDisparity = dtcem.RO_readSERDESRXDisparity({ ring: 0 });
    dtcRegisters.Ring1.SERDESRXDisparity = dtcem.RO_readSERDESRXDisparity({ ring: 1 });
    dtcRegisters.Ring2.SERDESRXDisparity = dtcem.RO_readSERDESRXDisparity({ ring: 2 });
    dtcRegisters.Ring3.SERDESRXDisparity = dtcem.RO_readSERDESRXDisparity({ ring: 3 });
    dtcRegisters.Ring4.SERDESRXDisparity = dtcem.RO_readSERDESRXDisparity({ ring: 4 });
    dtcRegisters.Ring5.SERDESRXDisparity = dtcem.RO_readSERDESRXDisparity({ ring: 5 });
    dtcRegisters.Ring0.CharacterError = dtcem.RO_readSERDESRXCharacterError({ ring: 0 });
    dtcRegisters.Ring1.CharacterError = dtcem.RO_readSERDESRXCharacterError({ ring: 1 });
    dtcRegisters.Ring2.CharacterError = dtcem.RO_readSERDESRXCharacterError({ ring: 2 });
    dtcRegisters.Ring3.CharacterError = dtcem.RO_readSERDESRXCharacterError({ ring: 3 });
    dtcRegisters.Ring4.CharacterError = dtcem.RO_readSERDESRXCharacterError({ ring: 4 });
    dtcRegisters.Ring5.CharacterError = dtcem.RO_readSERDESRXCharacterError({ ring: 5 });
    dtcRegisters.Ring0.UnlockError = dtcem.RO_readSERDESUnlockError({ ring: 0 });
    dtcRegisters.Ring1.UnlockError = dtcem.RO_readSERDESUnlockError({ ring: 1 });
    dtcRegisters.Ring2.UnlockError = dtcem.RO_readSERDESUnlockError({ ring: 2 });
    dtcRegisters.Ring3.UnlockError = dtcem.RO_readSERDESUnlockError({ ring: 3 });
    dtcRegisters.Ring4.UnlockError = dtcem.RO_readSERDESUnlockError({ ring: 4 });
    dtcRegisters.Ring5.UnlockError = dtcem.RO_readSERDESUnlockError({ ring: 5 });
    dtcRegisters.Ring0.PLLLocked = dtcem.RO_readSERDESPLLLocked({ ring: 0 });
    dtcRegisters.Ring1.PLLLocked = dtcem.RO_readSERDESPLLLocked({ ring: 1 });
    dtcRegisters.Ring2.PLLLocked = dtcem.RO_readSERDESPLLLocked({ ring: 2 });
    dtcRegisters.Ring3.PLLLocked = dtcem.RO_readSERDESPLLLocked({ ring: 3 });
    dtcRegisters.Ring4.PLLLocked = dtcem.RO_readSERDESPLLLocked({ ring: 4 });
    dtcRegisters.Ring5.PLLLocked = dtcem.RO_readSERDESPLLLocked({ ring: 5 });
    dtcRegisters.Ring0.OverflowOrUnderflow = dtcem.RO_readSERDESOverflowOrUnderflow({ ring: 0 });
    dtcRegisters.Ring1.OverflowOrUnderflow = dtcem.RO_readSERDESOverflowOrUnderflow({ ring: 1 });
    dtcRegisters.Ring2.OverflowOrUnderflow = dtcem.RO_readSERDESOverflowOrUnderflow({ ring: 2 });
    dtcRegisters.Ring3.OverflowOrUnderflow = dtcem.RO_readSERDESOverflowOrUnderflow({ ring: 3 });
    dtcRegisters.Ring4.OverflowOrUnderflow = dtcem.RO_readSERDESOverflowOrUnderflow({ ring: 4 });
    dtcRegisters.Ring5.OverflowOrUnderflow = dtcem.RO_readSERDESOverflowOrUnderflow({ ring: 5 });
    dtcRegisters.Ring0.FIFOHalfFull = dtcem.RO_readSERDESBufferFIFOHalfFull({ ring: 0 });
    dtcRegisters.Ring1.FIFOHalfFull = dtcem.RO_readSERDESBufferFIFOHalfFull({ ring: 1 });
    dtcRegisters.Ring2.FIFOHalfFull = dtcem.RO_readSERDESBufferFIFOHalfFull({ ring: 2 });
    dtcRegisters.Ring3.FIFOHalfFull = dtcem.RO_readSERDESBufferFIFOHalfFull({ ring: 3 });
    dtcRegisters.Ring4.FIFOHalfFull = dtcem.RO_readSERDESBufferFIFOHalfFull({ ring: 4 });
    dtcRegisters.Ring5.FIFOHalfFull = dtcem.RO_readSERDESBufferFIFOHalfFull({ ring: 5 });
    dtcRegisters.Ring0.RXBufferStatus = dtcem.RO_readSERDESRXBufferStatus({ ring: 0 });
    dtcRegisters.Ring1.RXBufferStatus = dtcem.RO_readSERDESRXBufferStatus({ ring: 1 });
    dtcRegisters.Ring2.RXBufferStatus = dtcem.RO_readSERDESRXBufferStatus({ ring: 2 });
    dtcRegisters.Ring3.RXBufferStatus = dtcem.RO_readSERDESRXBufferStatus({ ring: 3 });
    dtcRegisters.Ring4.RXBufferStatus = dtcem.RO_readSERDESRXBufferStatus({ ring: 4 });
    dtcRegisters.Ring5.RXBufferStatus = dtcem.RO_readSERDESRXBufferStatus({ ring: 5 });
    dtcRegisters.Ring0.ResetDone = dtcem.RO_readSERDESResetDone({ ring: 0 });
    dtcRegisters.Ring1.ResetDone = dtcem.RO_readSERDESResetDone({ ring: 1 });
    dtcRegisters.Ring2.ResetDone = dtcem.RO_readSERDESResetDone({ ring: 2 });
    dtcRegisters.Ring3.ResetDone = dtcem.RO_readSERDESResetDone({ ring: 3 });
    dtcRegisters.Ring4.ResetDone = dtcem.RO_readSERDESResetDone({ ring: 4 });
    dtcRegisters.Ring5.ResetDone = dtcem.RO_readSERDESResetDone({ ring: 5 });
    dtcRegisters.Timestamp = dtcem.RO_readTimestampPreset();
    dtcRegisters.PROMFIFOFull = dtcem.RO_readFPGAPROMProgramFIFOFull();
    dtcRegisters.PROMReady = dtcem.RO_readFPGAPROMReady();
    regDump = dtcRegisters;
}

function startTest(dma, packetSize, loopback, txChecker, rxGenerator) {
    var test = DTC.StartTest(dma, packetSize, loopback, txChecker, rxGenerator);
    return test.GetState();
};

function stopTest(dma) {
    var test = DTC.StopTest(dma);
    return test.GetState();
};

function getDMAStats(dma, dir) {
    var stats = DTC.ReadDMAStats(dma, dir);
    var output = {};
    //console.log(stats.at(0));
    output.Throughput = stats.at(0).LBR;
    output.DMAActive = stats.at(0).LAT;
    output.DMAWait = stats.at(0).LWT;
    return output;
}

function getDMAState(dma, dir) {
    var state = DTC.ReadDMAState(dma, dir);
    var output = {};
    //console.log(state);
    output.BDErrors = state.BDerrs;
    output.BDSErrors = state.BDSerrs;
    output.SWBDs = state.BDs;
    output.SWBuffs = state.Buffers;
    output.Interrupts = state.IntEnab;
    output.stats = getDMAStats(dma, dir);
    return output;
}

function getPCIeStats() {
    var stats = DTC.ReadPCIeStats();
    var output = {};
    output.WritesRate = stats.LTX;
    output.ReadsRate = stats.LRX;
    return output;
}

function getPCIeState() {
    var stats = DTC.ReadPCIeState();
    var output = {};
    switch (stats.LinkState) {
        case 0:
            output.LinkStatus = "Down";
            break;
        case 1:
            output.LinkStatus = "Up";
            break;
    }
    output.LinkWidth = stats.LinkWidth + "x";
    switch (stats.LinkSpeed) {
        case 1:
            output.LinkSpeed = "2.5";
            break;
        case 2:
            output.LinkSpeed = "5";
            break;
    }
    output.VendorID = stats.VendorId;
    output.DeviceID = stats.DeviceId;
    output.MPS = stats.MPS;
    output.MRRS = stats.MRRS;
    switch (stats.IntMode) {
        case 0:
            output.Interrupts = "None"
            break;
        case 1:
            output.Interrupts = "Legacy";
            break;
        case 2:
            output.Interrupts = "MSI";
            break;
        case 3:
            output.Interrupts = "MSI-X"
            break;

    }
    output.credits = {};
    output.credits.ph = stats.InitFCPH;
    output.credits.pd = stats.InitFCPD;
    output.credits.nph = stats.InitFCNPH;
    output.credits.npd = stats.InitFCNPD;
    output.credits.ch = stats.InitFCCplH;
    output.credits.cd = stats.InitFCCplD;
    output.stats = getPCIeStats();
    return output;
}

function getSystemStatus() {
    var status = {};
    status.path0 = {};
    status.path1 = {};
    status.path0.TX = getDMAState(dtc.DTC_DMA_Engine_DAQ, dtc.DTC_DMA_Direction_C2S);
    status.path0.RX = getDMAState(dtc.DTC_DMA_Engine_DAQ, dtc.DTC_DMA_Direction_S2C);
    status.path1.TX = getDMAState(dtc.DTC_DMA_Engine_DCS, dtc.DTC_DMA_Direction_C2S);
    status.path1.RX = getDMAState(dtc.DTC_DMA_Engine_DCS, dtc.DTC_DMA_Direction_S2C);
    status.pcie = getPCIeState();
    
    systemStatus = status;
};

function getDMAReadRate(channel) {
    dtcem.RO_SystemStatus();
    var dmastats = systemStatus.path0.RX.stats;
    if (channel === 1) {
        dmastats = systemStatus.path1.RX.stats;
    }
    return dmastats.Throughput;
};

function getDMAWriteRate(channel) {
    dtcem.RO_SystemStatus();
    var dmastats = systemStatus.path0.TX.stats;
    if (channel === 1) {
        dmastats = systemStatus.path1.TX.stats;
    }
    return dmastats.Throughput;
};

function read(address) {
    var addr = parseInt(address, 16);
    //console.log("Reading " + address);
    return DTC.RegisterRead(addr);
};

/* Register Writes have been removed as of v1.0, sorry
function write(address, value) {
    var addr = parseInt(address, 16);
    var val = parseInt(value, 16);
    console.log("Writing " + val + " to " + addr);
    dtcem.Err = DTC.WriteRegister(val, addr);
    return read(address);
};
*/

//
// Master Init Function
// Runs once on the node.js cluster master
//
dtcem.MasterInitFunction = function () {
    //Only the master should log Ganglia metrics
    setInterval(SendStatistics, 15000);
    
    console.log("Setting up log file");
    if (!fs.existsSync("./DTC.log")) {
        console.log("Creating new log file");
        fs.writeFileSync("./DTC.log", "Log file created at " + new Date().toLocaleString());
    }
    console.log("Done setting up log");
}


//
// RO_ Functions
// Any authenticated user can run these
//
dtcem.RO_readDesignVersion = function () {
    return DTC.ReadDesignVersion();
}

dtcem.RO_readResetDTC = function () {
    return DTC.ReadResetDTC();
};

dtcem.RO_readClearLatchedErrors = function () {
    return DTC.ReadClearLatchedErrors();
};

dtcem.RO_readSERDESLoopback = function (POST) {
    return DTC.ReadSERDESLoopback(parseInt(POST.ring));
};

dtcem.RO_readROCEmulator = function () {
    return DTC.ReadROCEmulator();
}

dtcem.RO_readRingEnabled = function (POST) {
    return DTC.ReadRingEnabled(parseInt(POST.ring));
}

dtcem.RO_readResetSERDES = function (POST) {
    return DTC.ReadResetSERDES(parseInt(POST.ring));
}

dtcem.RO_readSERDESResetDone = function (POST) {
    return DTC.ReadResetSERDESDone(parseInt(POST.ring));
}

dtcem.RO_readSERDESRXDisparity = function (POST) {
    switch (DTC.ReadSERDESRXDisparityError(parseInt(POST.ring)).GetData(true)) {
        case 0:
            return { low: 0, high: 0 };
        case 1:
            return { low: 1, high: 0 };
        case 2:
            return { low: 0, high: 1 };
        case 3:
            return { low: 1, high: 1 };
    }
}

dtcem.RO_readSERDESRXCharacterError = function (POST) {
    switch (DTC.ReadSERDESRXCharacterNotInTableError(parseInt(POST.ring)).GetData(true)) {
        case 0:
            return { low: 0, high: 0 };
        case 1:
            return { low: 1, high: 0 };
        case 2:
            return { low: 0, high: 1 };
        case 3:
            return { low: 1, high: 1 };
    }
}

dtcem.RO_readSERDESUnlockError = function (POST) {
    return DTC.ReadSERDESUnlockError(parseInt(POST.ring));
}

dtcem.RO_readSERDESPLLLocked = function (POST) {
    return DTC.ReadSERDESPLLLocked(parseInt(POST.ring));
}

dtcem.RO_readSERDESOverflowOrUnderflow = function (POST) {
    return DTC.ReadSERDESOverflowOrUnderflow(parseInt(POST.ring));
}

dtcem.RO_readSERDESBufferFIFOHalfFull = function (POST) {
    return DTC.ReadSERDESBufferFIFOHalfFull(parseInt(POST.ring));
}

dtcem.RO_readSERDESRXBufferStatus = function (POST) {
    var output = { Nominal: 0, Empty: 0, Full: 0, Underflow: 0, Overflow: 0 };
    switch (DTC.ReadSERDESRXBufferStatus(parseInt(POST.ring)).GetStatus()) {
        case 0:
            output.Nominal = 1;
            break;
        case 1:
            output.Empty = 1;
            break;
        case 2:
            output.Full = 1;
            break;
        case 5:
            output.Underflow = 1;
            break;
        case 6:
            output.Overflow = 1;
            break;
    }
    return output;
}

dtcem.RO_readTimestampPreset = function () {
    return DTC.ReadTimestampPreset().GetTimestamp(true);
}

dtcem.RO_readFPGAPROMProgramFIFOFull = function () {
    return DTC.ReadFPGAPROMProgramFIFOFull();
}

dtcem.RO_readFPGAPROMReady = function () {
    return DTC.ReadFPGAPROMReady();
}

dtcem.RO_regDump = function () {
    if (rdTime.getTime() + 1000 < new Date().getTime()) {
        console.log("Getting new regDump");
        getRegDump();
    }
    
    return regDump;
};

dtcem.RO_SystemStatus = function () {
    if (ssTime.getTime() + 1000 < new Date().getTime()) {
        console.log("Getting new SystemStatus");
        getSystemStatus();
        ssTime = new Date();
    }
    
    return systemStatus;
}

dtcem.RO_ReadLog = function () {
    console.log("Reading DTC log file");
    var logContent = "" + fs.readFileSync("./DTC.log");
    return logContent;
};

dtcem.RO_RegIO = function (POST) {
    console.log("In RO Register IO handler");
    if (POST.option === "write") {
        throw new TypeError("Unauthorized Access Attempt!");
    }
    
    return read(POST.address).toString(16);
}


//
// RW_ Functions
// Only authorized users can run these!
//
dtcem.RW_resetDTC = function (POST) {
    DTC.ResetDTC();
    logMessage("the DTC", "reset", POST.who);
    return dtcem.RO_readResetDTC();
};

dtcem.RW_toggleClearLatchedErrors = function (POST) {
    var val = DTC.ToggleClearLatchedErrors();
    logMessage("ClearLatchedErrors (" + val + ")", "toggled", POST.who);
    return val;
};

dtcem.RW_toggleSERDESLoopback = function (POST) {
    var val = DTC.ToggleSERDESLoopback(POST.ring);
    logMessage("SERDES Loopback on ring " + POST.ring + " (" + val + ")", "toggled", POST.who);
    return val;
};

dtcem.RW_toggleROCEmulator = function () {
    var val = DTC.ToggleROCEmulator();
    logMessage("ROC Emulator (" + val + ")", "toggled", POST.who);
    return val;
}

dtcem.RW_toggleRingEnabled = function (POST) {
    var val = DTC.ToggleRingEnabled(POST.ring);
    logMessage("Ring Enabled on ring " + POST.ring + " (" + val + ")", "toggled", POST.who);
    return val;
}

dtcem.RW_resetSERDES = function (POST) {
    logMessage("the SERDES on ring " + POST.ring, "is resetting", POST.who);
    return DTC.ResetSERDES(parseInt(POST.ring));
}

dtcem.RW_setTimestampPreset = function (POST) {
    var ts = DTC.WriteTimestampPreset(new dtc.DTC_Timestamp(parseInt(parseInt(POST.ring), 16)));
    logMessage("the timestamp preset (" + ts.GetTimestamp(true) + ")", "set", POST.who);
    return ts.GetTimestamp(true);
}

dtcem.RW_WriteLog = function (POST) {
    logMessage(parseInt(POST.ring), "says", POST.who);
    res.end(readLog());
    console.log("Done sending log message reply");
}

/* Script handler removed as of v1.0, sorry...
dtcem.RW_RunScript = function (POST) {
    console.log("In Script handler");
    
    var success = true;
    var status = "Success!";
    var text = parseInt(POST.ring);
    var lines = text.split("\n");
    for (var i = 0; i < lines.length; i++) {
        var thisLine = lines[i];
        var thisLineSplit = thisLine.split(" ");
        var address = thisLineSplit[0];
        var val = thisLineSplit[1];
        logMessage("a write: " + val + " to " + address, "scripted", POST.who);
        var value = write(address, val);
        if (value !== val) { success = false; }
    }
    
    if (success) {
        logMessage("the script was run successfully.", "noticed that", POST.who);
    }
    else {
        logMessage("the script had an error!", "noticed that", POST.who);
        status = "Script did not run successfully!!!";
        console.log("DTC Error Status: " + dtcem.Err);
    }
    
    return status;
}
*/

dtcem.RW_TestControl = function (POST) {
    console.log("Starting or Stopping a DMA test");
    var dma = POST.dma;
    var started = POST.started;
    var packetSize = POST.size;
    var enableLoopback = POST.loopbackEnabled;
    var enableTXChecker = POST.txChecker;
    var enableRXGenerator = POST.rxGenerator;
    if (started) {
        logMessage("a DMA test on channel " + dma, "started", POST.who);
        started = startTest(dma, packetSize, enableLoopback, enableTXChecker, enableRXGenerator);
    } else {
        logMessage("a DMA test on channel " + dma, "stopped", POST.who);
        started = stopTest(dma);
    }
    
    return started;
}

/* Register writing removed as of v1.0, sorry
dtcem.RW_RegIO = function (POST) {
    console.log("In RO Register IO handler");
    if (POST.option === "read") {
        return dtcem.RO_RegIO(POST);
    }
    
    logMessage(POST.value.toString(16) + " to " + POST.address.toString(16), "wrote", POST.who);
    value = write(POST.address, POST.value);
    
    console.log("Replying with value " + value.toString(16));
    return value.toString(16);
}
*/

//
// GET_ Functions
// Get data in json form for d3 graphs
// Format: { value:, time:, name: }
//
dtcem.GET_Send = function () {
    if (sTime.getTime() + 800 < new Date().getTime()) {
        send = read("0x900C") / 4;
        sTime = new Date();
    }
    
    return { name: "send", value: send, time: new Date() };
}

dtcem.GET_Receive = function () {
    if (rTime.getTime() + 800 < new Date().getTime()) {
        receive = read("0x9010") / 4;
        rTime = new Date();
    }
    
    return { name: "receive", value: receive, time: new Date() };
}

dtcem.GET_SPayload = function () {
    if (spTime.getTime() + 800 < new Date().getTime()) {
        sendP = read("0x9014") / 4;
        spTime = new Date();
    }
    
    return { name: "spayload", value: sendP, time: new Date() };
}

dtcem.GET_RPayload = function () {
    if (rpTime.getTime() + 800 < new Date().getTime()) {
        receiveP = read("0x9018") / 4;
        rpTime = new Date();
    }
    
    return { name: "rpayload", value: receiveP, time: new Date() };
}

dtcem.GET_PCIeTransmit = function () {
    dtcem.RO_SystemStatus();
    return { name: "pcieTX", value: systemStatus.pcie.stats.WritesRate, time: new Date() };
};

dtcem.GET_PCIeReceive = function () {
    dtcem.RO_SystemStatus();
    return { name: "pcieRX", value: systemStatus.pcie.stats.ReadsRate, time: new Date() };
};

dtcem.GET_DMA0Transmit = function () {
    return { name: "dma0TX", value: getDMAWriteRate(0), time: new Date() };
};

dtcem.GET_DMA0Receive = function () {
    return { name: "dma0RX", value: getDMAReadRate(0), time: new Date() };
};

dtcem.GET_DMA1Transmit = function () {
    return { name: "dma1TX", value: getDMAWriteRate(1), time: new Date() };
};

dtcem.GET_DMA1Receive = function () {
    return { name: "dma1RX", value: getDMAReadRate(1), time: new Date() };
};

//
// module.exports
// A function that puts the emitter into the given array
//
module.exports = function (module_holder) {
    module_holder["DTC"] = dtcem;
};