#include <stdlib.h>
#include <math.h>
#include "dispersion.h"

/* functions */

/* vector addition */
Vec2 vec2_add(Vec2 a, Vec2 b)
{
        return (Vec2){ a.x + b.x, a.y + b.y };
}

/* scalar multiplication */
Vec2 vec2_scale(Vec2 v, double s)
{
        return (Vec2){ v.x * s, v.y * s };
}

/* 
 * make_initial_state
 *
 * Builds a launch state from a position, speed, and launch angle.
 *
 * pos      : launch position (meters)
 * speed    : launch speed magnitude (m/s)
 * angle_rad: launch angle above horizontal, in RADIANS (not degrees)
 *
 * Returns a State with the given position and the decomposed velocity.
 */
State make_initial_state(Vec2 pos, double speed, double angle_rad)
{
        State s;
        s.pos = pos;
        s.vel = (Vec2){ speed * cos(angle_rad), speed * sin(angle_rad) };

        return s;
}

Vec2 compute_accel(State s)
{
        (void)s; // placeholder so compiler doesn't complain.
                 // s needed once drag implemented later.

        return (Vec2){ 0.0, -9.81 };
}

/*
 * step
 *
 * Advances the state forward by one timestep dt using semi-implicit Euler 
 * integration: velocity is updated first, then position is updated using the
 * NEW velocity. 
 *
 * s : current state
 * dt: timestep (seconds)
 *
 * Returns the state advanced by dt. Does not modify the input state. 
 */
State step(State s, double dt)
{
        State next;

        // compute velocity first:
        // v(t + dt) = v(t) + a(t) * dt
        Vec2 accel = compute_accel(s);
        next.vel = vec2_add(s.vel, vec2_scale(accel, dt));

        // compute position from new velocity:
        // x(t + dt) = x(t) + v(t + dt) * dt
        next.pos = vec2_add(s.pos, vec2_scale(next.vel, dt));

        return next;
}

/* 
 * simulate
 *
 * Advances the state from the initial launch until the projectile hits the 
 * ground. Returns the range: the horizontal landing distance.
 *
 * initial: launch state (position and velocity)
 * dt     : timestep (seconds)
 *
 * Returns the range (horizontal distance at impact, in meters).
 */
double simulate(State initial, double dt)
{
        State s = initial;

        do {
                s = step(s, dt);
        } while (s.pos.y > 0.0);

        return s.pos.x;
}

double analytic_range(double speed, double angle_rad)
{
        return speed * speed * sin(2 * angle_rad) / 9.81;
}

/*
 * uniform01
 *
 * Returns a uniform sample in the interval (0, 1].
 *
 */
double uniform01()
{
        return ( (double)rand() + 1.0 ) / ( (double)RAND_MAX + 1.0 );
}

/*
 * box_muller
 *
 * Returns a sample from a standard normal distribution (mean=0, stddev=1).
 */
double box_muller()
{
        double u1 = uniform01();
        double u2 = uniform01();

        return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
}

/*
 * gaussian
 *
 * Returns a sample from a normal distribution with the given mean and 
 * standard deviation.
 *
 */
double gaussian(double mean, double stddev)
{
        return stddev * box_muller() + mean;
}

double mean(double *values, size_t n)
{
        double sum = 0.0;

        for (size_t i = 0; i < n; i++) {
                sum += values[i];
        }

        return sum / n;
}

double stddev(double *values, size_t n)
{
        double avg = mean(values, n);
        double sum_sq_dev = 0.0;

        for (size_t i = 0; i < n; i++) {
                double dev = values[i] - avg;
                sum_sq_dev += dev * dev;
        }

        return sqrt(sum_sq_dev / (n - 1));
}

double min(double *values, size_t n)
{
        double min = values[0];
        for (size_t i = 1; i < n; i++) {
                if (values[i] < min) {
                        min = values[i];
                }
        }
        
        return min;
}

double max(double *values, size_t n)
{
        double max = values[0];
        for (size_t i = 1; i < n; i++) {
                if (values[i] > max) {
                        max = values[i];
                }
        }

        return max;
}

double hit_probability(double *values, size_t n, double lo, double hi)
{
        size_t count = 0;
        for (size_t i = 0; i < n; i++) {
                if (values[i] >= lo && values[i] <= hi) {
                        count++;
                }
        }
        
        return (double)count / n;
}

/*
int write_values_csv(const char *filename, double *values, size_t n)
{
        FILE *f = fopen(filename, "w");
        if (f == NULL) {
                perror("Error opening file");
                return 1;
        }

        for (size_t i = 0; i < n; i++) {
                fprintf(f, "%.3f\n", values[i]);  
        }

        if (fclose(f) != 0) {
                perror("Error closing file");
                return 1;
        }

        return 0;
}
*/
