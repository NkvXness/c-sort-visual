# c-sort-visual

Sorting algorithm visualizer written in C99 using [raylib](https://www.raylib.com/).

Algorithms run step-by-step so animation speed is fully independent of
sorting speed — from frame-by-frame stepping to 300 steps/second.



## Algorithms

| Algorithm      | Best       | Average    | Worst      | Memory   |
| -------------- | ---------- | ---------- | ---------- | -------- |
| Bubble Sort    | O(n)       | O(n²)      | O(n²)      | O(1)     |
| Selection Sort | O(n²)      | O(n²)      | O(n²)      | O(1)     |
| Insertion Sort | O(n)       | O(n²)      | O(n²)      | O(1)     |
| Merge Sort     | O(n log n) | O(n log n) | O(n log n) | O(n)     |
| Quick Sort     | O(n log n) | O(n log n) | O(n²)      | O(log n) |

## Build

Requires **raylib ≥ 5.0** and **gcc** (or clang).

**Linux (apt)**

```bash
sudo apt install libraylib-dev
make run
```

**macOS (Homebrew)**

```bash
brew install raylib
make run
```

**Windows (MSYS2)**

```bash
pacman -S mingw-w64-x86_64-raylib
make run
```

## Controls

| Key / Action          | Effect                        |
| --------------------- | ----------------------------- |
| `1` – `5`             | Switch algorithm              |
| `Enter` / Play button | Start sorting                 |
| `Space` / Shuffle     | Shuffle and reset             |
| `P`                   | Pause / Resume                |
| `→` / Step button     | Advance one step while paused |
| Array size slider     | Resize array (resets sort)    |
| Speed slider          | Steps per second (1–300)      |

## Project structure

```
src/
  main.c        — window, main loop, input handling
  array.c/.h    — heap-allocated array, Fisher-Yates shuffle
  sort.c/.h     — step-queue system + five algorithm generators
  visualizer.c/.h — bar rendering, color lerp, stats overlay
  ui.c/.h       — button and slider widgets
```

## License

MIT
