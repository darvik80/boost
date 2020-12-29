#include "logging/Logger.h"

#include "ApplicationService.h"

#include "EventManager.h"

using namespace boost;
using namespace boost::system;

int main() {
    auto handler = std::make_shared<EventChannelHandler>();

    EventManagerV1 v1;

    v1.subscribe<EventActive>(handler);
    v1.subscribe<EventInactive>(handler);
    v1.subscribe<EventSubscribe>(handler);

    v1.raiseEvent(EventActive{});
    v1.raiseEvent(EventInactive{});

    v1.raiseEvent(EventSubscribe{});

    handler.reset();

    v1.raiseEvent(EventSubscribe{});

    std::cout << "---" << std::endl;

    handler = std::make_shared<EventChannelHandler>();
    EventManagerV2 v2;
    v2.subscribe<EventSubscribe>(handler);
    v2.subscribe<EventActive>(handler);
    v2.subscribe<EventActive>([](const EventActive& event) -> bool {
        std::cout << "LambdaEventHandler: " << event.name() << std::endl;
        return false;
    });


    v2.raiseEvent(EventSubscribe{});
    v2.raiseEvent(EventInactive{});
    handler.reset();
    v2.raiseEvent(EventActive{});

    return 0;
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
