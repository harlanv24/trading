#pragma once

#include <cassert>
#include <chrono>
#include <compare> // needed for operator<=>

struct Duration {
    Duration() : value(std::chrono::nanoseconds(0)) {}
    explicit Duration(std::chrono::nanoseconds v) : value(v) {
        assert(value.count() >= 0);
    }

    std::chrono::nanoseconds ns() const {
        return value;
    }

    auto operator<=>(const Duration &) const = default;

  private:
    std::chrono::nanoseconds value;
};

struct Timestamp {

    Timestamp() : value(std::chrono::nanoseconds(0)) {}
    explicit Timestamp(std::chrono::nanoseconds v) : value(v) {
        assert(value.count() >= 0);
    }

    std::chrono::nanoseconds ns() const {
        return value;
    }

    Timestamp &operator+=(Duration d) {
        value += d.ns();
        return *this;
    }

    auto operator<=>(const Timestamp &) const = default;

  private:
    std::chrono::nanoseconds value;
};
