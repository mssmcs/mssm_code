#include "rand.h"
#include <chrono>



int randomInt(int minVal, int maxVal)
{
    static Rand rnd;
    return rnd.randomInt(minVal, maxVal);
}

double randomDouble(double minVal, double maxVal)
{
    static Rand rnd;
    return rnd.randomDouble(minVal, maxVal);
}

bool randomTrue(double pct)
{
    static Rand rnd;
    return rnd.randomTrue(pct);
}


Rand::Rand()
    : mersenneTwister(randDevice())
{
    mersenneTwister.seed(static_cast<unsigned int>((std::chrono::steady_clock::now().time_since_epoch()).count())); // should'nt be necesary... bug in GCC?
}

int Rand::randomInt(int minVal, int maxVal)
{
    std::uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(mersenneTwister);
}

double Rand::randomDouble(double minVal, double maxVal)
{
    std::uniform_real_distribution<double> dist(minVal, maxVal);
    return dist(mersenneTwister);
}

bool Rand::randomTrue(double pct)
{
    if (pct <= 0.0)
    {
        return false;
    }
    if (pct >= 1.0)
    {
        return true;
    }
    std::uniform_real_distribution<double> dist(0, 1);
    return dist(mersenneTwister) <= pct;
}
