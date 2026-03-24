#ifndef TRACKED_H
#define TRACKED_H

#include <iostream>
#include <cmath>
#include <iomanip>

/*

Usage:

    Tracked<int>::reset();

    {
        Tracked<int> t1{5};
        Tracked<int> t2{t1};
        Tracked<int> t3;
        t3 = t1;
    }

    Tracked<int>::results(100); 


*/

namespace gjh_tracked {


template <typename T>
class Tracked {
private:
    static size_t initializeCount;
    static size_t createCount;
    static size_t deleteCount;
    static size_t copyCounter;
    static size_t assignCounter;
    static size_t compareCounter;
    T value;
public:
    Tracked(const T& v = T{}) : value{v} { createCount++; initializeCount++; }
    ~Tracked() { deleteCount++; }
    Tracked(const Tracked<T>& other) : value{other.value} { copyCounter++; createCount++; }
    Tracked<T>& operator=(const Tracked<T>& other) { value = other.value; assignCounter++; return *this; }
    Tracked<T>& operator=(const T& other) { value = other; assignCounter++; return *this; }
    Tracked(Tracked<T>&& other) : value{std::move(other.value)} { copyCounter++; createCount++; }
    Tracked<T>& operator=(const Tracked<T>&& other) { value = std::move(other.value); assignCounter++; return *this; }
    Tracked<T>& operator=(const T&& other) { value = std::move(other); assignCounter++; return *this; }
    bool operator== (const Tracked<T>& other) const { compareCounter++; return value == other.value; }
    bool operator== (const T& v) const { compareCounter++; return value == v; }
    bool operator!= (const Tracked<T>& other) const { compareCounter++; return value != other.value; }
    bool operator!= (const T& v) const { compareCounter++; return value != v; }
    bool operator> (const Tracked<T>& other) const { compareCounter++; return value > other.value; }
    bool operator> (const T& other) const { compareCounter++; return value > other; }
    bool operator< (const Tracked<T>& other) const { compareCounter++; return value < other.value;  }
    bool operator< (const T& other) const { compareCounter++; return value < other;  }
    bool operator>= (const Tracked<T>& other) const { compareCounter++; return value >= other.value; }
    bool operator>= (const T& other) const { compareCounter++; return value >= other; }
    bool operator<= (const Tracked<T>& other) const { compareCounter++; return value <= other.value; }
    bool operator<= (const T& other) const { compareCounter++; return value <= other; }

    T getValue() const { return value; }

    static void results(int n);
    static void reset();
};

template <typename T> bool operator== (const T& v, const Tracked<T>& v2) { return v2.getValue() == v; }
template <typename T> bool operator!= (const T& v, const Tracked<T>& v2) { return v2.getValue() != v; }
template <typename T> bool operator> (const T& v, const Tracked<T>& v2) { return v2.getValue() > v; }
template <typename T> bool operator< (const T& v, const Tracked<T>& v2) { return v2.getValue() < v; }
template <typename T> bool operator<= (const T& v, const Tracked<T>& v2) { return v2.getValue() >= v; }
template <typename T> bool operator>= (const T& v, const Tracked<T>& v2) { return v2.getValue() <= v; }

template <typename T> std::ostream& operator<< (std::ostream& strm, const Tracked<T>& v) { return strm << v.getValue(); }


template <typename T> size_t Tracked<T>::initializeCount = 0;
template <typename T> size_t Tracked<T>::createCount = 0;
template <typename T> size_t Tracked<T>::deleteCount = 0;
template <typename T> size_t Tracked<T>::copyCounter = 0;
template <typename T> size_t Tracked<T>::assignCounter = 0;
template <typename T> size_t Tracked<T>::compareCounter = 0;


template <typename T>
void Tracked<T>::results(int n)
{
    std::cout << "Created:     " << createCount << std::endl;
    std::cout << "Deleted:     " << deleteCount << std::endl;
    std::cout << "Leaked:      " << (createCount - deleteCount) << std::endl;
    std::cout << "Initialized: " << initializeCount << std::endl;
    std::cout << "Copied:      " << copyCounter << std::endl;
    std::cout << "Assigned:    " << assignCounter << std::endl;
    std::cout << "Compared:    " << compareCounter << std::endl;

    double totalOps = createCount + deleteCount + initializeCount + copyCounter + assignCounter + compareCounter;

    std::cout << "Total Ops:   " << totalOps << std::endl;

    if (n > 0) {
        std::cout << "n         "  << (n) << ", ";
        std::cout << "log n     "  << (log2(n)) << ", ";
        std::cout << "n log n   "  << (n*log2(n)) << ", ";
        std::cout << "n^2       "  << (n*n) << ", ";
        std::cout << "n^2 log n "  << (n*n*log2(n)) << ", ";
        std::cout << "n^3       "  << (n*n*n) << ", ";
        std::cout << "n^3 log n "  << (n*n*n*log2(n)) << ", ";

        std::cout << "ops / n         "  << totalOps / (n) << ", ";
        std::cout << "ops / log n     "  << totalOps / (log2(n)) << ", ";
        std::cout << "ops / n log n   "  << totalOps / (n*log2(n)) << ", ";
        std::cout << "ops / n^2       "  << totalOps / (n*n) << ", ";
        std::cout << "ops / n^2 log n "  << totalOps / (n*n*log2(n)) << ", ";
        std::cout << "ops / n^3       "  << totalOps / (n*n*n) << ", ";
        std::cout << "ops / n^3 log n "  << totalOps / (n*n*n*log2(n)) << ", ";
    }


    std::cout << std::endl << std::endl;
}

template <typename T>
void Tracked<T>::reset()
{
    initializeCount = 0;
    createCount = 0;
    deleteCount = 0;
    copyCounter = 0;
    assignCounter = 0;
    compareCounter = 0;
}

} // namespace gjh_tracked


#endif // TRACKED_H
