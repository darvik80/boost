//
// Created by Ivan Kishchenko on 29.11.2020.
//

#include "Scheduler.h"
#include "logging/Logger.h"

void SimpleScheduler::execute(const Runnable &runnable) {
    _service.post(runnable);
}

void SimpleScheduler::schedule(const Runnable &runnable, const PosixTime &time) {
    auto timer = std::make_shared<DeadlineTimer>(_service, time);
    _timers.insert(timer);
    timer->async_wait([this, timer, &runnable](const ErrorCode &err) {
        if (!err) {
            runnable();
        }

        _timers.erase(timer);
    });
}

void fixedRateCallback(const ErrorCode &err, const DeadlineTimerPtr &timer, const PosixDuration &duration,
                       Runnable &runnable) {
    if (!err) {
        runnable();

        timer->expires_at(timer->expires_at() + duration);
        timer->async_wait(
                [timer, duration, runnable](const ErrorCode &error) {
                    fixedRateCallback(error, timer, duration, runnable);
                }
        );
    }
}

void SimpleScheduler::scheduleAtFixedRate(const Runnable &runnable, const PosixDuration &duration) {
    auto timer = std::make_shared<DeadlineTimer>(_service, duration);
    _timers.insert(timer);

    timer->async_wait(
            [timer, duration, runnable](const ErrorCode &error) {
                fixedRateCallback(error, timer, duration, runnable);
            }
    );
}

void fixedDelayCallback(const ErrorCode &err, const DeadlineTimerPtr &timer, const PosixDuration &duration,
                        const std::function<void()> &runnable) {
    if (!err) {
        runnable();

        timer->expires_from_now(duration);
        timer->async_wait(
                [timer, duration, runnable](const ErrorCode &error) {
                    fixedDelayCallback(error, timer, duration, runnable);
                }
        );
    }
}


void SimpleScheduler::scheduleWithFixedDelay(const Runnable &runnable, const PosixDuration &duration) {
    auto timer = std::make_shared<DeadlineTimer>(_service, duration);
    _timers.insert(timer);

    timer->async_wait(
            [timer, duration, runnable](const ErrorCode &error) {
                fixedDelayCallback(error, timer, duration, runnable);
            }
    );
}

void SimpleScheduler::cancel() {
    LOG(info) << "Cancel timers";
    for (const auto &timer : _timers) {
        timer->cancel();
    }
    _timers.clear();
}
