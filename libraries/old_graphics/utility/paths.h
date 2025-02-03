#ifndef PATHS_H
#define PATHS_H

#include <string>

class Paths
{
public:
    static std::string findAsset(const std::string& filename);
    static std::string executablePath();
    static std::string documentsFolder();
};

#endif // PATHS_H
