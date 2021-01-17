//
// Created by Ivan Kishchenko on 17.01.2021.
//

#ifndef BOOST_SIMPLECOMMAND_H
#define BOOST_SIMPLECOMMAND_H

#include "Command.h"
#include "IoServiceHolder.h"

#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/strict_lock.hpp>
#include <boost/thread.hpp>
#include <utility>
#include <boost/thread/with_lock_guard.hpp>

#include "logging/Logger.h"

class SimpleCommand : public Command, public std::enable_shared_from_this<SimpleCommand>, public boost::basic_lockable_adapter<boost::mutex> {
    PosixDuration _duration{PosixSeconds{0}};

public:
    explicit SimpleCommand() = default;

    explicit SimpleCommand(PosixDuration duration)
            : _duration(std::move(duration)) {}

    ErrorFuture execute() override {
        auto self = shared_from_this();
        auto promise = std::make_shared<ErrorPromise>();

        DeadlineTimerPtr timer;
        if (!_duration.is_zero()) {
            timer = std::make_shared<DeadlineTimer>(IoServiceHolder::get_mutable_instance());
            timer->expires_from_now(PosixSeconds{5});
            timer->async_wait([self, promise](const ErrorCode &code) {
                boost::with_lock_guard(*self, [promise, code]() {
                    if (!code) {
                        LOG(info) << "Timeout";
                        promise->set_value(boost::asio::error::make_error_code(boost::asio::error::timed_out));
                    } else {
                        LOG(info) << "Timer Err: " << code.message();
                    }
                });
            });
        }

        boost::asio::dispatch(IoServiceHolder::get_mutable_instance(), [self, promise, timer]() {
            boost::with_lock_guard(*self, [promise, timer]() {
                if (timer && timer->cancel()) {
                    LOG(info) << "Set Value, " << timer->expires_from_now().seconds();
                    promise->set_value(ErrorCode{});
                } else if (!timer) {
                    LOG(info) << "Set Value, no timer";
                    promise->set_value(ErrorCode{});
                } else {
                    LOG(info) << "Timeout already happens";
                }
            });
        });

        return promise->get_future();
    }
};


#endif //BOOST_SIMPLECOMMAND_H
