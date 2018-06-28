#include <bkbase/formats.h>

namespace bkbase {


DigitFormatsFast<1> Digit2("01");
DigitFormatsFast<3> Digit8("01234567");
DigitFormats Digit10("0123456789");
DigitFormatsFast<4> Digit16("0123456789ABCDEF", "0123456789abcdef");
DigitFormats Digit58("123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz");
DigitFormatsFast<6> Digit64("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

} // bkbase
