//
// Created by Ivan Kishchenko on 28.12.2020.
//

#ifndef BOOST_EVENTMANAGER_H
#define BOOST_EVENTMANAGER_H

#include <boost/signals2.hpp>
#include <memory>
#include <iostream>

class Event {
public:
    [[nodiscard]] virtual const char *name() const = 0;
};

class EventActive : public Event {
public:
    [[nodiscard]] const char *name() const override {
        return "active";
    }
};

class EventInactive : public Event {
public:
    [[nodiscard]] const char *name() const override {
        return "inactive";
    }
};

class EventSubscribe : public Event {
public:
    [[nodiscard]] const char *name() const override {
        return "subscribe";
    }
};

class EventHandler {
public:
    typedef std::shared_ptr<EventHandler> Ptr;

    virtual ~EventHandler() = default;
};

template<class T>
class TEventHandler : virtual public EventHandler {
public:
    virtual void onEvent(const T &event) = 0;
};

class EventChannelHandler : public TEventHandler<EventInactive>, public TEventHandler<EventActive>, public TEventHandler<EventSubscribe> {
public:
    void onEvent(const EventActive &event) override {
        std::cout << "EventHandler " << event.name() << std::endl;
    }

    void onEvent(const EventInactive &event) override {
        std::cout << "EventHandler " << event.name() << std::endl;
    }

    void onEvent(const EventSubscribe &event) override {
        std::cout << "EventHandler " << event.name() << std::endl;
    }

    ~EventChannelHandler() override {
        std::cout << "Destroy EventChannelHandler" << std::endl;
    }
};

class EventManager {
public:
    virtual void raiseEvent(const Event &event) = 0;
};

class EventManagerV1 : public EventManager {
private:
    boost::signals2::signal<void(const Event &)> _signal;
public:
    template<class E>
    void subscribe(const EventHandler::Ptr &handler) {
        auto *channel = dynamic_cast< TEventHandler<E> *>(handler.get());
        if (!channel) {
            throw std::invalid_argument(std::string("Handler not support event ") + typeid(E).name());
        }

        _signal.connect(boost::signals2::signal<void(const Event &)>::slot_type(
                [channel](const Event &event) {
                    const auto *real = dynamic_cast<const E *>(&event);
                    if (real) {
                        channel->onEvent(*real);
                    }
                }
        ).track_foreign(handler));
    }

    void raiseEvent(const Event &event) override {
        std::cout << "raise event v1: " << typeid(event).name() << std::endl;
        _signal(event);

        std::cout << "subs v1: " << _signal.num_slots() << std::endl;
    }
};

#include <unordered_map>

class EventManagerV2 : public EventManager {
public:
    typedef boost::signals2::signal<void(const Event &)> Signal;
private:
    std::unordered_map<std::type_index, Signal> _signals;
public:
    template<class E>
    void subscribe(const EventHandler::Ptr &handler) {
        auto *channel = dynamic_cast< TEventHandler<E> *>(handler.get());
        if (!channel) {
            throw std::invalid_argument(std::string("Handler not support event ") + typeid(E).name());
        }

        auto& signal = _signals[typeid(E)];
        signal.connect(boost::signals2::signal<void(const Event &)>::slot_type(
                [channel](const Event &event) {
                    const auto *real = static_cast<const E *>(&event);
                    if (real) {
                        channel->onEvent(*real);
                    }
                }
        ).track_foreign(handler));
    }

    template<class E>
    boost::signals2::connection subscribe(const std::function<bool(const E&)>& callback) {
        auto& signal = _signals[typeid(E)];
        return signal.connect_extended(
                [callback](const boost::signals2::connection &conn, const Event &event)  {
                    const auto *real = static_cast<const E *>(&event);
                    if (!callback(*real)) {
                        conn.disconnect();
                    }
                }
        );
    }

    void raiseEvent(const Event &event) override {
        auto iter = _signals.find(typeid(event));
        if (iter != _signals.end()) {
            std::cout << "raise event v2: " << iter->first.name() << std::endl;
            iter->second(event);

            std::cout << "subs v2: " << iter->second.num_slots() << std::endl;
        } else {
            std::cout << "miss event v2: " << typeid(event).name() << std::endl;
        }

    }
};

#endif //BOOST_EVENTMANAGER_H
