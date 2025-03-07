#include "color.h"
//#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

namespace mssm {

Color Color::fromHexRGB(std::string hexstring)
{
    if (hexstring.empty()) {
        return Color(255,255,255);
    }
    else {
        // strip 0x prefix
        if (hexstring.substr(0,2) == "0x") {
            hexstring = hexstring.substr(2);
        }
        return Color::fromIntRGB(stoi(hexstring, 0, 16));
    }
}

string Color::toHexRGB() const
{
    if (a == 0) {
        return "none";
    }
    stringstream ss;
    ss << "#" << hex << setw(2) << (int)r << hex << setw(2) << (int)g << hex << setw(2) << (int)b;
    return ss.str();
}

}
