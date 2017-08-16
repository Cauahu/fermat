#ifndef FERMAT_COMMON_FORMAT_H_
#define FERMAT_COMMON_FORMAT_H_

#include <fermat/common/string_ref.h>
#include <fermat/common/stack_buffer.h>
#include <fermat/common/basic_buffer.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>
#include <ostream>
#include <iterator>
#include <cstdint>
#include <utility>


#ifdef __GNUC__
# define FORMAT_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
# define FORMAT_GCC_EXTENSION __extension__
# if FORMAT_GCC_VERSION >= 406
#  pragma GCC diagnostic push
// Disable the warning about "long long" which is sometimes reported even
// when using __extension__.
#  pragma GCC diagnostic ignored "-Wlong-long"
// Disable the warning about declaration shadowing because it affects too
// many valid cases.
#  pragma GCC diagnostic ignored "-Wshadow"
// Disable the warning about implicit conversions that may change the sign of
// an integer; silencing it otherwise would require many explicit casts.
#  pragma GCC diagnostic ignored "-Wsign-conversion"
# endif
# if __cplusplus >= 201103L || defined __GXX_EXPERIMENTAL_CXX0X__
#  define FORMAT_HAS_GXX_CXX11 1
# endif
#else
# define FORMAT_GCC_EXTENSION
#endif

#if defined(__clang__) && !defined(__INTEL_COMPILER)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation"
#endif

#ifdef __GNUC_LIBSTD__
# define FORMAT_GNUC_LIBSTD_VERSION (__GNUC_LIBSTD__ * 100 + __GNUC_LIBSTD_MINOR__)
#endif

#ifdef __has_feature
# define FORMAT_HAS_FEATURE(x) __has_feature(x)
#else
# define FORMAT_HAS_FEATURE(x) 0
#endif

#ifdef __has_builtin
# define FORMAT_HAS_BUILTIN(x) __has_builtin(x)
#else
# define FORMAT_HAS_BUILTIN(x) 0
#endif

#ifdef __has_cpp_attribute
# define FORMAT_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
# define FORMAT_HAS_CPP_ATTRIBUTE(x) 0
#endif

#ifndef FORMAT_USE_VARIADIC_TEMPLATES
// Variadic templates are available in GCC since version 4.4
// (http://gcc.gnu.org/projects/cxx0x.html) and in Visual C++
// since version 2013.
# define FORMAT_USE_VARIADIC_TEMPLATES \
   (FORMAT_HAS_FEATURE(cxx_variadic_templates) || \
       (FORMAT_GCC_VERSION >= 404 && FORMAT_HAS_GXX_CXX11) || _MSC_VER >= 1800)
#endif

#ifndef FORMAT_USE_RVALUE_REFERENCES
// Don't use rvalue references when compiling with clang and an old libstdc++
// as the latter doesn't provide std::move.
# if defined(FORMAT_GNUC_LIBSTD_VERSION) && FORMAT_GNUC_LIBSTD_VERSION <= 402
#  define FORMAT_USE_RVALUE_REFERENCES 0
# else
#  define FORMAT_USE_RVALUE_REFERENCES \
    (FORMAT_HAS_FEATURE(cxx_rvalue_references) || \
        (FORMAT_GCC_VERSION >= 403 && FORMAT_HAS_GXX_CXX11) || _MSC_VER >= 1600)
# endif
#endif

// Check if exceptions are disabled.
#if defined(__GNUC__) && !defined(__EXCEPTIONS)
# define FORMAT_EXCEPTIONS 0
#endif
#if defined(_MSC_VER) && !_HAS_EXCEPTIONS
# define FORMAT_EXCEPTIONS 0
#endif
#ifndef FORMAT_EXCEPTIONS
# define FORMAT_EXCEPTIONS 1
#endif

#ifndef FORMAT_THROW
# if FORMAT_EXCEPTIONS
#  define FORMAT_THROW(x) throw x
# else
#  define FORMAT_THROW(x) assert(false)
# endif
#endif

// Define FORMAT_USE_NOEXCEPT to make C++ Format use noexcept (C++11 feature).
#ifndef FORMAT_USE_NOEXCEPT
# define FORMAT_USE_NOEXCEPT 0
#endif

#ifndef FORMAT_NOEXCEPT
# if FORMAT_EXCEPTIONS
#  if FORMAT_USE_NOEXCEPT || FORMAT_HAS_FEATURE(cxx_noexcept) || \
    (FORMAT_GCC_VERSION >= 408 && FORMAT_HAS_GXX_CXX11) || \
    _MSC_VER >= 1900
#   define FORMAT_NOEXCEPT noexcept
#  else
#   define FORMAT_NOEXCEPT throw()
#  endif
# else
#  define FORMAT_NOEXCEPT
# endif
#endif

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#ifndef FORMAT_USE_DELETED_FUNCTIONS
# define FORMAT_USE_DELETED_FUNCTIONS 0
#endif

#if FORMAT_USE_DELETED_FUNCTIONS || FORMAT_HAS_FEATURE(cxx_deleted_functions) || \
  (FORMAT_GCC_VERSION >= 404 && FORMAT_HAS_GXX_CXX11) || _MSC_VER >= 1800
# define FORMAT_DELETED_OR_UNDEFINED  = delete
# define FORMAT_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete
#else
# define FORMAT_DELETED_OR_UNDEFINED
# define FORMAT_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    TypeName& operator=(const TypeName&)
#endif

#ifndef FORMAT_USE_USER_DEFINED_LITERALS
// All compilers which support UDLs also support variadic templates. This
// makes the fermat::literals implementation easier. However, an explicit check
// for variadic templates is added here just in case.
# define FORMAT_USE_USER_DEFINED_LITERALS \
   FORMAT_USE_VARIADIC_TEMPLATES && FORMAT_USE_RVALUE_REFERENCES && \
   (FORMAT_HAS_FEATURE(cxx_user_literals) || \
       (FORMAT_GCC_VERSION >= 407 && FORMAT_HAS_GXX_CXX11) || _MSC_VER >= 1900)
#endif

#ifndef FORMAT_ASSERT
# define FORMAT_ASSERT(condition, message) assert((condition) && message)
#endif


#if FORMAT_GCC_VERSION >= 400 || FORMAT_HAS_BUILTIN(__builtin_clz)
# define FORMAT_BUILTIN_CLZ(n) __builtin_clz(n)
#endif

#if FORMAT_GCC_VERSION >= 400 || FORMAT_HAS_BUILTIN(__builtin_clzll)
# define FORMAT_BUILTIN_CLZLL(n) __builtin_clzll(n)
#endif

#  pragma GCC diagnostic ignored "-Wshadow"

namespace fermat {

namespace internal {
struct DummyInt
{
    int data[2];
    operator int() const
    {
        return 0;
    }
};
typedef std::numeric_limits<fermat::internal::DummyInt> FPUtil;

// Dummy implementations of system functions such as signbit and ecvt called
// if the latter are not available.
inline DummyInt signbit(...)
{
    return DummyInt();
}
inline DummyInt _ecvt_s(...)
{
    return DummyInt();
}
inline DummyInt isinf(...)
{
    return DummyInt();
}
inline DummyInt _finite(...)
{
    return DummyInt();
}
inline DummyInt isnan(...)
{
    return DummyInt();
}
inline DummyInt _isnan(...)
{
    return DummyInt();
}

// A helper function to suppress bogus "conditional expression is constant"
// warnings.
template <typename T>
inline T check(T value)
{
    return value;
}
} //namespace internal

} //namespace fermat

namespace std
{
// Standard permits specialization of std::numeric_limits. This specialization
// is used to resolve ambiguity between isinf and std::isinf in glibc:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=48891
// and the same for isnan and signbit.
template <>
class numeric_limits<fermat::internal::DummyInt>:
    public std::numeric_limits<int> {
public:
    // Portable version of isinf.
    template <typename T>
    static bool isinfinity(T x)
    {
        using namespace fermat::internal;
        // The resolution "priority" is:
        // isinf macro > std::isinf > ::isinf > fermat::internal::isinf
        if (check(sizeof(isinf(x)) == sizeof(bool) ||
                  sizeof(isinf(x)) == sizeof(int))) {
            return isinf(x) != 0;
        }
        return !_finite(static_cast<double>(x));
    }

    // Portable version of isnan.
    template <typename T>
    static bool isnotanumber(T x)
    {
        using namespace fermat::internal;
        if (check(sizeof(isnan(x)) == sizeof(bool) ||
                  sizeof(isnan(x)) == sizeof(int))) {
            return isnan(x) != 0;
        }
        return _isnan(static_cast<double>(x)) != 0;
    }

    // Portable version of signbit.
    static bool isnegative(double x)
    {
        using namespace fermat::internal;
        if (check(sizeof(signbit(x)) == sizeof(int)))
            return signbit(x) != 0;
        if (x < 0) return true;
        if (!isnotanumber(x)) return false;
        int dec = 0, sign = 0;
        char buffer[2];  // The buffer size must be >= 2 or _ecvt_s will fail.
        _ecvt_s(buffer, sizeof(buffer), x, 0, &dec, &sign);
        return sign != 0;
    }
};
}  // namespace std

namespace fermat{

FORMAT_GCC_EXTENSION typedef long long LongLong;
FORMAT_GCC_EXTENSION typedef unsigned long long ULongLong;
#if FORMAT_USE_RVALUE_REFERENCES
using std::move;
#endif

template <typename Char>
class BasicWriter;

typedef BasicWriter<char> Writer;
typedef BasicWriter<wchar_t> WWriter;

namespace internal
{
template <typename Char>
class BasicArgFormatter;
}

template <typename CharType,
          typename ArgFormatter = internal::BasicArgFormatter<CharType> >
class BasicFormatter;

template <typename Char, typename T>
void format(BasicFormatter<Char> &f, const Char *&format_str, const T &value);

class FormatError: public std::runtime_error {
public:
    explicit FormatError(CStringRef message)
        : std::runtime_error(message.c_str())
    {}
};

namespace internal {

template <typename T>
struct MakeUnsigned
{
    typedef T Type;
};

#define FORMAT_SPECIALIZE_MAKE_UNSIGNED(T, U) \
  template <> \
  struct MakeUnsigned<T> { typedef U Type; }

FORMAT_SPECIALIZE_MAKE_UNSIGNED(char, unsigned char);
FORMAT_SPECIALIZE_MAKE_UNSIGNED(signed char, unsigned char);
FORMAT_SPECIALIZE_MAKE_UNSIGNED(short, unsigned short);
FORMAT_SPECIALIZE_MAKE_UNSIGNED(int, unsigned);
FORMAT_SPECIALIZE_MAKE_UNSIGNED(long, unsigned long);
FORMAT_SPECIALIZE_MAKE_UNSIGNED(LongLong, ULongLong);

// Casts nonnegative integer to unsigned.
template <typename Int>
inline typename MakeUnsigned<Int>::Type to_unsigned(Int value)
{
    FORMAT_ASSERT(value >= 0, "negative value");
    return static_cast<typename MakeUnsigned<Int>::Type>(value);
}

// The number of characters to store in the StackBuffer object itself
// to avoid dynamic memory allocation.
enum
{
    INLINE_BUFFER_SIZE = 500
};

template <typename Char>
class FixedBuffer: public fermat::BasicBuffer<Char>
{
public:
    FixedBuffer(Char *array, std::size_t size): fermat::BasicBuffer<Char>(array, size)
    {}

protected:
    void grow(std::size_t size);
};

template <typename Char>
class BasicCharTraits {
public:
    typedef Char *CharPtr;
    static Char cast(int value)
    {
        return static_cast<Char>(value);
    }
};


template <typename Char>
class CharTraits;

template <>
class CharTraits<char>: public BasicCharTraits<char>
{
private:
    // Conversion from wchar_t to char is not allowed.
    static char convert(wchar_t);

public:
    static char convert(char value)
    {
        return value;
    }

    // Formats a floating-point number.
    template <typename T>
    static int format_float(char *buffer, std::size_t size,
                                    const char *format, unsigned width, int precision, T value);
};

template <>
class CharTraits<wchar_t>: public BasicCharTraits<wchar_t>
{
public:
    static wchar_t convert(char value)
    {
        return value;
    }
    static wchar_t convert(wchar_t value)
    {
        return value;
    }

    template <typename T>
    static int format_float(wchar_t *buffer, std::size_t size,
                                    const wchar_t *format, unsigned width, int precision, T value);
};

// Checks if a number is negative - used to avoid warnings.
template <bool IsSigned>
struct SignChecker
{
    template <typename T>
    static bool is_negative(T value)
    {
        return value < 0;
    }
};

template <>
struct SignChecker<false>
{
    template <typename T>
    static bool is_negative(T)
    {
        return false;
    }
};

// Returns true if value is negative, false otherwise.
// Same as (value < 0) but doesn't produce warnings if T is an unsigned type.
template <typename T>
inline bool is_negative(T value)
{
    return SignChecker<std::numeric_limits<T>::is_signed>::is_negative(value);
}

// Selects uint32_t if FitsIn32Bits is true, uint64_t otherwise.
template <bool FitsIn32Bits>
struct TypeSelector
{
    typedef uint32_t Type;
};

template <>
struct TypeSelector<false>
{
    typedef uint64_t Type;
};

template <typename T>
struct IntTraits
{
    // Smallest of uint32_t and uint64_t that is large enough to represent
    // all values of T.
    typedef typename
    TypeSelector<std::numeric_limits<T>::digits <= 32>::Type MainType;
};

