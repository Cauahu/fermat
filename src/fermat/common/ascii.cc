#include <fermat/common/ascii.h>

namespace fermat {

static const int c_s = static_cast<int>(AsciiType::eControl) | 
                     static_cast<int>(AsciiType::eSpace);
static const int p_s = static_cast<int>(AsciiType::ePrint) | 
                       static_cast<int>(AsciiType::eSpace);
static const int p_p_g = static_cast<int>(AsciiType::ePrint) | 
                         static_cast<int>(AsciiType::ePunct) | 
                         static_cast<int>(AsciiType::eGraph);
static const int p_p_g_d = static_cast<int>(AsciiType::ePrint) | 
                           static_cast<int>(AsciiType::ePunct) | 
                           static_cast<int>(AsciiType::eGraph) | 
                           static_cast<int>(AsciiType::eDigit);
static const int p_a_g_h_u = static_cast<int>(AsciiType::ePrint) | 
                             static_cast<int>(AsciiType::eAlpha) | 
                             static_cast<int>(AsciiType::eGraph) | 
                             static_cast<int>(AsciiType::eHexDigit) | 
                             static_cast<int>(AsciiType::eUpper);

static const int p_a_g_u = static_cast<int>(AsciiType::ePrint) | 
                             static_cast<int>(AsciiType::eAlpha) | 
                             static_cast<int>(AsciiType::eGraph) |  
                             static_cast<int>(AsciiType::eUpper);

static const int p_a_g_h_l = static_cast<int>(AsciiType::ePrint) | 
                             static_cast<int>(AsciiType::eAlpha) | 
                             static_cast<int>(AsciiType::eGraph) | 
                             static_cast<int>(AsciiType::eHexDigit) | 
                             static_cast<int>(AsciiType::eLowwer);
static const int p_a_g_l = static_cast<int>(AsciiType::ePrint) | 
                             static_cast<int>(AsciiType::eAlpha) | 
                             static_cast<int>(AsciiType::eGraph) |  
                             static_cast<int>(AsciiType::eLowwer);

const int Ascii::kCharacterProperties[128] = 
{
    /*00 . */ static_cast<int>(AsciiType::eControl),
    /*01 . */ static_cast<int>(AsciiType::eControl),
    /*02 . */ static_cast<int>(AsciiType::eControl),
    /*03 . */ static_cast<int>(AsciiType::eControl),
    /*04 . */ static_cast<int>(AsciiType::eControl),
    /*05 . */ static_cast<int>(AsciiType::eControl),
    /*06 . */ static_cast<int>(AsciiType::eControl),
    /*07 . */ static_cast<int>(AsciiType::eControl),
    /*08 . */ static_cast<int>(AsciiType::eControl),
    /*09 . */ c_s ,
    /*0a . */ c_s ,
    /*0b . */ c_s ,
    /*0c . */ c_s ,
    /*0d . */ c_s ,
    /*0e . */ static_cast<int>(AsciiType::eControl),
    /*0f . */ static_cast<int>(AsciiType::eControl),
    /*11 . */ static_cast<int>(AsciiType::eControl),
    /*12 . */ static_cast<int>(AsciiType::eControl),
    /*13 . */ static_cast<int>(AsciiType::eControl),
    /*14 . */ static_cast<int>(AsciiType::eControl),
    /*15 . */ static_cast<int>(AsciiType::eControl),
    /*16 . */ static_cast<int>(AsciiType::eControl),
    /*17 . */ static_cast<int>(AsciiType::eControl),
    /*18 . */ static_cast<int>(AsciiType::eControl),
    /*19 . */ static_cast<int>(AsciiType::eControl),
    /*1a . */ static_cast<int>(AsciiType::eControl),
    /*1b . */ static_cast<int>(AsciiType::eControl),
    /*1c . */ static_cast<int>(AsciiType::eControl),
    /*1d . */ static_cast<int>(AsciiType::eControl),
    /*1e . */ static_cast<int>(AsciiType::eControl),
    /*1f . */ static_cast<int>(AsciiType::eControl),
    /*20   */ p_s,
    /*21 ! */ p_p_g,
    /*22 " */ p_p_g,
    /*23 # */ p_p_g,
    /*24 $ */ p_p_g,
    /*25 % */ p_p_g,
    /*26 & */ p_p_g,
    /*27 ' */ p_p_g,
    /*28 ( */ p_p_g,
    /*29 ) */ p_p_g,
    /*2a * */ p_p_g,
    /*2b + */ p_p_g,
    /*2c , */ p_p_g,
    /*2d - */ p_p_g,
    /*2e . */ p_p_g,
    /*2f / */ p_p_g,
    /*30 0 */ p_p_g_d,
    /*31 1 */ p_p_g_d,
    /*32 2 */ p_p_g_d,
    /*33 3 */ p_p_g_d,
    /*34 4 */ p_p_g_d,
    /*35 5 */ p_p_g_d,
    /*36 6 */ p_p_g_d,
    /*37 7 */ p_p_g_d,
    /*38 8 */ p_p_g_d,
    /*39 9 */ p_p_g_d,
    /*3a : */ p_p_g,
    /*3b ; */ p_p_g,
    /*3c < */ p_p_g,
    /*3d = */ p_p_g,
    /*3e > */ p_p_g,
    /*3f ? */ p_p_g,
    /*40 @ */ p_p_g,
    /*41 A */ p_a_g_h_u,
    /*42 B */ p_a_g_h_u,
    /*43 C */ p_a_g_h_u,
    /*44 D */ p_a_g_h_u,
    /*45 E */ p_a_g_h_u,
    /*46 F */ p_a_g_h_u,
    /*47 G */ p_a_g_u,
    /*48 H */ p_a_g_u,
    /*49 I */ p_a_g_u,
    /*4a J */ p_a_g_u,
    /*4b K */ p_a_g_u,
    /*4c L */ p_a_g_u,
    /*4d M */ p_a_g_u,
    /*4e N */ p_a_g_u,
    /*4f O */ p_a_g_u,
    /*50 P */ p_a_g_u,
    /*51 Q */ p_a_g_u,
    /*52 R */ p_a_g_u,
    /*53 S */ p_a_g_u,
    /*54 T */ p_a_g_u,
    /*55 U */ p_a_g_u,
    /*56 V */ p_a_g_u,
    /*57 W */ p_a_g_u,
    /*58 X */ p_a_g_u,
    /*59 Y */ p_a_g_u,
    /*5a Z */ p_a_g_u,
    /*5b [ */ p_p_g,
    /*5c \ */ p_p_g,
    /*5d ] */ p_p_g,
    /*5e ^ */ p_p_g,
    /*5f _ */ p_p_g,
    /*60 ` */ p_p_g,
    /*61 a */ p_a_g_h_l,
    /*62 b */ p_a_g_h_l,
    /*63 c */ p_a_g_h_l,
    /*64 d */ p_a_g_h_l,
    /*65 e */ p_a_g_h_l,
    /*66 f */ p_a_g_h_l,
    /*67 g */ p_a_g_l,
    /*68 h */ p_a_g_l,
    /*69 i */ p_a_g_l,
    /*6a j */ p_a_g_l,
    /*6b k */ p_a_g_l,
    /*6c l */ p_a_g_l,
    /*6d m */ p_a_g_l,
    /*6e n */ p_a_g_l,
    /*6f o */ p_a_g_l,
    /*70 p */ p_a_g_l,
    /*71 q */ p_a_g_l,
    /*72 r */ p_a_g_l,
    /*73 s */ p_a_g_l,
    /*74 t */ p_a_g_l,
    /*75 u */ p_a_g_l,
    /*76 v */ p_a_g_l,
    /*77 w */ p_a_g_l,
    /*78 x */ p_a_g_l,
    /*79 y */ p_a_g_l,
    /*7a z */ p_a_g_l,
    /*7b { */ p_p_g,
    /*7c | */ p_p_g,
    /*7d } */ p_p_g,
    /*7e ~ */ p_p_g,
    /*7f . */ static_cast<int>(AsciiType::eControl)
};


}