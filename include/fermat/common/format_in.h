
#include <fermat/common/format.h>
#include <string.h>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstddef>

#ifndef FERMAT_COMMON_FORMAT_H_
#error "can not be use without format.h"
#endif

using fermat::internal::Arg;

#if FORMAT_EXCEPTIONS
# define FORMAT_TRY try
# define FORMAT_CATCH(x) catch (x)
#else
# define FORMAT_TRY if (true)
# define FORMAT_CATCH(x) if (false)
#endif

#define FORMAT_FUNC inline

static inline fermat::internal::Null<> strerror_r(int, char *, ...)
{
	return fermat::internal::Null<>();
}
static inline fermat::internal::Null<> strerror_s(char *, std::size_t, ...)
{
	return fermat::internal::Null<>();
}

namespace fermat {

namespace {

inline int fmt_snprintf(char *buffer, size_t size, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vsnprintf_s(buffer, size, _TRUNCATE, format, args);
    va_end(args);
    return result;
}
# define FORMAT_SNPRINTF fmt_snprintf
# define FORMAT_SWPRINTF swprintf

// Checks if a value fits in int - used to avoid warnings about comparing
// signed and unsigned integers.
template <bool IsSigned>
struct IntChecker
{
    template <typename T>
    static bool fits_in_int(T value)
    {
        unsigned max = INT_MAX;
        return value <= max;
    }
    static bool fits_in_int(bool)
    {
        return true;
    }
};

template <>
struct IntChecker<true>
{
    template <typename T>
    static bool fits_in_int(T value)
    {
        return value >= INT_MIN && value <= INT_MAX;
    }
    static bool fits_in_int(int)
    {
        return true;
    }
};

const char RESET_COLOR[] = "\x1b[0m";

typedef void(*FormatFunc)(fermat::Writer &, int, fermat::StringRef);

// Portable thread-safe version of strerror.
// Sets buffer to point to a string describing the error code.
// This can be either a pointer to a string stored in buffer,
// or a pointer to some static immutable string.
// Returns one of the following values:
//   0      - success
//   ERANGE - buffer is not large enough to store the error message
//   other  - failure
// Buffer should be at least of size 1.
int safe_strerror(
    int error_code, char *&buffer, std::size_t buffer_size) FORMAT_NOEXCEPT
{
    FORMAT_ASSERT(buffer != 0 && buffer_size != 0, "invalid buffer");

    class StrError {
    private:
        int            _error_code;
        char          *_&buffer;
        std::size_t    _buffer_size;

        // A noop assignment operator to avoid bogus warnings.
        void operator=(const StrError &)
        {}

        // Handle the result of XSI-compliant version of strerror_r.
        int handle(int result)
        {
            // glibc versions before 2.13 return result in errno.
            return result == -1 ? errno : result;
        }

        // Handle the result of GNU-specific version of strerror_r.
        int handle(char *message)
        {
            // If the buffer is full then the message is probably truncated.
            if (message == _buffer && strlen(_buffer) == _buffer_size - 1)
                return ERANGE;
            buffer_ = message;
            return 0;
        }

        // Handle the case when strerror_r is not available.
        int handle(fermat::internal::Null<>)
        {
            return fallback(strerror_s(_buffer, _buffer_size, _error_code));
        }

        // Fallback to strerror_s when strerror_r is not available.
        int fallback(int result)
        {
            // If the buffer is full then the message is probably truncated.
            return result == 0 && strlen(_buffer) == _buffer_size - 1 ?
                ERANGE : result;
        }

        // Fallback to strerror if strerror_r and strerror_s are not available.
        int fallback(fermat::internal::Null<>)
        {
            errno = 0;
            _buffer = strerror(_error_code);
            return errno;
        }

    public:
        StrError(int err_code, char *&buf, std::size_t buf_size)
            : _error_code(err_code), _buffer(buf), _buffer_size(buf_size)
        {}

        int run()
        {
            strerror_r(0, 0, "");  // Suppress a warning about unused strerror_r.
            return handle(strerror_r(_error_code, _buffer, _buffer_size));
        }
    };
    return StrError(error_code, buffer, buffer_size).run();
}

void format_error_code(fermat::Writer &out, int error_code,
    fermat::StringRef message) FORMAT_NOEXCEPT
{
    out.clear();
    static const char SEP[] = ": ";
    static const char ERROR_STR[] = "error ";
    // Subtract 2 to account for terminating null characters in SEP and ERROR_STR.
    std::size_t error_code_size = sizeof(SEP) + sizeof(ERROR_STR) - 2;
    typedef fermat::internal::IntTraits<int>::MainType MainType;
    MainType abs_value = static_cast<MainType>(error_code);
    if (internal::is_negative(error_code)) {
        abs_value = 0 - abs_value;
        ++error_code_size;
    }
    error_code_size += fermat::internal::count_digits(abs_value);
    if (message.size() <= fermat::internal::INLINE_BUFFER_SIZE - error_code_size)
        out << message << SEP;
    out << ERROR_STR << error_code;
    assert(out.size() <= fermat::internal::INLINE_BUFFER_SIZE);
}

void report_error(FormatFunc func,
    int error_code, fermat::StringRef message) FORMAT_NOEXCEPT
{
    fermat::MemoryWriter full_message;
    func(full_message, error_code, message);
    // Use Writer::data instead of Writer::c_str to avoid potential memory
    // allocation.
    std::fwrite(full_message.data(), full_message.size(), 1, stderr);
    std::fputc('\n', stderr);
}

// IsZeroInt::visit(arg) returns true iff arg is a zero integer.
class IsZeroInt: public fermat::internal::ArgVisitor<IsZeroInt, bool>
{
public:
    template <typename T>
    bool visit_any_int(T value)
    {
        return value == 0;
    }
};

// Checks if an argument is a valid printf width specifier and sets
// left alignment if it is negative.
class WidthHandler: public fermat::internal::ArgVisitor<WidthHandler, unsigned>
{
private:
    fermat::FormatSpec &_spec;