 void report_unknown_type(char code, const char *type);

// Static data is placed in this class template to allow header-only
// configuration.
template <typename T = void>
struct  BasicData
{
    static const uint32_t POWERS_OF_10_32[];
    static const uint64_t POWERS_OF_10_64[];
    static const char DIGITS[];
};

typedef BasicData<> Data;

#ifdef FORMAT_BUILTIN_CLZLL
// Returns the number of decimal digits in n. Leading zeros are not counted
// except for n == 0 in which case count_digits returns 1.
inline unsigned count_digits(uint64_t n)
{
    // Based on http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10
    // and the benchmark https://github.com/localvoid/cxx-benchmark-count-digits.
    int t = (64 - FORMAT_BUILTIN_CLZLL(n | 1)) * 1233 >> 12;
    return to_unsigned(t) - (n < Data::POWERS_OF_10_64[t]) + 1;
}
#else
// Fallback version of count_digits used when __builtin_clz is not available.
inline unsigned count_digits(uint64_t n)
{
    unsigned count = 1;
    for (;;) {
        // Integer division is slow so do it for a group of four digits instead
        // of for every digit. The idea comes from the talk by Alexandrescu
        // "Three Optimization Tips for C++". See speed-test for a comparison.
        if (n < 10) return count;
        if (n < 100) return count + 1;
        if (n < 1000) return count + 2;
        if (n < 10000) return count + 3;
        n /= 10000u;
        count += 4;
    }
}
#endif

#ifdef FORMAT_BUILTIN_CLZ
// Optional version of count_digits for better performance on 32-bit platforms.
inline unsigned count_digits(uint32_t n)
{
    int t = (32 - FORMAT_BUILTIN_CLZ(n | 1)) * 1233 >> 12;
    return to_unsigned(t) - (n < Data::POWERS_OF_10_32[t]) + 1;
}
#endif

// Formats a decimal unsigned integer value writing into buffer.
template <typename UInt, typename Char>
inline void format_decimal(Char *buffer, UInt value, unsigned num_digits)
{
    buffer += num_digits;
    while (value >= 100) {
        // Integer division is slow so do it for a group of two digits instead
        // of for every digit. The idea comes from the talk by Alexandrescu
        // "Three Optimization Tips for C++". See speed-test for a comparison.
        unsigned index = static_cast<unsigned>((value % 100) * 2);
        value /= 100;
        *--buffer = Data::DIGITS[index + 1];
        *--buffer = Data::DIGITS[index];
    }
    if (value < 10) {
        *--buffer = static_cast<char>('0' + value);
        return;
    }
    unsigned index = static_cast<unsigned>(value * 2);
    *--buffer = Data::DIGITS[index + 1];
    *--buffer = Data::DIGITS[index];
}

void format_system_error(fermat::Writer &out, int error_code,
                                 fermat::StringRef message) FORMAT_NOEXCEPT;

// A formatting argument value.
struct Value {
    template <typename Char>
    struct StringValue {
        const Char *value;
        std::size_t size;
    };

    typedef void(*FormatFunc)(
        void *formatter, const void *arg, void *format_str_ptr);

    struct CustomValue {
        const void *value;
        FormatFunc format;
    };

    union {
        int                         int_value;
        unsigned                    uint_value;
        LongLong                    long_long_value;
        ULongLong                   ulong_long_value;
        double                      double_value;
        long double                 long_double_value;
        const void                 *pointer;
        StringValue<char>           string;
        StringValue<signed char>    sstring;
        StringValue<unsigned char>  ustring;
        StringValue<wchar_t>        wstring;
        CustomValue                 custom;
    };

    enum Type
    {
        NONE, 
        NAMED_ARG,
        // Integer types should go first,
        INT, 
        UINT, 
        LONG_LONG, 
        ULONG_LONG, 
        BOOL, 
        CHAR, 
        LAST_INTEGER_TYPE = CHAR,
        // followed by floating-point types.
        DOUBLE, 
        LONG_DOUBLE, 
        LAST_NUMERIC_TYPE = LONG_DOUBLE,
        CSTRING, 
        STRING, 
        WSTRING, 
        POINTER, 
        CUSTOM
    };
};

// A formatting argument. It is a trivially copyable/constructible type to
// allow storage in StackBuffer.
struct Arg: Value
{
    Type type;
};

template <typename Char>
struct NamedArg;

template <typename T = void>
struct Null
{};

// A helper class template to enable or disable overloads taking wide
// characters and strings in MakeValue.
template <typename T, typename Char>
struct WCharHelper
{
    typedef Null<T> Supported;
    typedef T Unsupported;
};

template <typename T>
struct WCharHelper<T, wchar_t>
{
    typedef T Supported;
    typedef Null<T> Unsupported;
};

typedef char Yes[1];
typedef char No[2];

// These are non-members to workaround an overload resolution bug in bcc32.
Yes &convert(fermat::ULongLong);
Yes &convert(std::ostream &);
No &convert(...);

template <typename T>
T &get();

struct DummyStream: std::ostream {
    DummyStream();  // Suppress a bogus warning in MSVC.
    // Hide all operator<< overloads from std::ostream.
    void operator<<(Null<>);
};

No &operator<<(std::ostream &, int);

template<typename T, bool ENABLE_CONVERSION>
struct ConvertToIntImpl {
    enum {
        value = false
    };
};

template<typename T>
struct ConvertToIntImpl<T, true> {
    // Convert to int only if T doesn't have an overloaded operator<<.
    enum {
        value = sizeof(convert(get<DummyStream>() << get<T>())) == sizeof(No)
    };
};

template<typename T, bool ENABLE_CONVERSION>
struct ConvertToIntImpl2 {
    enum {
        value = false
    };
};

template<typename T>
struct ConvertToIntImpl2<T, true> {
    enum {
        // Don't convert numeric types.
        value = ConvertToIntImpl<T, !std::numeric_limits<T>::is_specialized>::value
    };
};

template<typename T>
struct ConvertToInt {
    enum {
        enable_conversion = sizeof(convert(get<T>())) == sizeof(Yes)
    };
    
    enum {
        value = ConvertToIntImpl2<T, enable_conversion>::value
    };
};

#define FORMAT_DISABLE_CONVERSION_TO_INT(Type) \
  template <> \
  struct ConvertToInt<Type> {  enum { value = 0 }; }

// Silence warnings about convering float to int.
FORMAT_DISABLE_CONVERSION_TO_INT(float);
FORMAT_DISABLE_CONVERSION_TO_INT(double);
FORMAT_DISABLE_CONVERSION_TO_INT(long double);

template<bool B, class T = void>
struct EnableIf
{};

template<class T>
struct EnableIf<true, T> {
    typedef T type;
};

template<bool B, class T, class F>
struct Conditional {
    typedef T type;
};

template<class T, class F>
struct Conditional<false, T, F> {
    typedef F type;
};

// For bcc32 which doesn't understand ! in template arguments.
template<bool>
struct Not {
    enum {
        value = 0
    };
};

template<>
struct Not<false> {
    enum {
        value = 1
    };
};

// Makes an Arg object from any type.
template <typename Formatter>
class MakeValue: public Arg {
public:
    typedef typename Formatter::Char Char;

private:
    // The following two methods are private to disallow formatting of
    // arbitrary pointers. If you want to output a pointer cast it to
    // "void *" or "const void *". In particular, this forbids formatting
    // of "[const] volatile char *" which is printed as bool by iostreams.
    // Do not implement!
    template <typename T>
    MakeValue(const T *value);
    template <typename T>
    MakeValue(T *value);

    // The following methods are private to disallow formatting of wide
    // characters and strings into narrow strings as in
    //   fermat::format("{}", L"test");
    // To fix this, use a wide format string: fermat::format(L"{}", L"test").
    MakeValue(typename WCharHelper<wchar_t, Char>::Unsupported);
    MakeValue(typename WCharHelper<wchar_t *, Char>::Unsupported);
    MakeValue(typename WCharHelper<const wchar_t *, Char>::Unsupported);
    MakeValue(typename WCharHelper<const std::wstring &, Char>::Unsupported);
    MakeValue(typename WCharHelper<WStringRef, Char>::Unsupported);

    void set_string(StringRef str)
    {
        string.value = str.data();
        string.size = str.size();
    }

    void set_string(WStringRef str)
    {
        wstring.value = str.data();
        wstring.size = str.size();
    }

    // Formats an argument of a custom type, such as a user-defined class.
    template <typename T>
    static void format_custom_arg(
        void *formatter, const void *arg, void *format_str_ptr)
    {
        format(*static_cast<Formatter*>(formatter),
               *static_cast<const Char**>(format_str_ptr),
               *static_cast<const T*>(arg));
    }

public:
    MakeValue()
    {}

#define FORMAT_MAKE_VALUE_(Type, field, TYPE, rhs) \
  MakeValue(Type value) { field = rhs; } \
  static uint64_t type(Type) { return Arg::TYPE; }

#define FORMAT_MAKE_VALUE(Type, field, TYPE) \
  FORMAT_MAKE_VALUE_(Type, field, TYPE, value)

    FORMAT_MAKE_VALUE(bool, int_value, BOOL)
    FORMAT_MAKE_VALUE(short, int_value, INT)
    FORMAT_MAKE_VALUE(unsigned short, uint_value, UINT)
    FORMAT_MAKE_VALUE(int, int_value, INT)
    FORMAT_MAKE_VALUE(unsigned, uint_value, UINT)

    MakeValue(long value)
    {
        // To minimize the number of types we need to deal with, long is
        // translated either to int or to long long depending on its size.
        if (check(sizeof(long) == sizeof(int)))
            int_value = static_cast<int>(value);
        else
            long_long_value = value;
    }
    static uint64_t type(long)
    {
        return sizeof(long) == sizeof(int) ? Arg::INT : Arg::LONG_LONG;
    }

    MakeValue(unsigned long value)
    {
        if (check(sizeof(unsigned long) == sizeof(unsigned)))
            uint_value = static_cast<unsigned>(value);
        else
            ulong_long_value = value;
    }
    static uint64_t type(unsigned long)
    {
        return sizeof(unsigned long) == sizeof(unsigned) ?
               Arg::UINT : Arg::ULONG_LONG;
    }

    FORMAT_MAKE_VALUE(LongLong, long_long_value, LONG_LONG)
    FORMAT_MAKE_VALUE(ULongLong, ulong_long_value, ULONG_LONG)
    FORMAT_MAKE_VALUE(float, double_value, DOUBLE)
    FORMAT_MAKE_VALUE(double, double_value, DOUBLE)
    FORMAT_MAKE_VALUE(long double, long_double_value, LONG_DOUBLE)
    FORMAT_MAKE_VALUE(signed char, int_value, INT)
    FORMAT_MAKE_VALUE(unsigned char, uint_value, UINT)
    FORMAT_MAKE_VALUE(char, int_value, CHAR)

    MakeValue(typename WCharHelper<wchar_t, Char>::Supported value)
    {
        int_value = value;
    }
    static uint64_t type(wchar_t)
    {
        return Arg::CHAR;
    }

#define FORMAT_MAKE_STR_VALUE(Type, TYPE) \
  MakeValue(Type value) { set_string(value); } \
  static uint64_t type(Type) { return Arg::TYPE; }

    FORMAT_MAKE_VALUE(char *, string.value, CSTRING)
    FORMAT_MAKE_VALUE(const char *, string.value, CSTRING)
    FORMAT_MAKE_VALUE(const signed char *, sstring.value, CSTRING)
    FORMAT_MAKE_VALUE(const unsigned char *, ustring.value, CSTRING)
    FORMAT_MAKE_STR_VALUE(const std::string &, STRING)
    FORMAT_MAKE_STR_VALUE(StringRef, STRING)
    FORMAT_MAKE_VALUE_(CStringRef, string.value, CSTRING, value.c_str())

#define FORMAT_MAKE_WSTR_VALUE(Type, TYPE) \
  MakeValue(typename WCharHelper<Type, Char>::Supported value) { \
    set_string(value); \
  } \
  static uint64_t type(Type) { return Arg::TYPE; }

    FORMAT_MAKE_WSTR_VALUE(wchar_t *, WSTRING)
    FORMAT_MAKE_WSTR_VALUE(const wchar_t *, WSTRING)
    FORMAT_MAKE_WSTR_VALUE(const std::wstring &, WSTRING)
    FORMAT_MAKE_WSTR_VALUE(WStringRef, WSTRING)

    FORMAT_MAKE_VALUE(void *, pointer, POINTER)
    FORMAT_MAKE_VALUE(const void *, pointer, POINTER)

