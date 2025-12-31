#pragma once

#include "data/market_event.h"
#include <fstream>
#include <string>

// Append-only JSONL writer for MarketEvent entries.
class JsonlWriter {
  public:
    explicit JsonlWriter(const std::string &path);
    ~JsonlWriter();

    bool good() const;
    void write(const MarketEvent &event);
    void flush();

  private:
    std::ofstream out_;
};
