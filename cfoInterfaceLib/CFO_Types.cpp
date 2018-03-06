#include "CFO_Types.h"
#include <sstream>
#include <iomanip>

CFOLib::CFO_RXStatusConverter::CFO_RXStatusConverter(CFO_RXStatus status) : status_(status) { }

CFOLib::CFO_SimMode CFOLib::CFO_SimModeConverter::ConvertToSimMode(std::string modeName)
{
	if (modeName.find("isabled") != std::string::npos)
	{
		return CFO_SimMode_Disabled;
	}
	if (modeName.find("racker") != std::string::npos)
	{
		return CFO_SimMode_Tracker;
	}
	if (modeName.find("alorimeter") != std::string::npos)
	{
		return CFO_SimMode_Calorimeter;
	}
	if (modeName.find("osmic") != std::string::npos)
	{
		return CFO_SimMode_CosmicVeto;
	}
	if (modeName.find("oopback") != std::string::npos)
	{
		return CFO_SimMode_Loopback;
	}
	if (modeName.find("CFO") != std::string::npos || modeName.find("cfo") != std::string::npos)
	{
		return CFO_SimMode_NoCFO;
	}
	if (modeName.find("mulator") != std::string::npos)
	{
		return CFO_SimMode_ROCEmulator;
	}
	if (modeName.find("erformance") != std::string::npos)
	{
		return CFO_SimMode_Performance;
	}
	if(modeName.find("arge") != std::string::npos)
	{
		return CFO_SimMode_LargeFile;
	}

	try
	{
		auto modeInt = static_cast<CFO_SimMode>(stoi(modeName, nullptr, 10));
		return modeInt != CFO_SimMode_Invalid ? modeInt : CFO_SimMode_Disabled;
	}
	catch (...)
	{
		return CFO_SimMode_Invalid;
	}
}

CFOLib::CFO_Timestamp::CFO_Timestamp()
	: timestamp_(0) {}

CFOLib::CFO_Timestamp::CFO_Timestamp(const uint64_t timestamp)
	: timestamp_(timestamp) {}

CFOLib::CFO_Timestamp::CFO_Timestamp(const uint32_t timestampLow, const uint16_t timestampHigh)
{
	SetTimestamp(timestampLow, timestampHigh);
}

CFOLib::CFO_Timestamp::CFO_Timestamp(const uint8_t* timeArr, int offset)
{
	auto arr = reinterpret_cast<const uint64_t*>(timeArr + offset);
	timestamp_ = *arr;
}

CFOLib::CFO_Timestamp::CFO_Timestamp(const std::bitset<48> timestamp)
	: timestamp_(timestamp.to_ullong()) {}

void CFOLib::CFO_Timestamp::SetTimestamp(const uint32_t timestampLow, const uint16_t timestampHigh)
{
	timestamp_ = timestampLow + (static_cast<const uint64_t>(timestampHigh) << 32);
}

void CFOLib::CFO_Timestamp::GetTimestamp(const uint8_t* timeArr, int offset) const
{
	for (auto i = 0; i < 6; i++)
	{
	  const_cast<uint8_t*>(timeArr)[i + offset] = static_cast<uint8_t>(timestamp_ >> i * 8);
	}
}

std::string CFOLib::CFO_Timestamp::toJSON(bool arrayMode) const
{
	std::stringstream ss;
	if (arrayMode)
	{
		uint8_t ts[6];
		GetTimestamp(ts, 0);
		ss << "\"timestamp\": [" << static_cast<int>(ts[0]) << ",";
		ss << static_cast<int>(ts[1]) << ",";
		ss << static_cast<int>(ts[2]) << ",";
		ss << static_cast<int>(ts[3]) << ",";
		ss << static_cast<int>(ts[4]) << ",";
		ss << static_cast<int>(ts[5]) << "]";
	}
	else
	{
		ss << "\"timestamp\": " << timestamp_;
	}
	return ss.str();
}