    template <typename T>
    MakeValue(const T &value,
              typename EnableIf<Not<
              ConvertToInt<T>::value>::value, int>::type = 0)
    {
        custom.value = &value;
        custom.format = &format_custom_arg<T>;
    }

    template <typename T>
    MakeValue(const T &value,
              typename EnableIf<ConvertToInt<T>::value, int>::type = 0)
    {
        int_value = value;
    }

    template <typename T>
    static uint64_t type(const T &)
    {
        return ConvertToInt<T>::value ? Arg::INT : Arg::CUSTOM;
    }

    // Additional template param `Char_` is needed here because make_type always
    // uses char.
    template <typename Char_>
    MakeValue(const NamedArg<Char_> &value)
    {
        pointer = &value;
    }

    template <typename Char_>
    static uint64_t type(const NamedArg<Char_> &)
    {
        return Arg::NAMED_ARG;
    }
};

template <typename Formatter>
class MakeArg: public Arg
{
public:
    MakeArg()
    {
        type = Arg::NONE;
    }

    template <typename T>
    MakeArg(const T &value)
        : Arg(MakeValue<Formatter>(value))
    {
        type = static_cast<Arg::Type>(MakeValue<Formatter>::type(value));
    }
};

template <typename Char>
struct NamedArg: Arg
{
    BasicStringRef<Char> name;

    template <typename T>
    NamedArg(BasicStringRef<Char> argname, const T &value)
        : Arg(MakeArg< BasicFormatter<Char> >(value)), name(argname)
    {}
};

#define FORMAT_DISPATCH(call) static_cast<Impl*>(this)->call

// An argument visitor.
// To use ArgVisitor define a subclass that implements some or all of the
// visit methods with the same signatures as the methods in ArgVisitor,
// for example, visit_int(int).
// Specify the subclass name as the Impl template parameter. Then calling
// ArgVisitor::visit for some argument will dispatch to a visit method
// specific to the argument type. For example, if the argument type is
// double then visit_double(double) method of a subclass will be called.
// If the subclass doesn't contain a method with this signature, then
// a corresponding method of ArgVisitor will be called.
//
// Example:
//  class MyArgVisitor : public ArgVisitor<MyArgVisitor, void> {
//   public:
//    void visit_int(int value) { print("{}", value); }
//    void visit_double(double value) { print("{}", value ); }
//  };
//
// ArgVisitor uses the curiously recurring template pattern:
// http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
template <typename Impl, typename Result>
class ArgVisitor {
public:
    void report_unhandled_arg()
    {}

    Result visit_unhandled_arg()
    {
        FORMAT_DISPATCH(report_unhandled_arg());
        return Result();
    }

    Result visit_int(int value)
    {
        return FORMAT_DISPATCH(visit_any_int(value));
    }
    Result visit_long_long(LongLong value)
    {
        return FORMAT_DISPATCH(visit_any_int(value));
    }
    Result visit_uint(unsigned value)
    {
        return FORMAT_DISPATCH(visit_any_int(value));
    }
    Result visit_ulong_long(ULongLong value)
    {
        return FORMAT_DISPATCH(visit_any_int(value));
    }
    Result visit_bool(bool value)
    {
        return FORMAT_DISPATCH(visit_any_int(value));
    }
    Result visit_char(int value)
    {
        return FORMAT_DISPATCH(visit_any_int(value));
    }
    template <typename T>
    Result visit_any_int(T)
    {
        return FORMAT_DISPATCH(visit_unhandled_arg());
    }

    Result visit_double(double value)
    {
        return FORMAT_DISPATCH(visit_any_double(value));
    }
    Result visit_long_double(long double value)
    {
        return FORMAT_DISPATCH(visit_any_double(value));
    }
    template <typename T>
    Result visit_any_double(T)
    {
        return FORMAT_DISPATCH(visit_unhandled_arg());
    }

    Result visit_cstring(const char *)
    {
        return FORMAT_DISPATCH(visit_unhandled_arg());
    }
    Result visit_string(Arg::StringValue<char>)
    {
        return FORMAT_DISPATCH(visit_unhandled_arg());
    }
    Result visit_wstring(Arg::StringValue<wchar_t>)
    {
        return FORMAT_DISPATCH(visit_unhandled_arg());
    }
    Result visit_pointer(const void *)
    {
        return FORMAT_DISPATCH(visit_unhandled_arg());
    }
    Result visit_custom(Arg::CustomValue)
    {
        return FORMAT_DISPATCH(visit_unhandled_arg());
    }

    Result visit(const Arg &arg)
    {
        switch (arg.type)
        {
        default:
            FORMAT_ASSERT(false, "invalid argument type");
            return Result();
        case Arg::INT:
            return FORMAT_DISPATCH(visit_int(arg.int_value));
        case Arg::UINT:
            return FORMAT_DISPATCH(visit_uint(arg.uint_value));
        case Arg::LONG_LONG:
            return FORMAT_DISPATCH(visit_long_long(arg.long_long_value));
        case Arg::ULONG_LONG:
            return FORMAT_DISPATCH(visit_ulong_long(arg.ulong_long_value));
        case Arg::BOOL:
            return FORMAT_DISPATCH(visit_bool(arg.int_value != 0));
        case Arg::CHAR:
            return FORMAT_DISPATCH(visit_char(arg.int_value));
        case Arg::DOUBLE:
            return FORMAT_DISPATCH(visit_double(arg.double_value));
        case Arg::LONG_DOUBLE:
            return FORMAT_DISPATCH(visit_long_double(arg.long_double_value));
        case Arg::CSTRING:
            return FORMAT_DISPATCH(visit_cstring(arg.string.value));
        case Arg::STRING:
            return FORMAT_DISPATCH(visit_string(arg.string));
        case Arg::WSTRING:
            return FORMAT_DISPATCH(visit_wstring(arg.wstring));
        case Arg::POINTER:
            return FORMAT_DISPATCH(visit_pointer(arg.pointer));
        case Arg::CUSTOM:
            return FORMAT_DISPATCH(visit_custom(arg.custom));
        }
    }
};

class RuntimeError: public std::runtime_error
{
protected:
    RuntimeError(): std::runtime_error("")
    {}
};

template <typename Char>
class PrintfArgFormatter;

template <typename Char>
class ArgMap;

}  // namespace internal

/** An argument list. */
class ArgList {
private:
    // To reduce compiled code size per formatting function call, types of first
    // MAX_PACKED_ARGS arguments are passed in the types_ field.
    uint64_t _types;
    union
    {
        // If the number of arguments is less than MAX_PACKED_ARGS, the argument
        // values are stored in values_, otherwise they are stored in args_.
        // This is done to reduce compiled code size as storing larger objects
        // may require more code (at least on x86-64) even if the same amount of
        // data is actually copied to stack. It saves ~10% on the bloat test.
        const internal::Value  *_values;
        const internal::Arg    *_args;
    };

    internal::Arg::Type type(unsigned index) const
    {
        unsigned shift = index * 4;
        uint64_t mask = 0xf;
        return static_cast<internal::Arg::Type>(
                   (_types & (mask << shift)) >> shift);
    }

    template <typename Char>
    friend class internal::ArgMap;

public:
    // Maximum number of arguments with packed types.
    enum
    {
        MAX_PACKED_ARGS = 16
    };

    ArgList(): _types(0)
    {}

    ArgList(ULongLong types, const internal::Value *values)
        : _types(types), _values(values)
    {}
    ArgList(ULongLong types, const internal::Arg *args)
        : _types(types), _args(args)
    {}

    /** Returns the argument at specified index. */
    internal::Arg operator[](unsigned index) const
    {
        using internal::Arg;
        Arg arg;
        bool use_values = type(MAX_PACKED_ARGS - 1) == Arg::NONE;
        if (index < MAX_PACKED_ARGS)
        {
            Arg::Type arg_type = type(index);
            internal::Value &val = arg;
            if (arg_type != Arg::NONE)
                val = use_values ? _values[index] : _args[index];
            arg.type = arg_type;
            return arg;
        }
        if (use_values)
        {
            // The index is greater than the number of arguments that can be stored
            // in values, so return a "none" argument.
            arg.type = Arg::NONE;
            return arg;
        }
        for (unsigned i = MAX_PACKED_ARGS; i <= index; ++i)
        {
            if (_args[i].type == Arg::NONE)
                return _args[i];
        }
        return _args[index];
    }
};

enum Alignment
{
    ALIGN_DEFAULT, 
    ALIGN_LEFT, 
    ALIGN_RIGHT, 
    ALIGN_CENTER, 
    ALIGN_NUMERIC
};

// Flags.
enum
{
    SIGN_FLAG = 1, 
    PLUS_FLAG = 2, 
    MINUS_FLAG = 4, 
    HASH_FLAG = 8,
    CHAR_FLAG = 0x10  // Argument has char type - used in error reporting.
};

// An empty format specifier.
struct EmptySpec
{};

// A type specifier.
template <char TYPE>
struct TypeSpec: EmptySpec {
    Alignment align() const
    {
        return ALIGN_DEFAULT;
    }
    unsigned width() const
    {
        return 0;
    }
    int precision() const
    {
        return -1;
    }
    bool flag(unsigned) const
    {
        return false;
    }
    char type() const
    {
        return TYPE;
    }
    char fill() const
    {
        return ' ';
    }
};

// A width specifier.
struct WidthSpec {
    unsigned _width;
    // Fill is always wchar_t and cast to char if necessary to avoid having
    // two specialization of WidthSpec and its subclasses.
    wchar_t _fill;

    WidthSpec(unsigned width, wchar_t fill) : _width(width), _fill(fill)
    {}

    unsigned width() const
    {
        return _width;
    }
    wchar_t fill() const
    {
        return _fill;
    }
};

// An alignment specifier.
struct AlignSpec: WidthSpec
{
    Alignment _align;

    AlignSpec(unsigned width, wchar_t fill, Alignment align = ALIGN_DEFAULT)
        : WidthSpec(width, fill), _align(align)
    {}

    Alignment align() const
    {
        return _align;
    }

    int precision() const
    {
        return -1;
    }
};

// An alignment and type specifier.
template <char TYPE>
struct AlignTypeSpec: AlignSpec
{
    AlignTypeSpec(unsigned width, wchar_t fill): AlignSpec(width, fill)
    {}

    bool flag(unsigned) const
    {
        return false;
    }
    char type() const
    {
        return TYPE;
    }
};

// A full format specifier.
struct FormatSpec: AlignSpec
{
    unsigned  _flags;
    int       _precision;
    char      _type;

    FormatSpec(
        unsigned width = 0, char type = 0, wchar_t fill = ' ')
        : AlignSpec(width, fill), _flags(0), _precision(-1), _type(type)
    {}

    bool flag(unsigned f) const
    {
        return (_flags & f) != 0;
    }
    int precision() const
    {
        return _precision;
    }
    char type() const
    {
        return _type;
    }
};

// An integer format specifier.
template <typename T, typename SpecT = TypeSpec<0>, typename Char = char>
class IntFormatSpec: public SpecT {
private:
    T _value;

public:
    IntFormatSpec(T val, const SpecT &spec = SpecT())
        : SpecT(spec), _value(val)
    {}

    T value() const
    {
        return _value;
    }
};

// A string format specifier.
template <typename Char>
class StrFormatSpec: public AlignSpec {
public:
    template <typename FillChar>
    StrFormatSpec(const Char *str, unsigned width, FillChar fill)
        : AlignSpec(width, fill), _str(str)
    {
        internal::CharTraits<Char>::convert(FillChar());
    }

