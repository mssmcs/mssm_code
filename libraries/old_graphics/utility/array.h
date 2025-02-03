#ifndef ARRAY_H
#define ARRAY_H

// although ArrayBase is basically a vector<>, I'm calling it Array to minimize confusion with
// the mathematical concept of a vector, which we use when doing graphics programming
// I'm also adding a few methods like sort, insertAtIndex to simplify things and avoid the need
// to introduce iterators to first semester students
// another difference from vector is that size() returns a signed int, to avoid some other painful "gotchas"

#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include "fmt/core.h"

using namespace std;

#ifdef DISABLE_ARRAY_SAFETY_CHECKS
constexpr bool enable_array_safety_checks = false;
#else
constexpr bool enable_array_safety_checks = true;
#endif

template<typename T, typename Q = T>
class ArrayBase {
private:
    vector<Q> elements;
public:
    class iterator {
    public:
        using difference_type = int;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;
    private:
        ArrayBase* ptr;
        int idx;
    public:
        iterator(ArrayBase* ptr, int idx) : ptr{ptr}, idx{idx} { }

        void checkSameContainer(const iterator& other) const;

        iterator& operator+=(difference_type rhs);
        iterator& operator-=(difference_type rhs);

        reference operator*() const { return (*ptr)[idx]; }
        pointer operator->() const;
        reference operator[](difference_type rhs) const;

        difference_type operator-(const iterator& rhs) const;
        iterator operator+(difference_type rhs) const;
        iterator operator-(difference_type rhs) const;

        bool operator==(const iterator& rhs) const {
            if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
            return idx == rhs.idx; }
        bool operator!=(const iterator& rhs) const {
            if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
            return idx != rhs.idx; }
        bool operator>(const iterator& rhs) const  {
            if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
            return idx > rhs.idx; }
        bool operator<(const iterator& rhs) const  {
            if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
            return idx < rhs.idx; }
        bool operator>=(const iterator& rhs) const {
            if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
            return idx >= rhs.idx; }
        bool operator<=(const iterator& rhs) const {
            if constexpr (enable_array_safety_checks) { checkSameContainer(rhs); }
            return idx <= rhs.idx; }

    public:

        iterator& operator++();
        iterator operator++(int);
        iterator& operator--();
        iterator operator--(int);
    };

public:
    ArrayBase();
    ArrayBase(initializer_list<Q> items);
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
    iterator begin() { return {this, 0}; }
    iterator end() { return {this, static_cast<int>(elements.size()) }; }
    const iterator begin() const { return {this, 0}; }
    const iterator end() const { return {this, elements.size()}; }
    void sort();
    T& front();
    T& back();
    const T& front() const;
    const T& back() const;
    bool empty() const { return elements.empty(); }
    vector<Q>& asVector() { return elements; }
    const vector<Q>& asVector() const { return elements; }
private:
    constexpr bool isIndexInRangeIncludingEnd(int index) const { return index >= 0 && index <= elements.size(); }
    inline void throwOnBadIndexIncludingEnd(int index) const;
    inline void throwOnBadIndex(int index) const;
    void throwIndexError(int index) const;
};


template<typename T, typename Q>
void ArrayBase<T, Q>::iterator::checkSameContainer(const iterator& other) const
{
    if (other.ptr != ptr) {
        throw std::logic_error("Can't mix iterators from different ArrayBase<T,Q> containers!");
    }
}

template<typename T, typename Q>
typename ArrayBase<T,Q>::iterator ArrayBase<T,Q>::iterator::operator++(int)
{
    ArrayBase<T,Q>::iterator tmp = *this;
    idx++;
    if constexpr (enable_array_safety_checks) {
        if (!ptr->isIndexInRangeIncludingEnd(idx)) { ptr->throwIndexError(idx); }
    }
    return tmp;
}

template<typename T, typename Q>
typename ArrayBase<T,Q>::iterator &ArrayBase<T,Q>::iterator::operator++()
{
    idx++;
    if constexpr (enable_array_safety_checks) {
        if (!ptr->isIndexInRangeIncludingEnd(idx)) {
            ptr->throwIndexError(idx);
        }
    }
    return *this;
}

