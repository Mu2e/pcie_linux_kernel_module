#include "DTC_Types.h"


DTC::DTC_Timestamp::DTC_Timestamp()
	: timestamp_(0) {}

DTC::DTC_Timestamp::DTC_Timestamp(uint64_t timestamp)
	: timestamp_(timestamp) {}

DTC::DTC_Timestamp::DTC_Timestamp(uint32_t timestampLow, uint16_t timestampHigh)
{
	SetTimestamp(timestampLow, timestampHigh);
}

DTC::DTC_Timestamp::DTC_Timestamp(uint8_t *timeArr)
{
	timestamp_ = 0;
	for (int i = 0; i < 6; ++i)
	{
		uint64_t temp = (uint64_t)timeArr[i] << i * 8;
		timestamp_ += temp;
	}
}

DTC::DTC_Timestamp::DTC_Timestamp(std::bitset<48> timestamp)
	: timestamp_(timestamp.to_ullong()) {}

void DTC::DTC_Timestamp::GetTimestamp(uint8_t *arr)
{
	for (int i = 0; i < 6; i++)
	{
		arr[i] = static_cast<uint8_t>(timestamp_ >> i * 8);
	}
}

void DTC::DTC_Timestamp::SetTimestamp(uint32_t timestampLow, uint16_t timestampHigh)
{
	timestamp_ = timestampLow + timestampHigh * 0x10000;
}


DTC::DTC_DataPacket::DTC_DataPacket(uint8_t* data)
{
	for (int i = 0; i < 16; ++i)
	{
		dataWords_[i] = data[i];
	}
}

void DTC::DTC_DataPacket::SetWord(int index, uint8_t data)
{
	dataWords_[index] = data;
}

uint8_t DTC::DTC_DataPacket::GetWord(int index)
{
	return dataWords_[index];
}


DTC::DTC_DMAPacket::DTC_DMAPacket(DTC_PacketType type, DTC_Ring_ID ring, DTC_ROC_ID roc, uint8_t packetCount)
	: ringID_(ring), rocID_(roc), packetType_(type), packetCount_(packetCount) {}

DTC::DTC_DMAPacket::DTC_DMAPacket(DTC_PacketType type, DTC_Ring_ID ring, DTC_ROC_ID roc, uint8_t* data, uint8_t packetCount)
	: ringID_(ring), rocID_(roc), packetType_(type), packetCount_(packetCount)
{
	for (int i = 0; i < 12; ++i)
	{
		data_[i] = data[i];
	}
}

DTC::DTC_DataPacket DTC::DTC_DMAPacket::ConvertToDataPacket()
{
	DTC_DataPacket output;
	uint8_t word1A = (uint8_t)rocID_;
	word1A += (uint8_t)packetType_ << 4;
	uint8_t word1B = static_cast<uint8_t>(ringID_);
	output.SetWord(0, word1A);
	output.SetWord(1, word1B);
	output.SetWord(2, packetCount_);
	for (int i = 0; i < 12; ++i)
	{
		output.SetWord(i + 4, data_[i]);
	}
	return output;
}

DTC::DTC_DMAPacket::DTC_DMAPacket(DTC_DataPacket in)
{
	uint8_t word0 = in.GetWord(0);
	std::bitset<4> roc = word0;
	word0 >>= 4;
	std::bitset<4> packetType = word0;
	uint8_t word1 = in.GetWord(1);
	std::bitset<4> ringID = word1;
	packetCount_ = in.GetWord(2);
	for (int i = 0; i < 12; ++i)
	{
		data_[i] = in.GetWord(i + 4);
	}

	ringID_ = static_cast<DTC_Ring_ID>(ringID.to_ulong());
	rocID_ = static_cast<DTC_ROC_ID>(roc.to_ulong());
	packetType_ = static_cast<DTC_PacketType>(packetType.to_ulong());
}


DTC::DTC_DCSRequestPacket::DTC_DCSRequestPacket(DTC_Ring_ID ring, DTC_ROC_ID roc)
	: DTC_DMAPacket(DTC_PacketType_DCSRequest, ring, roc) {}

