#ifndef FERMAT_COMMON_NUMERIC_PARSER_H_
#define FERMAT_COMMON_NUMERIC_PARSER_H_
#include <string>

namespace fermat {

class NumberParser {
public:
	static const unsigned short kNumBaseOct = 010;
	static const unsigned short kNumBaseDes = 10;
	static const unsigned short kNumBaseHex = 0x10;

	/*!
	 * Parses an integer value in decimal notation from the given string.
	 * Returns true if a valid integer has been found, false otherwise. 
	 * If parsing was not successful, value is undefined.
	 */
	static bool parse_int(const std::string& s, int& value, char thousandSeparator = ',');

	/*!
	 * Parses an unsigned integer value in decimal notation from the given string.
	 * Returns true if a valid integer has been found, false otherwise. 
	 * If parsing was not successful, value is undefined.
	 */
	static bool parse_unsigned(const std::string& s, unsigned& value, char thousandSeparator = ',');

	/*!
	 * Parses an unsigned integer value in hexadecimal notation from the given string.
	 * Returns true if a valid integer has been found, false otherwise. 
	 * If parsing was not successful, value is undefined.
	 */
	static bool parse_hex(const std::string& s, unsigned& value);
	
	/*!
	 * Parses an unsigned integer value in octal notation from the given string.
	 * Returns true if a valid integer has been found, false otherwise. 
	 * If parsing was not successful, value is undefined.
	 */
	static bool parse_oct(const std::string& s, unsigned& value);

	/*!
	 * Parses a 64-bit integer value in decimal notation from the given string.
	 * Returns true if a valid integer has been found, false otherwise. 
	 * If parsing was not successful, value is undefined.
	 */
	static bool parse_int64(const std::string& s, int64_t& value, char thousandSeparator = ',');

	/*!
	 * Parses a 64-bit integer value in decimal notation from the given string.
	 * Returns true if a valid integer has been found, false otherwise. 
	 * If parsing was not successful, value is undefined.
	 * most effective!
	 */
	static bool atoi(const std::string& s, int64_t& value);

	/*!
	 * Parses a 64-bit integer value in decimal notation from the given string.
	 * the string like "123M" or "123m" "123k", "123K".
	 * Returns true if a valid integer has been found, false otherwise. 
	 * If parsing was not successful, value is undefined.
	 * most effective!
	 */
	static bool atoix(const std::string &s, int64_t &value);

	/*!
	 * Parses an unsigned 64-bit integer value in decimal notation from the given string.
	 * Returns true if a valid integer has been found, false otherwise. 
	 * If parsing was not successful, value is undefined.
	 */
	static bool parse_uint64(const std::string& s, uint64_t& value, char thousandSeparator = ',');

	/*!
	 * Parses an unsigned 64-bit integer value in hexadecimal notation from the given string.
	 * Returns true if a valid integer has been found, false otherwise. 
	 * If parsing was not successful, value is undefined.
	 */
	static bool parse_hex64(const std::string& s, uint64_t& value);

	/*!
	 * Parses an unsigned 64-bit integer value in octal notation from the given string.
	 * Returns true if a valid integer has been found, false otherwise. 
	 * If parsing was not successful, value is undefined.
	 */
	static bool parse_oct64(const std::string& s, uint64_t& value);

	/*!
	 * parses a double value in decimal floating point notation
	 * from the given string.
	 * Returns true if a valid floating point number has been found,
	 * false otherwise.
	 * If parsing was not successful, value is undefined.
	 */
	static bool parse_float(const std::string& s, double& value, char decimalSeparator = '.', char thousandSeparator = ',');

	/*!
	 * parses a double value in decimal floating point notation
	 * from the given string.
	 * Returns true if a valid floating point number has been found,
	 * false otherwise.
	 * If parsing was not successful, value is undefined.
	 * most effective!
	 */
	static bool parse_float_si(const std::string &s, double &value);

	/*!
	 * Parses a bool value in decimal or string notation
	 * from the given string.
	 * Valid forms are: "0", "1", "true", "on", false", "yes", "no", "off".
	 * String forms are NOT case sensitive.
	 * Returns true if a valid bool number has been found,
	 * false otherwise.
	 * If parsing was not successful, value is undefined.
	 */
	static bool parse_bool(const std::string& s, bool& value);
	

};
}
#endif