#ifndef MSSM_PRINT_COMPAT_H
#define MSSM_PRINT_COMPAT_H

// This header provides compatibility for C++23's print/println functions
// It will use the native implementation when available, and fallback to
// the fmt library when needed

// First check if the CMake-defined variable exists
#if defined(HAS_CPP23_PRINT) && HAS_CPP23_PRINT
// Use the value from CMake
#define MSSM_HAS_CPP23_PRINT 1
#elif __cpp_lib_print >= 202207L
// Fallback to checking the feature test macro
#define MSSM_HAS_CPP23_PRINT 1
#else
// Neither CMake nor feature test macro indicates print is available
#define MSSM_HAS_CPP23_PRINT 0
#endif

// Include the appropriate headers based on what's available
#if MSSM_HAS_CPP23_PRINT
#include <print>
#else
#include <fmt/core.h>
#include <fmt/format.h>

// Create std::print and std::println fallbacks using fmt
namespace std {
// Format string with variadic arguments - using fmt::format_string instead of std::string_view
template<typename... Args>
void print(fmt::format_string<Args...> fmt_str, Args&&... args) {
    fmt::print(fmt_str, std::forward<Args>(args)...);
}

template<typename... Args>
void println(fmt::format_string<Args...> fmt_str, Args&&... args) {
    fmt::print(fmt_str, std::forward<Args>(args)...);
    fmt::print("\n");
}

// Special case for no arguments
inline void println() {
    fmt::print("\n");
}
}
#endif

// Define formatter macros for use in implementing custom types
#if !MSSM_HAS_CPP23_PRINT
// Helper macro to define formatters for custom types in fmt
#define MSSM_DEFINE_FORMATTER(Type, FormatFunc) \
template <> \
    struct fmt::formatter<Type> { \
        constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin()) { \
            return ctx.begin(); \
    } \
        template <typename FormatContext> \
        auto format(const Type& value, FormatContext& ctx) const -> decltype(ctx.out()) { \
            return FormatFunc(value, ctx); \
    } \
}
#else
// Helper macro to define formatters for custom types in std
#define MSSM_DEFINE_FORMATTER(Type, FormatFunc) \
template <> \
    struct std::formatter<Type> : std::formatter<std::string> { \
        template <typename FormatContext> \
        auto format(const Type& value, FormatContext& ctx) const { \
            return FormatFunc(value, ctx); \
    } \
}
#endif

#endif // MSSM_PRINT_COMPAT_H