template<typename T, typename Q>
typename ArrayBase<T,Q>::iterator ArrayBase<T,Q>::iterator::operator--(int)
{
    ArrayBase<T,Q>::iterator tmp = *this;
    idx--;
    if constexpr (enable_array_safety_checks) {
        if (!ptr->isIndexInRangeIncludingEnd(idx)) {
            ptr->throwIndexError(idx);
        }
    }
    return tmp;
}

template<typename T, typename Q>
typename ArrayBase<T,Q>::iterator &ArrayBase<T,Q>::iterator::operator--()
{
    idx--;
    if constexpr (enable_array_safety_checks) {
        if (!ptr->isIndexInRangeIncludingEnd(idx)) {
            ptr->throwIndexError(idx);
        }
    }
    return *this;
}

template<typename T, typename Q>
typename ArrayBase<T,Q>::iterator::difference_type ArrayBase<T,Q>::iterator::operator-(const iterator& rhs) const
{
    if constexpr (enable_array_safety_checks) {
        checkSameContainer(rhs);
    }
    return idx - rhs.idx;
}

template<typename T, typename Q>
typename ArrayBase<T,Q>::iterator ArrayBase<T,Q>::iterator::operator+(difference_type rhs) const
{
    ArrayBase<T,Q>::iterator ret = *this;
    ret.idx += rhs;
    if constexpr (enable_array_safety_checks) {
        if (!ret.ptr->isIndexInRangeIncludingEnd(ret.idx)) {
            ret.ptr->throwIndexError(ret.idx);
        }
    }
    return ret;
}

template<typename T, typename Q>
typename ArrayBase<T,Q>::iterator ArrayBase<T,Q>::iterator::operator-(difference_type rhs) const
{
    ArrayBase<T,Q>::iterator ret = *this;
    ret.idx -= rhs;
    if constexpr (enable_array_safety_checks) {
        if (!ret.ptr->isIndexInRangeIncludingEnd(ret.idx)) {
            ret.ptr->throwIndexError(ret.idx);
        }
    }
    return ret;
}

template<typename T, typename Q>
typename ArrayBase<T,Q>::iterator operator+(typename ArrayBase<T,Q>::iterator::difference_type lhs, const typename ArrayBase<T,Q>::iterator& rhs)
{
    typename ArrayBase<T,Q>::iterator ret = rhs;
    ret.idx += lhs;
    if constexpr (enable_array_safety_checks) {
        if (!ret.ptr->isIndexInRangeIncludingEnd(ret.idx)) {
            ret.ptr->throwIndexError(ret.idx);
        }
    }
    return ret;
}

template<typename T, typename Q>
typename ArrayBase<T,Q>::iterator operator-(typename ArrayBase<T,Q>::iterator::difference_type lhs, const typename ArrayBase<T,Q>::iterator& rhs)
{
    typename ArrayBase<T,Q>::iterator ret = rhs;
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
ArrayBase<T,Q>::ArrayBase(initializer_list<Q> items)
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
            string msg = "Error:  ArrayBase cannot be resized to a negative size!";
            cout << msg << endl;
            throw invalid_argument(msg);
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
     string msg = "Error:  ArrayBase index " + to_string(index) + " out of range";
    if (elements.size() > 0) {
        msg.append(" (0, " + to_string(elements.size()-1) + ")");
    }
    else {
        msg.append(" (ArrayBase is empty!");
    }
    cerr << msg << endl;
    throw range_error(msg);
}

struct BoolWrapper {
    bool val;
    BoolWrapper() {}
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
    Array(initializer_list<T> items) : ArrayBase<T,T>{items} {}
};

template<>
class Array<bool> : public ArrayBase<bool, BoolWrapper>
{
public:
    Array() {}
    Array(initializer_list<BoolWrapper> items) : ArrayBase<bool, BoolWrapper>{items} {}
};

template <typename T>
struct fmt::formatter<Array<T>> : fmt::formatter<T> {
    template <typename FormatContext>
    auto format(Array<T> c, FormatContext& ctx) const {
        auto out = ctx.out();
        auto format_element = [&](const T& value) {
            ctx.advance_to(out);
            out = formatter<T>::format(value, ctx);
        };
        out = fmt::format_to(out, "{{");
        for (int i = 0; i < c.size(); i++) {
            if (i > 0) {
                out = fmt::format_to(out, ", ");
            }
            format_element(c[i]);
        }
        return fmt::format_to(out, "}}");
    }
};

#endif // ARRAY_H
