#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <fstream>
#include <string>
#include <vector>


class CsvWriter
{
private:
    std::ofstream _writer;
    bool          _needComma;
public:
    CsvWriter(const std::string& filename);
   ~CsvWriter();

private:

    void WriteGeneral(bool writeEndOfLine, const std::vector<std::string>& data);
    void WriteGeneral(bool writeEndOfLine, std::initializer_list<std::string> values);

public:

    void WriteLine(const std::vector<std::string>& values)
    {
        WriteGeneral(true, values);
    }

    void WriteLine(std::initializer_list<std::string> values)
    {
        WriteGeneral(true, values);
    }

    void Write(const std::vector<std::string>& values)
    {
        WriteGeneral(false, values);
    }

    void Write(std::initializer_list<std::string> values)
    {
        WriteGeneral(false, values);
    }


};

#endif // CSVWRITER_H
