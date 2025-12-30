#pragma once

#include <cassert>
#include <compare> // needed for operator<=>
#include <cstdint>

/* Price will be stored as integer ticks to ensure consistency across platforms
and architecture. This also allows comparisons/sorting to be done predictably
without floating point nonsense. Also won't have to deal with rounding or JSON
serialization/deserialization weirdness dealing with native floats. We just keep
as an int and then convert to a float with fixed precision (1e-6) when necessary.
*/

inline constexpr int64_t kPriceScale = 1'000'000;

struct Price {
    Price() : value(0) {}
    explicit Price(int64_t ticks) : value(ticks) {
        assert(value >= 0);
    }
    int64_t ticks() const {
        return value;
    }

    auto operator<=>(const Price &) const = default;

    Price &operator+=(Price other) {
        value += other.ticks();
        return *this;
    }

    Price &operator-=(Price other) {
        assert(value >= other.ticks());
        value -= other.ticks();
        return *this;
    }

  private:
    int64_t value;
};
