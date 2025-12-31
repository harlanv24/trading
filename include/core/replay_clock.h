#pragma once

#include "clock.h"
#include "time.h"
#include <chrono>
#include <stdexcept>
#include <thread>

/*
ReplayClock advances based on recorded timestamps.
- realtime=true: sleep for recorded gaps scaled by speed (speed >= 1.0). For
example, if there is a 0.5 sec gap between events and realtime=true with a speed
of 2.0, the user will observe the events with 0.25 sec between in real time.
- realtime=false: no sleeping; advances immediately.
*/
struct ReplayClock : IClock {
    explicit ReplayClock(bool realtime = true, double speed = 1.0)
        : realtime_(realtime), speed_(speed) {
        if (realtime_ && speed_ < 1.0) {
            throw std::invalid_argument("ReplayClock speed must be >= 1.0 in realtime mode");
        }
        if (speed_ <= 0.0) {
            throw std::invalid_argument("ReplayClock speed must be positive");
        }
    }

    Timestamp now() const override {
        return current_;
    }

    void advance_to(Timestamp next) {
        if (initialized_) {
            auto delta = next.ns() - current_.ns();
            if (realtime_) {
                auto sleep_ns = delta / speed_;
                if (sleep_ns.count() > 0) {
                    std::this_thread::sleep_for(sleep_ns);
                }
            }
        } else {
            initialized_ = true;
        }
        current_ = next;
    }

    void sleep_until(Timestamp t) override {
        if (t > current_) {
            advance_to(t);
        }
    }

    void sleep_for(Duration d) override {
        advance_to(Timestamp(current_.ns() + d.ns()));
    }

  private:
    bool realtime_;
    double speed_;
    bool initialized_{false};
    Timestamp current_{};
};
