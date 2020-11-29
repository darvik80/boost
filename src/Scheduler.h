//
// Created by Ivan Kishchenko on 29.11.2020.
//

#ifndef BOOST_SCHEDULER_H
#define BOOST_SCHEDULER_H

#include <boost/noncopyable.hpp>
#include <UserDef.h>
#include <unordered_set>

class Scheduler {
public:
    virtual void execute(const Runnable &runnable) = 0;

    virtual void schedule(const Runnable &runnable, const PosixTime &time) = 0;

    virtual void scheduleAtFixedRate(const Runnable &runnable, const PosixDuration &duration) = 0;

    virtual void scheduleWithFixedDelay(const Runnable &runnable, const PosixDuration &duration) = 0;
};

class SimpleScheduler : boost::noncopyable, public Scheduler {
private:
    IoService &_service;
    std::unordered_set<DeadlineTimerPtr> _timers;
public:
    explicit SimpleScheduler(IoService &service) : _service(service) {

    }

    void execute(const Runnable &runnable) override;

    void schedule(const Runnable &runnable, const PosixTime &time) override;

    void scheduleAtFixedRate(const Runnable &runnable, const PosixDuration &duration) override;

    void scheduleWithFixedDelay(const Runnable &runnable, const PosixDuration &duration) override;

    void cancel();
};


#endif //BOOST_SCHEDULER_H
