#pragma once

#include "time.h"

struct IClock {
    virtual ~IClock() = default;
    virtual Timestamp now() const = 0;
    virtual void sleep_until(Timestamp t) = 0;
    virtual void sleep_for(Duration d) = 0;
};