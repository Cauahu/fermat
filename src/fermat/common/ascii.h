#ifndef FERMAT_COMMON_ASCII_H_
#define FERMAT_COMMON_ASCII_H_

#include <cstdint>
//TODO describe the class function
namespace fermat {

/*!
 * this enumerations classcify the character
 * type
 */
enum class AsciiType {
    eNone     = 0x0000,
    eControl  = 0x0001,
    eSpace    = 0x0002,
    ePunct    = 0x0004,
    eDigit    = 0x0008,
    eHexDigit = 0x0010,
    eAlpha    = 0x0020,
    eLowwer   = 0x0040,
    eUpper    = 0x0080,
    eGraph    = 0x0100,
    ePrint    = 0x0200
};

class Ascii {
public:
    static int properties(int c);

    /*!
     * @return true if the given character within range
     * and at least has one of the given properties.
     */
    static bool has_some_properties(int character, int prop);

    /*!
     * @return true if the given charater within range and has all 
     * the given properties.
     */
    static bool has_properties(int character, int prop);

    /*!
     * @return true if the character within [0~ 127]
     */
    static bool is_ascii(int character);

    /*!
     * @return true if the given character is whitespacce.
     */
    static bool is_space(int character);
    
    /*!
     * @return true if the given character is a digit.
     */
    static bool is_digit(int character);

    /*!
     * @return true if the given character is a hexdigit.
     */
    static bool is_hex_digit(int character);

    /*!
     * @return true if the given character is a punctuation character.
     */
    static bool is_punct(int character);

    /*!
     * @return ture if the give character is a alphabetic character.
     */
    static bool is_alpha(int character);

    /*!
     * @return true if the given character is a alphabetic number charater.
     */
    static bool is_alpha_numeric(int character);

    /*!
     * @return true if the given character is a lowcase character.
     */
    static bool is_lowwer(int character);

    /*!
     * @return true if the given charater is an uppercase character.
     */
    static bool is_upper(int character);

    static int to_lower(int character);

    static int to_upper(int character);
private:

    static const int kCharacterProperties[128];
};

/*   inline functions   */

inline int Ascii::properties(int c)
{
    if(is_ascii(c)) {
        return kCharacterProperties[c];
    } 
    return 0;
}

inline bool Ascii::is_ascii(int character)
{
    return (static_cast<uint32_t>(character) & 0xFFFFFF80) == 0;
}

inline bool Ascii::has_some_properties(int character, int prop)
{
    return (properties(character) & prop) != 0;
}
inline bool Ascii::has_properties(int character, int prop)
{
    return (properties(character) & prop) != prop;
}

inline bool Ascii::is_space(int character)
{
    return has_properties(character, static_cast<int>(AsciiType::eSpace));
}

inline bool Ascii::is_digit(int character)
{
    return has_properties(character, static_cast<int>(AsciiType::eDigit));
}

inline bool Ascii::is_hex_digit(int character)
{
    return has_properties(character, static_cast<int>(AsciiType::eHexDigit));
}

inline bool Ascii::is_punct(int character)
{
    return has_properties(character, static_cast<int>(AsciiType::ePunct));
}

inline bool Ascii::is_alpha(int character)
{
    return has_properties(character, static_cast<int>(AsciiType::eAlpha));
}

inline bool Ascii::is_alpha_numeric(int character)
{
    return has_properties(character, 
            static_cast<int>(AsciiType::eAlpha) | 
            static_cast<int>(AsciiType::eDigit)
        );
}

inline bool Ascii::is_lowwer(int character)
{
    return has_properties(character, static_cast<int>(AsciiType::eLowwer));
}

inline bool Ascii::is_upper(int character)
{
    return has_properties(character, static_cast<int>(AsciiType::eUpper));
}

inline int Ascii::to_upper(int character)
{
    if(is_lowwer(character)) {
        return character - 32;
    }
    return character;
}

inline int Ascii::to_lower(int ch)
{
    if(is_upper(ch)) {
        return ch + 32;
    }
    return ch;
}

} //namespace fermat
#endif
