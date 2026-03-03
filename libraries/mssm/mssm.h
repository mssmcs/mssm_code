#ifndef MSSM_H
#define MSSM_H

#include <iostream>
#include <sstream>

// Include print_compat.h first for print/println compatibility
#include "print_compat.h"

#include "array.h"
#include "rand.h"
#include "paths.h"

using namespace std;
using mssm::Array;

// These overloads are now provided by print_compat.h
// They're removed to avoid conflicts
// template <typename T>
// void println(T value)
// {
//     println("{}", value);
// }
// inline void println() { std::println(""); }

char readChar();
int readInt();
double readDouble();
string readLine();
string readWord();

template <typename T>
Array<T> readArray()
{
    string line = readLine();
    stringstream ss(line);
    Array<T> values;
    while (ss) {
        T value;
        if (ss >> value) {
            values.append(value);
        }
    }
    return values;
}

bool tryReadChar(char &value);
bool tryReadInt(int &value);
bool tryReadDouble(double &value);
void delay(int milliseconds);

namespace mssm {
void installCrashHandlers();
}

#define main safemain

#endif // MSSM_H