    FORMAT_DISALLOW_COPY_AND_ASSIGN(WidthHandler);

public:
    explicit WidthHandler(fermat::FormatSpec &spec): _spec(spec)
    {}

    void report_unhandled_arg()
    {
        FORMAT_THROW(fermat::FormatError("width is not integer"));
    }

    template <typename T>
    unsigned visit_any_int(T value)
    {
        typedef typename fermat::internal::IntTraits<T>::MainType UnsignedType;
        UnsignedType width = static_cast<UnsignedType>(value);
        if (fermat::internal::is_negative(value)) {
            spec_.align_ = fermat::ALIGN_LEFT;
            width = 0 - width;
        }
        if (width > INT_MAX)
            FORMAT_THROW(fermat::FormatError("number is too big"));
        return static_cast<unsigned>(width);
    }
};

class PrecisionHandler:
    public fermat::internal::ArgVisitor<PrecisionHandler, int>
{
public:
    void report_unhandled_arg()
    {
        FORMAT_THROW(fermat::FormatError("precision is not integer"));
    }

    template <typename T>
    int visit_any_int(T value)
    {
        if (!IntChecker<std::numeric_limits<T>::is_signed>::fits_in_int(value))
            FORMAT_THROW(fermat::FormatError("number is too big"));
        return static_cast<int>(value);
    }
};

template <typename T, typename U>
struct is_same
{
    enum
    {
        value = 0
    };
};

template <typename T>
struct is_same<T, T>
{
    enum
    {
        value = 1
    };
};

// An argument visitor that converts an integer argument to T for printf,
// if T is an integral type. If T is void, the argument is converted to
// corresponding signed or unsigned type depending on the type specifier:
// 'd' and 'i' - signed, other - unsigned)
template <typename T = void>
class ArgConverter: public fermat::internal::ArgVisitor<ArgConverter<T>, void>
{
private:
    fermat::internal::Arg &_arg;
    wchar_t                _type;

    FORMAT_DISALLOW_COPY_AND_ASSIGN(ArgConverter);

public:
    ArgConverter(fermat::internal::Arg &arg, wchar_t type)
        : _arg(arg), _type(type)
    {}

    void visit_bool(bool value)
    {
        if (_type != 's')
            visit_any_int(value);
    }

