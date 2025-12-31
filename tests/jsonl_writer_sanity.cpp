#include "data/jsonl_writer.h"
#include "data/market_event.h"
#include "data/snapshot.h"
#include "data/delta.h"
#include "book/order_book.h"
#include "core/ids.h"
#include "core/price.h"
#include "core/size.h"
#include "core/time.h"
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

std::string ReadFile(const std::string &path) {
    std::ifstream in(path);
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

int main() {
    const std::string path = "/tmp/jsonl_writer_sanity.jsonl";
    std::remove(path.c_str());
    {
        JsonlWriter writer(path);

        Snapshot snap{};
        snap.session_id = SessionId{"session-1"};
        snap.market_id = MarketId{"market-1"};
        snap.recv_ts = Timestamp(std::chrono::nanoseconds(1'000));
        snap.capture_ns = Timestamp(std::chrono::nanoseconds(2'000));
        snap.exchange_ts = std::nullopt;
        snap.neg_risk = std::nullopt;
        snap.book_state.bids.emplace(Price(100), Size(200));
        snap.book_state.asks.emplace(Price(110), Size(150));
        MarketEvent snap_ev{};
        snap_ev.type = EventType::Snapshot;
        snap_ev.payload = snap;
        writer.write(snap_ev);

        Delta delta{};
        delta.session_id = SessionId{"session-1"};
        delta.market_id = MarketId{"market-1"};
        delta.recv_ts = Timestamp(std::chrono::nanoseconds(3'000));
        delta.capture_ns = Timestamp(std::chrono::nanoseconds(4'000));
        delta.exchange_ts = std::nullopt;
        delta.side = Side::Bid;
        delta.price = Price(105);
        delta.size = Size(250);
        MarketEvent delta_ev{};
        delta_ev.type = EventType::Delta;
        delta_ev.payload = delta;
        writer.write(delta_ev);

        writer.flush();
    }

    std::string content = ReadFile(path);
    std::vector<std::string> lines;
    std::string::size_type pos = 0;
    while (true) {
        auto next = content.find('\n', pos);
        if (next == std::string::npos) {
            if (pos < content.size()) {
                lines.push_back(content.substr(pos));
            }
            break;
        }
        lines.push_back(content.substr(pos, next - pos));
        pos = next + 1;
    }

    std::vector<std::string> expected{
        R"({"event_type":"snapshot","session_id":"session-1","market_id":"market-1","recv_ts":1000,"capture_ns":2000,"bids":[[100,200]],"asks":[[110,150]]})",
        R"({"event_type":"delta","session_id":"session-1","market_id":"market-1","recv_ts":3000,"capture_ns":4000,"side":"bid","price_ticks":105,"size_ticks":250})"};

    if (lines != expected) {
        std::cerr << "jsonl_writer output mismatch\n";
        std::cerr << "Expected:\n";
        for (const auto &l : expected)
            std::cerr << l << "\n";
        std::cerr << "Got:\n";
        for (const auto &l : lines)
            std::cerr << l << "\n";
        return 1;
    }

    std::cout << "jsonl_writer sanity passed\n";
    return 0;
}
