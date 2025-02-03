
#include "parseinput.h"

#include <algorithm>
#include <cctype>
#include <optional>
#include <ranges>
#include <string>

using namespace std;

ParseInput::ParseInput(const std::string& txt)
    : _text(txt), root(this), parent(nullptr), start(0), pos(0), failed(false)
{
}

ParseInput::ParseInput(ParseInput& par)
    : root(par.root), parent(&par), start(par.start + par.pos), pos(0), failed(false)
{
    _text = text(); // TODO: REMOVE THIS! just to help with debugging
}

ParseInput::~ParseInput()
{
    if (parent)
    {
        parent->pos += pos;
    }
}

bool ParseInput::fail(const std::string& errorMsg)
{
    pos = 0;
    failed = true;
    errorMessage = errorMsg.empty() ? "Parsing failed." : errorMsg;
    return false;
}

bool ParseInput::isEnd() const
{
    return pos >= root->_text.size();
}

void ParseInput::skipWhitespace()
{
    while (pos < root->_text.size() && std::isspace(root->_text[start + pos]))
    {
        ++pos;
    }
}

void ParseInput::advance(size_t n)
{
    pos += n;
}

bool ParseInput::readInt(int& value, bool skipWs)
{
    if (skipWs) skipWhitespace();
    auto subrange = std::ranges::subrange(root->_text.begin() + start + pos, root->_text.end());
    auto it = subrange.begin();

    size_t len = 0;
    try
    {
        value = std::stoi(std::string(it, subrange.end()), &len);
    }
    catch (...)
    {
        return fail("Failed to read integer.");
    }

    advance(len);
    return success();
}

bool ParseInput::readDouble(double& value, bool skipWs)
{
    if (skipWs) skipWhitespace();
    auto subrange = std::ranges::subrange(root->_text.begin() + start + pos, root->_text.end());
    auto it = subrange.begin();

    size_t len = 0;
    try
    {
        value = std::stod(std::string(it, subrange.end()), &len);
    }
    catch (...)
    {
        return fail("Failed to read double.");
    }

    advance(len);
    return success();
}

bool ParseInput::readToken(std::string& value, bool skipWs)
{
    if (skipWs) skipWhitespace();
    auto subrange = std::ranges::subrange(root->_text.begin() + start + pos, root->_text.end());
    auto it = std::ranges::find_if_not(subrange, [](unsigned char c) { return std::isalnum(c); });

    if (it == subrange.begin()) return fail("Failed to read token.");

    value = std::string(subrange.begin(), it);
    advance(it - subrange.begin());
    return success();
}

bool ParseInput::readChar(char& value, bool skipWs)
{
    if (skipWs) skipWhitespace();
    if (isEnd()) return fail("Reached end of input.");

    value = root->_text[start + pos];
    advance(1);
    return success();
}

bool ParseInput::readFixedToken(const std::string& expected, bool skipWs)
{
    if (skipWs) skipWhitespace();
    auto subrange = std::ranges::subrange(root->_text.begin() + start + pos, root->_text.end());

    if (!std::ranges::equal(expected, subrange | std::views::take(expected.size())))
    {
        return fail("Fixed token mismatch.");
    }

    advance(expected.size());
    return success();
}

bool ParseInput::readFixedChar(char c, bool skipWs)
{
    char value;
    if (!readChar(value, skipWs)) return false;
    if (value != c) return fail("Fixed character mismatch.");
    return success();
}

bool ParseInput::skip(char c, bool skipWs)
{
    if (skipWs) {
        while (pos < root->_text.size() && (std::isspace(root->_text[start + pos]) || root->_text[start + pos] == c))
        {
            ++pos;
        }
    }
    else {
        if (pos < root->_text.size() && root->_text[start + pos] == c)
        {
            ++pos;
        }
    }
    return true;
}


bool ParseInput::readOneOf(const char *charSet, int& index, bool skipWs)
{
    char value;
    if (!readChar(value, skipWs)) return false;
    index = 0;
    while (*charSet)
    {
        if (value == *charSet)
        {
            return success();
        }
        ++charSet;
        ++index;
    }
    return fail("Character not found in the set.");
}