    template <typename U>
    void visit_any_int(U value)
    {
        bool is_signed = type_ == 'd' || type_ == 'i';
        using fermat::internal::Arg;
        typedef typename fermat::internal::Conditional<
            is_same<T, void>::value, U, T>::type TargetType;
        if (sizeof(TargetType) <= sizeof(int)) {
            // Extra casts are used to silence warnings.
            if (is_signed) {
                _arg.type = Arg::INT;
                _arg.int_value = static_cast<int>(static_cast<TargetType>(value));
            } else {
                _arg.type = Arg::UINT;
                typedef typename fermat::internal::MakeUnsigned<TargetType>::Type Unsigned;
                _arg.uint_value = static_cast<unsigned>(static_cast<Unsigned>(value));
            }
        } else {
            if (is_signed) {
                _arg.type = Arg::LONG_LONG;
                // glibc's printf doesn't sign extend arguments of smaller types:
                //   std::printf("%lld", -42);  // prints "4294967254"
                // but we don't have to do the same because it's a UB.
                _arg.long_long_value = static_cast<fermat::LongLong>(value);
            } else {
                _arg.type = Arg::ULONG_LONG;
                _arg.ulong_long_value =
                    static_cast<typename fermat::internal::MakeUnsigned<U>::Type>(value);
            }
        }
    }
};

// Converts an integer argument to char for printf.
class CharConverter: public fermat::internal::ArgVisitor<CharConverter, void>
{
private:
    fermat::internal::Arg &_arg;

    FORMAT_DISALLOW_COPY_AND_ASSIGN(CharConverter);

public:
    explicit CharConverter(fermat::internal::Arg &arg): _arg(arg)
    {}

    template <typename T>
    void visit_any_int(T value)
    {
        _arg.type = Arg::CHAR;
        _arg.int_value = static_cast<char>(value);
    }
};

// Write the content of w to os.
void write(std::ostream &os, fermat::Writer &w)
{
    const char *data = w.data();
    typedef internal::MakeUnsigned<std::streamsize>::Type UnsignedStreamSize;
    UnsignedStreamSize size = w.size();
    UnsignedStreamSize max_size =
        internal::to_unsigned((std::numeric_limits<std::streamsize>::max)());
    do {
        UnsignedStreamSize n = size <= max_size ? size : max_size;
        os.write(data, static_cast<std::streamsize>(n));
        data += n;
        size -= n;
    } while (size != 0);
}
}  // namespace

namespace internal {

template <typename Char>
class PrintfArgFormatter:
public ArgFormatterBase<PrintfArgFormatter<Char>, Char>
{

    void write_null_pointer()
    {
        this->spec()._type = 0;
        this->write("(nil)");
    }

    typedef ArgFormatterBase<PrintfArgFormatter<Char>, Char> Base;

public:
    PrintfArgFormatter(BasicWriter<Char> &w, FormatSpec &s)
        : ArgFormatterBase<PrintfArgFormatter<Char>, Char>(w, s)
    {}

    void visit_bool(bool value)
    {
        FormatSpec &fmt_spec = this->spec();
        if (fmt_spec._type != 's')
            return this->visit_any_int(value);
        fmt_spec._type = 0;
        this->write(value);
    }

    void visit_char(int value)
    {
        const FormatSpec &fmt_spec = this->spec();
        BasicWriter<Char> &w = this->writer();
        if (fmt_spec.type_ && fmt_spec._type != 'c')
            w.write_int(value, fmt_spec);
        typedef typename BasicWriter<Char>::CharPtr CharPtr;
        CharPtr out = CharPtr();
        if (fmt_spec._width > 1) {
            Char fill = ' ';
            out = w.grow_buffer(fmt_spec._width);
            if (fmt_spec._align != ALIGN_LEFT) {
                std::fill_n(out, fmt_spec._width - 1, fill);
                out += fmt_spec._width - 1;
            } else {
                std::fill_n(out + 1, fmt_spec._width - 1, fill);
            }
        } else {
            out = w.grow_buffer(1);
        }
        *out = static_cast<Char>(value);
    }

    void visit_cstring(const char *value)
    {
        if (value)
            Base::visit_cstring(value);
        else if (this->spec().type_ == 'p')
            write_null_pointer();
        else
            this->write("(null)");
    }

    void visit_pointer(const void *value)
    {
        if (value)
            return Base::visit_pointer(value);
        this->spec()._type = 0;
        write_null_pointer();
    }

    void visit_custom(Arg::CustomValue c)
    {
        BasicFormatter<Char> formatter(ArgList(), this->writer());
        const Char format_str[] = { '}', 0 };
        const Char *format = format_str;
        c.format(&formatter, c.value, &format);
    }
};

}
}