    const Char *str() const
    {
        return _str;
    }
private:
    const Char *_str;
};

/**
Returns an integer format specifier to format the value in base 2.
*/
IntFormatSpec<int, TypeSpec<'b'> > bin(int value);

/**
Returns an integer format specifier to format the value in base 8.
*/
IntFormatSpec<int, TypeSpec<'o'> > oct(int value);

/**
Returns an integer format specifier to format the value in base 16 using
lower-case letters for the digits above 9.
*/
IntFormatSpec<int, TypeSpec<'x'> > hex(int value);

/**
Returns an integer formatter format specifier to format in base 16 using
upper-case letters for the digits above 9.
*/
IntFormatSpec<int, TypeSpec<'X'> > hexu(int value);

template <char TYPE_CODE, typename Char>
IntFormatSpec<int, AlignTypeSpec<TYPE_CODE>, Char> pad(
    int value, unsigned width, Char fill = ' ');

#define FORMAT_DEFINE_INT_FORMATTERS(TYPE) \
inline IntFormatSpec<TYPE, TypeSpec<'b'> > bin(TYPE value) { \
  return IntFormatSpec<TYPE, TypeSpec<'b'> >(value, TypeSpec<'b'>()); \
} \
 \
inline IntFormatSpec<TYPE, TypeSpec<'o'> > oct(TYPE value) { \
  return IntFormatSpec<TYPE, TypeSpec<'o'> >(value, TypeSpec<'o'>()); \
} \
 \
inline IntFormatSpec<TYPE, TypeSpec<'x'> > hex(TYPE value) { \
  return IntFormatSpec<TYPE, TypeSpec<'x'> >(value, TypeSpec<'x'>()); \
} \
 \
inline IntFormatSpec<TYPE, TypeSpec<'X'> > hexu(TYPE value) { \
  return IntFormatSpec<TYPE, TypeSpec<'X'> >(value, TypeSpec<'X'>()); \
} \
 \
template <char TYPE_CODE> \
inline IntFormatSpec<TYPE, AlignTypeSpec<TYPE_CODE> > pad( \
    IntFormatSpec<TYPE, TypeSpec<TYPE_CODE> > f, unsigned width) { \
  return IntFormatSpec<TYPE, AlignTypeSpec<TYPE_CODE> >( \
      f.value(), AlignTypeSpec<TYPE_CODE>(width, ' ')); \
} \
 \
/* For compatibility with older compilers we provide two overloads for pad, */ \
/* one that takes a fill character and one that doesn't. In the future this */ \
/* can be replaced with one overload making the template argument Char      */ \
/* default to char (C++11). */ \
template <char TYPE_CODE, typename Char> \
inline IntFormatSpec<TYPE, AlignTypeSpec<TYPE_CODE>, Char> pad( \
    IntFormatSpec<TYPE, TypeSpec<TYPE_CODE>, Char> f, \
    unsigned width, Char fill) { \
  return IntFormatSpec<TYPE, AlignTypeSpec<TYPE_CODE>, Char>( \
      f.value(), AlignTypeSpec<TYPE_CODE>(width, fill)); \
} \
 \
inline IntFormatSpec<TYPE, AlignTypeSpec<0> > pad( \
    TYPE value, unsigned width) { \
  return IntFormatSpec<TYPE, AlignTypeSpec<0> >( \
      value, AlignTypeSpec<0>(width, ' ')); \
} \
 \
template <typename Char> \
inline IntFormatSpec<TYPE, AlignTypeSpec<0>, Char> pad( \
   TYPE value, unsigned width, Char fill) { \
 return IntFormatSpec<TYPE, AlignTypeSpec<0>, Char>( \
     value, AlignTypeSpec<0>(width, fill)); \
}

FORMAT_DEFINE_INT_FORMATTERS(int)
FORMAT_DEFINE_INT_FORMATTERS(long)
FORMAT_DEFINE_INT_FORMATTERS(unsigned)
FORMAT_DEFINE_INT_FORMATTERS(unsigned long)
FORMAT_DEFINE_INT_FORMATTERS(LongLong)
FORMAT_DEFINE_INT_FORMATTERS(ULongLong)


template <typename Char>
inline StrFormatSpec<Char> pad(
    const Char *str, unsigned width, Char fill = ' ')
{
    return StrFormatSpec<Char>(str, width, fill);
}

inline StrFormatSpec<wchar_t> pad(
    const wchar_t *str, unsigned width, char fill = ' ')
{
    return StrFormatSpec<wchar_t>(str, width, fill);
}

namespace internal {

template <typename Char>
class ArgMap {
private:
    typedef std::vector<std::pair<fermat::BasicStringRef<Char>, internal::Arg> > MapType;
    typedef typename MapType::value_type Pair;

    MapType  _map;

public:
     void init(const ArgList &args);

    const internal::Arg* find(const fermat::BasicStringRef<Char> &name) const
    {
        for (typename MapType::const_iterator it = _map.begin(), end = _map.end();
                it != end; ++it) {
            if (it->first == name) {
                return &it->second;
            }
        }
        return 0;
    }
};

template <typename Impl, typename Char>
class ArgFormatterBase: public ArgVisitor<Impl, void> {
private:
    BasicWriter<Char> &_writer;
    FormatSpec        &_spec;

    FORMAT_DISALLOW_COPY_AND_ASSIGN(ArgFormatterBase);

    void write_pointer(const void *p)
    {
        _spec._flags = HASH_FLAG;
        _spec._type = 'x';
        _writer.write_int(reinterpret_cast<uintptr_t>(p), _spec);
    }

protected:
    BasicWriter<Char> &writer()
    {
        return _writer;
    }
    FormatSpec &spec()
    {
        return _spec;
    }

    void write(bool value)
    {
        const char *str_value = value ? "true" : "false";
        Arg::StringValue<char> str = { str_value, std::strlen(str_value) };
        _writer.write_str(str, _spec);
    }

    void write(const char *value)
    {
        Arg::StringValue<char> str = { value, value != 0 ? std::strlen(value) : 0 };
        _writer.write_str(str, _spec);
    }

public:
    ArgFormatterBase(BasicWriter<Char> &w, FormatSpec &s)
        : _writer(w), _spec(s)
    {}

    template <typename T>
    void visit_any_int(T value)
    {
        _writer.write_int(value, _spec);
    }

    template <typename T>
    void visit_any_double(T value)
    {
        _writer.write_double(value, _spec);
    }

    void visit_bool(bool value)
    {
        if (_spec._type)
            return visit_any_int(value);
        write(value);
    }

    void visit_char(int value)
    {
        if (_spec._type && _spec._type != 'c') {
            _spec._flags |= CHAR_FLAG;
            _writer.write_int(value, _spec);
            return;
        }
        if (_spec._align == ALIGN_NUMERIC || _spec._flags != 0)
            FORMAT_THROW(FormatError("invalid format specifier for char"));
        typedef typename BasicWriter<Char>::CharPtr CharPtr;
        Char fill = internal::CharTraits<Char>::cast(_spec.fill());
        CharPtr out = CharPtr();
        const unsigned CHAR_WIDTH = 1;
        if (_spec._width > CHAR_WIDTH) {
            out = _writer.grow_buffer(_spec._width);
            if (_spec._align == ALIGN_RIGHT) {
                std::uninitialized_fill_n(out, _spec._width - CHAR_WIDTH, fill);
                out += _spec._width - CHAR_WIDTH;
            } else if (_spec._align == ALIGN_CENTER) {
                out = _writer.fill_padding(out, _spec._width,
                                           internal::check(CHAR_WIDTH), fill);
            } else {
                std::uninitialized_fill_n(out + CHAR_WIDTH,
                                          _spec._width - CHAR_WIDTH, fill);
            }
        } else {
            out = _writer.grow_buffer(CHAR_WIDTH);
        }
        *out = internal::CharTraits<Char>::cast(value);
    }

    void visit_cstring(const char *value)
    {
        if (_spec._type == 'p')
            return write_pointer(value);
        write(value);
    }

    void visit_string(Arg::StringValue<char> value)
    {
        _writer.write_str(value, _spec);
    }

    using ArgVisitor<Impl, void>::visit_wstring;

    void visit_wstring(Arg::StringValue<Char> value)
    {
        _writer.write_str(value, _spec);
    }

    void visit_pointer(const void *value)
    {
        if (_spec._type && _spec._type != 'p')
            report_unknown_type(_spec._type, "pointer");
        write_pointer(value);
    }
};

// An argument formatter.
template <typename Char>
class BasicArgFormatter:
    public ArgFormatterBase<BasicArgFormatter<Char>, Char> {
private:
    BasicFormatter<Char>  &_formatter;
    const Char            *_format;

public:
    BasicArgFormatter(BasicFormatter<Char> &f, FormatSpec &s, const Char *fmt)
        : ArgFormatterBase<BasicArgFormatter<Char>, Char>(f.writer(), s),
          _formatter(f), _format(fmt)
    {}

    void visit_custom(Arg::CustomValue c)
    {
        c.format(&_formatter, c.value, &_format);
    }
};

class FormatterBase {
private:
    ArgList _args;
    int _next_arg_index;

    // Returns the argument with specified index.
     Arg do_get_arg(unsigned arg_index, const char *&error);

protected:
    const ArgList &args() const
    {
        return _args;
    }

    explicit FormatterBase(const ArgList &args)
    {
        _args = args;
        _next_arg_index = 0;
    }

    // Returns the next argument.
    Arg next_arg(const char *&error)
    {
        if (_next_arg_index >= 0)
            return do_get_arg(internal::to_unsigned(_next_arg_index++), error);
        error = "cannot switch from manual to automatic argument indexing";
        return Arg();
    }

    // Checks if manual indexing is used and returns the argument with
    // specified index.
    Arg get_arg(unsigned arg_index, const char *&error)
    {
        return check_no_auto_index(error) ? do_get_arg(arg_index, error) : Arg();
    }

    bool check_no_auto_index(const char *&error)
    {
        if (_next_arg_index > 0)
        {
            error = "cannot switch from automatic to manual argument indexing";
            return false;
        }
        _next_arg_index = -1;
        return true;
    }

    template <typename Char>
    void write(BasicWriter<Char> &w, const Char *start, const Char *end)
    {
        if (start != end)
            w << BasicStringRef<Char>(start, internal::to_unsigned(end - start));
    }
};

// A printf formatter.
template <typename Char>
class PrintfFormatter: private FormatterBase
{
private:
    void parse_flags(FormatSpec &spec, const Char *&s);

    // Returns the argument with specified index or, if arg_index is equal
    // to the maximum unsigned value, the next argument.
    Arg get_arg(const Char *s,
                unsigned arg_index = (std::numeric_limits<unsigned>::max)());

    // Parses argument index, flags and width and returns the argument index.
    unsigned parse_header(const Char *&s, FormatSpec &spec);

public:
    explicit PrintfFormatter(const ArgList &args): FormatterBase(args)
    {}
     void format(BasicWriter<Char> &writer,
                        BasicCStringRef<Char> format_str);
};

} //namespace internal

template <typename CharType, typename ArgFormatter>
class BasicFormatter: private internal::FormatterBase {
public:
    /** The character type for the output. */
    typedef CharType Char;

private:
    BasicWriter<Char>       &_writer;
    internal::ArgMap<Char>   _map;

    FORMAT_DISALLOW_COPY_AND_ASSIGN(BasicFormatter);

    using internal::FormatterBase::get_arg;

    // Checks if manual indexing is used and returns the argument with
    // specified name.
    internal::Arg get_arg(BasicStringRef<Char> arg_name, const char *&error);

    // Parses argument index and returns corresponding argument.
    internal::Arg parse_arg_index(const Char *&s);

    // Parses argument name and returns corresponding argument.
    internal::Arg parse_arg_name(const Char *&s);

public:
    /**
    \rst
    Constructs a ``BasicFormatter`` object. References to the arguments and
    the writer are stored in the formatter object so make sure they have
    appropriate lifetimes.
    \endrst
    */
    BasicFormatter(const ArgList &args, BasicWriter<Char> &w)
        : internal::FormatterBase(args), _writer(w)
    {}

    /** Returns a reference to the writer associated with this formatter. */
    BasicWriter<Char> &writer()
    {
        return _writer;
    }

    /** Formats stored arguments and writes the output to the writer. */
    void format(BasicCStringRef<Char> format_str);

    // Formats a single argument and advances format_str, a format string pointer.
    const Char *format(const Char *&format_str, const internal::Arg &arg);
};

// Generates a comma-separated list with results of applying f to
// numbers 0..n-1.
# define FORMAT_GEN(n, f) FORMAT_GEN##n(f)
# define FORMAT_GEN1(f)  f(0)
# define FORMAT_GEN2(f)  FORMAT_GEN1(f),  f(1)
# define FORMAT_GEN3(f)  FORMAT_GEN2(f),  f(2)
# define FORMAT_GEN4(f)  FORMAT_GEN3(f),  f(3)
# define FORMAT_GEN5(f)  FORMAT_GEN4(f),  f(4)
# define FORMAT_GEN6(f)  FORMAT_GEN5(f),  f(5)
# define FORMAT_GEN7(f)  FORMAT_GEN6(f),  f(6)
# define FORMAT_GEN8(f)  FORMAT_GEN7(f),  f(7)
# define FORMAT_GEN9(f)  FORMAT_GEN8(f),  f(8)
# define FORMAT_GEN10(f) FORMAT_GEN9(f),  f(9)
# define FORMAT_GEN11(f) FORMAT_GEN10(f), f(10)
# define FORMAT_GEN12(f) FORMAT_GEN11(f), f(11)
# define FORMAT_GEN13(f) FORMAT_GEN12(f), f(12)
# define FORMAT_GEN14(f) FORMAT_GEN13(f), f(13)
# define FORMAT_GEN15(f) FORMAT_GEN14(f), f(14)

namespace internal {

inline uint64_t make_type()
{
    return 0;
}

template <typename T>
inline uint64_t make_type(const T &arg)
{
    return MakeValue< BasicFormatter<char> >::type(arg);
}

template <unsigned N, bool/*IsPacked*/ = (N < ArgList::MAX_PACKED_ARGS)>
          struct ArgArray;

template <unsigned N>
struct ArgArray<N, true/*IsPacked*/> {
    typedef Value Type[N > 0 ? N : 1];