DTC::DTC_DCSRequestPacket::DTC_DCSRequestPacket(DTC_Ring_ID ring, DTC_ROC_ID roc, uint8_t* data)
	: DTC_DMAPacket(DTC_PacketType_DCSRequest, ring, roc, data) {}


DTC::DTC_ReadoutRequestPacket::DTC_ReadoutRequestPacket(DTC_Ring_ID ring, DTC_ROC_ID maxROC)
	: DTC_DMAPacket(DTC_PacketType_ReadoutRequest, ring, maxROC), timestamp_() {}

DTC::DTC_ReadoutRequestPacket::DTC_ReadoutRequestPacket(DTC_Ring_ID ring, DTC_Timestamp timestamp, DTC_ROC_ID maxROC)
	: DTC_DMAPacket(DTC_PacketType_ReadoutRequest, ring, maxROC), timestamp_(timestamp) {}

DTC::DTC_DMAPacket DTC::DTC_ReadoutRequestPacket::ConvertToDMAPacket()
{
	uint8_t data[12];
	timestamp_.GetTimestamp(data);
	return DTC_DMAPacket(DTC_PacketType_ReadoutRequest, ringID_, rocID_, data);
}

DTC::DTC_ReadoutRequestPacket::DTC_ReadoutRequestPacket(DTC_DMAPacket in) : DTC_DMAPacket(in), timestamp_(data_) {}

DTC::DTC_ReadoutRequestPacket::DTC_ReadoutRequestPacket(DTC_DataPacket in) : DTC_DMAPacket(in), timestamp_(data_){
}


DTC::DTC_DataRequestPacket::DTC_DataRequestPacket(DTC_Ring_ID ring, DTC_ROC_ID roc)
	: DTC_DMAPacket(DTC_PacketType_DataRequest, ring, roc), timestamp_() {}

DTC::DTC_DataRequestPacket::DTC_DataRequestPacket(DTC_Ring_ID ring, DTC_ROC_ID roc, DTC_Timestamp timestamp)
	: DTC_DMAPacket(DTC_PacketType_DataRequest, ring, roc), timestamp_(timestamp) {}

DTC::DTC_DataRequestPacket::DTC_DataRequestPacket(DTC_DataPacket in) : DTC_DMAPacket(in), timestamp_(data_) {}

DTC::DTC_DataRequestPacket::DTC_DataRequestPacket(DTC_DMAPacket in) : DTC_DMAPacket(in), timestamp_(data_) {}

DTC::DTC_DMAPacket DTC::DTC_DataRequestPacket::ConvertToDMAPacket()
{
	uint8_t data[12];
	timestamp_.GetTimestamp(data);
	return DTC_DMAPacket(DTC_PacketType_DataRequest, ringID_, rocID_, data);
}

DTC::DTC_DCSReplyPacket::DTC_DCSReplyPacket(DTC_Ring_ID ring)
	: DTC_DMAPacket(DTC_PacketType_DCSReply, ring, DTC_ROC_Unused) {}

DTC::DTC_DCSReplyPacket::DTC_DCSReplyPacket(DTC_Ring_ID ring, uint8_t* data)
	: DTC_DMAPacket(DTC_PacketType_DCSReply, ring, DTC_ROC_Unused, data) {}

DTC::DTC_DCSReplyPacket::DTC_DCSReplyPacket(DTC_DMAPacket&& right) : DTC_DMAPacket(right) {}

DTC::DTC_DataHeaderPacket::DTC_DataHeaderPacket(DTC_Ring_ID ring, uint8_t packetCount)
	: DTC_DMAPacket(DTC_PacketType_DataHeader, ring, DTC_ROC_Unused, packetCount) {}

DTC::DTC_DataHeaderPacket::DTC_DataHeaderPacket(DTC_Ring_ID ring, uint8_t packetCount, DTC_Timestamp timestamp)
	: DTC_DMAPacket(DTC_PacketType_DataHeader, ring, DTC_ROC_Unused, packetCount), timestamp_(timestamp) {}

