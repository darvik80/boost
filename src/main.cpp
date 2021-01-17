#include "logging/Logger.h"

#include "ApplicationService.h"

#include "EventManager.h"

using namespace boost;
using namespace boost::system;

#include "SimpleCommand.h"

int main() {
    logging::Logger::init(logging::LoggerProperties{boost::log::trivial::info, true, false, ""});

    LOG(info) << "Start application";

    ApplicationService service;

    auto cmd1 = std::make_shared<SimpleCommand>();
    cmd1->execute();

    auto cmd2 = std::make_shared<SimpleCommand>(PosixSeconds{5});
    cmd2->execute();

//    service.execute([]() {
//        LOG(warning) << "Hello World execute";
//    });
//
//    service.schedule([]() {
//        LOG(warning) << "Hello World schedule";
//    }, boost::posix_time::second_clock::universal_time() + PosixSeconds{3});
//
//    service.scheduleAtFixedRate([]() {
//        LOG(warning) << "Fixed rate";
//    }, PosixSeconds {5});
//
//    service.scheduleWithFixedDelay([]() {
//        LOG(warning) << "Fixed delay";
//    }, PosixSeconds {10});

    service.run();

    LOG(info) << "Stop application";
    return 0;
}
