# Architecture

## Goals
- Deterministic record/replay of Polymarket CLOB market data (order book + trades).
- Modular, readable, and educational structure.
- Paper trading uses live market data but simulated execution only.
- C++20 + CMake; minimal dependencies.

## System Overview
The system has two primary modes:
1) Record: fetch REST snapshot + subscribe to WS deltas/trades and append events to a rolling JSONL log.
2) Replay/Paper: consume the same event format to drive a deterministic replay engine, order book reconstruction, and execution simulator.

The same core engine powers both backtesting and paper trading. The only difference is the event source (file replay vs live WS/REST).

## Modules
- core/
  - Shared types, IDs, timestamps, configuration, logging, and clocks.
- data/
  - Event schema, JSONL reader/writer, rolling log management.
- md/
  - Market data adapters: REST snapshot + WS deltas/trades.
- book/
  - Order book reconstruction and validation (snapshot + deltas).
- sim/
  - Execution simulator: top-of-book fills + latency model.
- bt/
  - Replay engine: event loop, time control, wiring strategy + sim.
- paper/
  - Live market data wiring to simulated execution (no real trading).
- strategy/
  - Strategy API with callbacks for book/trades/fills.
- pnl/
  - Position + PnL accounting, basic fee model.
- cli/
  - CLI commands: record, replay, paper.

## Provider-Agnostic Adapter Layer
To keep iteration fast now and allow switching providers later (e.g., Kalshi), the market data and order routing should be isolated behind a small adapter boundary.

Recommended boundary:
- md/
  - Provider-specific REST/WS clients (e.g., Polymarket, Kalshi).
  - Normalizes incoming data into the internal MarketEvent schema.
- data/
  - Only consumes normalized events; does not depend on provider payloads.

Design notes:
- Keep provider IDs/fields in a dedicated metadata block so core logic stays unchanged.
- Favor a thin translation layer over complex abstraction; it should be easy to swap the source without touching book/replay/sim.

## Key Interfaces (Conceptual)
- IClock
  - now(), sleep_until(ts)
  - SystemClock for real-time, ReplayClock for deterministic replay pacing.
- IEventReader / IEventWriter
  - next_event() / write(event)
  - Backed by JSONL for MVP.
- IMarketDataSource
  - start(); callbacks for snapshot/delta/trade.
  - Implemented by WS/REST adapters.
- IOrderBook
  - apply_snapshot(), apply_delta(), best_bid/ask()
- IExecutionSim
  - submit(order), on_book_update(), on_trade(), fill callbacks
- IStrategy
  - on_start(), on_book(), on_trade(), on_fill(), on_end()

## Execution Flow
- Record
  - Load config; open rolling log; write record_meta
  - Pull REST snapshot -> write snapshot event
  - Subscribe WS deltas/trades -> write events
  - Periodic snapshots for recovery (interval-based)

- Replay
  - Open JSONL; read record_meta
  - Initialize order book
  - Apply snapshot(s) + deltas in recorded order
  - Drive strategy callbacks and simulated execution

- Paper
  - Same as replay but with live event source
  - Submit orders only to simulator

## Determinism Principles
- Event ordering is based on recorded sequence + capture ordering.
- Replay never reorders by exchange timestamps.
- Deltas applied strictly in log order; gaps trigger resync.

## Storage Strategy (MVP)
- One rolling JSONL log per market: data/rolling/<market_id>.jsonl
- Append-only; new session starts with a new record_meta line.
- Periodic snapshots reduce replay warm-up time.
