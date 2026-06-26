#include "runtime/netcode/k3h4/netcode_k3h4_export.h"
#include "runtime/support/test_support.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

typedef enum BananaExportFailureMode
{
    BANANA_EXPORT_FAIL_NONE = 0,
    BANANA_EXPORT_FAIL_FOPEN = 1,
    BANANA_EXPORT_FAIL_FWRITE = 2,
    BANANA_EXPORT_FAIL_RENAME = 3,
} BananaExportFailureMode;

static BananaExportFailureMode g_failure_mode = BANANA_EXPORT_FAIL_NONE;

int banana_test_mkdir(const char *path)
{
    (void)path;
    return 0;
}

FILE *banana_test_fopen(const char *filename, const char *mode)
{
    (void)filename;
    (void)mode;
    if (g_failure_mode == BANANA_EXPORT_FAIL_FOPEN)
        return NULL;
    return tmpfile();
}

size_t banana_test_fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
    (void)ptr;
    (void)stream;
    if (g_failure_mode == BANANA_EXPORT_FAIL_FWRITE)
        return 0;
    return size * count;
}

int banana_test_rename(const char *old_path, const char *new_path)
{
    (void)old_path;
    (void)new_path;
    if (g_failure_mode == BANANA_EXPORT_FAIL_RENAME)
    {
        errno = EACCES;
        return -1;
    }
    return 0;
}

int banana_test_remove(const char *path)
{
    (void)path;
    return 0;
}

static RuntimeNetcodeK3h4Output make_valid_export_state(void)
{
    RuntimeNetcodeK3h4Output state;
    memset(&state, 0, sizeof(state));

    state.envelope.byte_order_tag = 0x01020304u;
    state.cluster_count = 1;
    state.dimensions = 1;
    state.vector_count = 1;
    state.centers[0].center_q16[0] = 65536;
    state.radii[0].inscribed_radius_q16 = 65536;
    state.weighted_voronoi_scores[0].cluster_id = 99;
    state.weighted_voronoi_scores[0].weighted_score_q16 = 32768;

    return state;
}

static void test_export_returns_write_failed_when_fopen_fails(void **state)
{
    RuntimeNetcodeK3h4Output output;
    BananaK3h4ExportStatus status;
    (void)state;

    output = make_valid_export_state();
    g_failure_mode = BANANA_EXPORT_FAIL_FOPEN;

    status = banana_k3h4_export_training_artifact("iofail01", 0, &output, "out/io-failure");

    BANANA_TEST_ASSERT_INT_EQ((int)status,
                              (int)BANANA_K3H4_EXPORT_WRITE_FAILED,
                              "export must return WRITE_FAILED when fopen returns null");
}

static void test_export_returns_write_failed_when_fwrite_fails(void **state)
{
    RuntimeNetcodeK3h4Output output;
    BananaK3h4ExportStatus status;
    (void)state;

    output = make_valid_export_state();
    g_failure_mode = BANANA_EXPORT_FAIL_FWRITE;

    status = banana_k3h4_export_training_artifact("iofail02", 0, &output, "out/io-failure");

    BANANA_TEST_ASSERT_INT_EQ((int)status,
                              (int)BANANA_K3H4_EXPORT_WRITE_FAILED,
                              "export must return WRITE_FAILED when fwrite writes fewer bytes");
}

static void test_export_returns_write_failed_when_rename_fails(void **state)
{
    RuntimeNetcodeK3h4Output output;
    BananaK3h4ExportStatus status;
    (void)state;

    output = make_valid_export_state();
    g_failure_mode = BANANA_EXPORT_FAIL_RENAME;

    status = banana_k3h4_export_training_artifact("iofail03", 0, &output, "out/io-failure");

    BANANA_TEST_ASSERT_INT_EQ((int)status,
                              (int)BANANA_K3H4_EXPORT_WRITE_FAILED,
                              "export must return WRITE_FAILED when rename fails");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_export_returns_write_failed_when_fopen_fails),
    BANANA_TEST_CASE(test_export_returns_write_failed_when_fwrite_fails),
    BANANA_TEST_CASE(test_export_returns_write_failed_when_rename_fails)
)
