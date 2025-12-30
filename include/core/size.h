#pragma once

#include <cassert>
#include <compare> // needed for operator<=>
#include <cstdint>

/* Same justification as price.h
 */

inline constexpr int64_t kSizeScale = 1'000'000;

struct Size {
    Size() : value(0) {}
    explicit Size(int64_t ticks) : value(ticks) {
        assert(value >= 0);
    }
    int64_t ticks() const {
        return value;
    }

    auto operator<=>(const Size &) const = default;

    Size &operator+=(Size other) {
        value += other.ticks();
        return *this;
    }

    Size &operator-=(Size other) {
        assert(value >= other.ticks());
        value -= other.ticks();
        return *this;
    }

  private:
    int64_t value;
};
