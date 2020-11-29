//
// Created by Ivan Kishchenko on 29.11.2020.
//

#ifndef BOOST_APPLICATIONSERVICE_H
#define BOOST_APPLICATIONSERVICE_H

#include "UserDef.h"
#include "Scheduler.h"

class Service {
public:
    virtual void run() = 0;
    virtual void post(const Runnable &runnable) = 0;
};

class ApplicationService : public Service, public Scheduler {
private:
    IoService _service;
    SimpleScheduler _scheduler;
    SignalSet _signals;
public:
    ApplicationService();

    void run() override;

    void post(const Runnable &runnable) override;

    void execute(const Runnable &runnable) override;

    void schedule(const Runnable &runnable, const PosixTime &time) override;

    void scheduleAtFixedRate(const Runnable &runnable, const PosixDuration &duration) override;

    void scheduleWithFixedDelay(const Runnable &runnable, const PosixDuration &duration) override;
};


#endif //BOOST_APPLICATIONSERVICE_H
