# Data Schema (JSONL)

## Overview
Each line is one JSON object. The log is append-only. A rolling log contains multiple sessions, each introduced by a record_meta line.

## Common Fields
- event_type: string (record_meta, snapshot, delta, trade, resync, heartbeat)
- session_id: string (UUID or timestamp-based)
- market_id: string
- seq: integer (monotonic sequence if provided)
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
- config: { rest_base_url, ws_url, snapshot_interval_seconds }

Example:
{
  "event_type": "record_meta",
  "session_id": "2025-01-01T00:00:00Z_001",
  "market_id": "0x1234",
  "recorder_version": "0.1.0",
  "start_ts": 1735689600000000000,
  "config": {
    "rest_base_url": "https://example",
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
- seq (if available)
- exchange_ts (if available)
- recv_ts
- capture_ns
- bids: array of [price, size]
- asks: array of [price, size]
- snapshot_id: string (optional)

## delta
Incremental updates to price levels.

Fields:
- event_type: "delta"
- session_id
- market_id
- seq
- exchange_ts (optional)
- recv_ts
- capture_ns
- bids: array of [price, size] (size=0 to delete)
- asks: array of [price, size] (size=0 to delete)

## trade
Trade prints as reported by the venue.

Fields:
- event_type: "trade"
- session_id
- market_id
- trade_id: string (if provided)
- side: "buy" | "sell" (if provided)
- price: number
- size: number
- exchange_ts (optional)
- recv_ts
- capture_ns

## resync
Marks a gap or reset, and indicates that a new snapshot was fetched.

Fields:
- event_type: "resync"
- session_id
- market_id
- reason: string (e.g., "gap_detected")
- recv_ts
- capture_ns

## heartbeat
Optional marker to record capture gaps or keepalive timing.

Fields:
- event_type: "heartbeat"
- session_id
- market_id
- recv_ts
- capture_ns

## Ordering Rules
- Primary ordering: seq (when present)
- Tie-breaker: capture_ns
- Never reorder by exchange_ts

## Market Type (MVP)
- Single market type only for MVP.
- Price/size stored as integer ticks (scaled); encoded as JSON integers for deterministic replay.

## Implementation Notes
- JSONL reader/writer uses RapidJSON for speed.
- JSON parsing is confined to the data IO boundary; internal types stay fully typed.
