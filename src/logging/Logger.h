//
// Created by Kishchenko, Ivan on 10/2/20.
//

#ifndef MQTT_LOGGER_H
#define MQTT_LOGGER_H

#include <boost/log/trivial.hpp>

#define LOG(level)  BOOST_LOG_TRIVIAL(level)

namespace logging {

    struct LoggerProperties {
        boost::log::trivial::severity_level level;
        bool console;
        bool file;
        std::string fileName;
    };

    class Logger {
    public:
        static void init(const LoggerProperties& props);
    };

}


#endif //MQTT_LOGGER_H
