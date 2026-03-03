# Layout Architecture Notes

This document captures the core runtime rules for the layout system.

## Runtime Model

- The layout tree is retained in memory (`LayoutBase` nodes).
- `LayoutManager` runs a frame loop in this order:
  1. propagate events
  2. resize if dirty
  3. draw base tree
  4. draw overlays

## Event Routing

- Mouse/key events are propagated top-down from root.
- Overlays are dispatched first from topmost to bottommost.
- If an overlay consumes/claims hover, remaining dispatch runs in hover-only mode.
- Hover-chain updates happen during mouse propagation.

## Overlay Lifecycle

- Overlays are tracked in `LayoutContext::overlays`.
- Overlay adds/removes requested during event dispatch are queued.
- Queued overlay mutations are applied after dispatch completes to avoid mutation while iterating.
- Overlay layers are recomputed after add/remove to keep ordering stable.

## Focus Rules

- Keyboard focus and drag focus are owned by `LayoutContext`.
- Focus is represented as a single active element pointer for each channel.
- Nodes maintain local/ancestor focus flags for fast routing decisions.

## Resize / Invalidation

- `setNeedsResize()` marks geometry/layout dirty.
- Use it only when geometry can change (split position, active tab, overlay changes, content scrolling requiring child reposition).
- Pure visual updates (hover/pressed/value color changes) should avoid forced relayout.
