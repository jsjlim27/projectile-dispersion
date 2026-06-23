# Projectile Dispersion Simulator

A Monte Carlo simulator for projectile launch dispersion, written in C. Given a
commanded launch speed and angle — each with an associated uncertainty — the
program runs thousands of randomized trials, integrates each projectile's
trajectory, and reports the statistical spread of where they land.

This is the kind of analysis used in vehicle dynamics and guidance work: you
rarely launch with exact conditions, so you characterize the *distribution* of
outcomes rather than a single nominal result.

## What it does

Each trial draws a *realized* speed and angle from Gaussian distributions
centered on the commanded values, then integrates the projectile's flight under
gravity until it returns to the ground. Collecting the landing ranges across
many trials produces a dispersion distribution, summarized by:

- **Nominal range** — the analytic (vacuum, flat-ground) range for the commanded
  inputs, used as a validation oracle.
- **Mean / standard deviation** — the center and spread of the landing
  distribution. The standard deviation *is* the dispersion.
- **Min / max** — the extreme landings observed.
- **Hit probability** — the fraction of trials landing within a target window
  (here, the nominal range ± 5%).

## How it works

The simulation rests on a few deliberate numerical choices:

- **Semi-implicit (symplectic) Euler integration.** Each step updates velocity
  first, then advances position using the *new* velocity. This ordering is
  energy-stable over long integrations, unlike explicit Euler, which tends to
  gain energy and drift.
- **Box–Muller transform** for Gaussian sampling, built on a uniform generator
  that excludes zero so the transform's logarithm is always well-defined.
- **Analytic validation.** The closed-form range `v² · sin(2θ) / g` provides an
  independent check on the integrator for the vacuum, ground-launch case.

The code is organized so the simulation core is fully portable and free of any
I/O or platform concerns — the command-line program is just one front-end built
on top of it.

## File structure

```
.
├── dispersion.h    Public interface: types (Vec2, State) and function prototypes
├── dispersion.c    Pure simulation core: integrator, sampling, statistics
├── main.c          Command-line front-end: argument parsing, reporting
├── Makefile        Build rules
└── README.md
```

The split keeps `dispersion.c` a clean, reusable library — `main.c` supplies the
CLI, but the core has no dependence on it and could back other front-ends
equally well.

## Building

Requires a C compiler (`gcc` or `clang`) and a standard math library. No
third-party dependencies.

Using the Makefile:

```sh
make
```

Or compile directly:

```sh
gcc dispersion.c main.c -o sim -lm
```

The `-lm` flag links the math library (`sin`, `cos`, `sqrt`, `log`), which is
required.

## Usage

Run with defaults:

```sh
./sim
```

Override any parameter with a flag:

```
-s <speed>          commanded launch speed (m/s)
-a <angle>          commanded launch angle (degrees)
-d <speed_stddev>   standard deviation of launch speed (m/s)
-g <angle_stddev>   standard deviation of launch angle (degrees)
-n <n_trials>       number of Monte Carlo trials
-h                  print usage and exit
```

Flags may be given in any order; omitted parameters use their defaults.

### Examples

```sh
./sim                          # all defaults
./sim -s 50 -a 35              # 50 m/s at 35 degrees
./sim -s 50 -a 35 -n 100000    # 100,000 trials
./sim -d 1.5 -g 2.0            # larger launch uncertainty
```

## Example output

```
=== Configuration ===
Commanded speed       :   90.00 m/s
Commanded angle       :   45.00 deg
Speed std dev         :    1.20 m/s
Angle std dev         :    0.70 deg
Trials                :   10000

Running simulation... Done!

=== Results ===
Nominal range         :  825.69 m
Range mean            :  825.54 m
Range std dev         :   21.77 m
Range min             :  742.31 m
Range max             :  913.88 m
Hit probability       :   94.35 %  [784-867 m window]
```

(Exact numbers vary run to run, since each run is seeded from the system clock.)

## Notes

- The integrator detects impact on the first step where the projectile passes
  below ground level, so the reported range is long by approximately one
  timestep's worth of travel — negligible at the default timestep.
- The default timestep is fine for the scales above; very high speeds or very
  long flights would benefit from a smaller value.
