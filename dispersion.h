#ifndef DISPERSION_H
#define DISPERSION_H

#include <stddef.h> /* for size_t  */

/* type definitions */
typedef struct Vec2 { double x; double y; } Vec2;
typedef struct State { Vec2 pos; Vec2 vel; } State;

/* function prototypes */
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_scale(Vec2 v, double s);
State make_initial_state(Vec2 pos, double speed, double angle_rad);
Vec2 compute_accel(State s);
State step(State s, double dt);
double simulate(State initial, double dt);
double analytic_range(double speed, double angle_rad);

/*
 * uniform01
 *
 * Returns a uniform sample in the interval (0, 1].
 *
 */
double uniform01(void);

/*
 * box_muller
 *
 * Returns a sample from a standard normal distribution (mean=0, stddev=1).
 */
double box_muller(void);

/*
 * gaussian
 *
 * Returns a sample from a normal distribution with the given mean and 
 * standard deviation.
 */
double gaussian(double mean, double stddev);

/*
 * mean
 *
 * Returns the average of values.
 */
double mean(double *values, size_t n);

/* 
 * stddev
 *
 * Returns the standard deviation of values.
 */
double stddev(double *values, size_t n);

/*
 * min
 *
 * Returns the mininum number in values.
 */
double min(double *values, size_t n);

/*
 * max
 *
 * Returns the maximum number in values.
 */
double max(double *values, size_t n);

/*
 * hit_probability
 *
 */
double hit_probability(double *values, size_t n, double lo, double hi);

#endif
