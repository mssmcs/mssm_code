#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <thread>

#include "mssm.h"

#undef main

int safemain();

namespace {
void onFatalSignal(int sig)
{
    std::fprintf(stderr, "\nFatal signal %d received. The program will exit.\n", sig);
    std::fflush(stderr);
    std::_Exit(128 + sig);
}
}

namespace mssm {
void installCrashHandlers()
{
    static bool installed = false;
    if (installed) {
        return;
    }
    installed = true;

    std::set_terminate([]() {
        std::fprintf(stderr, "\nUnhandled exception caused program termination.\n");
        auto ex = std::current_exception();
        if (ex) {
            try {
                std::rethrow_exception(ex);
            }
            catch (const std::exception& e) {
                std::fprintf(stderr, "Exception: %s\n", e.what());
            }
            catch (...) {
                std::fprintf(stderr, "Exception: unknown\n");
            }
        }
        std::fflush(stderr);
        std::abort();
    });

    std::signal(SIGABRT, onFatalSignal);
    std::signal(SIGFPE, onFatalSignal);
    std::signal(SIGILL, onFatalSignal);
    std::signal(SIGSEGV, onFatalSignal);
}
}

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
    mssm::installCrashHandlers();
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
