#pragma once

#include "clock.h"
#include "time.h"
#include <cassert>

struct ReplayClock : IClock {
    explicit ReplayClock(Timestamp start) : current_(start) {}

    Timestamp now() const override {
        return current_;
    }

    void sleep_until(Timestamp t) override {
        assert(t >= current_);
        current_ = t;
    }

    void sleep_for(Duration d) override {
        current_ += d;
    }

  private:
    Timestamp current_;
};