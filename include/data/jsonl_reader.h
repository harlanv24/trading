#pragma once

#include "data/market_event.h"
#include <cstddef>
#include <fstream>
#include <optional>
#include <string>

// JSONL reader to convert logs into MarketEvent entries.
class JsonlReader {
  public:
    explicit JsonlReader(const std::string &path);
    ~JsonlReader();
    const bool eof();
    std::optional<MarketEvent> next_event();
    size_t line_number() const {
        return line_number_;
    }

  private:
    std::ifstream in_;
    size_t line_number_; // for logging/debugging
};
