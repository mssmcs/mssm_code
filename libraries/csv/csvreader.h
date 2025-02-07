#ifndef CSVREADER_H
#define CSVREADER_H

#include <string>
#include <vector>
#include <fstream>
#include <functional>

/*
class CsvReader
{
    std::string _filename;
public:
    CsvReader(const std::string &filename);
   ~CsvReader();
    std::vector<std::vector<std::string>> read();
};
*/

class CsvReader
{
private:
    std::ifstream _reader;
    bool _endOfLine;
    bool _endOfFile;
    bool _syntaxError;
    bool _skipBlankRows;

    static constexpr int bufferSize = 4096;

    int  state;
    char buffer[bufferSize];
    int  pos;
    int  length;

public:
    CsvReader(const std::string& filename, bool skipBlankRows);
   ~CsvReader();

    bool SyntaxError() { return _syntaxError; }

    std::vector<std::vector<std::string>> read();

    void process(std::function<void(std::vector<std::string>&&)> func);

    bool readLine(std::vector<std::string>& oneRow);
};

#endif // CSVREADER_H
