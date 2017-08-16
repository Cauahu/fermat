#ifndef FERMAT_COMMON_NUMEIC_FORMATTER_H_
#define FERMAT_COMMON_NUMEIC_FORMATTER_H_
#include <fermat/common/numeric_string.h>
#include <string>
#include <cstdint>

namespace fermat {

enum class BoolFormat {
    eFmtTrueFalse,
    eFmtYesNo,
    eFmtOnOff
};

class NumberFormatter {
public:
    /*!
     * format int value in decimal nonation.
     */
    static std::string format(int value);

    /*!
     * Formats an integer value in decimal notation,
	 * right justified in a field having at least
	 * the specified width.
     */
    static std::string format(int value, int width);

    /*!
     * Formats an integer value in decimal notation,
	 * right justified and zero-padded in a field
	 * having at least the specified width.
     */
    static std::string format0(int value, int width);

    /*!
     * Formats an int value in hexadecimal notation.
	 * If prefix is true, "0x" prefix is prepended to the 
	 * resulting string.
	 * The value is treated as unsigned.
     */
    static std::string format_hex(int value, bool prefix = false);

    /*!
     * Formats a int value in hexadecimal notation,
	 * right justified and zero-padded in
	 * a field having at least the specified width.
	 * If prefix is true, "0x" prefix is prepended to the 
	 * resulting string.
	 * The value is treated as unsigned.
     */
	static std::string format_hex(int value, int width, bool prefix = false);

    /*!
     * Formats an unsigned int value in decimal notation.
     */
    static std::string format(unsigned value);

	/*!
     * Formats an unsigned long int in decimal notation,
	 * right justified in a field having at least the
	 * specified width.	
     */
	static std::string format(unsigned value, int width);
	
    /*!
     * Formats an unsigned int value in decimal notation,
	 * right justified and zero-padded in a field having at
	 * least the specified width.
     */
	static std::string format0(unsigned int value, int width);

    /*!
     * Formats an unsigned int value in hexadecimal notation.
	 * If prefix is true, "0x" prefix is prepended to the 
	 * resulting string.
     */
	static std::string format_hex(unsigned value, bool prefix = false);

    /*!
     * Formats a int value in hexadecimal notation,
	 * right justified and zero-padded in
	 * a field having at least the specified width.
	 * If prefix is true, "0x" prefix is prepended to the 
	 * resulting string.
     */
	static std::string format_hex(unsigned value, int width, bool prefix = false);

    /*!
     * Formats a 64-bit integer value in decimal notation.
     */
	static std::string format(int64_t value);
		
    /*!
     * Formats a 64-bit integer value in decimal notation,
	 * right justified in a field having at least the specified width.
     */
	static std::string format(int64_t value, int width);
	
    /*!
     * Formats a 64-bit integer value in decimal notation,
	 * right justified and zero-padded in a field having at least
	 * the specified width.
     */
	static std::string format0(int64_t value, int width);

    /*!
     * Formats a 64-bit integer value in hexadecimal notation.
	 * If prefix is true, "0x" prefix is prepended to the 
	 * resulting string.
	 * The value is treated as unsigned.
     */
	static std::string format_hex(int64_t value, bool prefix = false);

    /*!
     * Formats a 64-bit integer value in hexadecimal notation,
	 * right justified and zero-padded in a field having at least
	 * the specified width.
	 * The value is treated as unsigned.
	 * If prefix is true, "0x" prefix is prepended to the resulting string.
     */
	static std::string format_hex(int64_t value, int width, bool prefix = false);

    /*!
     * Formats an unsigned 64-bit integer value in decimal notation.
     */
	static std::string format(uint64_t value);

    /*!
     * Formats an unsigned 64-bit integer value in decimal notation,
	 * right justified in a field having at least the specified width.
     */
	static std::string format(uint64_t value, int width);

