#ifndef PARSEINPUT_H
#define PARSEINPUT_H

#include <cctype>
#include <optional>
#include <string>


class ParseInput
{
    std::string _text;
    ParseInput* root;
    ParseInput* parent;
    size_t start;
    size_t pos;
    bool failed;
    std::optional<std::string> errorMessage;

public:
    ParseInput(const std::string& text);
    ParseInput(ParseInput& par);
    ~ParseInput();

    std::string text() const { return root->_text.substr(start + pos); }

    bool readInt(int& value, bool skipWs = true);
    bool readDouble(double& value, bool skipWs = true);
    bool readToken(std::string& value, bool skipWs = true);
    bool readChar(char& value, bool skipWs = true);
    bool readFixedToken(const std::string& expected, bool skipWs = true);
    bool readFixedChar(char c, bool skipWs = true);
    bool skip(char c, bool skipWs = true);
    bool readOneOf(const char* charSet, int& index, bool skipWs = true);

    bool fail(const std::string& errorMsg = "");
    bool success() { return true; }
    bool gotFail() const { return failed; }
    std::optional<std::string> getErrorMessage() const { return errorMessage; }

private:
    void skipWhitespace();
    bool isEnd() const;
    void advance(size_t n);
};

#endif // PARSEINPUT_H
