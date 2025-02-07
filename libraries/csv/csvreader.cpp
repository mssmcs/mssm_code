#include "csvreader.h"
#include <fstream>
#include <sstream>


using namespace std;

const int stateBefore = 0;
const int stateInField = 1;
const int stateInQuote = 2;
const int stateOnQuote = 3;
const int stateAfter = 4;
const int myEof = -1;

CsvReader::CsvReader(const std::string& filename, bool skipBlankRows) : _reader(filename)
{
    _skipBlankRows = skipBlankRows;
    _endOfLine = false;
    _endOfFile = false;
    _syntaxError = false;

    state = stateBefore;
    pos = 0;
    length = 0;
}

CsvReader::~CsvReader()
{
}


std::vector<std::vector<std::string>> CsvReader::read()
{
    std::vector<std::vector<string>> result;

    auto f = [&](std::vector<string>&& v) { result.push_back(std::move(v)); };

    process(f);

    return result;
}

void CsvReader::process(std::function<void(std::vector<std::string>&&)> func)
{
    std::vector<string> oneRow;

    stringstream ss;
    stringstream white;

    do
    {
        _endOfFile = false;
        _endOfLine = false;

        while (!_endOfLine)
        {
            int c;

            // read next character (buffered)
            if (pos >= length)
            {
                _reader.read(buffer, bufferSize);
                length = _reader.gcount();
                if (length == 0)
                {
                    c = -1;
                }
                else
                {
                    pos = 0;
                    c = buffer[pos++];
                }
            }
            else
            {
                c = buffer[pos++];
            }

            switch (state)
            {
                case stateBefore:
                    switch (c)
                    {
                        case '"':
                            // quote
                            state = stateInQuote;
                            break;
                        case ',':
                            // comma
                            oneRow.push_back(ss.str());
                            ss.str("");
                            state = stateBefore;
                            break;
                        case ' ':
                        case '\t':
                        case '\r':
                            // whitespace
                            break;
                        case '\n':
                            if (state == stateBefore) {
                                oneRow.push_back(ss.str());
                            }
                            _endOfLine = true;
                            if (!_skipBlankRows || oneRow.size() > 0)
                            {
                                func(std::move(oneRow));
                                oneRow.clear();
                            }
                            break;
                        case myEof:
                            _endOfLine = true;
                            _endOfFile = true;
                            if (oneRow.size() > 0) // ignore 'dangling empty line' at end of file
                            {
                                func(std::move(oneRow));
                                oneRow.clear();
                            }
                            break;
                        default:
                            ss.put(char(c));
                            state = stateInField;
                            break;
                    }
                    break;
                case stateInField:
                    switch (c)
                    {
                        case '"':
                            // quote (syntax error!)
                            _syntaxError = true;
                            state = stateInQuote;
                            break;
                        case ',':
                            // comma
                            oneRow.push_back(ss.str());
                            ss.str("");
                            white.str("");
                            state = stateBefore;
                            break;
                        case ' ':
                        case '\t':
                        case '\r':
                            // whitespace
                            white.put(char(c));
                            break;
                        case '\n':
                            _endOfLine = true;
                            oneRow.push_back(ss.str());
                            ss.str("");
                            func(std::move(oneRow));
                            oneRow.clear();
                            white.str("");
                            break;
                        case myEof:
                            _endOfLine = true;
                            _endOfFile = true;
                            oneRow.push_back(ss.str());
                            ss.str("");
                            func(std::move(oneRow));
                            oneRow.clear();
                            white.str("");
                            break;
                        default:
                            if (white.str().size() > 0)
                            {
                                ss << white.str();
                                white.str("");
                            }
                            ss.put(char(c));
                            state = stateInField;
                            break;
                    }
                    break;
                case stateInQuote:
                    switch (c)
                    {
                        case '"':
                            // quote
                            state = stateOnQuote;
                            break;
                        case myEof:
                            // syntax error (truncated)
                            _syntaxError = true;
                            _endOfLine = true;
                            _endOfFile = true;
                            oneRow.push_back(ss.str());
                            ss.str("");
                            func(std::move(oneRow));
                            oneRow.clear();
                            break;
                        default:
                            ss.put(char(c));
                            break;
                    }
                    break;
                case stateOnQuote:
                    switch (c)
                    {
                        case '"':
                            // quote
                            ss.put(char(c));
                            state = stateInQuote;
                            break;
                        case ',':
                            // comma
                            oneRow.push_back(ss.str());
                            ss.str("");
                            state = stateBefore;
                            break;
                        case '\n':
                            _endOfLine = true;
                            oneRow.push_back(ss.str());
                            ss.str("");
                            state = stateBefore;
                            func(std::move(oneRow));
                            oneRow.clear();
                            break;
                        case myEof:
                            _endOfLine = true;
                            _endOfFile = true;
                            oneRow.push_back(ss.str());
                            ss.str("");
                            state = stateBefore;
                            func(std::move(oneRow));
                            oneRow.clear();
                            break;
                        case ' ':
                        case '\t':
                        case '\r':
                            // whitespace
                            oneRow.push_back(ss.str());
                            ss.str("");
                            state = stateAfter;
                            break;
                        default:
                            // unexpected character (syntax error)
                            _syntaxError = true;
                            oneRow.push_back(ss.str());
                            ss.str("");
                            func(std::move(oneRow));
                            oneRow.clear();
                            state = stateAfter;
                            break;
                    }
                    break;
                case stateAfter:
                    switch (c)
                    {
                        case ' ':
                        case '\t':
                        case '\r':
                            // whitespace
                            break;
                        case ',':
                            // comma
                            state = stateBefore;
                            break;
                        case '\n':
                            _endOfLine = true;
                            state = stateBefore;
                            func(std::move(oneRow));
                            oneRow.clear();
                            break;
                        case myEof:
                            _endOfLine = true;
                            _endOfFile = true;
                            state = stateBefore;
                            func(std::move(oneRow));
                            oneRow.clear();
                            break;
                        default:
                            // syntax error!
                            _syntaxError = true;
                            break;
                    }
                    break;
            }
        }
    } while (!_endOfFile);
}

