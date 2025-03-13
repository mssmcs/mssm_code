#include <thread>


#include "mssm.h"

#undef main

int safemain();

bool wantEatWhiteSpace = false; // nasty global variable to make the read functions behave a little more intuitively for
                            // new programmers who shouldn't have to worry about the idiosyncracies of stream input

void resetCin()
{
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.clear();
    wantEatWhiteSpace = false;
}


void afterStreamRead()
{
   // clearWs();
    wantEatWhiteSpace = true;
}
char readChar()
{
    char value;
    if (cin >> value) {
        afterStreamRead();
        return value;
    }
    resetCin();
    throw invalid_argument("Expected a character!");
}

int readInt()
{
    int value;
    if (cin >> value) {
        afterStreamRead();
        return value;
    }
    resetCin();
    throw invalid_argument("Expected an integer!");
}

double readDouble()
{
    double value;
    if (cin >> value) {
        afterStreamRead();
        return value;
    }
    resetCin();
    throw invalid_argument("Expected a number!");
}

string readLine()
{
    string text;
    string prefix;
    if (wantEatWhiteSpace) {
        char c;
        do {
            c = cin.get();
        } while (isspace(c) && c != '\n');
        if (c != '\n') {
            prefix.append(1, c);
        }
        wantEatWhiteSpace = false;
    }
    getline(cin, text);
    if (!cin) {
        resetCin();
        throw invalid_argument("Error reading string!");
    }
    return prefix + text;
}

string readWord()
{
    string value;
    if (cin >> value) {
        afterStreamRead();
        return value;
    }
    resetCin();
    throw invalid_argument("Expected a word!");
}



bool tryReadChar(char &value)
{
    if (cin >> value) {
        afterStreamRead();
        return true;
    }
    resetCin();
    value = 0;
    return false;
}

bool tryReadInt(int &value)
{
    if (cin >> value) {
        afterStreamRead();
        return true;
    }
    resetCin();
    value = std::numeric_limits<int>::lowest();
    return false;
}

bool tryReadDouble(double &value)
{
    if (cin >> value) {
        afterStreamRead();
        return true;
    }
    resetCin();
    value = std::numeric_limits<double>::quiet_NaN();
    return false;
}

void delay(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int main()
{
    try {
        int result = safemain();
        println("");
        cout.flush();
        delay(10);
        return result;
    }
    catch (const std::exception& ex) {
        println("");
        std::println("Exception thrown: {}", ex.what());
        println("");
        cout.flush();
        delay(10);
        return -1;
    }
}