void fermat::SystemError::init(
	int err_code, CStringRef format_str, ArgList args)
{
	error_code_ = err_code;
	MemoryWriter w;
	internal::format_system_error(w, err_code, format(format_str, args));
	std::runtime_error &base = *this;
	base = std::runtime_error(w.str());
}

template <typename T>
int fermat::internal::CharTraits<char>::format_float(
	char *buffer, std::size_t size, const char *format,
	unsigned width, int precision, T value)
{
	if (width == 0) {
		return precision < 0 ?
			FORMAT_SNPRINTF(buffer, size, format, value) :
			FORMAT_SNPRINTF(buffer, size, format, precision, value);
	}
	return precision < 0 ?
		FORMAT_SNPRINTF(buffer, size, format, width, value) :
		FORMAT_SNPRINTF(buffer, size, format, width, precision, value);
}

template <typename T>
int fermat::internal::CharTraits<wchar_t>::format_float(
	wchar_t *buffer, std::size_t size, const wchar_t *format,
	unsigned width, int precision, T value)
{
	if (width == 0) {
		return precision < 0 ?
			FORMAT_SWPRINTF(buffer, size, format, value) :
			FORMAT_SWPRINTF(buffer, size, format, precision, value);
	}
	return precision < 0 ?
		FORMAT_SWPRINTF(buffer, size, format, width, value) :
		FORMAT_SWPRINTF(buffer, size, format, width, precision, value);
}

template <typename T>
const char fermat::internal::BasicData<T>::DIGITS[] =
"0001020304050607080910111213141516171819"
"2021222324252627282930313233343536373839"
"4041424344454647484950515253545556575859"
"6061626364656667686970717273747576777879"
"8081828384858687888990919293949596979899";

#define FORMAT_POWERS_OF_10(factor) \
  factor * 10, \
  factor * 100, \
  factor * 1000, \
  factor * 10000, \
  factor * 100000, \
  factor * 1000000, \
  factor * 10000000, \
  factor * 100000000, \
  factor * 1000000000

template <typename T>
const uint32_t fermat::internal::BasicData<T>::POWERS_OF_10_32[] = {
	0, FORMAT_POWERS_OF_10(1)
};

template <typename T>
const uint64_t fermat::internal::BasicData<T>::POWERS_OF_10_64[] = {
	0,
	FORMAT_POWERS_OF_10(1),
	FORMAT_POWERS_OF_10(fermat::ULongLong(1000000000)),
	// Multiply several constants instead of using a single long long constant
	// to avoid warnings about C++98 not supporting long long.
	fermat::ULongLong(1000000000) * fermat::ULongLong(1000000000) * 10
};

void fermat::internal::report_unknown_type(char code, const char *type)
{
	(void)type;
	if (std::isprint(static_cast<unsigned char>(code))) {
		FORMAT_THROW(fermat::FormatError(
			fermat::format("unknown format code '{}' for {}", code, type)));
	}
	FORMAT_THROW(fermat::FormatError(
		fermat::format("unknown format code '\\x{:02x}' for {}",
			static_cast<unsigned>(code), type)));
}

#if FORMAT_USE_WINDOWS_H

fermat::internal::UTF8ToUTF16::UTF8ToUTF16(fermat::StringRef s)
{
	static const char ERROR_MSG[] = "cannot convert string from UTF-8 to UTF-16";
	if (s.size() > INT_MAX)
		FORMAT_THROW(WindowsError(ERROR_INVALID_PARAMETER, ERROR_MSG));
	int s_size = static_cast<int>(s.size());
	int length = MultiByteToWideChar(
		CP_UTF8, MB_ERR_INVALID_CHARS, s.data(), s_size, 0, 0);
	if (length == 0)
		FORMAT_THROW(WindowsError(GetLastError(), ERROR_MSG));
	buffer_.resize(length + 1);
	length = MultiByteToWideChar(
		CP_UTF8, MB_ERR_INVALID_CHARS, s.data(), s_size, &buffer_[0], length);
	if (length == 0)
		FORMAT_THROW(WindowsError(GetLastError(), ERROR_MSG));
	buffer_[length] = 0;
}

