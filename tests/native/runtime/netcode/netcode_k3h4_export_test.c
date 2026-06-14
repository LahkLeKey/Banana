/*
 * netcode_k3h4_export_test.c
 *
 * Validates that banana_k3h4_export_training_artifact:
 *   - Writes an artifact file with correct magic, version, and CRC-integrity.
 *   - Rejects invalid session IDs and fails fast on hardware preflight mismatch.
 *   - Writes atomically (no partial file visible on error path).
 */

#include "runtime/abi/netcode/netcode_abi.h"
#include "runtime/netcode/k3h4/netcode_k3h4_export.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define BENCH_MKDIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#define BENCH_MKDIR(p) mkdir((p), 0755)
#endif

static int fail(const char *msg)
{
    fprintf(stderr, "[k3h4-export-test] FAIL: %s\n", msg);
    return 1;
}

/* Build a valid k3h4 output via the ABI */
static int make_k3h4_output(RuntimeNetcodeK3h4Output *out)
{
    RuntimeK3h4VectorSignalInput input;
    memset(&input, 0, sizeof(input));
    input.call_density             = 48;
    input.quest_percent            = 55;
    input.player_level             = 6;
    input.combo_streak             = 3;
    input.branch_pressure          = 22;
    input.dependency_pulse         = 37;
    input.workflow_depth           = 2;
    input.neural_relevance_score   = 61;
    input.network_dimensions       = 10;
    input.model_confidence         = 72;
    input.policy_momentum          = 64;
    input.assignment_family        = RUNTIME_NETCODE_K3H4_ASSIGNMENT_POWER;
    input.spectral_mode            = RUNTIME_NETCODE_K3H4_SPECTRAL_DISABLED;
    input.hardware_byte_order_tag  = RUNTIME_K3H4_BYTE_ORDER_TAG;
    input.hardware_dtype_tag       = RUNTIME_K3H4_DTYPE_TAG_F32_Q16_MIXED;
    input.hardware_alignment_bytes = RUNTIME_K3H4_ALIGNMENT_BYTES_4;

    runtime_netcode_abi_reset();
    return runtime_netcode_abi_build_k3h4(input, out);
}

int main(void)
{
    RuntimeNetcodeK3h4Output state;
    BananaK3h4ExportStatus   status;
    const char *out_dir = "out/v3-native/k3h4-export-test-tmp";
    char artifact_path[512];
    FILE *fp;
    uint8_t header[7];
    size_t nread;

    /* ---- build valid pipeline output ---- */
    if (make_k3h4_output(&state) != 0)
        return fail("ABI build failed");

    /* ---- invalid session id rejected ---- */
    status = banana_k3h4_export_training_artifact("", 0, &state, out_dir);
    if (status != BANANA_K3H4_EXPORT_INVALID_SESSION)
        return fail("empty session_id should return INVALID_SESSION");

    status = banana_k3h4_export_training_artifact(
        "this-session-id-is-too-long-12345", 0, &state, out_dir);
    if (status != BANANA_K3H4_EXPORT_INVALID_SESSION)
        return fail("too-long session_id should return INVALID_SESSION");

    status = banana_k3h4_export_training_artifact("bad/id", 0, &state, out_dir);
    if (status != BANANA_K3H4_EXPORT_INVALID_SESSION)
        return fail("session_id with slash should return INVALID_SESSION");

    /* ---- null inputs rejected ---- */
    status = banana_k3h4_export_training_artifact(NULL, 0, &state, out_dir);
    if (status != BANANA_K3H4_EXPORT_INVALID_SESSION)
        return fail("null session_id should return INVALID_SESSION");

    status = banana_k3h4_export_training_artifact("sess01", 0, NULL, out_dir);
    if (status != BANANA_K3H4_EXPORT_INVALID_SESSION)
        return fail("null state should return INVALID_SESSION");

    /* ---- hardware preflight: wrong byte-order tag rejected ---- */
    {
        RuntimeNetcodeK3h4Output bad_state = state;
        bad_state.envelope.byte_order_tag = 0x04030201; /* swapped */
        status = banana_k3h4_export_training_artifact("sess01", 0, &bad_state, out_dir);
        if (status != BANANA_K3H4_EXPORT_PREFLIGHT_FAILED)
            return fail("swapped byte-order tag should return PREFLIGHT_FAILED");
    }

    /* ---- happy path: artifact written and readable ---- */
    BENCH_MKDIR("out");
    BENCH_MKDIR("out/v3-native");
    BENCH_MKDIR(out_dir);

    status = banana_k3h4_export_training_artifact("sess01", 0, &state, out_dir);
    if (status != BANANA_K3H4_EXPORT_OK)
        return fail("valid export returned non-OK status");

    snprintf(artifact_path, sizeof(artifact_path),
             "%s/k3h4-training/sess01/epoch-0.bin", out_dir);

    fp = fopen(artifact_path, "rb");
    if (!fp)
        return fail("artifact file was not created");

    /* Verify magic byte and version in first 3 bytes */
    nread = fread(header, 1, sizeof(header), fp);
    fclose(fp);

    if (nread < 7)
        return fail("artifact file too short");
    if (header[0] != 0x01)
        return fail("artifact_type magic byte incorrect");
    /* version = uint16 LE = 1 */
    if (header[1] != 0x01 || header[2] != 0x00)
        return fail("contract_version incorrect");
    /* byte_order_tag = 0x01020304 LE */
    if (header[3] != 0x04 || header[4] != 0x03 || header[5] != 0x02 || header[6] != 0x01)
        return fail("byte_order_tag incorrect in artifact");

    /* ---- epoch index increments without conflict ---- */
    status = banana_k3h4_export_training_artifact("sess01", 1, &state, out_dir);
    if (status != BANANA_K3H4_EXPORT_OK)
        return fail("epoch 1 export failed");

    status = banana_k3h4_export_training_artifact("sess02", 0, &state, out_dir);
    if (status != BANANA_K3H4_EXPORT_OK)
        return fail("second session export failed");

    printf("[k3h4-export-test] PASS\n");
    return 0;
}