    /*!
     * Formats an unsigned 64-bit integer value in decimal notation,
	 * right justified and zero-padded in a field having at least the
	 * specified width.
     */
	static std::string format0(uint64_t value, int width);

    /*!
     * Formats a 64-bit integer value in hexadecimal notation.
	 * If prefix is true, "0x" prefix is prepended to the 
	 * resulting string.
     */
	static std::string format_hex(uint64_t value, bool prefix = false);

    /*!
     * Formats a 64-bit integer value in hexadecimal notation,
	 * right justified and zero-padded in a field having at least
	 * the specified width. If prefix is true, "0x" prefix is
	 * prepended to the resulting string.
     */
	static std::string format_hex(uint64_t value, int width, bool prefix = false);

    /*!
     * Formats a float value in decimal floating-point notation,
	 * according to std::printf's %g format with a precision of 8 fractional digits.
     */
	static std::string format(float value);

    /*!
     * Formats a double value in decimal floating-point notation,
	 * according to std::printf's %f format with the given precision.
     */
	static std::string format(float value, int precision);

    /*!
     * Formats a double value in decimal floating-point notation,
	 * right justified in a field of the specified width,
	 * with the number of fractional digits given in precision.
     */
	static std::string format(float value, int width, int precision);

    /*!
     * Formats a double value in decimal floating-point notation,
	 * according to std::printf's %g format with a precision of 16 fractional digits.
     */
	static std::string format(double value);

    /*!
     * Formats a double value in decimal floating-point notation,
	 * according to std::printf's %f format with the given precision.
     */
	static std::string format(double value, int precision);

    /*!
     * Formats a double value in decimal floating-point notation,
	 * right justified in a field of the specified width,
	 * with the number of fractional digits given in precision.
     */
	static std::string format(double value, int width, int precision);

    /*!
     * Formats a pointer in an eight (32-bit architectures) or
	 * sixteen (64-bit architectures) characters wide
	 * field in hexadecimal notation.
     */
	static std::string format(const void* ptr);

    /*!
     * Formats a bool value in decimal/text notation,
	 * according to format parameter.
     */
	static std::string format(bool value, BoolFormat format = BoolFormat::eFmtTrueFalse);
	
    /*!
     * Formats an integer value in decimal notation.
     */
	static void append(std::string& str, int value);

    /*!
     * Formats an integer value in decimal notation,
	 * right justified in a field having at least
	 * the specified width.
     */
	static void append(std::string& str, int value, int width);

    /*!
     * Formats an integer value in decimal notation,
	 * right justified and zero-padded in a field
	 * having at least the specified width.
     */
	static void append0(std::string& str, int value, int width);

    /*!
     * Formats an int value in hexadecimal notation.
	 * The value is treated as unsigned.
     */
	static void append_hex(std::string& str, int value);

    /*!
     * Formats a int value in hexadecimal notation,
	 * right justified and zero-padded in
	 * a field having at least the specified width.
	 * The value is treated as unsigned.
     */
	static void append_hex(std::string& str, int value, int width);

    /*!
     * Formats an unsigned int value in decimal notation.
     */
	static void append(std::string& str, unsigned value);

    /*!
     * Formats an unsigned long int in decimal notation,
	 * right justified in a field having at least the
	 * specified width.
     */
	static void append(std::string& str, unsigned value, int width);

    /*!
     * Formats an unsigned int value in decimal notation,
	 * right justified and zero-padded in a field having at
	 * least the specified width.
     */
	static void append0(std::string& str, unsigned int value, int width);

    /*!
     * Formats an unsigned int value in hexadecimal notation.
     */
	static void append_hex(std::string& str, unsigned value);
	
    /*!
     * Formats a int value in hexadecimal notation,
	 * right justified and zero-padded in
	 * a field having at least the specified width.
     */
	static void append_hex(std::string& str, unsigned value, int width);

    /*!
     * Formats a 64-bit integer value in decimal notation.
     */
    static void append(std::string& str, int64_t value);
	