std::string CFOLib::CFO_Timestamp::toPacketFormat() const
{
	uint8_t ts[6];
	GetTimestamp(ts, 0);
	std::stringstream ss;
	ss << std::setfill('0') << std::hex;
	ss << "0x" << std::setw(6) << static_cast<int>(ts[1]) << "\t" << "0x" << std::setw(6) << static_cast<int>(ts[0]) << "\n";
	ss << "0x" << std::setw(6) << static_cast<int>(ts[3]) << "\t" << "0x" << std::setw(6) << static_cast<int>(ts[2]) << "\n";
	ss << "0x" << std::setw(6) << static_cast<int>(ts[5]) << "\t" << "0x" << std::setw(6) << static_cast<int>(ts[4]) << "\n";
	return ss.str();
}

CFOLib::CFO_SERDESRXDisparityError::CFO_SERDESRXDisparityError() : data_(0) {}

CFOLib::CFO_SERDESRXDisparityError::CFO_SERDESRXDisparityError(std::bitset<2> data) : data_(data) {}

CFOLib::CFO_SERDESRXDisparityError::CFO_SERDESRXDisparityError(uint32_t data, CFO_Ring_ID ring)
{
	std::bitset<32> dataSet = data;
	uint32_t ringBase = static_cast<uint8_t>(ring) * 2;
	data_[0] = dataSet[ringBase];
	data_[1] = dataSet[ringBase + 1];
}

CFOLib::CFO_CharacterNotInTableError::CFO_CharacterNotInTableError() : data_(0) {}

CFOLib::CFO_CharacterNotInTableError::CFO_CharacterNotInTableError(std::bitset<2> data) : data_(data) {}

CFOLib::CFO_CharacterNotInTableError::CFO_CharacterNotInTableError(uint32_t data, CFO_Ring_ID ring)
{
	std::bitset<32> dataSet = data;
	uint32_t ringBase = static_cast<uint8_t>(ring) * 2;
	data_[0] = dataSet[ringBase];
	data_[1] = dataSet[ringBase + 1];
}


std::string CFOLib::Utilities::FormatByteString(double bytes, std::string extraUnit)
{
	auto res = FormatBytes(bytes);
	std::stringstream s;
	s << std::setprecision(5) << res.first << " " << res.second <<  extraUnit << " (" << std::to_string(static_cast<uint64_t>(bytes)) << " bytes" << extraUnit << ")";
	return s.str();
}

std::pair<double, std::string> CFOLib::Utilities::FormatBytes(double bytes)
{
	auto val = bytes;
	auto unit = "bytes";
	auto kb = bytes / 1024.0;

	if (kb > 1)
	{
		auto mb = kb / 1024.0;
		if (mb > 1)
		{
			auto gb = mb / 1024.0;
			if (gb > 1)
			{

				auto tb = gb / 1024.0;
				if (tb > 1)
				{
					val = tb;
					unit = "TB";
				}
				else
				{
					val = gb;
					unit = "GB";

				}
			}
			else
			{
				val = mb;
				unit = "MB";
			}
		}
		else
		{
			val = kb;
			unit = "KB";
		}
	}

	return std::make_pair(val, unit);
}

std::string CFOLib::Utilities::FormatTimeString(double seconds)
{
	auto res = FormatTime(seconds);
	std::stringstream s;
	s << std::setprecision(5) << res.first << " " << res.second;
	return s.str();
}

std::pair<double, std::string> CFOLib::Utilities::FormatTime(double seconds)
{

	auto val = seconds;
	auto unit = "s";

	if (seconds > 1)
	{
		auto min = seconds / 60.0;
		if (min > 1) {
			auto ho = min / 60.0;
			if (ho > 1) {
				auto day = ho / 24.0;
				if (day > 1) {
					val = day;
					unit = "days";
				}
				else {
					val = ho;
					unit = "hours";
				}
			}
			else
			{
				val = min;
				unit = "minutes";
			}
		}
	}
	else
	{
		auto ms = seconds * 1000;
		if (ms > 1)
		{
			val = ms;
			unit = "ms";
		}
		else
		{
			auto us = ms * 1000;
			if (us > 1)
			{
				val = us;
				unit = "us";
			}
			else
			{
				auto ns = us * 1000;
				val = ns;
				unit = "ns";
			}
		}
	}

	return std::make_pair(val, unit);
}