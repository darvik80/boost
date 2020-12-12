//
// Created by Ivan Kishchenko on 29.11.2020.
//

#include "Scheduler.h"

#include <utility>
#include "logging/Logger.h"
#include "IoServiceHolder.h"

IdleTimer::IdleTimer(std::string_view name, const Runnable &idleCallback, const PosixDuration &idleDuration)
        : Component(name), _idleTimeout(IoServiceHolder::get_mutable_instance()), _idleCallback(idleCallback)
        , _idleDuration(idleDuration) {

}

void IdleTimer::reset() {
    _idleTimeout.expires_from_now(_idleDuration);
    _idleTimeout.async_wait([this](const ErrorCode &err) {
        if (!err) {
            _idleCallback();
            reset();
        } else {
            LOG(debug) << "Timer " << name() << " canceled";
        }
    });
}

void IdleTimer::cancel() {
    _idleTimeout.cancel();
}

SimpleScheduler::SimpleScheduler(const Runnable &idleCallback, const PosixDuration &idleDuration)
        : _idleTimer("heartbeat", idleCallback, idleDuration) {
    _idleTimer.reset();
}

SimpleScheduler::SimpleScheduler()
        : _idleTimer("heartbeat", []() { LOG(info) << "On Idle"; }, PosixSeconds{10}) {
}

void SimpleScheduler::executeRunnable(const Runnable &runnable) {
    _idleTimer.reset();
    runnable();
}


void SimpleScheduler::execute(const Runnable &runnable) {
    IoServiceHolder::get_mutable_instance().post([runnable, this]() {
        executeRunnable(runnable);
    });
}

void SimpleScheduler::schedule(const Runnable &runnable, const PosixTime &time) {
    auto timer = std::make_shared<DeadlineTimer>(IoServiceHolder::get_mutable_instance(), time);
    _timers.insert(timer);
    timer->async_wait([this, timer, &runnable](const ErrorCode &err) {
        if (!err) {
            executeRunnable(runnable);
        }

        _timers.erase(timer);
    });
}

void fixedRateCallback(const ErrorCode &err, const DeadlineTimerPtr &timer, const PosixDuration &duration, Runnable &runnable) {
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
    auto timer = std::make_shared<DeadlineTimer>(IoServiceHolder::get_mutable_instance(), duration);
    _timers.insert(timer);

    timer->async_wait(
            [this, timer, duration, runnable](const ErrorCode &err) {
                if (!err) {
                    fixedRateCallback(err, timer, duration, [runnable, this]() {
                        executeRunnable(runnable);
                    });
                }
            }
    );
}

void fixedDelayCallback(const ErrorCode &err, const DeadlineTimerPtr &timer, const PosixDuration &duration, const std::function<void()> &runnable) {
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
    auto timer = std::make_shared<DeadlineTimer>(IoServiceHolder::get_mutable_instance(), duration);
    _timers.insert(timer);

    timer->async_wait(
            [this, timer, duration, runnable](const ErrorCode &err) {
                if (!err) {
                    fixedDelayCallback(err, timer, duration, [runnable, this]() {
                        executeRunnable(runnable);
                    });
                }
            }
    );
}

void SimpleScheduler::cancel() {
    LOG(info) << "Cancel timers";
    for (const auto &timer : _timers) {
        timer->cancel();
    }
    _timers.clear();
    _idleTimer.cancel();
}