bool CsvReader::readLine(std::vector<string>& oneRow)
{
    oneRow.clear();

    stringstream ss;
    stringstream white;

    while (!_endOfFile)
    {
        int c;

        // read next character (buffered)
        if (pos >= length)
        {
            _reader.read(buffer, bufferSize);
            length = _reader.gcount();
            if (length == 0)
            {
                c = -1;
            }
            else
            {
                pos = 0;
                c = buffer[pos++];
            }
        }
        else
        {
            c = buffer[pos++];
        }

        switch (state)
        {
            case stateBefore:
                switch (c)
                {
                    case '"':
                        // quote
                        state = stateInQuote;
                        break;
                    case ',':
                        // comma
                        oneRow.push_back(ss.str());
                        ss.str("");
                        state = stateBefore;
                        break;
                    case ' ':
                    case '\t':
                    case '\r':
                        // whitespace
                        break;
                    case '\n':
                        _endOfLine = true;
                        if (!_skipBlankRows || oneRow.size() > 0)
                        {
                            return true;
                        }
                        break;
                    case myEof:
                        _endOfLine = true;
                        _endOfFile = true;
                        if (oneRow.size() > 0) // ignore 'dangling empty line' at end of file
                        {
                            return true;
                        }
                        break;
                    default:
                        ss.put(char(c));
                        state = stateInField;
                        break;
                }
                break;
            case stateInField:
                switch (c)
                {
                    case '"':
                        // quote (syntax error!)
                        _syntaxError = true;
                        state = stateInQuote;
                        break;
                    case ',':
                        // comma
                        oneRow.push_back(ss.str());
                        ss.str("");
                        white.str("");
                        state = stateBefore;
                        break;
                    case ' ':
                    case '\t':
                    case '\r':
                        // whitespace
                        white.put(char(c));
                        break;
                    case '\n':
                        _endOfLine = true;
                        oneRow.push_back(ss.str());
                        return true;
                    case myEof:
                        _endOfLine = true;
                        _endOfFile = true;
                        oneRow.push_back(ss.str());
                        return true;
                    default:
                        if (white.str().size() > 0)
                        {
                            ss << white.str();
                            white.str("");
                        }
                        ss.put(char(c));
                        state = stateInField;
                        break;
                }
                break;
            case stateInQuote:
                switch (c)
                {
                    case '"':
                        // quote
                        state = stateOnQuote;
                        break;
                    case myEof:
                        // syntax error (truncated)
                        _syntaxError = true;
                        _endOfLine = true;
                        _endOfFile = true;
                        return true;
                    default:
                        ss.put(char(c));
                        break;
                }
                break;
            case stateOnQuote:
                switch (c)
                {
                    case '"':
                        // quote
                        ss.put(char(c));
                        state = stateInQuote;
                        break;
                    case ',':
                        // comma
                        oneRow.push_back(ss.str());
                        ss.str("");
                        state = stateBefore;
                        break;
                    case '\n':
                        _endOfLine = true;
                        oneRow.push_back(ss.str());
                        state = stateBefore;
                        return true;
                    case myEof:
                        _endOfLine = true;
                        _endOfFile = true;
                        oneRow.push_back(ss.str());
                        state = stateBefore;
                        return true;
                    case ' ':
                    case '\t':
                    case '\r':
                        // whitespace
                        oneRow.push_back(ss.str());
                        ss.str("");
                        state = stateAfter;
                        break;
                    default:
                        // unexpected character (syntax error)
                        _syntaxError = true;
                        oneRow.push_back(ss.str());
                        state = stateAfter;
                        return true;
                }
                break;
            case stateAfter:
                switch (c)
                {
                    case ' ':
                    case '\t':
                    case '\r':
                        // whitespace
                        break;
                    case ',':
                        // comma
                        state = stateBefore;
                        break;
                    case '\n':
                        _endOfLine = true;
                        state = stateBefore;
                        return true;
                    case myEof:
                        _endOfLine = true;
                        _endOfFile = true;
                        state = stateBefore;
                        return true;
                    default:
                        // syntax error!
                        _syntaxError = true;
                        break;
                }
                break;
            }
    }

    return false;
}
