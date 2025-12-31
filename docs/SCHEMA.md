# Data Schema (JSONL)

## Overview
Each line is one JSON object. The log is append-only. A rolling log contains multiple sessions, each introduced by a record_meta line.

## Common Fields
- event_type: string (record_meta, snapshot, delta, trade, resync, heartbeat)
- session_id: string (UUID or timestamp-based)
- market_id: string
- exchange_ts: integer (nanoseconds since epoch, optional)
- recv_ts: integer (nanoseconds since epoch, local wall clock)
- capture_ns: integer (monotonic capture time for tie-break ordering)

## record_meta
Used to start a new capture session within a rolling log.

Fields:
- event_type: "record_meta"
- session_id
- market_id
- recorder_version
- start_ts (recv_ts)
- config: { rest_url, ws_url, snapshot_interval_seconds }

Example:
{
  "event_type": "record_meta",
  "session_id": "2025-01-01T00:00:00Z_001",
  "market_id": "0x1234",
  "recorder_version": "0.1.0",
  "start_ts": 1735689600000000000,
  "config": {
    "rest_url": "https://example",
    "ws_url": "wss://example",
    "snapshot_interval_seconds": 30
  }
}

## snapshot
Full depth snapshot of the order book for the chosen market type.

Fields:
- event_type: "snapshot"
- session_id
- market_id
- exchange_ts (if available)
- recv_ts
- capture_ns
- neg_risk: boolean (optional)
- bids: array of [price_ticks, size_ticks] (int64)
- asks: array of [price_ticks, size_ticks] (int64)

## delta
Incremental updates to price levels.

Fields:
- event_type: "delta"
- session_id
- market_id
- exchange_ts (optional)
- recv_ts
- capture_ns
- neg_risk: boolean (optional)
- side: "bid" | "ask"
- price_ticks: int64
- size_ticks: int64 (0 deletes the level)

## trade
Trade prints as reported by the venue.

Fields:
- event_type: "trade"
- session_id
- market_id
- TODO: not defined yet (struct stub exists)

## resync
Marks a gap or reset, and indicates that a new snapshot was fetched.

Fields:
- event_type: "resync"
- session_id
- market_id
- TODO: not defined yet (struct stub exists)

## heartbeat
Optional marker to record capture gaps or keepalive timing.

Fields:
- event_type: "heartbeat"
- session_id
- market_id
- TODO: not defined yet (struct stub exists)

## Ordering Rules
- Primary ordering: capture_ns
- Tie-breaker: recv_ts (or ingress order if identical)
- Never reorder by exchange_ts

## Market Type (MVP)
- Single market type only for MVP.
- Price/size stored as integer ticks (scaled); encoded as JSON integers for deterministic replay.
- Scale: 1e6 ticks per unit (see `kPriceScale`, `kSizeScale`).

## Implementation Notes
- JSONL reader/writer uses RapidJSON for speed.
- JSON parsing is confined to the data IO boundary; internal types stay fully typed.
