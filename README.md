# Branch Predictor Simulator

A cycle-accurate C++ simulator modeling bimodal, gshare, and hybrid branch predictors, evaluated across multiple configurations using real-world SPEC 2006/2017 branch traces.

---

## Overview

Implements three branch predictor architectures with configurable parameters:

- **Bimodal** — PC-indexed 2-bit saturating counter table
- **Gshare** — PC XORed with global branch history register for index generation
- **Hybrid** — Chooser table dynamically selects between bimodal and gshare per branch

All prediction tables are initialized to **weakly taken (2)** at the start of simulation. The global history register initializes to all zeros.

---

## Predictor Architectures

### Bimodal (`n = 0`)
- Index: bits `m+1` down to `2` of the branch PC (lowest 2 bits discarded)
- Prediction: counter ≥ 2 → taken, else not-taken
- Update: increment on taken, decrement on not-taken (saturates at 0 and 3)

### Gshare (`n > 0`)
- Index: upper `n` bits of the `m` PC bits XORed with the `n`-bit global history register
- Same prediction and update rules as bimodal
- History register update: shift right by 1, insert actual outcome into MSB

### Hybrid
- Obtains predictions from both gshare and bimodal independently
- Chooser table (`2^k` counters, initialized to 1) selects which prediction to use
- Counter ≥ 2 → use gshare prediction; else → use bimodal prediction
- Only the selected predictor's table is updated; gshare history register is **always** updated
- Chooser counter update policy:

| Outcome | Chooser Update |
|---|---|
| Both correct or both wrong | No change |
| Gshare correct, bimodal wrong | Increment (saturates at 3) |
| Bimodal correct, gshare wrong | Decrement (saturates at 0) |

---

## Build

```bash
make
```
Produces the `sim` executable. For GDB debugging, replace `-O3` with `-g` in the Makefile.

---

## Usage

**Bimodal:**
```bash
./sim bimodal <M2> <tracefile>
```

**Gshare:**
```bash
./sim gshare <M1> <N> <tracefile>
```

**Hybrid:**
```bash
./sim hybrid <K> <M1> <N> <M2> <tracefile>
```

### Arguments

| Argument | Description |
|---|---|
| `M2` | Number of PC bits used to index the bimodal table |
| `M1` | Number of PC bits used to index the gshare table |
| `N` | Number of global branch history register bits (`N ≤ M1`; `N=0` reduces to bimodal) |
| `K` | Number of PC bits used to index the hybrid chooser table |
| `tracefile` | Path to the branch trace file |

### Examples

```bash
./sim bimodal 12 traces/gcc_trace.txt
./sim gshare 12 8 traces/gcc_trace.txt
./sim hybrid 8 12 8 12 traces/gcc_trace.txt
```

---

## Trace Format

```
<hex branch PC> t|n
```

- `t` — branch is actually taken
- `n` — branch is actually not-taken

**Example:**
```
00a3b5fc t
00a3b604 t
00a3b60c n
```

---

## Output

Each simulation prints:

1. The simulator command (predictor configuration + trace file)
2. Number of predictions (total dynamic branches)
3. Number of mispredictions
4. Misprediction rate (`mispredictions / predictions`)
5. Final contents of the branch predictor table
