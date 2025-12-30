#pragma once

#include "clock.h"
#include "time.h"
#include <thread>
#include <chrono>

struct SystemClock : IClock {
    Timestamp now() const override {
        return Timestamp{
            std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch()
            )
        };
    }

    void sleep_until(Timestamp t) override {
        using Clock = std::chrono::system_clock;
        std::this_thread::sleep_until(
            std::chrono::time_point<Clock, std::chrono::nanoseconds>(t.ns())
        );
    }

    void sleep_for(Duration d) override {
        std::this_thread::sleep_for(std::chrono::nanoseconds(d.ns()));
    }
};