//
// Created by Ivan Kishchenko on 29.11.2020.
//

#include "ApplicationService.h"
#include "logging/Logger.h"

using namespace boost;

ApplicationService::ApplicationService()
: _scheduler(_service)
, _signals(_service, SIGINT, SIGTERM, SIGQUIT)
{
    _signals.async_wait([this](const ErrorCode &error, int signalNumber) {
        if (!error) {
            LOG(info) << "Shutdown signal received";
            _scheduler.cancel();
        }
    });
}

void ApplicationService::run() {
    _service.run();
}

void ApplicationService::post(const Runnable &runnable) {
    _service.post(runnable);
}

void ApplicationService::execute(const Runnable &runnable) {
    _scheduler.execute(runnable);
}

void ApplicationService::schedule(const Runnable &runnable, const PosixTime &time) {
    _scheduler.schedule(runnable, time);
}

void ApplicationService::scheduleAtFixedRate(const Runnable &runnable, const PosixDuration &duration) {
    _scheduler.scheduleAtFixedRate(runnable, duration);
}

void ApplicationService::scheduleWithFixedDelay(const Runnable &runnable, const PosixDuration &duration) {
    _scheduler.scheduleWithFixedDelay(runnable, duration);
}
