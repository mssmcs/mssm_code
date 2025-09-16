#ifndef MSSM_ARRAY_H
#define MSSM_ARRAY_H

// Include the print_compat.h header instead of directly including <print>
#include "print_compat.h"

// Rest of includes
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <sstream>

namespace mssm {

#ifdef DISABLE_ARRAY_SAFETY_CHECKS
constexpr bool enable_array_safety_checks = false;
#else
constexpr bool enable_array_safety_checks = true;
#endif

template <typename ARR, typename EL>
class Array_iter {
public:
    using difference_type = int;
    using value_type = EL;
    using pointer = EL*;
    using reference = EL&;
    using Arrray_Array_iter_category = std::random_access_iterator_tag;
private:
    ARR ptr;
    int idx;
public:
    Array_iter(ARR ptr, int idx) : ptr{ptr}, idx{idx} { }
    void checkSameContainer(const Array_iter& other) const;

    reference operator*() const { return (*ptr)[idx]; }

    difference_type operator-(const Array_iter& rhs) const;
    Array_iter operator+(difference_type rhs) const;
    Array_iter operator-(difference_type rhs) const;

    bool operator==(const Array_iter& rhs) const {
        if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
        return idx == rhs.idx; }
    bool operator!=(const Array_iter& rhs) const {
        if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
        return idx != rhs.idx; }
    bool operator>(const Array_iter& rhs) const  {
        if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
        return idx > rhs.idx; }
    bool operator<(const Array_iter& rhs) const  {
        if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
        return idx < rhs.idx; }
    bool operator>=(const Array_iter& rhs) const {
        if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
        return idx >= rhs.idx; }
    bool operator<=(const Array_iter& rhs) const {
        if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
        return idx <= rhs.idx; }

public:

    Array_iter& operator++();
    Array_iter operator++(int);
    Array_iter& operator--();
    Array_iter operator--(int);
};


template<typename T, typename Q = T>
class ArrayBase {
private:
    std::vector<Q> elements;
public:

public:
    ArrayBase();
    ArrayBase(std::initializer_list<Q> items);
    T& operator[](int index);
    const T& operator[](int index) const;
    void append(T element);
    void push_back(T element) { append(element); }
    void removeAtIndex(int index);
    void insertAtIndex(int index, T element);
    int  size() const { return static_cast<int>(elements.size()); }
    void resize(int newSize, const T& value = T{});
    void clear();
    constexpr bool isIndexInRange(int index) const { return index >= 0 && index < elements.size(); }
    Array_iter<ArrayBase*, T> begin() { return {this, 0}; }
    Array_iter<ArrayBase*, T> end() { return {this, static_cast<int>(elements.size()) }; }
    Array_iter<const ArrayBase*, const T> begin() const { return {this, 0}; }
    Array_iter<const ArrayBase*, const T> end() const { return {this, elements.size()}; }
    void sort();
    T& front();
    T& back();
    const T& front() const;
    const T& back() const;
    bool empty() const { return elements.empty(); }
    std::vector<Q>& asVector() { return elements; }
    const std::vector<Q>& asVector() const { return elements; }

    operator std::vector<Q>&() { return elements; }
    operator const std::vector<Q>&() const { return elements; }

private:
    constexpr bool isIndexInRangeIncludingEnd(int index) const { return index >= 0 && index <= elements.size(); }
    inline void throwOnBadIndexIncludingEnd(int index) const;
    inline void throwOnBadIndex(int index) const;
    void throwIndexError(int index) const;