fermat::internal::UTF16ToUTF8::UTF16ToUTF8(fermat::WStringRef s)
{
	if (int error_code = convert(s)) {
		FORMAT_THROW(WindowsError(error_code,
			"cannot convert string from UTF-16 to UTF-8"));
	}
}

int fermat::internal::UTF16ToUTF8::convert(fermat::WStringRef s)
{
	if (s.size() > INT_MAX)
		return ERROR_INVALID_PARAMETER;
	int s_size = static_cast<int>(s.size());
	int length = WideCharToMultiByte(CP_UTF8, 0, s.data(), s_size, 0, 0, 0, 0);
	if (length == 0)
		return GetLastError();
	buffer_.resize(length + 1);
	length = WideCharToMultiByte(
		CP_UTF8, 0, s.data(), s_size, &buffer_[0], length, 0, 0);
	if (length == 0)
		return GetLastError();
	buffer_[length] = 0;
	return 0;
}

void fermat::WindowsError::init(
	int err_code, CStringRef format_str, ArgList args)
{
	error_code_ = err_code;
	MemoryWriter w;
	internal::format_windows_error(w, err_code, format(format_str, args));
	std::runtime_error &base = *this;
	base = std::runtime_error(w.str());
}

void fermat::internal::format_windows_error(
	fermat::Writer &out, int error_code,
	fermat::StringRef message) FORMAT_NOEXCEPT
{
	FORMAT_TRY{
		MemoryBuffer<wchar_t, INLINE_BUFFER_SIZE> buffer;
	buffer.resize(INLINE_BUFFER_SIZE);
	for (;;) {
		wchar_t *system_message = &buffer[0];
		int result = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			system_message, static_cast<uint32_t>(buffer.size()), 0);
		if (result != 0) {
			UTF16ToUTF8 utf8_message;
			if (utf8_message.convert(system_message) == ERROR_SUCCESS) {
				out << message << ": " << utf8_message;
				return;
			}
			break;
		}
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			break;  // Can't get error message, report error code instead.
		buffer.resize(buffer.size() * 2);
	}
	} FORMAT_CATCH(...)
	{}
	fermat::format_error_code(out, error_code, message);  // 'fermat::' is for bcc32.
}

#endif  // FORMAT_USE_WINDOWS_H

void fermat::internal::format_system_error(
	fermat::Writer &out, int error_code,
	fermat::StringRef message) FORMAT_NOEXCEPT
{
	FORMAT_TRY{
		MemoryBuffer<char, INLINE_BUFFER_SIZE> buffer;
	buffer.resize(INLINE_BUFFER_SIZE);
	for (;;) {
		char *system_message = &buffer[0];
		int result = safe_strerror(error_code, system_message, buffer.size());
		if (result == 0) {
			out << message << ": " << system_message;
			return;
		}
		if (result != ERANGE)
			break;  // Can't get error message, report error code instead.
		buffer.resize(buffer.size() * 2);
	}
	} FORMAT_CATCH(...)
	{}
	fermat::format_error_code(out, error_code, message);  // 'fermat::' is for bcc32.
}

