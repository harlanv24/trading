#pragma once

#include "core/price.h"
#include "core/size.h"
#include <functional>
#include <map>

using BidMap = std::map<Price, Size, std::greater<Price>>;
using AskMap = std::map<Price, Size, std::less<Price>>;

struct OrderBook {
    BidMap bids;
    AskMap asks;
};
