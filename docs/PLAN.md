# Milestones and Success Criteria

## Milestone List (High Level)
- M0: Repo skeleton + docs
- M1: Event schema + JSONL IO
- M2: Order book reconstruction
- M3: Replay engine (offline)
- M4: Toy execution sim
- M5: Recorder (Polymarket data)

## Decisions (MVP)
- Price/size stored as integer ticks (no floats); scale fixed and consistent across logs.
- Core enums use scoped enums (`enum class`).
- JSONL reader/writer uses RapidJSON for speed; JSON confined to the IO boundary.

## Task List (Commit-Sized)
1) Project skeleton (CMake + layout + CLI stub)
   - Add core library, CLI, and tests targets
   - Create include/ and src/ subfolders per module
   - CLI prints a stub message
2) Core types: identifiers and side
   - Define Side enum (bid/ask)
   - Define MarketId, OrderId, TradeId types (string or integer aliases)
   - Add a small header-only test for type usage (compile-only)
3) Core types: price/size and math
   - Define Price and Size types (integer ticks or decimal wrapper)
   - Decide and document numeric representation in SCHEMA.md
4) Core time types
   - Define Timestamp (nanoseconds since epoch)
   - Define Duration (nanoseconds)
5) Clock interface
   - IClock interface with now() and sleep_until()
6) System clock implementation
   - SystemClock uses std::chrono::system_clock
7) Replay clock implementation
   - ReplayClock advances based on event timestamps
   - Support “as-fast-as-possible” mode
8) Event schema: types and structures
   - MarketEvent variant with record_meta, snapshot, delta, trade, resync, heartbeat
   - Define shared event fields and per-event payloads
9) Schema documentation sync
   - Update SCHEMA.md with exact field types and examples
10) JSONL writer
   - Append-only writer, stable field ordering
   - Write record_meta and sample events
11) JSONL writer tests
   - Serialize a sample event and assert JSON content
12) JSONL reader
   - Streaming reader that yields MarketEvent
   - Detect unknown event_type and return error
13) JSONL reader tests
   - Round-trip event -> JSONL -> event
14) Order book core
   - Apply snapshot and deltas
   - Track best bid/ask and depth size
15) Order book tests
   - Known snapshot + deltas -> expected best bid/ask
16) Replay engine core
   - Replay loop consumes events and updates book
   - Emits callbacks to strategy and simulator
17) Strategy interface
   - Callbacks: on_start/on_book/on_trade/on_fill/on_end
18) Sample strategy
   - Minimal strategy that logs best bid/ask or spread
19) Execution simulator core
   - Submit order API and fixed latency model
   - Top-of-book fills only
20) PnL/position accounting
   - Positions per side and realized/unrealized PnL
21) Simulator tests
   - Deterministic fill results for a known book sequence
22) Recorder: REST snapshot
   - Fetch snapshot and normalize into snapshot event
23) Recorder: WS market channel
   - Subscribe to deltas and trades; normalize to events
24) Rolling log management
   - Append-only JSONL, new record_meta per session
25) CLI: record
   - Start recorder and write rolling log
26) CLI: replay
   - Run replay engine against a JSONL log
27) CLI: book-rebuild
   - Replay and print best bid/ask timeline
28) CLI: paper
   - Live WS/REST + simulator only (no real trades)

## MVP: Record + Replay + Toy Simulation
Goal: Deterministically record and replay market data for one market type using a rolling JSONL log.

Scope
- Record REST snapshot + WS deltas/trades into JSONL.
- Replay engine to rebuild order book and drive strategy callbacks.
- Execution simulator: top-of-book fills with fixed latency.
- CLI commands: record, replay, book-rebuild.
- Unit tests for schema parsing and order book reconstruction.

Success Criteria
- Can record a rolling log for a chosen market.
- Replay reproduces the same best bid/ask sequence across runs.
- Strategy callbacks fire deterministically with identical event ordering.
- Simulator produces fills consistent with top-of-book and latency settings.
- Tests pass locally.

## V1: Paper Trading + PnL
Goal: Run strategies on live market data without real trades.

Scope
- Paper trading mode using live WS/REST input and simulated execution.
- PnL accounting and position tracking.
- Config file support with CLI overrides.
- Additional tests for simulator fills and PnL calculations.

Success Criteria
- Strategy can run in paper mode and report PnL.
- Orders never leave the simulator (no real trades).
- Config + CLI override behavior documented and tested.

## V2: Robustness + Performance
Goal: Improve resilience and usability without changing core design.

Scope
- Gap detection and automatic resync.
- Optional faster binary log format or Parquet export.
- Replay pacing controls and faster-than-real-time mode.
- Scenario tests from golden logs.

Success Criteria
- Resyncs safely on missing sequences without crashes.
- Replay supports both real-time and accelerated modes.
- Golden test logs reproduce identical outcomes.