template <typename Char>
void fermat::internal::ArgMap<Char>::init(const ArgList &args)
{
	if (!map_.empty())
		return;
	typedef internal::NamedArg<Char> NamedArg;
	const NamedArg *named_arg = 0;
	bool use_values =
		args.type(ArgList::MAX_PACKED_ARGS - 1) == internal::Arg::NONE;
	if (use_values) {
		for (unsigned i = 0;/*nothing*/; ++i) {
			internal::Arg::Type arg_type = args.type(i);
			switch (arg_type) {
			case internal::Arg::NONE:
				return;
			case internal::Arg::NAMED_ARG:
				named_arg = static_cast<const NamedArg*>(args.values_[i].pointer);
				map_.push_back(Pair(named_arg->name, *named_arg));
				break;
			default:
				/*nothing*/;
			}
		}
		return;
	}
	for (unsigned i = 0; i != ArgList::MAX_PACKED_ARGS; ++i) {
		internal::Arg::Type arg_type = args.type(i);
		if (arg_type == internal::Arg::NAMED_ARG) {
			named_arg = static_cast<const NamedArg*>(args.args_[i].pointer);
			map_.push_back(Pair(named_arg->name, *named_arg));
		}
	}
	for (unsigned i = ArgList::MAX_PACKED_ARGS;/*nothing*/; ++i) {
		switch (args.args_[i].type) {
		case internal::Arg::NONE:
			return;
		case internal::Arg::NAMED_ARG:
			named_arg = static_cast<const NamedArg*>(args.args_[i].pointer);
			map_.push_back(Pair(named_arg->name, *named_arg));
			break;
		default:
			/*nothing*/;
		}
	}
}

template <typename Char>
void fermat::internal::FixedBuffer<Char>::grow(std::size_t)
{
	FORMAT_THROW(std::runtime_error("buffer overflow"));
}

Arg fermat::internal::FormatterBase::do_get_arg(
	unsigned arg_index, const char *&error)
{
	Arg arg = args_[arg_index];
	switch (arg.type) {
	case Arg::NONE:
		error = "argument index out of range";
		break;
	case Arg::NAMED_ARG:
		arg = *static_cast<const internal::Arg*>(arg.pointer);
		break;
	default:
		/*nothing*/;
	}
	return arg;
}

template <typename Char>
void fermat::internal::PrintfFormatter<Char>::parse_flags(
	FormatSpec &spec, const Char *&s)
{
	for (;;) {
		switch (*s++) {
		case '-':
			spec.align_ = ALIGN_LEFT;
			break;
		case '+':
			spec.flags_ |= SIGN_FLAG | PLUS_FLAG;
			break;
		case '0':
			spec.fill_ = '0';
			break;
		case ' ':
			spec.flags_ |= SIGN_FLAG;
			break;
		case '#':
			spec.flags_ |= HASH_FLAG;
			break;
		default:
			--s;
			return;
		}
	}
}

template <typename Char>
Arg fermat::internal::PrintfFormatter<Char>::get_arg(
	const Char *s, unsigned arg_index)
{
	(void)s;
	const char *error = 0;
	Arg arg = arg_index == UINT_MAX ?
		next_arg(error) : FormatterBase::get_arg(arg_index - 1, error);
	if (error)
		FORMAT_THROW(FormatError(!*s ? "invalid format string" : error));
	return arg;
}

template <typename Char>
unsigned fermat::internal::PrintfFormatter<Char>::parse_header(
	const Char *&s, FormatSpec &spec)
{
	unsigned arg_index = UINT_MAX;
	Char c = *s;
	if (c >= '0' && c <= '9') {
		// Parse an argument index (if followed by '$') or a width possibly
		// preceded with '0' flag(s).
		unsigned value = parse_nonnegative_int(s);
		if (*s == '$') {  // value is an argument index
			++s;
			arg_index = value;
		}
		else {
			if (c == '0')
				spec.fill_ = '0';
			if (value != 0) {
				// Nonzero value means that we parsed width and don't need to
				// parse it or flags again, so return now.
				spec.width_ = value;
				return arg_index;
			}
		}
	}
	parse_flags(spec, s);
	// Parse width.
	if (*s >= '0' && *s <= '9') {
		spec.width_ = parse_nonnegative_int(s);
	}
	else if (*s == '*') {
		++s;
		spec.width_ = WidthHandler(spec).visit(get_arg(s));
	}
	return arg_index;
}

