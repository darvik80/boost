#include "logging/Logger.h"

#include "ApplicationService.h"

using namespace boost;
using namespace boost::system;


int main() {
    logging::Logger::init(logging::LoggerProperties{boost::log::trivial::info, true, false, ""});

    LOG(info) << "Start application";

    ApplicationService service;

    service.execute([]() {
        LOG(warning) << "Hello World execute";
    });

    service.schedule([]() {
        LOG(warning) << "Hello World schedule";
    }, boost::posix_time::second_clock::universal_time() + PosixSeconds{3});

    service.scheduleAtFixedRate([]() {
        LOG(warning) << "Fixed rate";
    }, PosixSeconds {5});

    service.scheduleWithFixedDelay([]() {
        LOG(warning) << "Fixed delay";
    }, PosixSeconds {10});

    service.run();

    LOG(info) << "Stop application";
    return 0;
}