    template <typename Formatter, typename T>
    static Value make(const T &value)
    {
        Value result = MakeValue<Formatter>(value);
        // Workaround a bug in Apple LLVM version 4.2 (clang-425.0.28) of clang:
        // https://github.com/cppformat/cppformat/issues/276
        (void)result.custom.format;
        return result;
    }
};

template <unsigned N>
struct ArgArray<N, false/*IsPacked*/> {
    typedef Arg Type[N + 1]; // +1 for the list end Arg::NONE

    template <typename Formatter, typename T>
    static Arg make(const T &value)
    {
        return MakeArg<Formatter>(value);
    }
};

#if FORMAT_USE_VARIADIC_TEMPLATES
template <typename Arg, typename... Args>
inline uint64_t make_type(const Arg &first, const Args & ... tail)
{
    return make_type(first) | (make_type(tail...) << 4);
}

#else

struct ArgType {
    uint64_t type;

    ArgType(): type(0)
    {}

    template <typename T>
    ArgType(const T &arg) : type(make_type(arg))
    {}
};

# define FORMAT_ARG_TYPE_DEFAULT(n) ArgType t##n = ArgType()

inline uint64_t make_type(FORMAT_GEN15(FORMAT_ARG_TYPE_DEFAULT))
{
    return t0.type | (t1.type << 4) | (t2.type << 8) | (t3.type << 12) |
           (t4.type << 16) | (t5.type << 20) | (t6.type << 24) | (t7.type << 28) |
           (t8.type << 32) | (t9.type << 36) | (t10.type << 40) | (t11.type << 44) |
           (t12.type << 48) | (t13.type << 52) | (t14.type << 56);
}
#endif

template <class Char>
class FormatBuf: public std::basic_streambuf<Char> {
private:
    typedef typename std::basic_streambuf<Char>::int_type int_type;
    typedef typename std::basic_streambuf<Char>::traits_type traits_type;

    BasicBuffer<Char>  &_buffer;
    Char               *_start;

public:
    FormatBuf(BasicBuffer<Char> &buffer): _buffer(buffer), _start(&buffer[0])
    {
        this->setp(_start, _start + _buffer.capacity());
    }

    int_type overflow(int_type ch = traits_type::eof())
    {
        if (!traits_type::eq_int_type(ch, traits_type::eof())) {
            size_t size = this->size();
            _buffer.resize(size);
            _buffer.reserve(size * 2);

            _start = &_buffer[0];
            _start[size] = traits_type::to_char_type(ch);
            this->setp(_start + size + 1, _start + size * 2);
        }
        return ch;
    }

