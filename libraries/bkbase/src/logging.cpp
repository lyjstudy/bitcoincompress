#include <bkbase/logging.h>
#include <thread>

namespace bkbase
{

    namespace logging = boost::log;
    namespace attrs = boost::log::attributes;
    namespace expr = boost::log::expressions;
    namespace keywords = boost::log::keywords;

    class current_thread_name : public attrs::constant<std::string> {
        typedef attrs::constant<std::string> base_type;
    public:
        current_thread_name(const std::string &value) : base_type(value) {
        }
        explicit current_thread_name(attrs::cast_source const& source) : base_type(source) {
        }
    };

    void LoggingThreadName(const std::string &name) {
        logging::core::get()->add_thread_attribute("ThreadName", current_thread_name(name));
    }

    void LoggingInitialize(LoggingLevel fileLevel, LoggingLevel consoleLevel) {
        if (consoleLevel < LOGMAX) {
            logging::add_console_log (
                std::clog,
                keywords::filter = expr::attr< LoggingLevel >("Severity").or_default(LOGINFO) >= consoleLevel,
                keywords::format = expr::format("[%1%] [%2%] [%3%] <%4%> %5%")
                    % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S.%f")
                    % expr::attr< std::string >("ThreadName")
                    % expr::format_named_scope("Scope", keywords::format = "%n")
                    % expr::attr< LoggingLevel >("Severity")
                    % expr::message
            );
        }
        if (fileLevel < LOGMAX) {
            // TODO: Add file logging
        }

        logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
        logging::core::get()->add_global_attribute("Scope", attrs::named_scope());
    }
} // bkbase
