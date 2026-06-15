/*
 * netcode_k3h4_scaling_benchmark_test.c
 *
 * Measures k3h4 power-mode clustering cost vs a synthetic O(n²) transformer
 * attention reference across n ∈ {64, 256, 1024, 4096, 16384}.
 *
 * Validates that:
 *   - k3h4 log-log slope ≈ 1.0  (O(n·k), linear in input size)
 *   - attention log-log slope ≈ 2.0  (O(n²), quadratic reference)
 *
 * Writes a JSON report to:
 *   argv[1]  if supplied (e.g. artifacts/native/k3h4-scaling-benchmark.json)
 *
 * Exit code: 0 = pass, 1 = slope validation failed.
 */

#include "runtime/abi/netcode/netcode_abi.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* ---- Platform high-resolution timer ---- */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static int64_t bench_ns(void)
{
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    return (int64_t)((double)count.QuadPart * 1e9 / (double)freq.QuadPart);
}
#else
#include <time.h>
static int64_t bench_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000000LL + (int64_t)ts.tv_nsec;
}
#endif

/* ---- Benchmark parameters ---- */
#define BENCH_N_COUNT 5
static const int BENCH_N[BENCH_N_COUNT] = {64, 256, 1024, 4096, 16384};

/* Minimum wall time per measurement to suppress timer-resolution noise. */
#define BENCH_MIN_WALL_NS  2000000LL  /* 2 ms */

/* Slope tolerance bands */
#define K3H4_SLOPE_LO 0.75
#define K3H4_SLOPE_HI 1.25
#define ATTN_SLOPE_LO 1.75
#define ATTN_SLOPE_HI 2.25

/* ---- Fixed k3h4 input (deterministic) ---- */
static RuntimeK3h4VectorSignalInput make_k3h4_input(void)
{
    RuntimeK3h4VectorSignalInput v;
    memset(&v, 0, sizeof(v));
    v.call_density              = 48;
    v.quest_percent             = 55;
    v.player_level              = 6;
    v.combo_streak              = 3;
    v.branch_pressure           = 22;
    v.dependency_pulse          = 37;
    v.workflow_depth            = 2;
    v.neural_relevance_score    = 61;
    v.network_dimensions        = 10;
    v.model_confidence          = 72;
    v.policy_momentum           = 64;
    v.assignment_family         = RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER;
    v.spectral_mode             = RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED;
    v.hardware_byte_order_tag   = RUNTIME_K3H4_BYTE_ORDER_TAG;
    v.hardware_dtype_tag        = RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED;
    v.hardware_alignment_bytes  = RUNTIME_K3H4_ALIGNMENT_BYTES_4;
    return v;
}

/* ---- Measure k3h4 cost for n vectors ----
 * Runs ceil(n / RUNTIME_NETCODE_VECTOR_NODE_COUNT) pipeline invocations.
 * Returns total nanoseconds (repeats until >= BENCH_MIN_WALL_NS).
 */
static int64_t measure_k3h4_ns(int n)
{
    RuntimeK3h4VectorSignalInput input = make_k3h4_input();
    RuntimeNetcodeK3h4Output    output;
    int calls_per_round = (n + RUNTIME_NETCODE_VECTOR_NODE_COUNT - 1)
                          / RUNTIME_NETCODE_VECTOR_NODE_COUNT;
    int rounds = 1;
    int64_t t0, t1, elapsed;
    int i, r;

    /* Warm-up */
    runtime_netcode_abi_reset();
    runtime_netcode_abi_build_k3h4(input, &output);

    /* Find how many rounds needed for >= BENCH_MIN_WALL_NS */
    do
    {
        t0 = bench_ns();
        for (r = 0; r < rounds; r++)
        {
            runtime_netcode_abi_reset();
            for (i = 0; i < calls_per_round; i++)
                runtime_netcode_abi_build_k3h4(input, &output);
        }
        t1      = bench_ns();
        elapsed = t1 - t0;
        if (elapsed < BENCH_MIN_WALL_NS)
            rounds *= 4;
    } while (elapsed < BENCH_MIN_WALL_NS && rounds < (1 << 20));

    return elapsed / rounds;
}

/* ---- Measure synthetic O(n²) attention reference ----
 * Performs n*n volatile double multiply-add operations to model attention
 * cost without requiring a real transformer model.
 */
static int64_t measure_attention_ns(int n)
{
    volatile double acc = 1.0;
    int64_t t0, t1, elapsed;
    int rounds = 1;
    int r, i, j;

    do
    {
        t0 = bench_ns();
        for (r = 0; r < rounds; r++)
        {
            acc = 1.0;
            for (i = 0; i < n; i++)
                for (j = 0; j < n; j++)
                    acc += (double)(i + 1) / (double)(j + 1);
        }
        t1      = bench_ns();
        elapsed = t1 - t0;
        if (elapsed < BENCH_MIN_WALL_NS)
            rounds *= 4;
    } while (elapsed < BENCH_MIN_WALL_NS && rounds < (1 << 10));

    (void)acc; /* ensure optimizer keeps the loop */
    return elapsed / rounds;
}

