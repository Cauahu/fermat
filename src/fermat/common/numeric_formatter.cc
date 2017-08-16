#include <fermat/common/numeric_formatter.h>
#include <algorithm>

namespace fermat {


std::string NumberFormatter::format(bool value, BoolFormat format)
{
	switch(format) {
		default:
		case BoolFormat::eFmtTrueFalse:
			if (value == true)
				return "true";
			return "false";
		case BoolFormat::eFmtYesNo:
			if (value == true)
				return "yes";
			return "no";
		case BoolFormat::eFmtOnOff:
			if (value == true)
				return "on";
			return "off";
	}
}


void NumberFormatter::append(std::string& str, int value)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	int_to_str(value, 10, result, sz);
	str.append(result, sz);
}


void NumberFormatter::append(std::string& str, int value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	int_to_str(value, 10, result, sz, false, width);
	str.append(result, sz);
}


void NumberFormatter::append0(std::string& str, int value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	int_to_str(value, 10, result, sz, false, width, '0');
	str.append(result, sz);
}


void NumberFormatter::append_hex(std::string& str, int value)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(static_cast<unsigned int>(value), 0x10, result, sz);
	str.append(result, sz);
}


void NumberFormatter::append_hex(std::string& str, int value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(static_cast<unsigned int>(value), 0x10, result, sz, false, width, '0');
	str.append(result, sz);
}


void NumberFormatter::append(std::string& str, unsigned value)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(value, 10, result, sz);
	str.append(result, sz);
}


void NumberFormatter::append(std::string& str, unsigned value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(value, 10, result, sz, false, width);
	str.append(result, sz);
}


void NumberFormatter::append0(std::string& str, unsigned int value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(value, 10, result, sz, false, width, '0');
	str.append(result, sz);
}


void NumberFormatter::append_hex(std::string& str, unsigned value)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(value, 0x10, result, sz);
	str.append(result, sz);
}


void NumberFormatter::append_hex(std::string& str, unsigned value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(value, 0x10, result, sz, false, width, '0');
	str.append(result, sz);
}


void NumberFormatter::append(std::string& str, int64_t value)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	int_to_str(value, 10, result, sz);
	str.append(result, sz);
}


void NumberFormatter::append(std::string& str, int64_t value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	int_to_str(value, 10, result, sz, false, width, '0');
	str.append(result, sz);
}


void NumberFormatter::append0(std::string& str, int64_t value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	int_to_str(value, 10, result, sz, false, width, '0');
	str.append(result, sz);
}


void NumberFormatter::append_hex(std::string& str, int64_t value)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(static_cast<uint64_t>(value), 0x10, result, sz);
	str.append(result, sz);
}


void NumberFormatter::append_hex(std::string& str, int64_t value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(static_cast<uint64_t>(value), 0x10, result, sz, false, width, '0');
	str.append(result, sz);
}


void NumberFormatter::append(std::string& str, uint64_t value)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(value, 10, result, sz);
	str.append(result, sz);
}


void NumberFormatter::append(std::string& str, uint64_t value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(value, 10, result, sz, false, width, '0');
	str.append(result, sz);
}


void NumberFormatter::append0(std::string& str, uint64_t value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(value, 10, result, sz, false, width, '0');
	str.append(result, sz);
}


void NumberFormatter::append_hex(std::string& str, uint64_t value)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(value, 0x10, result, sz);
	str.append(result, sz);
}


void NumberFormatter::append_hex(std::string& str, uint64_t value, int width)
{
	char result[kMaxIntStringLen];
	std::size_t sz = kMaxIntStringLen;
	uint_to_str(value, 0x10, result, sz, false, width, '0');
	str.append(result, sz);
}

void NumberFormatter::append(std::string& str, float value)
{
	char buffer[kMaxFLTStringLen];
	float_to_str(buffer, kMaxFLTStringLen, value);
	str.append(buffer);
}


void NumberFormatter::append(std::string& str, float value, int precision)
{
	char buffer[kMaxFLTStringLen];
	float_to_fixed_str(buffer, kMaxFLTStringLen, value, precision);
	str.append(buffer);
}


void NumberFormatter::append(std::string& str, float value, int width, int precision)
{
	std::string result;
	str.append(float_to_fixed_str(result, value, precision, width));
}


void NumberFormatter::append(std::string& str, double value)
{
	char buffer[kMaxFLTStringLen];
	double_to_str(buffer, kMaxFLTStringLen, value);
	str.append(buffer);
}


void NumberFormatter::append(std::string& str, double value, int precision)
{
	char buffer[kMaxFLTStringLen];
	double_to_fixed_str(buffer, kMaxFLTStringLen, value, precision);
	str.append(buffer);
}


void NumberFormatter::append(std::string& str, double value, int width, int precision)
{
	std::string result;
	str.append(double_to_fixed_str(result, value, precision, width));
}


void NumberFormatter::append(std::string& str, const void* ptr)
{

    static const char digits_hex[] = "0123456789ABCDEF";
    char buf[24];
    uintptr_t value = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t i = value;
    char* p = buf;

    do {
        int lsd = i % 16;
        i /= 16;
        *p++ = digits_hex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);
    str.append(buf);

}


}