    size_t size() const
    {
        return to_unsigned(this->pptr() - _start);
    }
};
}  // namespace internal

# define FORMAT_MAKE_TEMPLATE_ARG(n) typename T##n
# define FORMAT_MAKE_ARG_TYPE(n) T##n
# define FORMAT_MAKE_ARG(n) const T##n &v##n
# define FORMAT_ASSIGN_char(n) \
  arr[n] = fermat::internal::MakeValue< fermat::BasicFormatter<char> >(v##n)
# define FORMAT_ASSIGN_wchar_t(n) \
  arr[n] = fermat::internal::MakeValue< fermat::BasicFormatter<wchar_t> >(v##n)

#if FORMAT_USE_VARIADIC_TEMPLATES
// Defines a variadic function returning void.
# define FORMAT_VARIADIC_VOID(func, arg_type) \
  template <typename... Args> \
  void func(arg_type arg0, const Args & ... args) { \
    typedef fermat::internal::ArgArray<sizeof...(Args)> ArgArray; \
    typename ArgArray::Type array{ \
      ArgArray::template make<fermat::BasicFormatter<Char> >(args)...}; \
    func(arg0, fermat::ArgList(fermat::internal::make_type(args...), array)); \
  }

// Defines a variadic constructor.
# define FORMAT_VARIADIC_CTOR(ctor, func, arg0_type, arg1_type) \
  template <typename... Args> \
  ctor(arg0_type arg0, arg1_type arg1, const Args & ... args) { \
    typedef fermat::internal::ArgArray<sizeof...(Args)> ArgArray; \
    typename ArgArray::Type array{ \
      ArgArray::template make<fermat::BasicFormatter<Char> >(args)...}; \
    func(arg0, arg1, fermat::ArgList(fermat::internal::make_type(args...), array)); \
  }

#else

# define FORMAT_MAKE_REF(n) \
  fermat::internal::MakeValue< fermat::BasicFormatter<Char> >(v##n)
# define FORMAT_MAKE_REF2(n) v##n

// Defines a wrapper for a function taking one argument of type arg_type
// and n additional arguments of arbitrary types.
# define FORMAT_WRAP1(func, arg_type, n) \
  template <FORMAT_GEN(n, FORMAT_MAKE_TEMPLATE_ARG)> \
  inline void func(arg_type arg1, FORMAT_GEN(n, FORMAT_MAKE_ARG)) { \
    const fermat::internal::ArgArray<n>::Type array = {FORMAT_GEN(n, FORMAT_MAKE_REF)}; \
    func(arg1, fermat::ArgList( \
      fermat::internal::make_type(FORMAT_GEN(n, FORMAT_MAKE_REF2)), array)); \
  }

// Emulates a variadic function returning void on a pre-C++11 compiler.
# define FORMAT_VARIADIC_VOID(func, arg_type) \
  inline void func(arg_type arg) { func(arg, fermat::ArgList()); } \
  FORMAT_WRAP1(func, arg_type, 1) FORMAT_WRAP1(func, arg_type, 2) \
  FORMAT_WRAP1(func, arg_type, 3) FORMAT_WRAP1(func, arg_type, 4) \
  FORMAT_WRAP1(func, arg_type, 5) FORMAT_WRAP1(func, arg_type, 6) \
  FORMAT_WRAP1(func, arg_type, 7) FORMAT_WRAP1(func, arg_type, 8) \
  FORMAT_WRAP1(func, arg_type, 9) FORMAT_WRAP1(func, arg_type, 10)

# define FORMAT_CTOR(ctor, func, arg0_type, arg1_type, n) \
  template <FORMAT_GEN(n, FORMAT_MAKE_TEMPLATE_ARG)> \
  ctor(arg0_type arg0, arg1_type arg1, FORMAT_GEN(n, FORMAT_MAKE_ARG)) { \
    const fermat::internal::ArgArray<n>::Type array = {FORMAT_GEN(n, FORMAT_MAKE_REF)}; \
    func(arg0, arg1, fermat::ArgList( \
      fermat::internal::make_type(FORMAT_GEN(n, FORMAT_MAKE_REF2)), array)); \
  }

// Emulates a variadic constructor on a pre-C++11 compiler.
# define FORMAT_VARIADIC_CTOR(ctor, func, arg0_type, arg1_type) \
  FORMAT_CTOR(ctor, func, arg0_type, arg1_type, 1) \
  FORMAT_CTOR(ctor, func, arg0_type, arg1_type, 2) \
  FORMAT_CTOR(ctor, func, arg0_type, arg1_type, 3) \
  FORMAT_CTOR(ctor, func, arg0_type, arg1_type, 4) \
  FORMAT_CTOR(ctor, func, arg0_type, arg1_type, 5) \
  FORMAT_CTOR(ctor, func, arg0_type, arg1_type, 6) \
  FORMAT_CTOR(ctor, func, arg0_type, arg1_type, 7) \
  FORMAT_CTOR(ctor, func, arg0_type, arg1_type, 8) \
  FORMAT_CTOR(ctor, func, arg0_type, arg1_type, 9) \
  FORMAT_CTOR(ctor, func, arg0_type, arg1_type, 10)
#endif

// Generates a comma-separated list with results of applying f to pairs
// (argument, index).
#define FORMAT_FOR_EACH1(f, x0) f(x0, 0)
#define FORMAT_FOR_EACH2(f, x0, x1) \
  FORMAT_FOR_EACH1(f, x0), f(x1, 1)
#define FORMAT_FOR_EACH3(f, x0, x1, x2) \
  FORMAT_FOR_EACH2(f, x0 ,x1), f(x2, 2)
#define FORMAT_FOR_EACH4(f, x0, x1, x2, x3) \
  FORMAT_FOR_EACH3(f, x0, x1, x2), f(x3, 3)
#define FORMAT_FOR_EACH5(f, x0, x1, x2, x3, x4) \
  FORMAT_FOR_EACH4(f, x0, x1, x2, x3), f(x4, 4)
#define FORMAT_FOR_EACH6(f, x0, x1, x2, x3, x4, x5) \
  FORMAT_FOR_EACH5(f, x0, x1, x2, x3, x4), f(x5, 5)
#define FORMAT_FOR_EACH7(f, x0, x1, x2, x3, x4, x5, x6) \
  FORMAT_FOR_EACH6(f, x0, x1, x2, x3, x4, x5), f(x6, 6)
#define FORMAT_FOR_EACH8(f, x0, x1, x2, x3, x4, x5, x6, x7) \
  FORMAT_FOR_EACH7(f, x0, x1, x2, x3, x4, x5, x6), f(x7, 7)
#define FORMAT_FOR_EACH9(f, x0, x1, x2, x3, x4, x5, x6, x7, x8) \
  FORMAT_FOR_EACH8(f, x0, x1, x2, x3, x4, x5, x6, x7), f(x8, 8)
#define FORMAT_FOR_EACH10(f, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9) \
  FORMAT_FOR_EACH9(f, x0, x1, x2, x3, x4, x5, x6, x7, x8), f(x9, 9)


class SystemError: public internal::RuntimeError {
private:
    void init(int err_code, CStringRef format_str, ArgList args);

protected:
    int _error_code;

    typedef char Char;  // For FORMAT_VARIADIC_CTOR.

    SystemError()
    {}

public:
    SystemError(int error_code, CStringRef message)
    {
        init(error_code, message, ArgList());
    }
    FORMAT_VARIADIC_CTOR(SystemError, init, int, CStringRef)

    int error_code() const
    {
        return _error_code;
    }
};


template <typename Char>
class BasicWriter {
private:
    // Output buffer.
    BasicBuffer<Char> &_buffer;

    FORMAT_DISALLOW_COPY_AND_ASSIGN(BasicWriter);

    typedef typename internal::CharTraits<Char>::CharPtr CharPtr;

#if FORMAT_SECURE_SCL
    // Returns pointer value.
    static Char *get(CharPtr p)
    {
        return p.base();
    }
#else
    static Char *get(Char *p)
    {
        return p;
    }
#endif

    // Fills the padding around the content and returns the pointer to the
    // content area.
    static CharPtr fill_padding(CharPtr buffer,
                                unsigned total_size, std::size_t content_size, wchar_t fill);

    // Grows the buffer by n characters and returns a pointer to the newly
    // allocated area.
    CharPtr grow_buffer(std::size_t n)
    {
        std::size_t size = _buffer.size();
        _buffer.resize(size + n);
        return &_buffer[size];
    }

    // Writes an unsigned decimal integer.
    template <typename UInt>
    Char *write_unsigned_decimal(UInt value, unsigned prefix_size = 0)
    {
        unsigned num_digits = internal::count_digits(value);
        Char *ptr = get(grow_buffer(prefix_size + num_digits));
        internal::format_decimal(ptr + prefix_size, value, num_digits);
        return ptr;
    }

    // Writes a decimal integer.
    template <typename Int>
    void write_decimal(Int value)
    {
        typedef typename internal::IntTraits<Int>::MainType MainType;
        MainType abs_value = static_cast<MainType>(value);
        if (internal::is_negative(value))
        {
            abs_value = 0 - abs_value;
            *write_unsigned_decimal(abs_value, 1) = '-';
        }
        else
        {
            write_unsigned_decimal(abs_value, 0);
        }
    }

    // Prepare a buffer for integer formatting.
    CharPtr prepare_int_buffer(unsigned num_digits,
                               const EmptySpec &, const char *prefix, unsigned prefix_size)
    {
        unsigned size = prefix_size + num_digits;
        CharPtr p = grow_buffer(size);
        std::uninitialized_copy(prefix, prefix + prefix_size, p);
        return p + size - 1;
    }

    template <typename Spec>
    CharPtr prepare_int_buffer(unsigned num_digits,
                               const Spec &spec, const char *prefix, unsigned prefix_size);

    // Formats an integer.
    template <typename T, typename Spec>
    void write_int(T value, Spec spec);

    // Formats a floating-point number (double or long double).
    template <typename T>
    void write_double(T value, const FormatSpec &spec);

    // Writes a formatted string.
    template <typename StrChar>
    CharPtr write_str(const StrChar *s, std::size_t size, const AlignSpec &spec);

    template <typename StrChar>
    void write_str(const internal::Arg::StringValue<StrChar> &str,
                   const FormatSpec &spec);


    void operator<<(typename internal::WCharHelper<wchar_t, Char>::Unsupported);
    void operator<<(
        typename internal::WCharHelper<const wchar_t *, Char>::Unsupported);

    // Appends floating-point length specifier to the format string.
    // The second argument is only used for overload resolution.
    void append_float_length(Char *&format_ptr, long double)
    {
        *format_ptr++ = 'L';
    }

    template<typename T>
    void append_float_length(Char *&, T)
    {}

    template <typename Impl, typename Char_>
    friend class internal::ArgFormatterBase;

    friend class internal::PrintfArgFormatter<Char>;

protected:
    /**
    Constructs a ``BasicWriter`` object.
    */
    explicit BasicWriter(BasicBuffer<Char> &b): _buffer(b)
    {}

public:
    virtual ~BasicWriter()
    {}

    /**
    Returns the total number of characters written.
    */
    std::size_t size() const
    {
        return _buffer.size();
    }

    /**
    Returns a pointer to the output buffer content. No terminating null
    character is appended.
    */
    const Char *data() const FORMAT_NOEXCEPT
    {
        return &_buffer[0];
    }

    /**
    Returns a pointer to the output buffer content with terminating null
    character appended.
    */
    const Char *c_str() const
    {
        std::size_t size = _buffer.size();
        _buffer.reserve(size + 1);
        _buffer[size] = '\0';
        return &_buffer[0];
    }


    std::basic_string<Char> str() const
    {
        return std::basic_string<Char>(&_buffer[0], _buffer.size());
    }

    void write(BasicCStringRef<Char> format, ArgList args)
    {
        BasicFormatter<Char>(args, *this).format(format);
    }
    FORMAT_VARIADIC_VOID(write, BasicCStringRef<Char>)

    BasicWriter &operator<<(int value)
    {
        write_decimal(value);
        return *this;
    }
    BasicWriter &operator<<(unsigned value)
    {
        return *this << IntFormatSpec<unsigned>(value);
    }
    BasicWriter &operator<<(long value)
    {
        write_decimal(value);
        return *this;
    }
    BasicWriter &operator<<(unsigned long value)
    {
        return *this << IntFormatSpec<unsigned long>(value);
    }
    BasicWriter &operator<<(LongLong value)
    {
        write_decimal(value);
        return *this;
    }

    BasicWriter &operator<<(ULongLong value)
    {
        return *this << IntFormatSpec<ULongLong>(value);
    }

    BasicWriter &operator<<(double value)
    {
        write_double(value, FormatSpec());
        return *this;
    }


    BasicWriter &operator<<(long double value)
    {
        write_double(value, FormatSpec());
        return *this;
    }


    BasicWriter &operator<<(char value)
    {
        _buffer.push_back(value);
        return *this;
    }

    BasicWriter &operator<<(
        typename internal::WCharHelper<wchar_t, Char>::Supported value)
    {
        _buffer.push_back(value);
        return *this;
    }


    BasicWriter &operator<<(fermat::BasicStringRef<Char> value)
    {
        const Char *str = value.data();
        _buffer.append(str, str + value.size());
        return *this;
    }

    BasicWriter &operator<<(
        typename internal::WCharHelper<StringRef, Char>::Supported value)
    {
        const char *str = value.data();
        _buffer.append(str, str + value.size());
        return *this;
    }

    template <typename T, typename Spec, typename FillChar>
    BasicWriter &operator<<(IntFormatSpec<T, Spec, FillChar> spec)
    {
        internal::CharTraits<Char>::convert(FillChar());
        write_int(spec.value(), spec);
        return *this;
    }

    template <typename StrChar>
    BasicWriter &operator<<(const StrFormatSpec<StrChar> &spec)
    {
        const StrChar *s = spec.str();
        write_str(s, std::char_traits<Char>::length(s), spec);
        return *this;
    }

    void clear() FORMAT_NOEXCEPT
    {
        _buffer.clear();
    }
};

template <typename Char>
template <typename StrChar>
typename BasicWriter<Char>::CharPtr BasicWriter<Char>::write_str(
    const StrChar *s, std::size_t size, const AlignSpec &spec)
{
    CharPtr out = CharPtr();
    if (spec.width() > size) {
        out = grow_buffer(spec.width());
        Char fill = internal::CharTraits<Char>::cast(spec.fill());
        if (spec.align() == ALIGN_RIGHT) {
            std::uninitialized_fill_n(out, spec.width() - size, fill);
            out += spec.width() - size;
        } else if (spec.align() == ALIGN_CENTER) {
            out = fill_padding(out, spec.width(), size, fill);
        } else {
            std::uninitialized_fill_n(out + size, spec.width() - size, fill);
        }
    } else {
        out = grow_buffer(size);
    }
    std::uninitialized_copy(s, s + size, out);
    return out;
}

template <typename Char>
template <typename StrChar>
void BasicWriter<Char>::write_str(
    const internal::Arg::StringValue<StrChar> &s, const FormatSpec &spec)
{
    // Check if StrChar is convertible to Char.
    internal::CharTraits<Char>::convert(StrChar());
    if (spec._type && spec._type != 's')
        internal::report_unknown_type(spec._type, "string");
    const StrChar *str_value = s.value;
    std::size_t str_size = s.size;
    if (str_size == 0) {
        if (!str_value) {
            FORMAT_THROW(FormatError("string pointer is null"));
            return;
        }
    }
    std::size_t precision = static_cast<std::size_t>(spec._precision);
    if (spec._precision >= 0 && precision < str_size)
        str_size = precision;
    write_str(str_value, str_size, spec);
}

template <typename Char>
typename BasicWriter<Char>::CharPtr
BasicWriter<Char>::fill_padding(
    CharPtr buffer, unsigned total_size,
    std::size_t content_size, wchar_t fill)
{
    std::size_t padding = total_size - content_size;
    std::size_t left_padding = padding / 2;
    Char fill_char = internal::CharTraits<Char>::cast(fill);
    std::uninitialized_fill_n(buffer, left_padding, fill_char);
    buffer += left_padding;
    CharPtr content = buffer;
    std::uninitialized_fill_n(buffer + content_size,
                              padding - left_padding, fill_char);
    return content;
}

template <typename Char>
template <typename Spec>
typename BasicWriter<Char>::CharPtr
BasicWriter<Char>::prepare_int_buffer(
    unsigned num_digits, const Spec &spec,
    const char *prefix, unsigned prefix_size)
{
    unsigned width = spec.width();
    Alignment align = spec.align();
    Char fill = internal::CharTraits<Char>::cast(spec.fill());
    if (spec.precision() > static_cast<int>(num_digits)) {
        // Octal prefix '0' is counted as a digit, so ignore it if precision
        // is specified.
        if (prefix_size > 0 && prefix[prefix_size - 1] == '0')
            --prefix_size;
        unsigned number_size =
            prefix_size + internal::to_unsigned(spec.precision());
        AlignSpec subspec(number_size, '0', ALIGN_NUMERIC);
        if (number_size >= width)
            return prepare_int_buffer(num_digits, subspec, prefix, prefix_size);
        _buffer.reserve(width);
        unsigned fill_size = width - number_size;
        if (align != ALIGN_LEFT) {
            CharPtr p = grow_buffer(fill_size);
            std::uninitialized_fill(p, p + fill_size, fill);
        }
        CharPtr result = prepare_int_buffer(
                             num_digits, subspec, prefix, prefix_size);
        if (align == ALIGN_LEFT) {
            CharPtr p = grow_buffer(fill_size);
            std::uninitialized_fill(p, p + fill_size, fill);
        }
        return result;
    }
    unsigned size = prefix_size + num_digits;
    if (width <= size) {
        CharPtr p = grow_buffer(size);
        std::uninitialized_copy(prefix, prefix + prefix_size, p);
        return p + size - 1;
    }
    CharPtr p = grow_buffer(width);
    CharPtr end = p + width;
    if (align == ALIGN_LEFT) {
        std::uninitialized_copy(prefix, prefix + prefix_size, p);
        p += size;
        std::uninitialized_fill(p, end, fill);
    } else if (align == ALIGN_CENTER) {
        p = fill_padding(p, width, size, fill);
        std::uninitialized_copy(prefix, prefix + prefix_size, p);
        p += size;
    } else {
        if (align == ALIGN_NUMERIC) {
            if (prefix_size != 0) {
                p = std::uninitialized_copy(prefix, prefix + prefix_size, p);
                size -= prefix_size;
            }
        } else {
            std::uninitialized_copy(prefix, prefix + prefix_size, end - size);
        }
        std::uninitialized_fill(p, end - size, fill);
        p = end;
    }
    return p - 1;
}

template <typename Char>
template <typename T, typename Spec>
void BasicWriter<Char>::write_int(T value, Spec spec)
{
    unsigned prefix_size = 0;
    typedef typename internal::IntTraits<T>::MainType UnsignedType;
    UnsignedType abs_value = static_cast<UnsignedType>(value);
    char prefix[4] = "";
    if (internal::is_negative(value)) {
        prefix[0] = '-';
        ++prefix_size;
        abs_value = 0 - abs_value;
    } else if (spec.flag(SIGN_FLAG)) {
        prefix[0] = spec.flag(PLUS_FLAG) ? '+' : ' ';
        ++prefix_size;
    }
    switch (spec.type()) {
    case 0:
    case 'd': {
        unsigned num_digits = internal::count_digits(abs_value);
        CharPtr p = prepare_int_buffer(
                        num_digits, spec, prefix, prefix_size) + 1 - num_digits;
        internal::format_decimal(get(p), abs_value, num_digits);
        break;
    }
    case 'x':
    case 'X': {
        UnsignedType n = abs_value;
        if (spec.flag(HASH_FLAG)) {
            prefix[prefix_size++] = '0';
            prefix[prefix_size++] = spec.type();
        }
        unsigned num_digits = 0;
        do {
            ++num_digits;
        } while ((n >>= 4) != 0);
        Char *p = get(prepare_int_buffer(
                          num_digits, spec, prefix, prefix_size));
        n = abs_value;
        const char *digits = spec.type() == 'x' ?
                             "0123456789abcdef" : "0123456789ABCDEF";
        do {
            *p-- = digits[n & 0xf];
        } while ((n >>= 4) != 0);
        break;
    }
    case 'b':
    case 'B': {
        UnsignedType n = abs_value;
        if (spec.flag(HASH_FLAG)) {
            prefix[prefix_size++] = '0';
            prefix[prefix_size++] = spec.type();
        }
        unsigned num_digits = 0;
        do {
            ++num_digits;
        } while ((n >>= 1) != 0);
        Char *p = get(prepare_int_buffer(num_digits, spec, prefix, prefix_size));
        n = abs_value;
        do {
            *p-- = static_cast<Char>('0' + (n & 1));
        } while ((n >>= 1) != 0);
        break;
    }
    case 'o': {
        UnsignedType n = abs_value;
        if (spec.flag(HASH_FLAG))
            prefix[prefix_size++] = '0';
        unsigned num_digits = 0;
        do {
            ++num_digits;
        } while ((n >>= 3) != 0);
        Char *p = get(prepare_int_buffer(num_digits, spec, prefix, prefix_size));
        n = abs_value;
        do {
            *p-- = static_cast<Char>('0' + (n & 7));
        } while ((n >>= 3) != 0);
        break;
    }
    default:
        internal::report_unknown_type(
            spec.type(), spec.flag(CHAR_FLAG) ? "char" : "integer");
        break;
    }
}

template <typename Char>
template <typename T>
void BasicWriter<Char>::write_double(T value, const FormatSpec &spec)
{
    // Check type.
    char type = spec.type();
    bool upper = false;
    switch (type) {
    case 0:
        type = 'g';
        break;
    case 'e':
    case 'f':
    case 'g':
    case 'a':
        break;
    case 'F':
    case 'E':
    case 'G':
    case 'A':
        upper = true;
        break;
    default:
        internal::report_unknown_type(type, "double");
        break;
    }

    char sign = 0;
    // Use isnegative instead of value < 0 because the latter is always
    // false for NaN.
    if (internal::FPUtil::isnegative(static_cast<double>(value))) {
        sign = '-';
        value = -value;
    } else if (spec.flag(SIGN_FLAG)) {
        sign = spec.flag(PLUS_FLAG) ? '+' : ' ';
    }

    if (internal::FPUtil::isnotanumber(value)) {
        // Format NaN ourselves because sprintf's output is not consistent
        // across platforms.
        std::size_t nan_size = 4;
        const char *nan = upper ? " NAN" : " nan";
        if (!sign)
        {
            --nan_size;
            ++nan;
        }
        CharPtr out = write_str(nan, nan_size, spec);
        if (sign)
            *out = sign;
        return;
    }

    if (internal::FPUtil::isinfinity(value)) {
        // Format infinity ourselves because sprintf's output is not consistent
        // across platforms.
        std::size_t inf_size = 4;
        const char *inf = upper ? " INF" : " inf";
        if (!sign) {
            --inf_size;
            ++inf;
        }
        CharPtr out = write_str(inf, inf_size, spec);
        if (sign)
            *out = sign;
        return;
    }

    std::size_t offset = _buffer.size();
    unsigned width = spec.width();
    if (sign) {
        _buffer.reserve(_buffer.size() + (width > 1u ? width : 1u));
        if (width > 0)
            --width;
        ++offset;
    }

    // Build format string.
    enum
    {
        MAX_FORMAT_SIZE = 10
    }; // longest format: %#-*.*Lg
    Char format[MAX_FORMAT_SIZE];
    Char *format_ptr = format;
    *format_ptr++ = '%';
    unsigned width_for_sprintf = width;
    if (spec.flag(HASH_FLAG))
        *format_ptr++ = '#';
    if (spec.align() == ALIGN_CENTER) {
        width_for_sprintf = 0;
    } else {
        if (spec.align() == ALIGN_LEFT)
            *format_ptr++ = '-';
        if (width != 0)
            *format_ptr++ = '*';
    }
    if (spec.precision() >= 0)
    {
        *format_ptr++ = '.';
        *format_ptr++ = '*';
    }

    append_float_length(format_ptr, value);
    *format_ptr++ = type;
    *format_ptr = '\0';

    // Format using snprintf.
    Char fill = internal::CharTraits<Char>::cast(spec.fill());
    unsigned n = 0;
    Char *start = 0;
    for (;;) {
        std::size_t buffer_size = _buffer.capacity() - offset;
        start = &_buffer[offset];
        int result = internal::CharTraits<Char>::format_float(
                         start, buffer_size, format, width_for_sprintf, spec.precision(), value);
        if (result >= 0) {
            n = internal::to_unsigned(result);
            if (offset + n < _buffer.capacity())
                break;  // The buffer is large enough - continue with formatting.
            _buffer.reserve(offset + n + 1);
        } else {
            // If result is negative we ask to increase the capacity by at least 1,
            // but as std::vector, the buffer grows exponentially.
            _buffer.reserve(_buffer.capacity() + 1);
        }
    }
    if (sign) {
        if ((spec.align() != ALIGN_RIGHT && spec.align() != ALIGN_DEFAULT) ||
                *start != ' ') {
            *(start - 1) = sign;
            sign = 0;
        } else {
            *(start - 1) = fill;
        }
        ++n;
    }
    if (spec.align() == ALIGN_CENTER && spec.width() > n) {
        width = spec.width();
        CharPtr p = grow_buffer(width);
        std::memmove(get(p) + (width - n) / 2, get(p), n * sizeof(Char));
        fill_padding(p, spec.width(), n, fill);
        return;
    }
    if (spec.fill() != ' ' || sign) {
        while (*start == ' ')
            *start++ = fill;
        if (sign)
            *(start - 1) = sign;
    }
    grow_buffer(n);
}

template <typename Char, typename Allocator = std::allocator<Char> >
class BasicMemoryWriter: public BasicWriter<Char>
{
private:
    StackBuffer<Char, internal::INLINE_BUFFER_SIZE, Allocator> _buffer;

public:
    explicit BasicMemoryWriter(const Allocator& alloc = Allocator())
        : BasicWriter<Char>(_buffer), _buffer(alloc)
    {}

#if FORMAT_USE_RVALUE_REFERENCES
    BasicMemoryWriter(BasicMemoryWriter &&other)
        : BasicWriter<Char>(_buffer), _buffer(std::move(other._buffer))
    {}

    BasicMemoryWriter &operator=(BasicMemoryWriter &&other)
    {
        _buffer = std::move(other._buffer);
        return *this;
    }
#endif
};

typedef BasicMemoryWriter<char> MemoryWriter;
typedef BasicMemoryWriter<wchar_t> WMemoryWriter;


template <typename Char>
class BasicArrayWriter: public BasicWriter<Char> {
private:
    internal::FixedBuffer<Char> _buffer;

public:
    BasicArrayWriter(Char *array, std::size_t size)
        : BasicWriter<Char>(_buffer), _buffer(array, size)
    {}

    template <std::size_t SIZE>
    explicit BasicArrayWriter(Char(&array)[SIZE])
        : BasicWriter<Char>(_buffer), _buffer(array, SIZE)
    {}
};

typedef BasicArrayWriter<char> ArrayWriter;
typedef BasicArrayWriter<wchar_t> WArrayWriter;

// Formats a value.
template <typename Char, typename T>
void format(BasicFormatter<Char> &f, const Char *&format_str, const T &value)
{
    StackBuffer<Char, internal::INLINE_BUFFER_SIZE> buffer;

    internal::FormatBuf<Char> format_buf(buffer);
    std::basic_ostream<Char> output(&format_buf);
    output << value;

    BasicStringRef<Char> str(&buffer[0], format_buf.size());
    typedef internal::MakeArg< BasicFormatter<Char> > MakeArg;
    format_str = f.format(format_str, MakeArg(str));
}

// Reports a system error without throwing an exception.
// Can be used to report errors from destructors.
void report_system_error(int error_code,
                                 StringRef message) FORMAT_NOEXCEPT;


enum Color {
    BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
};

void print_colored(Color c, CStringRef format, ArgList args);

inline std::string format(CStringRef format_str, ArgList args)
{
    MemoryWriter w;
    w.write(format_str, args);
    return w.str();
}

inline std::wstring format(WCStringRef format_str, ArgList args)
{
    WMemoryWriter w;
    w.write(format_str, args);
    return w.str();
}

void print(std::FILE *f, CStringRef format_str, ArgList args);


void print(CStringRef format_str, ArgList args);

template <typename Char>
void printf(BasicWriter<Char> &w, BasicCStringRef<Char> format, ArgList args)
{
    internal::PrintfFormatter<Char>(args).format(w, format);
}

inline std::string sprintf(CStringRef format, ArgList args)
{
    MemoryWriter w;
    printf(w, format, args);
    return w.str();
}

inline std::wstring sprintf(WCStringRef format, ArgList args)
{
    WMemoryWriter w;
    printf(w, format, args);
    return w.str();
}


int fprintf(std::FILE *f, CStringRef format, ArgList args);

inline int printf(CStringRef format, ArgList args)
{
    return fprintf(stdout, format, args);
}

class FormatInt {
private:
    // BasicBuffer should be large enough to hold all digits (digits10 + 1),
    // a sign and a null character.
    enum
    {
        BUFFER_SIZE = std::numeric_limits<ULongLong>::digits10 + 3
    };
    mutable char  _buffer[BUFFER_SIZE];
    char         *_str;

    // Formats value in reverse and returns the number of digits.
    char *format_decimal(ULongLong value)
    {
        char *buffer_end = _buffer + BUFFER_SIZE - 1;
        while (value >= 100) {
            unsigned index = static_cast<unsigned>((value % 100) * 2);
            value /= 100;
            *--buffer_end = internal::Data::DIGITS[index + 1];
            *--buffer_end = internal::Data::DIGITS[index];
        }
        if (value < 10) {
            *--buffer_end = static_cast<char>('0' + value);
            return buffer_end;
        }
        unsigned index = static_cast<unsigned>(value * 2);
        *--buffer_end = internal::Data::DIGITS[index + 1];
        *--buffer_end = internal::Data::DIGITS[index];
        return buffer_end;
    }

    void FormatSigned(LongLong value)
    {
        ULongLong abs_value = static_cast<ULongLong>(value);
        bool negative = value < 0;
        if (negative){
            abs_value = 0 - abs_value;
        }
        _str = format_decimal(abs_value);
        if (negative) {
            *--_str = '-';
        }
    }

public:
    explicit FormatInt(int value)
    {
        FormatSigned(value);
    }
    explicit FormatInt(long value)
    {
        FormatSigned(value);
    }
    explicit FormatInt(LongLong value)
    {
        FormatSigned(value);
    }
    explicit FormatInt(unsigned value) : _str(format_decimal(value))
    {}
    explicit FormatInt(unsigned long value) : _str(format_decimal(value))
    {}
    explicit FormatInt(ULongLong value) : _str(format_decimal(value))
    {}

    /** Returns the number of characters written to the output buffer. */
    std::size_t size() const
    {
        return internal::to_unsigned(_buffer - _str + BUFFER_SIZE - 1);
    }

    /**
    Returns a pointer to the output buffer content. No terminating null
    character is appended.
    */
    const char *data() const
    {
        return _str;
    }

    /**
    Returns a pointer to the output buffer content with terminating null
    character appended.
    */
    const char *c_str() const
    {
        _buffer[BUFFER_SIZE - 1] = '\0';
        return _str;
    }

    /**
    \rst
    Returns the content of the output buffer as an ``std::string``.
    \endrst
    */
    std::string str() const
    {
        return std::string(_str, size());
    }
};

// Formats a decimal integer value writing into buffer and returns
// a pointer to the end of the formatted string. This function doesn't
// write a terminating null character.
template <typename T>
inline void format_decimal(char *&buffer, T value)
{
    typedef typename internal::IntTraits<T>::MainType MainType;
    MainType abs_value = static_cast<MainType>(value);
    if (internal::is_negative(value))
    {
        *buffer++ = '-';
        abs_value = 0 - abs_value;
    }
    if (abs_value < 100)
    {
        if (abs_value < 10)
        {
            *buffer++ = static_cast<char>('0' + abs_value);
            return;
        }
        unsigned index = static_cast<unsigned>(abs_value * 2);
        *buffer++ = internal::Data::DIGITS[index];
        *buffer++ = internal::Data::DIGITS[index + 1];
        return;
    }
    unsigned num_digits = internal::count_digits(abs_value);
    internal::format_decimal(buffer, abs_value, num_digits);
    buffer += num_digits;
}


template <typename T>
inline internal::NamedArg<char> arg(StringRef name, const T &arg)
{
    return internal::NamedArg<char>(name, arg);
}

template <typename T>
inline internal::NamedArg<wchar_t> arg(WStringRef name, const T &arg)
{
    return internal::NamedArg<wchar_t>(name, arg);
}

// The following two functions are deleted intentionally to disable
// nested named arguments as in ``format("{}", arg("a", arg("b", 42)))``.
template <typename Char>
void arg(StringRef, const internal::NamedArg<Char>&) FORMAT_DELETED_OR_UNDEFINED;
template <typename Char>
void arg(WStringRef, const internal::NamedArg<Char>&) FORMAT_DELETED_OR_UNDEFINED;
}

#if FORMAT_GCC_VERSION
// Use the system_header pragma to suppress warnings about variadic macros
// because suppressing -Wvariadic-macros with the diagnostic pragma doesn't
// work. It is used at the end because we want to suppress as little warnings
// as possible.
# pragma GCC system_header
#endif

// This is used to work around VC++ bugs in handling variadic macros.
#define FORMAT_EXPAND(args) args

// Returns the number of arguments.
// Based on https://groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s.
#define FORMAT_NARG(...) FORMAT_NARG_(__VA_ARGS__, FORMAT_RSEQ_N())
#define FORMAT_NARG_(...) FORMAT_EXPAND(FORMAT_ARG_N(__VA_ARGS__))
#define FORMAT_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define FORMAT_RSEQ_N() 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FORMAT_CONCAT(a, b) a##b
#define FORMAT_FOR_EACH_(N, f, ...) \
  FORMAT_EXPAND(FORMAT_CONCAT(FORMAT_FOR_EACH, N)(f, __VA_ARGS__))
#define FORMAT_FOR_EACH(f, ...) \
  FORMAT_EXPAND(FORMAT_FOR_EACH_(FORMAT_NARG(__VA_ARGS__), f, __VA_ARGS__))

#define FORMAT_ADD_ARG_NAME(type, index) type arg##index
#define FORMAT_GET_ARG_NAME(type, index) arg##index

#if FORMAT_USE_VARIADIC_TEMPLATES
# define FORMAT_VARIADIC_(Char, ReturnType, func, call, ...) \
  template <typename... Args> \
  ReturnType func(FORMAT_FOR_EACH(FORMAT_ADD_ARG_NAME, __VA_ARGS__), \
      const Args & ... args) { \
    typedef fermat::internal::ArgArray<sizeof...(Args)> ArgArray; \
    typename ArgArray::Type array{ \
      ArgArray::template make<fermat::BasicFormatter<Char> >(args)...}; \
    call(FORMAT_FOR_EACH(FORMAT_GET_ARG_NAME, __VA_ARGS__), \
      fermat::ArgList(fermat::internal::make_type(args...), array)); \
  }
#else
// Defines a wrapper for a function taking __VA_ARGS__ arguments
// and n additional arguments of arbitrary types.
# define FORMAT_WRAP(Char, ReturnType, func, call, n, ...) \
  template <FORMAT_GEN(n, FORMAT_MAKE_TEMPLATE_ARG)> \
  inline ReturnType func(FORMAT_FOR_EACH(FORMAT_ADD_ARG_NAME, __VA_ARGS__), \
      FORMAT_GEN(n, FORMAT_MAKE_ARG)) { \
    fermat::internal::ArgArray<n>::Type arr; \
    FORMAT_GEN(n, FORMAT_ASSIGN_##Char); \
    call(FORMAT_FOR_EACH(FORMAT_GET_ARG_NAME, __VA_ARGS__), fermat::ArgList( \
      fermat::internal::make_type(FORMAT_GEN(n, FORMAT_MAKE_REF2)), arr)); \
  }

# define FORMAT_VARIADIC_(Char, ReturnType, func, call, ...) \
  inline ReturnType func(FORMAT_FOR_EACH(FORMAT_ADD_ARG_NAME, __VA_ARGS__)) { \
    call(FORMAT_FOR_EACH(FORMAT_GET_ARG_NAME, __VA_ARGS__), fermat::ArgList()); \
  } \
  FORMAT_WRAP(Char, ReturnType, func, call, 1, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 2, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 3, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 4, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 5, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 6, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 7, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 8, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 9, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 10, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 11, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 12, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 13, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 14, __VA_ARGS__) \
  FORMAT_WRAP(Char, ReturnType, func, call, 15, __VA_ARGS__)
#endif  // FORMAT_USE_VARIADIC_TEMPLATES

#define FORMAT_VARIADIC(ReturnType, func, ...) \
  FORMAT_VARIADIC_(char, ReturnType, func, return func, __VA_ARGS__)

#define FORMAT_VARIADIC_W(ReturnType, func, ...) \
  FORMAT_VARIADIC_(wchar_t, ReturnType, func, return func, __VA_ARGS__)

#define FORMAT_CAPTURE_ARG_(id, index) ::fermat::arg(#id, id)

#define FORMAT_CAPTURE_ARG_W_(id, index) ::fermat::arg(L###id, id)

#define FORMAT_CAPTURE(...) FORMAT_FOR_EACH(FORMAT_CAPTURE_ARG_, __VA_ARGS__)

#define FORMAT_CAPTURE_W(...) FORMAT_FOR_EACH(FORMAT_CAPTURE_ARG_W_, __VA_ARGS__)

namespace fermat {
FORMAT_VARIADIC(std::string, format, CStringRef)
FORMAT_VARIADIC_W(std::wstring, format, WCStringRef)
FORMAT_VARIADIC(void, print, CStringRef)
FORMAT_VARIADIC(void, print, std::FILE *, CStringRef)

FORMAT_VARIADIC(void, print_colored, Color, CStringRef)
FORMAT_VARIADIC(std::string, sprintf, CStringRef)
FORMAT_VARIADIC_W(std::wstring, sprintf, WCStringRef)
FORMAT_VARIADIC(int, printf, CStringRef)
FORMAT_VARIADIC(int, fprintf, std::FILE *, CStringRef)

#if FORMAT_USE_IOSTREAMS

void print(std::ostream &os, CStringRef format_str, ArgList args);
FORMAT_VARIADIC(void, print, std::ostream &, CStringRef)

int fprintf(std::ostream &os, CStringRef format_str, ArgList args);
FORMAT_VARIADIC(int, fprintf, std::ostream &, CStringRef)
#endif

namespace internal {
template <typename Char>
inline bool is_name_start(Char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || '_' == c;
}

// Parses an unsigned integer advancing s to the end of the parsed input.
// This function assumes that the first character of s is a digit.
template <typename Char>
unsigned parse_nonnegative_int(const Char *&s)
{
    assert('0' <= *s && *s <= '9');
    unsigned value = 0;
    do
    {
        unsigned new_value = value * 10 + (*s++ - '0');
        // Check if value wrapped around.
        if (new_value < value)
        {
            value = (std::numeric_limits<unsigned>::max)();
            break;
        }
        value = new_value;
    }
    while ('0' <= *s && *s <= '9');
    // Convert to unsigned to prevent a warning.
    unsigned max_int = (std::numeric_limits<int>::max)();
    if (value > max_int)
        FORMAT_THROW(FormatError("number is too big"));
    return value;
}

inline void require_numeric_argument(const Arg &arg, char spec)
{
    if (arg.type > Arg::LAST_NUMERIC_TYPE)
    {
        std::string message =
            fermat::format("format specifier '{}' requires numeric argument", spec);
        FORMAT_THROW(fermat::FormatError(message));
    }
}

template <typename Char>
void check_sign(const Char *&s, const Arg &arg)
{
    char sign = static_cast<char>(*s);
    require_numeric_argument(arg, sign);
    if (arg.type == Arg::UINT || arg.type == Arg::ULONG_LONG)
    {
        FORMAT_THROW(FormatError(fermat::format(
                                  "format specifier '{}' requires signed argument", sign)));
    }
    ++s;
}
}  // namespace internal

template <typename Char, typename AF>
inline internal::Arg BasicFormatter<Char, AF>::get_arg(
    BasicStringRef<Char> arg_name, const char *&error)
{
    if (check_no_auto_index(error)) {
        _map.init(args());
        const internal::Arg *arg = _map.find(arg_name);
        if (arg)
            return *arg;
        error = "argument not found";
    }
    return internal::Arg();
}

template <typename Char, typename AF>
inline internal::Arg BasicFormatter<Char, AF>::parse_arg_index(const Char *&s)
{
    const char *error = 0;
    internal::Arg arg = *s < '0' || *s > '9' ?
                        next_arg(error) : get_arg(internal::parse_nonnegative_int(s), error);
    if (error)
    {
        FORMAT_THROW(FormatError(
                      *s != '}' && *s != ':' ? "invalid format string" : error));
    }
    return arg;
}

template <typename Char, typename AF>
inline internal::Arg BasicFormatter<Char, AF>::parse_arg_name(const Char *&s)
{
    assert(internal::is_name_start(*s));
    const Char *start = s;
    Char c;
    do
    {
        c = *++s;
    }
    while (internal::is_name_start(c) || ('0' <= c && c <= '9'));
    const char *error = 0;
    internal::Arg arg = get_arg(BasicStringRef<Char>(start, s - start), error);
    if (error)
        FORMAT_THROW(FormatError(error));
    return arg;
}

template <typename Char, typename ArgFormatter>
const Char *BasicFormatter<Char, ArgFormatter>::format(
    const Char *&format_str, const internal::Arg &arg)
{
    using internal::Arg;
    const Char *s = format_str;
    FormatSpec spec;
    if (*s == ':') {
        if (arg.type == Arg::CUSTOM) {
            arg.custom.format(this, arg.custom.value, &s);
            return s;
        }
        ++s;
        // Parse fill and alignment.
        if (Char c = *s) {
            const Char *p = s + 1;
            spec._align = ALIGN_DEFAULT;
            do {
                switch (*p) {
                case '<':
                    spec._align = ALIGN_LEFT;
                    break;
                case '>':
                    spec._align = ALIGN_RIGHT;
                    break;
                case '=':
                    spec._align = ALIGN_NUMERIC;
                    break;
                case '^':
                    spec._align = ALIGN_CENTER;
                    break;
                }
                if (spec._align != ALIGN_DEFAULT) {
                    if (p != s) {
                        if (c == '}') break;
                        if (c == '{')
                            FORMAT_THROW(FormatError("invalid fill character '{'"));
                        s += 2;
                        spec._fill = c;
                    }
                    else ++s;
                    if (spec._align == ALIGN_NUMERIC)
                        require_numeric_argument(arg, '=');
                    break;
                }
            } while (--p >= s);
        }

        // Parse sign.
        switch (*s) {
        case '+':
            check_sign(s, arg);
            spec._flags |= SIGN_FLAG | PLUS_FLAG;
            break;
        case '-':
            check_sign(s, arg);
            spec._flags |= MINUS_FLAG;
            break;
        case ' ':
            check_sign(s, arg);
            spec._flags |= SIGN_FLAG;
            break;
        }

        if (*s == '#') {
            require_numeric_argument(arg, '#');
            spec._flags |= HASH_FLAG;
            ++s;
        }

        // Parse zero flag.
        if (*s == '0') {
            require_numeric_argument(arg, '0');
            spec._align = ALIGN_NUMERIC;
            spec._fill = '0';
            ++s;
        }

        // Parse width.
        if ('0' <= *s && *s <= '9') {
            spec._width = internal::parse_nonnegative_int(s);
        } else if (*s == '{') {
            ++s;
            Arg width_arg = internal::is_name_start(*s) ?
                            parse_arg_name(s) : parse_arg_index(s);
            if (*s++ != '}')
                FORMAT_THROW(FormatError("invalid format string"));
            ULongLong value = 0;
            switch (width_arg.type) {
                case Arg::INT:
                    if (width_arg.int_value < 0)
                        FORMAT_THROW(FormatError("negative width"));
                    value = width_arg.int_value;
                    break;
                case Arg::UINT:
                    value = width_arg.uint_value;
                    break;
                case Arg::LONG_LONG:
                    if (width_arg.long_long_value < 0)
                        FORMAT_THROW(FormatError("negative width"));
                    value = width_arg.long_long_value;
                    break;
                case Arg::ULONG_LONG:
                    value = width_arg.ulong_long_value;
                    break;
                default:
                    FORMAT_THROW(FormatError("width is not integer"));
            }
            if (value >(std::numeric_limits<int>::max)())
                FORMAT_THROW(FormatError("number is too big"));
            spec._width = static_cast<int>(value);
        }

        // Parse precision.
        if (*s == '.') {
            ++s;
            spec._precision = 0;
            if ('0' <= *s && *s <= '9') {
                spec._precision = internal::parse_nonnegative_int(s);
            } else if (*s == '{') {
                ++s;
                Arg precision_arg = internal::is_name_start(*s) ?
                                    parse_arg_name(s) : parse_arg_index(s);
                if (*s++ != '}')
                    FORMAT_THROW(FormatError("invalid format string"));
                ULongLong value = 0;
                switch (precision_arg.type) {
                    case Arg::INT:
                        if (precision_arg.int_value < 0)
                            FORMAT_THROW(FormatError("negative precision"));
                        value = precision_arg.int_value;
                        break;
                    case Arg::UINT:
                        value = precision_arg.uint_value;
                        break;
                    case Arg::LONG_LONG:
                        if (precision_arg.long_long_value < 0)
                            FORMAT_THROW(FormatError("negative precision"));
                        value = precision_arg.long_long_value;
                        break;
                    case Arg::ULONG_LONG:
                        value = precision_arg.ulong_long_value;
                        break;
                    default:
                        FORMAT_THROW(FormatError("precision is not integer"));
                    }
                    if (value >(std::numeric_limits<int>::max)())
                        FORMAT_THROW(FormatError("number is too big"));
                    spec._precision = static_cast<int>(value);
            } else {
                FORMAT_THROW(FormatError("missing precision specifier"));
            }
            if (arg.type <= Arg::LAST_INTEGER_TYPE || arg.type == Arg::POINTER) {
                FORMAT_THROW(FormatError(
                              fermat::format("precision not allowed in {} format specifier",
                                          arg.type == Arg::POINTER ? "pointer" : "integer")));
            }
        }

        // Parse type.
        if (*s != '}' && *s)
            spec._type = static_cast<char>(*s++);
    }

    if (*s++ != '}')
        FORMAT_THROW(FormatError("missing '}' in format string"));

    // Format argument.
    ArgFormatter(*this, spec, s - 1).visit(arg);
    return s;
}

template <typename Char, typename AF>
void BasicFormatter<Char, AF>::format(BasicCStringRef<Char> format_str)
{
    const Char *s = format_str.c_str();
    const Char *start = s;
    while (*s) {
        Char c = *s++;
        if (c != '{' && c != '}') continue;
        if (*s == c) {
            write(_writer, start, s);
            start = ++s;
            continue;
        }
        if (c == '}')
            FORMAT_THROW(FormatError("unmatched '}' in format string"));
        write(_writer, start, s - 1);
        internal::Arg arg = internal::is_name_start(*s) ?
                            parse_arg_name(s) : parse_arg_index(s);
        start = s = format(s, arg);
    }
    write(_writer, start, s);
}
}  // namespace fermat

#if FORMAT_USE_USER_DEFINED_LITERALS
namespace fermat {
namespace internal {

template <typename Char>
struct UdlFormat {
    const Char *str;

    template <typename... Args>
    auto operator()(Args && ... args) const
    -> decltype(format(str, std::forward<Args>(args)...))
    {
        return format(str, std::forward<Args>(args)...);
    }
};

template <typename Char>
struct UdlArg {
    const Char *str;

    template <typename T>
    NamedArg<Char> operator=(T &&value) const
    {
        return { str, std::forward<T>(value) };
    }
};

} // namespace internal

inline namespace literals {


inline internal::UdlFormat<char>
operator"" _format(const char *s, std::size_t)
{
    return { s };
}
inline internal::UdlFormat<wchar_t>
operator"" _format(const wchar_t *s, std::size_t)
{
    return { s };
}


inline internal::UdlArg<char>
operator"" _a(const char *s, std::size_t)
{
    return { s };
}
inline internal::UdlArg<wchar_t>
operator"" _a(const wchar_t *s, std::size_t)
{
    return { s };
}

} // inline namespace literals
}
#endif // FORMAT_USE_USER_DEFINED_LITERALS

#if FORMAT_GCC_VERSION >= 406
# pragma GCC diagnostic pop
#endif

#if defined(__clang__) && !defined(__INTEL_COMPILER)
# pragma clang diagnostic pop
#endif


#ifdef FORMAT_HEADER_ONLY
# include "format_inl.h"
#endif

#endif