template <typename Char>
void fermat::internal::PrintfFormatter<Char>::format(
	BasicWriter<Char> &writer, BasicCStringRef<Char> format_str)
{
	const Char *start = format_str.c_str();
	const Char *s = start;
	while (*s) {
		Char c = *s++;
		if (c != '%') continue;
		if (*s == c) {
			write(writer, start, s);
			start = ++s;
			continue;
		}
		write(writer, start, s - 1);

		FormatSpec spec;
		spec.align_ = ALIGN_RIGHT;

		// Parse argument index, flags and width.
		unsigned arg_index = parse_header(s, spec);

		// Parse precision.
		if (*s == '.') {
			++s;
			if ('0' <= *s && *s <= '9') {
				spec.precision_ = static_cast<int>(parse_nonnegative_int(s));
			}
			else if (*s == '*') {
				++s;
				spec.precision_ = PrecisionHandler().visit(get_arg(s));
			}
		}

		Arg arg = get_arg(s, arg_index);
		if (spec.flag(HASH_FLAG) && IsZeroInt().visit(arg))
			spec.flags_ &= ~to_unsigned<int>(HASH_FLAG);
		if (spec.fill_ == '0') {
			if (arg.type <= Arg::LAST_NUMERIC_TYPE)
				spec.align_ = ALIGN_NUMERIC;
			else
				spec.fill_ = ' ';  // Ignore '0' flag for non-numeric types.
		}

		// Parse length and convert the argument to the required type.
		switch (*s++) {
		case 'h':
			if (*s == 'h')
				ArgConverter<signed char>(arg, *++s).visit(arg);
			else
				ArgConverter<short>(arg, *s).visit(arg);
			break;
		case 'l':
			if (*s == 'l')
				ArgConverter<fermat::LongLong>(arg, *++s).visit(arg);
			else
				ArgConverter<long>(arg, *s).visit(arg);
			break;
		case 'j':
			ArgConverter<intmax_t>(arg, *s).visit(arg);
			break;
		case 'z':
			ArgConverter<std::size_t>(arg, *s).visit(arg);
			break;
		case 't':
			ArgConverter<std::ptrdiff_t>(arg, *s).visit(arg);
			break;
		case 'L':
			// printf produces garbage when 'L' is omitted for long double, no
			// need to do the same.
			break;
		default:
			--s;
			ArgConverter<void>(arg, *s).visit(arg);
		}

		// Parse type.
		if (!*s)
			FORMAT_THROW(FormatError("invalid format string"));
		spec.type_ = static_cast<char>(*s++);
		if (arg.type <= Arg::LAST_INTEGER_TYPE) {
			// Normalize type.
			switch (spec.type_) {
			case 'i': case 'u':
				spec.type_ = 'd';
				break;
			case 'c':
				// TODO: handle wchar_t
				CharConverter(arg).visit(arg);
				break;
			}
		}

		start = s;

		// Format argument.
		internal::PrintfArgFormatter<Char>(writer, spec).visit(arg);
	}
	write(writer, start, s);
}

void fermat::report_system_error(
	int error_code, fermat::StringRef message) FORMAT_NOEXCEPT
{
	// 'fermat::' is for bcc32.
	fermat::report_error(internal::format_system_error, error_code, message);
}

#if FORMAT_USE_WINDOWS_H
void fermat::report_windows_error(
	int error_code, fermat::StringRef message) FORMAT_NOEXCEPT
{
	// 'fermat::' is for bcc32.
	fermat::report_error(internal::format_windows_error, error_code, message);
}
#endif

void fermat::print(std::FILE *f, CStringRef format_str, ArgList args)
{
	MemoryWriter w;
	w.write(format_str, args);
	std::fwrite(w.data(), 1, w.size(), f);
}

void fermat::print(CStringRef format_str, ArgList args)
{
	print(stdout, format_str, args);
}

void fermat::print(std::ostream &os, CStringRef format_str,
	ArgList args)
{
	MemoryWriter w;
	w.write(format_str, args);
	write(os, w);
}

void fermat::print_colored(Color c, CStringRef format, ArgList args)
{
	char escape[] = "\x1b[30m";
	escape[3] = static_cast<char>('0' + c);
	std::fputs(escape, stdout);
	print(format, args);
	std::fputs(RESET_COLOR, stdout);
}

int fermat::fprintf(std::FILE *f, CStringRef format, ArgList args)
{
	MemoryWriter w;
	printf(w, format, args);
	std::size_t size = w.size();
	return std::fwrite(w.data(), 1, size, f) < size ? -1 : static_cast<int>(size);
}

int fermat::fprintf(std::ostream &os, CStringRef format, ArgList args)
{
	MemoryWriter w;
	printf(w, format, args);
	write(os, w);
	return static_cast<int>(w.size());
}
