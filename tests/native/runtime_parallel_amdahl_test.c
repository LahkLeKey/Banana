#include "runtime/parallel_model.h"

#include <math.h>
#include <stdio.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

static int expect_near(const char *label, double actual, double expected, double tolerance)
{
    double delta = fabs(actual - expected);
    if (delta <= tolerance)
        return 1;

    fprintf(stderr,
            "%s expected=%.12f actual=%.12f delta=%.12f tolerance=%.12f\n",
            label,
            expected,
            actual,
            delta,
            tolerance);
    return 0;
}

int main(void)
{
    const double tol = 1e-9;
    const double s1 = 0.20;
    const double s2 = 0.05;
    const double sp_s1_p4 = runtime_parallel_amdahl_speedup(s1, 4);
    const double sp_s2_p8 = runtime_parallel_amdahl_speedup(s2, 8);
    const double eff_s1_p4 = runtime_parallel_amdahl_efficiency(s1, 4);
    const double sp_s1_p16 = runtime_parallel_amdahl_speedup(s1, 16);
    const double s1_limit = 1.0 / s1;
    const double inferred_s1 = runtime_parallel_amdahl_serial_fraction_from_speedup(sp_s1_p4, 4);

    if (!expect_near("amdahl s=0.20 p=4", sp_s1_p4, 2.5, tol))
        return 1;

    if (!expect_near("amdahl s=0.05 p=8", sp_s2_p8, 5.925925925925926, tol))
        return 1;

    if (!expect_near("efficiency s=0.20 p=4", eff_s1_p4, 0.625, tol))
        return 1;

    if (!expect_near("serial>=1 clamps to speedup=1", runtime_parallel_amdahl_speedup(1.3, 8), 1.0, tol))
        return 1;

    if (!expect_near("serial<=0 clamps to speedup=p", runtime_parallel_amdahl_speedup(-0.4, 8), 8.0, tol))
        return 1;

    if (!expect_near("workers<=1 speedup=1", runtime_parallel_amdahl_speedup(0.2, 1), 1.0, tol))
        return 1;

    if (!expect_true("speedup monotonic with workers", sp_s1_p16 > sp_s1_p4))
        return 1;

    if (!expect_true("speedup upper bounded by 1/serial", sp_s1_p16 <= (s1_limit + tol)))
        return 1;

    if (!expect_near("infer serial fraction from speedup", inferred_s1, s1, tol))
        return 1;

    return 0;
}
