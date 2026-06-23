#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "dispersion.h"

static const Vec2 VEC2_ZERO = { 0.0, 0.0 };

#define DT 0.0001

void print_usage(char *command)
{
        fprintf(stderr, 
                "Usage: %s [-s speed] [-a angle] [-d speed_stddev]"
                " [-g angle_stddev] [-n n_trials]\n", command);
}

int main(int argc, char *argv[])
{
        srand(time(NULL));

        /* defaults */
        double commanded_speed = 90;
        double commanded_angle_deg = 45.0;
        double speed_stddev = 0.5;
        double angle_stddev = 0.5;
        size_t n_trials = 10000;

        int opt;
        while ((opt = getopt(argc, argv, "s:a:d:g:n:h")) != -1) {
                switch (opt) {
                        case 's': commanded_speed = strtod(optarg, NULL); break;
                        case 'a': commanded_angle_deg = strtod(optarg, NULL); break;
                        case 'd': speed_stddev = strtod(optarg, NULL); break;
                        case 'g': angle_stddev = strtod(optarg, NULL); break;
                        case 'n': n_trials = strtoul(optarg, NULL, 10); break;
                        case 'h': print_usage(argv[0]); exit(0);
                        case '?': print_usage(argv[0]); exit(1);
                }
        }

        if (commanded_speed <= 0.0 || speed_stddev < 0.0 || angle_stddev < 0.0 || n_trials == 0) {
                fprintf(stderr, "Error: invalid parameter value\n");
                print_usage(argv[0]);
                exit(1);
        }

        printf("=== Configuration ===\n");
        printf("Commanded speed       : %7.2f m/s\n", commanded_speed); 
        printf("Commanded angle       : %7.2f deg\n", commanded_angle_deg);
        printf("Speed std dev         : %7.2f m/s\n", speed_stddev);
        printf("Angle std dev         : %7.2f deg\n", angle_stddev);
        printf("Trials                : %7zu\n", n_trials);

        printf("\n");
        printf("Running simulation...");
        fflush(stdout);

        double *range = malloc(n_trials * sizeof(double));
        if (range == NULL) {
                return EXIT_FAILURE;
        }

        for (size_t i = 0; i < n_trials; i++) {
                double realized_speed = gaussian(commanded_speed, speed_stddev);

                double realized_angle_deg = 
                        gaussian(commanded_angle_deg, angle_stddev);

                double realized_angle_rad = realized_angle_deg * M_PI / 180.0;

                State s = make_initial_state(VEC2_ZERO, 
                                             realized_speed, 
                                             realized_angle_rad);

                range[i] = simulate(s, DT);
        }

        printf(" Done!\n\n");

        double range_nominal = 
                analytic_range(commanded_speed, commanded_angle_deg * M_PI / 180.0);

        double lo = range_nominal - range_nominal * 0.05;
        double hi = range_nominal + range_nominal * 0.05;

        printf("=== Results ===\n");
        printf("Nominal range         : %7.2f m\n", range_nominal);
        printf("Range mean            : %7.2f m\n", mean(range, n_trials));
        printf("Range std dev         : %7.2f m\n", stddev(range, n_trials));
        printf("Range min             : %7.2f m\n", min(range, n_trials));
        printf("Range max             : %7.2f m\n", max(range, n_trials));
        printf("Hit probability       : %7.2f %%  [%.0f-%.0f m window]\n", 
                100.0 * hit_probability(range, n_trials, lo, hi), lo, hi);

        free(range);
        return EXIT_SUCCESS;
}
