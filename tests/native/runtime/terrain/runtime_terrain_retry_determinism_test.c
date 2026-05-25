#include "runtime/terrain/terrain_abi.h"
#include "runtime/terrain/terrain_generation.h"
#include "runtime/terrain/terrain_host.h"

#include <stdio.h>
#include <string.h>

#define TERRAIN_SIZE 32
#define TERRAIN_LAYERS 6

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    unsigned char retry_a[TERRAIN_SIZE * TERRAIN_SIZE];
    unsigned char retry_b[TERRAIN_SIZE * TERRAIN_SIZE];
    unsigned int fingerprint = 0u;
    int status = 0;
    unsigned int retry_lineage_a = 0u;
    unsigned int retry_lineage_b = 0u;

    memset(retry_a, 0, sizeof(retry_a));
    memset(retry_b, 0, sizeof(retry_b));

    status = runtime_terrain_generate_unexplored_contract(
        retry_a,
        TERRAIN_SIZE,
        TERRAIN_LAYERS,
        "world-retry",
        "lane-retry",
        2u,
        3,
        4,
        RUNTIME_TERRAIN_GENERATION_CONTRACT_V1,
        0u,
        RUNTIME_TERRAIN_GENERATION_STATUS_TIMEOUT,
        &fingerprint);
    if (!expect_true("injected timeout status", status == RUNTIME_TERRAIN_GENERATION_STATUS_TIMEOUT))
        return 1;
    if (!expect_true("timeout recoverable",
                     runtime_terrain_host_classify_generation_failure(status) ==
                         RUNTIME_TERRAIN_FAILURE_CLASS_RECOVERABLE))
        return 1;

    retry_lineage_a = runtime_terrain_abi_retry_fingerprint_lineage(fingerprint, 1u, status);
    retry_lineage_b = runtime_terrain_abi_retry_fingerprint_lineage(fingerprint, 1u, status);
    if (!expect_true("retry lineage deterministic", retry_lineage_a == retry_lineage_b))
        return 1;

    if (!expect_true("retry generation a",
                     runtime_terrain_generate_unexplored_contract(
                         retry_a,
                         TERRAIN_SIZE,
                         TERRAIN_LAYERS,
                         "world-retry",
                         "lane-retry",
                         2u,
                         3,
                         4,
                         RUNTIME_TERRAIN_GENERATION_CONTRACT_V1,
                         1u,
                         0,
                         &fingerprint) == RUNTIME_TERRAIN_GENERATION_STATUS_OK))
        return 1;

    if (!expect_true("retry generation b",
                     runtime_terrain_generate_unexplored_contract(
                         retry_b,
                         TERRAIN_SIZE,
                         TERRAIN_LAYERS,
                         "world-retry",
                         "lane-retry",
                         2u,
                         3,
                         4,
                         RUNTIME_TERRAIN_GENERATION_CONTRACT_V1,
                         1u,
                         0,
                         &fingerprint) == RUNTIME_TERRAIN_GENERATION_STATUS_OK))
        return 1;

    if (!expect_true("retry parity equal", memcmp(retry_a, retry_b, sizeof(retry_a)) == 0))
        return 1;

    if (!expect_true("invalid identity non-recoverable",
                     runtime_terrain_host_classify_generation_failure(
                         RUNTIME_TERRAIN_GENERATION_STATUS_INVALID_AREA_IDENTITY) ==
                         RUNTIME_TERRAIN_FAILURE_CLASS_NON_RECOVERABLE))
        return 1;

    return 0;
}
