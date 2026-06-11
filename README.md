# Multithreaded Matrix Pipeline

A C program that performs a sequence of matrix operations — addition,
multiplication, and reduction — using POSIX threads (pthreads) to
parallelise each stage.

This was developed as the final project for the Operating Systems course
(TT304) at UNICAMP. The goal was to study the performance impact of
multithreading by measuring the wall-clock time of each stage with 1, 2,
and 4 threads on matrices of different sizes.

> **Note:** The original source files were lost. This repository was
> reconstructed from a recorded presentation of the working program.
> The code is functionally identical to what was presented; the inline
> documentation is new.

---

## What it does

Given five square N×N integer matrices stored as text files, the program
runs the following pipeline:

```
D = A + B          (element-wise addition)
E = C × D          (matrix multiplication)
scalar = sum(E)    (reduction — sum of all elements)
```

Matrices D and E are written back to disk. The program prints the
reduction result and the elapsed CPU time for each stage.

---

## Pipeline stages

| Stage | Operation | Parallelism |
|-------|-----------|-------------|
| 1 | Read A and B from files | 2 threads read concurrently |
| 2 | D = A + B | Array split into N equal slices |
| 3 | Write D / Read C | 2 threads run concurrently |
| 4 | E = C × D | Row range split into N equal slices |
| 5 | Write E + reduce(E) | Write thread + N reduce threads run concurrently |

With `num_threads == 1` every stage runs sequentially in the main thread.

---

## Requirements

- GCC (C11)
- POSIX-compatible OS (Linux or macOS)
- `make`

---

## Build

```bash
make
```

The compiled binary is placed at `bin/exec`.

---

## Run

```bash
./bin/exec <threads> <dimension> <A> <B> <C> <D> <E>
```

| Argument | Description |
|----------|-------------|
| `threads` | Number of worker threads: `1`, `2`, or `4` |
| `dimension` | Side length N of the N×N matrices |
| `A` `B` `C` | Paths to input matrix files (read) |
| `D` `E` | Paths to output matrix files (written) |

### Example

```bash
./bin/exec 4 1000 \
    tests/test-1k/arqA.txt \
    tests/test-1k/arqB.txt \
    tests/test-1k/arqC.txt \
    tests/test-1k/arqD.txt \
    tests/test-1k/arqE.txt
```

### Makefile shortcuts

```bash
make run-100-1    # 100×100  matrices, 1 thread
make run-100-2    # 100×100  matrices, 2 threads
make run-100-4    # 100×100  matrices, 4 threads
make run-1000-1   # 1000×1000 matrices, 1 thread
make run-1000-2   # 1000×1000 matrices, 2 threads
make run-1000-4   # 1000×1000 matrices, 4 threads
```

Output is saved to `out/output-<size>-<threads>.txt`.

---

## Input file format

Each matrix file contains N×N space-separated integers in row-major order,
one row per line:

```
1 2 3
4 5 6
7 8 9
```

---

## Sample output

```
Reduction: 1234567890
Time Sum: 0.002341
Time Multiplication: 4.812034
Time Reduction: 0.001120
Time Global: 4.815495
```

---

## Project structure

```
.
├── include/
│   ├── errorMessages.h   # Guard/validation function declarations
│   ├── matrix.h          # Thread worker function declarations
│   ├── resources.h       # Data structures and allocator declarations
│   └── threads.h         # Pipeline orchestrator declarations
├── src/
│   ├── main.c            # Entry point and pipeline driver
│   ├── errorMessages.c   # Guard/validation implementations
│   ├── matrix.c          # Thread worker implementations
│   ├── resources.c       # Allocator implementations
│   └── threads.c         # Pipeline orchestrator implementations
├── tests/
│   ├── test-100/         # 100×100 test matrices (A, B, C)
│   ├── test-1k/          # 1000×1000 test matrices (A, B, C)
│   └── test-2k/          # 2000×2000 test matrices (A, B, C, D, E)
└── makefile
```

---

## Clean

```bash
make clean        # Remove binaries, object files, and output files
make clean-out    # Remove only the out/ directory
make clean-de     # Remove only the generated D and E test files
```
