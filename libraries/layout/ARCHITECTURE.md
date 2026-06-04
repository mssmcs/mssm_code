# Layout Architecture Notes

This document captures the core runtime rules for the layout system and practical notes for the `LayoutHelper` DSL.

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
- Each node may implement `onMouse`, then children run, then `onMouseDeferred` on the same node (used for scroll-wheel handling on parents after children).

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

- `setNeedsResize()` marks geometry/layout dirty and also requests a paint pass. `LayoutManager` runs a full `resize` pass on the root when the flag is set.
- `setNeedsPaint()` marks visual-only dirt (hover, pressed state, text caret/selection, value slider thumb). Does **not** run `resize()`.
- Pure visual updates should call `setNeedsPaint()`, not `setNeedsResize()`, unless layout geometry actually changes.

`LayoutManager::draw()` runs `resize()` only when `getNeedsResize()` is true. It draws when `getNeedsPaint()` is true, the keyboard focus caret is active (`isAnyKeyboardFocus()`), or the hover chain is non-empty (hover highlight and menu auto-open).

Hold **Alt** during run to see per-frame `resized:` and `painted:` stats in the overlay.

### When to call `setNeedsResize()`

| Call it | Use `setNeedsPaint()` instead |
|--------|-------------------------------|
| Window size change | Hover / highlight only |
| Splitter position change | Button pressed appearance |
| Active tab change | Text edit, selection, caret |
| Overlay open/close | Value `LayoutSlider` thumb move |
| Scroll offset change (`LayoutScrollBar`, scroll wheel on content) | |
| Any change that requires `resize()` to re-read child bounds or positions | |

## LayoutHelper DSL

Build UIs with `LayoutHelper::Builder` — a `std::function<LayoutPtr(LayoutContext*)>` — often produced by nested helpers in `layouthelper.h`.

### Split operators

| Syntax | Meaning |
|--------|---------|
| `a \| b` | Horizontal split (`LayoutSplitter`, left \| right) |
| `a / b` | Vertical split (top / bottom) |

Chains associate left-to-right: `a / b / c` is `((a / b) / c)`.

`HSplit(a, b)` and `VSplit(a, b)` are equivalent explicit forms.

### Stacks and grids

```cpp
VStack{Panel{RED}, Button("OK"), Slider{true}}
HStack{PURPLE, GREEN, BLUE}
Grid{{"A", "B"}, {"C", "D"}}
```

To configure the stack (grow index, justify), pass `LayoutConfig` as the first argument:

```cpp
auto growLast = [](LayoutStacked* s) { s->growIdx = s->numChildren() - 1; };
VStack{LayoutConfig<LayoutStacked>{growLast}, {Panel{RED}, Panel{BLUE}}}
```

### Tabs

Every tab needs a **label** and **content**. Use `tab()` or `TabWrapper`:

```cpp
HTabs{
    tab("File", VStack{...}),
    tab("Edit", Panel{CYAN}),
}
// equivalent: HTabs{{"File", VStack{...}}, {"Edit", Panel{CYAN}}}
```

### Menus

Menu bar items need visible labels for the bar buttons. Use `item()` for labeled entries:

```cpp
HMenu{
    item("Palette", VMenu{
        item("Green", Panel{GREEN}),
        item("Blue", Panel{BLUE}),
    }),
    item("Help", Panel{YELLOW}),
}
```

Shortcut for **unlabeled** submenu content (empty bar label — demo only):

```cpp
VMenu{GREEN, BLUE, PINK}  // Wrapper overload: colors only, no text on bar
```

Prefer `item("Green", Panel{GREEN})` for real UIs.

### Scroll areas

```cpp
Scroll{VStack{Button("One"), Button("Two"), "More text..."}}
```

`LayoutScroll` clips content to the viewport and shows scroll bars when the child's measured size exceeds the area.

### Scroll bar ↔ content contract

- `LayoutScroll::resize()` reads `vScroll->scrollOffset` / `hScroll->scrollOffset` and offsets the child (`contentRect.pos`).
- `LayoutScrollBar` owns thumb geometry and pixel offset; thumb drag and wheel call `context->setNeedsResize()` so the parent scroll area re-runs `resize()` and moves content.
- `LayoutSlider` is for numeric value controls only (not scroll bars).
- Mouse wheel over the scroll panel is handled in `LayoutScroll::onMouseDeferred` (also calls `setNeedsResize()`).

If you add a custom scroll container, mirror this: **scroll offset changes must trigger a resize pass on the scroll parent.**

## Building the tree

```cpp
LayoutManager windowLayout(&context, createLayout());  // Builder invoked once
// or
LayoutManager windowLayout(&context, LayoutStacked::make(...));
```

`Wrapper` implicitly converts colors, strings, labels, and child builders so DSL expressions compose without boilerplate.

## Unit tests

Headless GoogleTest targets live in [`tests/`](tests/):

- **`layout_sizebound_tests`** — `distributeSizes`, `hStack` / `vStack`, `constraintIntersectionSafe` (links only `sizebound.cpp` for a fast T1 build).
- **`window_event_mask_tests`** — `Event::hasMouseButtonDown` / `primaryMouseButtonDown` for `EvtType::MouseMove` (`arg` is a GLFW-style button bitmask).

Enable via **`MSSM_BUILD_TESTS=ON`** on any bundle, or open the [`tests/`](../../tests/) bundle (tests on by default). Run with `ctest` or the test executable; see [`../TESTING.md`](../TESTING.md).