    /*!
     * Formats a 64-bit integer value in decimal notation,
	 * right justified in a field having at least the specified width.
     */
	static void append(std::string& str, int64_t value, int width);

    /*!
     * Formats a 64-bit integer value in decimal notation,
	 * right justified and zero-padded in a field having at least
	 * the specified width.
     */
	static void append0(std::string& str, int64_t value, int width);

    /*!
     * Formats a 64-bit integer value in hexadecimal notation.
	 * The value is treated as unsigned.
     */
	static void append_hex(std::string& str, int64_t value);

    /*!
     * Formats a 64-bit integer value in hexadecimal notation,
	 * right justified and zero-padded in a field having at least
	 * the specified width.
	 * The value is treated as unsigned.
     */
	static void append_hex(std::string& str, int64_t value, int width);

    /*!
     * Formats an unsigned 64-bit integer value in decimal notation.
     */
	static void append(std::string& str, uint64_t value);
    
    /*!
     * Formats an unsigned 64-bit integer value in decimal notation,
	 * right justified in a field having at least the specified width.
     */
	static void append(std::string& str, uint64_t value, int width);

    /*!
     * Formats an unsigned 64-bit integer value in decimal notation,
	 * right justified and zero-padded in a field having at least the
	 * specified width.
     */
	static void append0(std::string& str, uint64_t value, int width);

    /*!
     * Formats a 64-bit integer value in hexadecimal notation.
     */
	static void append_hex(std::string& str, uint64_t value);
	
    /*!
     * Formats a 64-bit integer value in hexadecimal notation,
	 * right justified and zero-padded in a field having at least
	 * the specified width.
	 */
	static void append_hex(std::string& str, uint64_t value, int width);

    /*!
     * Formats a float value in decimal floating-point notation,
	 * according to std::printf's %g format with a precision of 8 fractional digits.
     */
	static void append(std::string& str, float value);

    /*!
     * Formats a double value in decimal floating-point notation,
	 * according to std::printf's %f format with the given precision.
     */
	static void append(std::string& str, float value, int precision);

    /*!
     * Formats a double value in decimal floating-point notation,
	 * right justified in a field of the specified width,
	 * with the number of fractional digits given in precision.
     */
	static void append(std::string& str, float value, int width, int precision);
	
    /*!
     * Formats a double value in decimal floating-point notation,
	 * according to std::printf's %g format with a precision of 16 fractional digits.
     */
	static void append(std::string& str, double value);

    /*!
     * Formats a double value in decimal floating-point notation,
	 * according to std::printf's %f format with the given precision.
     */
	static void append(std::string& str, double value, int precision);

    /*!
     * Formats a double value in decimal floating-point notation,
	 * right justified in a field of the specified width,
	 * with the number of fractional digits given in precision.
     */
	static void append(std::string& str, double value, int width, int precision);

