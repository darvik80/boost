//
// Created by Kishchenko, Ivan on 10/2/20.
//

#include "Logger.h"

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/formatting_ostream.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/support/date_time.hpp>

using namespace boost;

namespace mqtt::logging {

    BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", log::attributes::local_clock::value_type)
    BOOST_LOG_ATTRIBUTE_KEYWORD(threadId, "ThreadID", log::attributes::current_thread_id::value_type)

    void consoleFormatter(log::record_view const &rec, log::formatting_ostream &strm) {

        strm << rec[timestamp];

        auto severity = rec[log::trivial::severity];
        if (severity) {
            // Set the color
            switch (severity.get()) {
                case log::trivial::severity_level::info:
                    strm << "\033[32m";
                    break;
                case log::trivial::severity_level::warning:
                    strm << "\033[33m";
                    break;
                case log::trivial::severity_level::error:
                case log::trivial::severity_level::fatal:
                    strm << "\033[31m";
                    break;
                default:
                    break;
            }
        }

        strm << " <" << std::setw(7) << std::right << severity << "> ";
        strm << "\033[36m[ " << rec[threadId] << " ]";
        strm << "\033[0m : " << rec[log::expressions::smessage];
    }

    void fileFormatter(log::record_view const &rec, log::formatting_ostream &strm) {

        strm << rec[timestamp] << " <" << std::setw(7) << std::right << rec[log::trivial::severity] << "> ";
        strm << "[ " << rec[threadId] << " ] : " << rec[log::expressions::smessage];
    }

    void Logger::init(const LoggerProperties &props) {
        log::register_simple_formatter_factory<log::trivial::severity_level, char>("Severity");

        if (props.file) {
            log::add_file_log(
                    log::keywords::file_name = props.fileName,
                    log::keywords::rotation_size = 10 * 1024 * 1024,
                    log::keywords::time_based_rotation = log::sinks::file::rotation_at_time_point(0, 0, 0),
                    log::keywords::auto_flush = true
            )->set_formatter(&fileFormatter);
        }

        if (props.console) {
            log::add_console_log(std::cout)->set_formatter(&consoleFormatter);
        }

        log::core::get()->set_filter(
                log::trivial::severity >= props.level
        );

        log::add_common_attributes();
    }
}