    friend Array_iter<const ArrayBase*, const T>;
    friend Array_iter<ArrayBase*, T>;
};

// Include Array implementation details...
template<typename ARR, typename EL>
void Array_iter<ARR, EL>::checkSameContainer(const Array_iter<ARR, EL>& other) const
{
    if (other.ptr != ptr) {
        throw std::logic_error("Can't mix iterators from different ArrayBase<T,Q> containers!");
    }
}

template<typename ARR, typename EL>
Array_iter<ARR, EL> Array_iter<ARR, EL>::operator++(int)
{
    Array_iter tmp = *this;
    idx++;
    if constexpr (enable_array_safety_checks) {
        if (!ptr->isIndexInRangeIncludingEnd(idx)) { ptr->throwIndexError(idx); }
    }
    return tmp;
}

template<typename ARR, typename EL>
Array_iter<ARR, EL> &Array_iter<ARR, EL>::operator++()
{
    idx++;
    if constexpr (enable_array_safety_checks) {
        if (!ptr->isIndexInRangeIncludingEnd(idx)) {
            ptr->throwIndexError(idx);
        }
    }
    return *this;
}

template<typename ARR, typename EL>
Array_iter<ARR, EL> Array_iter<ARR, EL>::operator--(int)
{
    Array_iter tmp = *this;
    idx--;
    if constexpr (enable_array_safety_checks) {
        if (!ptr->isIndexInRangeIncludingEnd(idx)) {
            ptr->throwIndexError(idx);
        }
    }
    return tmp;
}

template<typename ARR, typename EL>
Array_iter<ARR, EL> &Array_iter<ARR, EL>::operator--()
{
    idx--;
    if constexpr (enable_array_safety_checks) {
        if (!ptr->isIndexInRangeIncludingEnd(idx)) {
            ptr->throwIndexError(idx);
        }
    }
    return *this;
}

template<typename ARR, typename EL>
typename Array_iter<ARR, EL>::difference_type Array_iter<ARR, EL>::operator-(const Array_iter<ARR, EL>& rhs) const
{
    if constexpr (enable_array_safety_checks) {
        checkSameContainer(rhs);
    }
    return idx - rhs.idx;
}

template<typename ARR, typename EL>
Array_iter<ARR, EL> Array_iter<ARR, EL>::operator+(difference_type rhs) const
{
    Array_iter ret = *this;
    ret.idx += rhs;
    if constexpr (enable_array_safety_checks) {
        if (!ret.ptr->isIndexInRangeIncludingEnd(ret.idx)) {
            ret.ptr->throwIndexError(ret.idx);
        }
    }
    return ret;
}

template<typename ARR, typename EL>
Array_iter<ARR, EL> Array_iter<ARR, EL>::operator-(difference_type rhs) const
{
    Array_iter ret = *this;
    ret.idx -= rhs;
    if constexpr (enable_array_safety_checks) {
        if (!ret.ptr->isIndexInRangeIncludingEnd(ret.idx)) {
            ret.ptr->throwIndexError(ret.idx);
        }
    }
    return ret;
}

template<typename ARR, typename EL>
Array_iter<ARR, EL> operator+(typename Array_iter<ARR, EL>::difference_type lhs, const Array_iter<ARR, EL>& rhs)
{
    Array_iter<ARR, EL> ret = rhs;
    ret.idx += lhs;
    if constexpr (enable_array_safety_checks) {
        if (!ret.ptr->isIndexInRangeIncludingEnd(ret.idx)) {
            ret.ptr->throwIndexError(ret.idx);
        }
    }
    return ret;
}

template<typename ARR, typename EL>
Array_iter<ARR, EL> operator-(typename Array_iter<ARR, EL>::difference_type lhs, const Array_iter<ARR, EL>& rhs)
{
    Array_iter<ARR, EL> ret = rhs;
    ret.idx -= lhs;
    if constexpr (enable_array_safety_checks) {
        if (!ret.ptr->isIndexInRangeIncludingEnd(ret.idx)) {
            ret.ptr->throwIndexError(ret.idx);
        }
    }
    return ret;
}

template<typename T, typename Q>
ArrayBase<T,Q>::ArrayBase()
{
}

template<typename T, typename Q>
ArrayBase<T,Q>::ArrayBase(std::initializer_list<Q> items)
    : elements{items}
{
}

template<typename T, typename Q>
T &ArrayBase<T,Q>::operator[](int index)
{
    if constexpr (enable_array_safety_checks) {
        if (!isIndexInRange(index)) {
            throwIndexError(index);
        }
    }
    return elements[index];
}

template<typename T, typename Q>
const T &ArrayBase<T,Q>::operator[](int index) const
{
    if constexpr (enable_array_safety_checks) {
        if (!isIndexInRange(index)) {
            throwIndexError(index);
        }
    }
    return elements[index];
}

template<typename T, typename Q>
void ArrayBase<T,Q>::append(T element)
{
    elements.push_back(element);
}

template<typename T, typename Q>
void ArrayBase<T,Q>::removeAtIndex(int index)
{
    if constexpr (enable_array_safety_checks) {
        if (!isIndexInRange(index)) {
            throwIndexError(index);
        }
    }
    elements.erase(elements.begin()+index);
}

template<typename T, typename Q>
void ArrayBase<T,Q>::insertAtIndex(int index, T element)
{
    if (index == elements.size()) {
        elements.push_back(element);
        return;
    }

    if constexpr (enable_array_safety_checks) {
        if (!isIndexInRange(index)) {
            throwIndexError(index);
        }
    }
    elements.insert(elements.begin()+index, element);
}

template<typename T, typename Q>
void ArrayBase<T,Q>::resize(int newSize, const T &value)
{
    if constexpr (enable_array_safety_checks) {
        if (newSize < 0) {
            std::string msg = "Error:  ArrayBase cannot be resized to a negative size!";
            std::cout << msg << std::endl;
            throw std::invalid_argument(msg);
        }
    }
    elements.resize(newSize, value);
}

template<typename T, typename Q>
void ArrayBase<T,Q>::clear()
{
    elements.clear();
}

template<typename T, typename Q>
void ArrayBase<T,Q>::sort()
{
    std::sort(elements.begin(), elements.end());
}

template<typename T, typename Q>
T &ArrayBase<T,Q>::front()
{
    if constexpr (enable_array_safety_checks) {
        if (!isIndexInRange(0)) {
            throwIndexError(0);
        }
    }
    return elements.front();
}

template<typename T, typename Q>
T &ArrayBase<T,Q>::back()
{
    if constexpr (enable_array_safety_checks) {
        if (!isIndexInRange(0)) {
            throwIndexError(0);
        }
    }
    return elements.back();
}

template<typename T, typename Q>
const T &ArrayBase<T,Q>::front() const
{
    if constexpr (enable_array_safety_checks) {
        if (!isIndexInRange(0)) {
            throwIndexError(0);
        }
    }
    return elements.front();
}

template<typename T, typename Q>
const T &ArrayBase<T,Q>::back() const
{
    if constexpr (enable_array_safety_checks) {
        if (!isIndexInRange(0)) {
            throwIndexError(0);
        }
    }
    return elements.back();
}

template<typename T, typename Q>
void ArrayBase<T,Q>::throwOnBadIndexIncludingEnd(int index) const
{
    if (!isIndexInRangeIncludingEnd(index)) {
        throwIndexError(index);
    }
}

template<typename T, typename Q>
void ArrayBase<T,Q>::throwOnBadIndex(int index) const
{
    if (!isIndexInRange(index)) {
        throwIndexError(index);
    }
}

template<typename T, typename Q>
void ArrayBase<T,Q>::throwIndexError(int index) const
{
    std::string msg = "Error:  ArrayBase index " + std::to_string(index) + " out of range";
    if (elements.size() > 0) {
        msg.append(" (0, " + std::to_string(elements.size()-1) + ")");
    }
    else {
        msg.append(" (ArrayBase is empty!");
    }
    std::cerr << msg << std::endl;
    throw std::range_error(msg);
}

struct BoolWrapper {
    bool val;
    BoolWrapper() : val{ false }  {}
    BoolWrapper(bool val): val(val) {}
    operator bool&() { return val; }
    operator const bool&() const { return val; }
    bool* operator&() { return &val; }
    const bool* operator&() const { return &val; }
};

template<typename T>
class Array : public ArrayBase<T,T>
{
public:
    Array() {}
    Array(std::initializer_list<T> items) : ArrayBase<T,T>{items} {}
};

template<>
class Array<bool> : public ArrayBase<bool, BoolWrapper>
{
public:
    Array() {}
    Array(std::initializer_list<BoolWrapper> items) : ArrayBase<bool, BoolWrapper>{items} {}
};

} // namespace mssm

// Generate string representation of Array for formatting
template <typename T>
std::string array_to_string(const mssm::Array<T>& arr) {
    std::string result = "{";
    for (int i = 0; i < arr.size(); i++) {
        if (i > 0) {
            result += ", ";
        }
        // Convert each element to string
        std::stringstream ss;
        ss << arr[i];
        result += ss.str();
    }
    result += "}";
    return result;
}

// Use the appropriate formatter based on whether C++23 print is available
#if MSSM_HAS_CPP23_PRINT
// C++23 formatter
template <typename T>
struct std::formatter<mssm::Array<T>> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const mssm::Array<T>& arr, FormatContext& ctx) const {
        return std::formatter<std::string>::format(array_to_string(arr), ctx);
    }
};
#else
// fmt formatter
template <typename T>
struct fmt::formatter<mssm::Array<T>> {
    constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const mssm::Array<T>& arr, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", array_to_string(arr));
    }
};
#endif

#endif // MSSM_ARRAY_H
