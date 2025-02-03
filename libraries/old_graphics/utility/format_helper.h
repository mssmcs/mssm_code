#ifndef FORMAT_HELPER_H
#define FORMAT_HELPER_H

#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/color.h"



// got the code below from here: https://godbolt.org/z/bqjo4GK3Y
//
#include <iostream>
#include <iterator>

// With this concept...
template <typename T>
concept trivially_formattable = requires (const T& x, fmt::format_context::iterator i) {
    { format_value(x, i) } -> std::same_as<fmt::format_context::iterator>;
};

// And this partial specialization...
template <trivially_formattable T>
struct fmt::formatter<T> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(const T& thing, auto& ctx) const {
        return format_value(thing, ctx.out());
    }
};

// Example
//// We can format this...
//struct MyThing {
//    int x, y, z;
//};
// with this...
//auto format_value(const MyThing& thing, auto i) {
//    return fmt::format_to(i, "({}, {}, {})", thing.x, thing.y, thing.z);
//}

#endif // FORMAT_HELPER_H