DTC::DTC_DataHeaderPacket::DTC_DataHeaderPacket(DTC_Ring_ID ring, uint8_t packetCount, DTC_Timestamp timestamp, uint8_t* data)
	: DTC_DMAPacket(DTC_PacketType_DataHeader, ring, DTC_ROC_Unused, packetCount), timestamp_(timestamp)
{
	for (int i = 0; i < 6; ++i)
	{
		data_[i] = data[i];
	}
}

DTC::DTC_DataHeaderPacket::DTC_DataHeaderPacket(DTC_DataPacket in) : DTC_DMAPacket(in), timestamp_(data_)
{
	for (int i = 0; i < 6; i++)
	{
		dataStart_[i] = data_[i + 6];
	}
}

DTC::DTC_DataHeaderPacket::DTC_DataHeaderPacket(DTC_DMAPacket in) : DTC_DMAPacket(in), timestamp_(data_)
{
	for (int i = 0; i < 6; i++)
	{
		dataStart_[i] = data_[i + 6];
	}
}

DTC::DTC_DMAPacket DTC::DTC_DataHeaderPacket::ConvertToDMAPacket()
{
	uint8_t data[12];
	timestamp_.GetTimestamp(data);
	for (int ii = 0; ii < 6; ++ii)
	{
		data[ii + 6] = dataStart_[ii];
	}
	return DTC_DMAPacket(DTC_PacketType_DataHeader, ringID_, rocID_, data);
}

DTC::DTC_SERDESRXDisparityError::DTC_SERDESRXDisparityError() : data_(0) {}

DTC::DTC_SERDESRXDisparityError::DTC_SERDESRXDisparityError(std::bitset<2> data) : data_(data) {}

DTC::DTC_SERDESRXDisparityError::DTC_SERDESRXDisparityError(uint32_t data, DTC_Ring_ID ring)
{
	std::bitset<32> dataSet = data;
	uint32_t ringBase = (uint8_t)ring * 2;
	data_[0] = dataSet[ringBase];
	data_[1] = dataSet[ringBase + 1];
}


DTC::DTC_CharacterNotInTableError::DTC_CharacterNotInTableError() : data_(0) {}

DTC::DTC_CharacterNotInTableError::DTC_CharacterNotInTableError(std::bitset<2> data) : data_(data) {}

DTC::DTC_CharacterNotInTableError::DTC_CharacterNotInTableError(uint32_t data, DTC_Ring_ID ring)
{
	std::bitset<32> dataSet = data;
	uint32_t ringBase = (uint8_t)ring * 2;
	data_[0] = dataSet[ringBase];
	data_[1] = dataSet[ringBase + 1];
}


DTC::DTC_SERDESRXBufferStatus::DTC_SERDESRXBufferStatus() : data_(0) {}

DTC::DTC_SERDESRXBufferStatus::DTC_SERDESRXBufferStatus(std::bitset<3> data) : data_(data) {}

DTC::DTC_SERDESRXBufferStatus::DTC_SERDESRXBufferStatus(uint32_t data, DTC_Ring_ID ring)
{
	std::bitset<32> dataSet = data;
	data_[0] = dataSet[(uint8_t)ring * 3];
	data_[1] = dataSet[(uint8_t)ring * 3 + 1];
	data_[2] = dataSet[(uint8_t)ring * 3 + 2];
}

DTC::DTC_RXBufferStatus DTC::DTC_SERDESRXBufferStatus::GetStatus()
{
	switch (data_.to_ulong())
	{
	case 0:
		return DTC_RXBufferStatus_Nominal;
	case 1:
		return DTC_RXBufferStatus_BufferEmpty;
	case 2:
		return DTC_RXBufferStatus_BufferFull;
	case 5:
		return DTC_RXBufferStatus_Underflow;
	case 6:
		return DTC_RXBufferStatus_Overflow;
	default:
		return DTC_RXBufferStatus_Unknown;
	}
}