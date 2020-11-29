#include <iostream>

#include <boost/asio.hpp>

#include "logging/Logger.h"

#define BOOST_THREAD_PROVIDES_FUTURE

#include <boost/thread/future.hpp>
#include <functional>

using namespace boost;
using namespace boost::system;


void onTimer(const error_code &err, asio::deadline_timer *timer, future<int> *future) {
    if (!err) {
        if (!future->is_ready()) {
            LOG(info) << "Data nor ready";
        } else {
            LOG(info) << "Got data: " << future->get();
        }

        timer->expires_from_now(posix_time::seconds(5));
        timer->async_wait([timer, future](const error_code &error) {
            ::onTimer(error, timer, future);
        });
    }
}

int main() {
    LOG(info) << "Start application";

    asio::io_service service;

    boost::promise<int> promise;

    service.post([&promise]() {
        LOG(info) << "Handle Promise";
        promise.set_value(5);
    });

    boost::asio::deadline_timer timer(service, posix_time::seconds(10));
    auto future = promise.get_future();
    timer.async_wait([&timer, &future](const error_code &err) {
        ::onTimer(err, &timer, &future);
    });

    asio::signal_set signals(service, SIGINT, SIGTERM, SIGQUIT);
    signals.async_wait([&timer](const error_code &error, int signalNumber) {
        if (!error) {
            LOG(info) << "Shutdown signal received";
            timer.cancel();
        }
    });

    service.run();

    LOG(info) << "Stop application";
    return 0;
}