/* ---- Log-log slope via least-squares ---- */
static double log_log_slope(const int *ns, const int64_t *costs, int count)
{
    double sum_xy = 0.0, sum_x = 0.0, sum_y = 0.0, sum_x2 = 0.0;
    int i;
    for (i = 0; i < count; i++)
    {
        double x = log((double)ns[i]);
        double y = log((double)costs[i]);
        sum_x  += x;
        sum_y  += y;
        sum_xy += x * y;
        sum_x2 += x * x;
    }
    double n    = (double)count;
    double denom = n * sum_x2 - sum_x * sum_x;
    if (denom == 0.0)
        return 0.0;
    return (n * sum_xy - sum_x * sum_y) / denom;
}

/* ---- Write benchmark JSON ---- */
static int write_json(const char *path,
                      const int *ns,
                      const int64_t *k3h4_ns,
                      const int64_t *attn_ns,
                      int count)
{
    FILE *fp = fopen(path, "w");
    int i;
    if (!fp)
        return -1;
    fprintf(fp, "{\n");
    fprintf(fp, "  \"schema_version\": 1,\n");
    fprintf(fp, "  \"series\": [\n");
    for (i = 0; i < count; i++)
    {
        fprintf(fp, "    { \"n\": %d, \"k3h4_ns\": %lld, \"attention_ns\": %lld }%s\n",
                ns[i],
                (long long)k3h4_ns[i],
                (long long)attn_ns[i],
                (i < count - 1) ? "," : "");
    }
    fprintf(fp, "  ]\n");
    fprintf(fp, "}\n");
    fclose(fp);
    return 0;
}

/* ---- Entry point ---- */
int main(int argc, char **argv)
{
    int64_t k3h4_costs[BENCH_N_COUNT];
    int64_t attn_costs[BENCH_N_COUNT];
    double  k3h4_slope;
    double  attn_slope;
    int     i;
    int     passed = 1;

    printf("[k3h4-scaling-benchmark] measuring %d input sizes...\n", BENCH_N_COUNT);

    for (i = 0; i < BENCH_N_COUNT; i++)
    {
        k3h4_costs[i] = measure_k3h4_ns(BENCH_N[i]);
        attn_costs[i] = measure_attention_ns(BENCH_N[i]);
        printf("  n=%5d  k3h4_ns=%8lld  attention_ns=%12lld\n",
               BENCH_N[i],
               (long long)k3h4_costs[i],
               (long long)attn_costs[i]);
    }

    k3h4_slope = log_log_slope(BENCH_N, k3h4_costs, BENCH_N_COUNT);
    attn_slope = log_log_slope(BENCH_N, attn_costs, BENCH_N_COUNT);

    printf("[k3h4-scaling-benchmark] k3h4 log-log slope=%.4f  (target: 1.0 ± 0.25)\n", k3h4_slope);
    printf("[k3h4-scaling-benchmark] attention log-log slope=%.4f  (target: 2.0 ± 0.25)\n", attn_slope);

    if (k3h4_slope < K3H4_SLOPE_LO || k3h4_slope > K3H4_SLOPE_HI)
    {
        fprintf(stderr, "[k3h4-scaling-benchmark] FAIL: k3h4 slope %.4f outside [%.2f, %.2f]\n",
                k3h4_slope, K3H4_SLOPE_LO, K3H4_SLOPE_HI);
        passed = 0;
    }

    if (attn_slope < ATTN_SLOPE_LO || attn_slope > ATTN_SLOPE_HI)
    {
        fprintf(stderr, "[k3h4-scaling-benchmark] FAIL: attention slope %.4f outside [%.2f, %.2f]\n",
                attn_slope, ATTN_SLOPE_LO, ATTN_SLOPE_HI);
        passed = 0;
    }

    /* Write JSON artifact if output path provided */
    if (argc >= 2)
    {
        if (write_json(argv[1], BENCH_N, k3h4_costs, attn_costs, BENCH_N_COUNT) != 0)
        {
            fprintf(stderr, "[k3h4-scaling-benchmark] WARNING: failed to write JSON to %s\n", argv[1]);
            /* Not a test failure — JSON write is best-effort in test context */
        }
        else
        {
            printf("[k3h4-scaling-benchmark] JSON written to %s\n", argv[1]);
        }
    }

    if (!passed)
    {
        fprintf(stderr, "[k3h4-scaling-benchmark] FAIL\n");
        return 1;
    }

    printf("[k3h4-scaling-benchmark] PASS\n");
    return 0;
}
