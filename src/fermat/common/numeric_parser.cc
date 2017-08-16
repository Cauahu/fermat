#include <fermat/common/numeric_parser.h>
#include <fermat/common/numeric_string.h>
#include <fermat/common/string.h>
#include <cmath>
namespace fermat {

bool NumberParser::parse_int(const std::string& s, int& value, char thSep)
{
	return str_to_int(s.c_str(), value, kNumBaseDes, thSep);
}

bool NumberParser::parse_unsigned(const std::string& s, unsigned& value, char thSep)
{
	return str_to_int(s.c_str(), value, kNumBaseDes, thSep);
}

bool NumberParser::parse_hex(const std::string& s, unsigned& value)
{
	int offset = 0;
	if (s.size() > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) offset = 2;
	return str_to_int(s.c_str() + offset, value, kNumBaseHex);
}

bool NumberParser::parse_oct(const std::string& s, unsigned& value)
{
	return str_to_int(s.c_str(), value, kNumBaseOct);
}

bool NumberParser::parse_int64(const std::string &s, int64_t &value, char thSep)
{
	return str_to_int(s.c_str(), value, kNumBaseDes, thSep);
}

bool NumberParser::parse_uint64(const std::string &s, uint64_t &value, char thSep)
{
	return str_to_int(s.c_str(), value, kNumBaseDes, thSep);
}

bool NumberParser::parse_hex64(const std::string& s, uint64_t &value)
{
	int offset = 0;
	if (s.size() > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) offset = 2;
	return str_to_int(s.c_str() + offset, value, kNumBaseHex);
}


bool NumberParser::parse_oct64(const std::string& s, uint64_t& value)
{
	return str_to_int(s.c_str(), value, kNumBaseOct);
}


bool NumberParser::parse_float(const std::string& s, double& value, char decSep, char thSep)
{
	return str_to_double(s.c_str(), value, decSep, thSep);
}


bool NumberParser::parse_bool(const std::string& s, bool& value)
{
	int n;
	if (NumberParser::parse_int(s, n)) {
		value = (n != 0);
		return true;
	}

	if (icompare(s, "true") == 0) {
		value = true;
		return true;
	} else if (icompare(s, "yes") == 0) {
		value = true;
		return true;
	} else if (icompare(s, "on") == 0) {
		value = true;
		return true;
	}
	
	if (icompare(s, "false") == 0) {
		value = false;
		return true;
	} else if (icompare(s, "no") == 0) {
		value = false;
		return true;
	} else if (icompare(s, "off") == 0) {
		value = false;
		return true;
	}
	
	return false;
}
bool NumberParser::atoi(const std::string& s, int64_t& value)
{
	const char *str = s.data();
	const char *end = s.data() + s.size();
	while (str !=end && *str > '\0' && *str <= ' ') {
    	str++;
  	}
	if(str == end) {
		return 0L;
	}
  	int32_t sign = 1;
  	int64_t num = 0;
	
  	if (*str == '-') {
    	str++;
    	sign = -1;
  	} else if (*str == '+') {
    	str++;
  	}
  	while (str != end && *str != '\0') {
    	if (*str < '0' || *str > '9') {
			break;
		}
   		num = num * 10 + *str - '0';
    	str++;
  }
   value = num * sign;
   return true;
}

bool NumberParser::parse_float_si(const std::string &s, double &value)
{
	const char *ptr = s.data();
	size_t size = s.size();
  	while (size > 0 && *ptr >= '\0' && *ptr <= ' ') {
    	ptr++;
    	size--;
 	}
  	int32_t sign = 1;
  	if (size > 0) {
    	if (*ptr == '-') {
     		ptr++;
      		size--;
      		sign = -1;
    	} else if (*ptr == '+') {
      		ptr++;
      		size--;
    	}
	}

	if (size > 2) {
		if ((ptr[0] == 'i' || ptr[0] == 'I') && (ptr[1] == 'n' || ptr[1] == 'N') &&
        	(ptr[2] == 'f' || ptr[2] == 'F')) return 0;
    	if ((ptr[0] == 'n' || ptr[0] == 'N') && (ptr[1] == 'a' || ptr[1] == 'A') &&
        	(ptr[2] == 'n' || ptr[2] == 'N')) return 0;
	}
  	long double num = 0;
  	int32_t col = 0;
  	while (size > 0) {
    	if (*ptr < '0' || *ptr > '9') break;
    	num = num * 10 + *ptr - '0';
    	ptr++;
    	size--;
    	if (num > 0) col++;
  	}

	if (size > 0 && *ptr == '.') {
    	ptr++;
    	size--;
    long double fract = 0.0;
    long double base = 10;
    while (col < 16 && size > 0) {
    	if (*ptr < '0' || *ptr > '9') break;
      	fract += (*ptr - '0') / base;
      	ptr++;
      	size--;
      	col++;
      	base *= 10;
    }
    num += fract;
  }
  if (size > 0 && (*ptr == 'e' || *ptr == 'E')) {
    ptr++;
    size--;
	long double base = 10.0;
	int64_t e; 
	bool r = atoi(std::string(ptr, size), e);
	if(!r) {
		return false;
	}
    num *= ::pow(base, e);
  }
  value  = static_cast<double>(num * sign);
  return true;
}

bool NumberParser::atoix(const std::string &s, int64_t &value)
{
	const char *str = s.data();
	const char *end = s.data() + s.size();
	while (str != end && *str > '\0' && *str <= ' ') {
        str++;
    }
    int32_t sign = 1;
    if(str == end) {
        return 0;
    }
    if (*str == '-') {
        str++;
        sign = -1;
    } else if (*str == '+') {
        str++;
    }
    long double num = 0;
    while (str != end && *str != '\0') {
        if (*str < '0' || *str > '9') {
            break;
        }
        num = num * 10 + *str - '0';
        str++;
    }
    if (str != end && *str == '.') {
        str++;
        long double base = 10;
        while (str != end && *str != '\0') {
            if (*str < '0' || *str > '9') {
                break;
            }
            num += (*str - '0') / base;
            str++;
            base *= 10;
        }
    }
    num *= sign;
    while (str != end && *str > '\0' && *str <= ' ') {
        str++;
    }
	if (str != end) {
	 
		if (*str == 'k' || *str == 'K') {
		  num *= 1LL << 10;
		} else if (*str == 'm' || *str == 'M') {
		   num *= 1LL << 20;
		} else if (*str == 'g' || *str == 'G') {
		  num *= 1LL << 30;
		} else if (*str == 't' || *str == 'T') {
		  num *= 1LL << 40;
		} else if (*str == 'p' || *str == 'P') {
		  num *= 1LL << 50;
		} else if (*str == 'e' || *str == 'E') {
		  num *= 1LL << 60;
		}
	}
	value =  static_cast<int64_t>(num);
	return true;
}
} //namespace fermat
