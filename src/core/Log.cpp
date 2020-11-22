#include "Log.hpp"

#include <QCoreApplication>
#include <QDateTime>

#include <fstream>

#include <iostream>

namespace paddock
{
namespace core
{
namespace
{
#ifdef NDEBUG
std::string _makeLogFileName()
{
    return "paddock." + std::to_string(QCoreApplication::applicationPid()) +
           ".txt";
}
#endif
} // namespace

struct Log
{
    friend class LogMessage;

    std::fstream _logFile;
    std::ostream& _out;

    Log()
#ifdef NDEBUG
        : _logFile(_makeLogFileName(), std::fstream::out)
        , _out(_logFile)
#else
        : _out(std::cerr)
#endif
    {
#ifdef NDEBUG
        if (!_logFile)
            std::cerr << "Could not create log file" << std::endl;
#endif
    }
};

LogMessage::LogMessage(LogMessage&& other) = default;
LogMessage& LogMessage::operator=(LogMessage&& other) = default;

LogMessage::~LogMessage()
{
    _log->_out << QDateTime::currentDateTime()
                      .toString("yyyy-MM-dd_hh-mm-ss ")
                      .toStdString()
               << _text.str() << std::endl;
}

LogMessage log()
{
    static Log log;
    return LogMessage(&log);
}

} // namespace core
} // namespace paddock