    /*!
     * Formats a pointer in an eight (32-bit architectures) or
	 * sixteen (64-bit architectures) characters wide
	 * field in hexadecimal notation.
     */
	static void append(std::string& str, const void* ptr);

};

//
//inlines
//

inline std::string NumberFormatter::format(int value)
{
	std::string result;
	int_to_str(value, 10, result);
	return result;
}


inline std::string NumberFormatter::format(int value, int width)
{
	std::string result;
	int_to_str(value, 10, result, false, width, ' ');
	return result;
}


inline std::string NumberFormatter::format0(int value, int width)
{
	std::string result;
	int_to_str(value, 10, result, false, width, '0');
	return result;
}


inline std::string NumberFormatter::format_hex(int value, bool prefix)
{
	std::string result;
	uint_to_str(static_cast<unsigned int>(value), 0x10, result, prefix);
	return result;
}


inline std::string NumberFormatter::format_hex(int value, int width, bool prefix)
{
	std::string result;
	uint_to_str(static_cast<unsigned int>(value), 0x10, result, prefix, width, '0');
	return result;
}


inline std::string NumberFormatter::format(unsigned value)
{
	std::string result;
	uint_to_str(value, 10, result);
	return result;
}


inline std::string NumberFormatter::format(unsigned value, int width)
{
	std::string result;
	uint_to_str(value, 10, result, false, width, ' ');
	return result;
}


inline std::string NumberFormatter::format0(unsigned int value, int width)
{
	std::string result;
	uint_to_str(value, 10, result, false, width, '0');
	return result;
}


inline std::string NumberFormatter::format_hex(unsigned value, bool prefix)
{
	std::string result;
	uint_to_str(value, 0x10, result, prefix);
	return result;
}


inline std::string NumberFormatter::format_hex(unsigned value, int width, bool prefix)
{
	std::string result;
	uint_to_str(value, 0x10, result, prefix, width, '0');
	return result;
}


inline std::string NumberFormatter::format(int64_t value)
{
	std::string result;
	int_to_str(value, 10, result);
	return result;
}


inline std::string NumberFormatter::format(int64_t value, int width)
{
	std::string result;
	int_to_str(value, 10, result, false, width, ' ');
	return result;
}


inline std::string NumberFormatter::format0(int64_t value, int width)
{
	std::string result;
	int_to_str(value, 10, result, false, width, '0');
	return result;
}


inline std::string NumberFormatter::format_hex(int64_t value, bool prefix)
{
	std::string result;
	uint_to_str(static_cast<uint64_t>(value), 0x10, result, prefix);
	return result;
}


inline std::string NumberFormatter::format_hex(int64_t value, int width, bool prefix)
{
	std::string result;
	uint_to_str(static_cast<uint64_t>(value), 0x10, result, prefix, width, '0');
	return result;
}


inline std::string NumberFormatter::format(uint64_t value)
{
	std::string result;
	uint_to_str(value, 10, result);
	return result;
}


inline std::string NumberFormatter::format(uint64_t value, int width)
{
	std::string result;
	uint_to_str(value, 10, result, false, width, ' ');
	return result;
}


inline std::string NumberFormatter::format0(uint64_t value, int width)
{
	std::string result;
	uint_to_str(value, 10, result, false, width, '0');
	return result;
}


inline std::string NumberFormatter::format_hex(uint64_t value, bool prefix)
{
	std::string result;
	uint_to_str(value, 0x10, result, prefix);
	return result;
}


inline std::string NumberFormatter::format_hex(uint64_t value, int width, bool prefix)
{
	std::string result;
	uint_to_str(value, 0x10, result, prefix, width, '0');
	return result;
}


inline std::string NumberFormatter::format(float value)
{
	char buffer[kMaxFLTStringLen];
	float_to_str(buffer, kMaxFLTStringLen, value);
	return std::string(buffer);
}


inline std::string NumberFormatter::format(float value, int precision)
{
	char buffer[kMaxFLTStringLen];
	float_to_fixed_str(buffer, kMaxFLTStringLen, value, precision);
	return std::string(buffer);
}


inline std::string NumberFormatter::format(float value, int width, int precision)
{
	std::string result;
	float_to_fixed_str(result, value, precision, width);
	return result;
}


inline std::string NumberFormatter::format(double value)
{
	char buffer[kMaxFLTStringLen];
	double_to_str(buffer, kMaxFLTStringLen, value);
	return std::string(buffer);
}


inline std::string NumberFormatter::format(double value, int precision)
{
	char buffer[kMaxFLTStringLen];
	double_to_fixed_str(buffer, kMaxFLTStringLen, value, precision);
	return std::string(buffer);
}


inline std::string NumberFormatter::format(double value, int width, int precision)
{
	std::string result;
	double_to_fixed_str(result, value, precision, width);
	return result;
}


inline std::string NumberFormatter::format(const void* ptr)
{
	std::string result;
	append(result, ptr);
	return result;
}

}
#endif
