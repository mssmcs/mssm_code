#ifndef MSSM_H
#define MSSM_H

#include <iostream>
#include <sstream>


#include "array.h"
#include "rand.h"
#include "paths.h"

#ifdef HASLIB_FMT
#include "fmt/core.h"
using fmt::print;
using fmt::println;
#endif

using namespace std;


template <typename T>
void print(T value)
{
    fmt::print("{}", value);
}

template <typename T>
void println(T value)
{
    println("{}", value);
}

inline void println() { fmt::println(""); }

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

#define main safemain

#endif // MSSM_H
