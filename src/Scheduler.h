//
// Created by Ivan Kishchenko on 29.11.2020.
//

#ifndef BOOST_SCHEDULER_H
#define BOOST_SCHEDULER_H

#include <boost/noncopyable.hpp>
#include <UserDef.h>
#include <unordered_set>
#include <utility>
#include "Component.h"

class Scheduler {
public:
    virtual void execute(const Runnable &runnable) = 0;

    virtual void schedule(const Runnable &runnable, const PosixTime &time) = 0;

    virtual void scheduleAtFixedRate(const Runnable &runnable, const PosixDuration &duration) = 0;

    virtual void scheduleWithFixedDelay(const Runnable &runnable, const PosixDuration &duration) = 0;
};

class IdleTimer : public Component {
    const Runnable _idleCallback;
    const PosixDuration _idleDuration;
    DeadlineTimer _idleTimeout;
public:
    IdleTimer(std::string_view name, const Runnable &idleCallback, const PosixDuration &idleDuration);

    void reset();
    void cancel();
};

class SimpleScheduler : boost::noncopyable, public Scheduler {
private:
    std::unordered_set<DeadlineTimerPtr> _timers;
    IdleTimer _idleTimer;

private:
    void executeRunnable(const Runnable &runnable);
public:
    SimpleScheduler(const Runnable &idleCallback, const PosixDuration &idleDuration);
    SimpleScheduler();

    void execute(const Runnable &runnable) override;

    void schedule(const Runnable &runnable, const PosixTime &time) override;

    void scheduleAtFixedRate(const Runnable &runnable, const PosixDuration &duration) override;

    void scheduleWithFixedDelay(const Runnable &runnable, const PosixDuration &duration) override;

    void cancel();
};


#endif //BOOST_SCHEDULER_H
