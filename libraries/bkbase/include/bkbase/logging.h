#pragma once


#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>

#include <boost/log/sources/logger.hpp>

#include <boost/log/support/date_time.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>


enum LoggingLevel
{
    LOGTRACE,       // 'Trace'
    LOGDEBUG,       // 'Debug'
    LOGINFO,        // 'Info'
    LOGWARNNING,    // 'Warn'
    LOGERROR,       // 'Error'
    LOGCRITICAL,    // 'Critical'
    LOGMAX,
};

// The formatting logic for the LoggingLevel
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, LoggingLevel lvl) {
    static const char* const str[] = {
        "Trace",
        "Debug",
        "Info",
        "Warn",
        "Error",
        "Critical",
    };
    if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
        strm << str[lvl];
    else
        strm << static_cast< int >(lvl);
    return strm;
}


namespace bkbase
{
    void LoggingInitialize(LoggingLevel fileLevel, LoggingLevel consoleLevel);
    void LoggingThreadName(const std::string &name);


    BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(GlobalLogger, boost::log::sources::severity_logger_mt<LoggingLevel>)

    #define BKTRACE() BOOST_LOG_SEV(::bkbase::GlobalLogger::get(), LOGTRACE)
    #define BKDEBUG() BOOST_LOG_SEV(::bkbase::GlobalLogger::get(), LOGDEBUG)
    #define BKINFO() BOOST_LOG_SEV(::bkbase::GlobalLogger::get(), LOGINFO)
    #define BKWARN() BOOST_LOG_SEV(::bkbase::GlobalLogger::get(), LOGWARNNING)
    #define BKERROR() BOOST_LOG_SEV(::bkbase::GlobalLogger::get(), LOGERROR)
    #define BKCRITICAL() BOOST_LOG_SEV(::bkbase::GlobalLogger::get(), LOGCRITICAL)
    
    #define BKLOGSCOPE(name) BOOST_LOG_NAMED_SCOPE(name)
    #define BKLOGSCOPEFUNC() BOOST_LOG_NAMED_SCOPE(BOOST_CURRENT_FUNCTION)
} // bkbase
