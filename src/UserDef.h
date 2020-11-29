//
// Created by Ivan Kishchenko on 29.11.2020.
//

#ifndef BOOST_USERDEF_H
#define BOOST_USERDEF_H

#include <boost/asio.hpp>
#include <functional>

typedef boost::asio::io_service IoService;
typedef boost::asio::signal_set SignalSet;
typedef boost::asio::deadline_timer DeadlineTimer;
typedef std::shared_ptr<DeadlineTimer> DeadlineTimerPtr;
typedef std::unique_ptr<DeadlineTimer> DeadlineTimerAutoPtr;

typedef boost::system::error_code ErrorCode;
typedef boost::posix_time::ptime PosixTime;

typedef boost::posix_time::time_duration PosixDuration;
typedef boost::posix_time::seconds PosixSeconds;
typedef boost::posix_time::microseconds PosixMicroseconds;

typedef const std::function<void()> Runnable;

#endif //BOOST_USERDEF_H
