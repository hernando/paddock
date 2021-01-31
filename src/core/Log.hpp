#pragma once

#include <sstream>

namespace paddock::core
{
class Log;
class LogMessage
{
public:
    friend LogMessage log();

    template <typename T>
    LogMessage& operator<<(const T& val);

    LogMessage(LogMessage&& other);
    LogMessage& operator=(LogMessage&& other);

    ~LogMessage();

private:
    Log* _log;
    std::stringstream _text;

    LogMessage(Log* log)
        : _log(log)
    {
    }
};

LogMessage log();

template <typename T>
LogMessage& LogMessage::operator<<(const T& val)
{
    if (_text.tellp())
        _text << ' ';
    _text << val;
    return *this;
}

} // namespace paddock::core
