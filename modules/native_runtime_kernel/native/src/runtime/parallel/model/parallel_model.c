#include "runtime/parallel/model/parallel_model.h"

static double clamp_serial_fraction(double serial_fraction)
{
    if (serial_fraction < 0.0)
        return 0.0;
    if (serial_fraction > 1.0)
        return 1.0;
    return serial_fraction;
}

double runtime_parallel_amdahl_speedup(double serial_fraction, int worker_count)
{
    double clamped_serial = clamp_serial_fraction(serial_fraction);
    double workers = (worker_count > 1) ? (double)worker_count : 1.0;
    double denominator = clamped_serial + ((1.0 - clamped_serial) / workers);

    if (denominator <= 0.0)
        return workers;

    return 1.0 / denominator;
}

double runtime_parallel_amdahl_efficiency(double serial_fraction, int worker_count)
{
    double workers = (worker_count > 1) ? (double)worker_count : 1.0;
    double speedup = runtime_parallel_amdahl_speedup(serial_fraction, worker_count);

    return speedup / workers;
}

double runtime_parallel_amdahl_serial_fraction_from_speedup(double speedup, int worker_count)
{
    double workers = (worker_count > 1) ? (double)worker_count : 1.0;
    double numerator = 0.0;
    double denominator = 0.0;
    double serial_fraction = 0.0;

    if (speedup <= 0.0 || workers <= 1.0)
        return 1.0;

    numerator = (1.0 / speedup) - (1.0 / workers);
    denominator = 1.0 - (1.0 / workers);
    if (denominator <= 0.0)
        return 1.0;

    serial_fraction = numerator / denominator;
    return clamp_serial_fraction(serial_fraction);
}
