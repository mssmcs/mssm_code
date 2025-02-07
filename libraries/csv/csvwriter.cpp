#include "csvwriter.h"

using namespace std;

CsvWriter::CsvWriter(const std::string& filename) : _writer(filename)
{
    _needComma = false;
}

CsvWriter::~CsvWriter()
{

}

void WriteQuotedItem(ostream &dest, string item)
{
    if (item.find_first_of("\",") != string::npos) {
        dest.put('"');
        for (auto c : item)
        {
            if (c == '"')
            {
                dest << "\"\"";
            }
            else
            {
                dest.put(c);
            }
        }
        dest.put('"');
    }
    else {
        dest << item;
    }
}

void WriteQuoted(ostream &ss, const vector<string>& data)
{
    bool needComma = false;
    for (auto s : data)
    {
        if (needComma)
        {
            ss.put(',');
        }
        else
        {
            needComma = true;
        }
        WriteQuotedItem(ss, s);
    }
}

void WriteQuoted(ostream &ss, std::initializer_list<string> data)
{
    bool needComma = false;
    for (auto s : data)
    {
        if (needComma)
        {
            ss.put(',');
        }
        else
        {
            needComma = true;
        }
        WriteQuotedItem(ss, s);
    }
}




void CsvWriter::WriteGeneral(bool writeEndOfLine, const vector<string>& values)
{
    if (_needComma)
    {
        _writer.put(',');
    }

    WriteQuoted(_writer, values);

    if (writeEndOfLine)
    {
        _writer << endl;
        _needComma = false;
    }
    else
    {
        _needComma = true;
    }
}

void CsvWriter::WriteGeneral(bool writeEndOfLine, std::initializer_list<string> values)
{
    if (_needComma)
    {
        _writer.put(',');
    }

    WriteQuoted(_writer, values);

    if (writeEndOfLine)
    {
        _writer << endl;
        _needComma = false;
    }
    else
    {
        _needComma = true;
    }
}


