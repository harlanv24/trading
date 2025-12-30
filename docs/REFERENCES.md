# Polymarket API References (Links)

These links are from the official Polymarket documentation sitemap and cover the public market data and CLOB WebSocket surfaces relevant to recording/replay. Use them as the source of truth when implementing market data adapters.

## CLOB Overview
- https://docs.polymarket.com/developers/CLOB/introduction
- https://docs.polymarket.com/developers/CLOB/quickstart
- https://docs.polymarket.com/developers/CLOB/status
- https://docs.polymarket.com/developers/CLOB/geoblock

## CLOB Authentication (for WS, if required)
- https://docs.polymarket.com/developers/CLOB/authentication
- https://docs.polymarket.com/developers/CLOB/websocket/wss-auth

## CLOB WebSocket (Market Data)
- https://docs.polymarket.com/developers/CLOB/websocket/wss-overview
- https://docs.polymarket.com/developers/CLOB/websocket/market-channel

## CLOB Public Client Methods (REST)
- https://docs.polymarket.com/developers/CLOB/clients/methods-public
- https://docs.polymarket.com/developers/CLOB/clients/methods-overview

## Trades and Time Series
- https://docs.polymarket.com/developers/CLOB/trades/trades-overview
- https://docs.polymarket.com/developers/CLOB/trades/trades
- https://docs.polymarket.com/developers/CLOB/timeseries

## Order Book Summaries (REST)
- https://docs.polymarket.com/api-reference/orderbook/get-order-book-summary
- https://docs.polymarket.com/api-reference/orderbook/get-multiple-order-books-summaries-by-request

## Market Discovery (REST)
- https://docs.polymarket.com/api-reference/markets/list-markets
- https://docs.polymarket.com/api-reference/markets/get-market-by-id
- https://docs.polymarket.com/api-reference/markets/get-market-by-slug

## Pricing (REST)
- https://docs.polymarket.com/api-reference/pricing/get-market-price
- https://docs.polymarket.com/api-reference/pricing/get-midpoint-price
- https://docs.polymarket.com/api-reference/pricing/get-multiple-market-prices
- https://docs.polymarket.com/api-reference/pricing/get-multiple-market-prices-by-request

## Rate Limits (Quickstart)
- https://docs.polymarket.com/quickstart/introduction/rate-limits

## Notes
- Avoid authenticated trading endpoints for MVP; use only public market data.
- If WS auth is required for market data, follow the authentication docs but do not manage private keys beyond access needed for data subscription.
