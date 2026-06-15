#include "netcode_k3h4_export.h"

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define BANANA_MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define BANANA_MKDIR(path) mkdir((path), 0755)
#endif

/* ---------------------------------------------------------------------------
 * Internal constants
 * ------------------------------------------------------------------------- */

#define K3H4_EXPORT_BYTE_ORDER_TAG      0x01020304u
#define K3H4_EXPORT_DTYPE_TAG_EXPECTED  1
#define K3H4_EXPORT_ALIGNMENT_EXPECTED  4

/* 43 bytes fixed header + up to 4 clusters × 80 bytes + 8 bytes trailer */
#define K3H4_EXPORT_BUFFER_MAX          4096

/* ---------------------------------------------------------------------------
 * CRC-32 (ISO 3309 polynomial 0xEDB88320)
 * ------------------------------------------------------------------------- */

static uint32_t k3h4_export_crc32_update(uint32_t crc, uint8_t byte)
{
    int bit;
    crc ^= byte;
    for (bit = 0; bit < 8; bit++)
    {
        uint32_t mask = (uint32_t)(-(int32_t)(crc & 1u));
        crc = (crc >> 1) ^ (0xEDB88320u & mask);
    }
    return crc;
}

static uint32_t k3h4_export_crc32(const uint8_t *buf, size_t len)
{
    /* CRC covers the serialized payload bytes exactly as written to disk. */
    uint32_t crc = 0xFFFFFFFFu;
    size_t i;
    for (i = 0; i < len; i++)
        crc = k3h4_export_crc32_update(crc, buf[i]);
    return ~crc;
}

/* ---------------------------------------------------------------------------
 * Little-endian write helpers
 * ------------------------------------------------------------------------- */

static size_t write_u8(uint8_t *buf, size_t pos, uint8_t v)
{
    buf[pos] = v;
    return pos + 1u;
}

static size_t write_le_u16(uint8_t *buf, size_t pos, uint16_t v)
{
    buf[pos]     = (uint8_t)(v & 0xFFu);
    buf[pos + 1] = (uint8_t)((v >> 8) & 0xFFu);
    return pos + 2u;
}

static size_t write_le_u32(uint8_t *buf, size_t pos, uint32_t v)
{
    buf[pos]     = (uint8_t)(v & 0xFFu);
    buf[pos + 1] = (uint8_t)((v >> 8) & 0xFFu);
    buf[pos + 2] = (uint8_t)((v >> 16) & 0xFFu);
    buf[pos + 3] = (uint8_t)((v >> 24) & 0xFFu);
    return pos + 4u;
}

static size_t write_le_i32(uint8_t *buf, size_t pos, int32_t v)
{
    return write_le_u32(buf, pos, (uint32_t)v);
}

static size_t write_le_f32(uint8_t *buf, size_t pos, float v)
{
    uint32_t bits;
    memcpy(&bits, &v, sizeof(bits));
    return write_le_u32(buf, pos, bits);
}

/* ---------------------------------------------------------------------------
 * Directory creation helper (single-level then nested)
 * ------------------------------------------------------------------------- */

static int ensure_dir(const char *path)
{
    int rc = BANANA_MKDIR(path);
    /* EEXIST is acceptable */
    return (rc == 0 || errno == EEXIST) ? 0 : -1;
}

static int ensure_dir_tree(char *path)
{
    /* Create each prefix in-place so nested session directories materialize
     * without requiring the caller to pre-create intermediate folders.
     */
    char *sep = path;
    while ((sep = strchr(sep + 1, '/')) != NULL)
    {
        *sep = '\0';
        if (ensure_dir(path) != 0)
        {
            *sep = '/';
            return -1;
        }
        *sep = '/';
    }
    return ensure_dir(path);
}

/* ---------------------------------------------------------------------------
 * Session-id validation
 * ------------------------------------------------------------------------- */

static int session_id_valid(const char *id)
{
    size_t len;
    size_t i;
    if (!id || id[0] == '\0')
        return 0;
    len = strlen(id);
    if (len > BANANA_K3H4_SESSION_ID_MAX_LEN)
        return 0;
    for (i = 0; i < len; i++)
    {
        char c = id[i];
        int ok = (c >= 'a' && c <= 'z') ||
                 (c >= 'A' && c <= 'Z') ||
                 (c >= '0' && c <= '9') ||
                 (c == '-') || (c == '_');
        if (!ok)
            return 0;
    }
    return 1;
}

/* ---------------------------------------------------------------------------
 * Public export function
 * ------------------------------------------------------------------------- */

BananaK3h4ExportStatus banana_k3h4_export_training_artifact(
    const char *session_id,
    int32_t epoch_index,
    const RuntimeNetcodeK3h4Output *state,
    const char *out_dir)
{
    uint8_t buf[K3H4_EXPORT_BUFFER_MAX];
    size_t pos = 0;
    size_t crc_end;
    uint32_t crc;
    int cluster;
    int dim;
    int cluster_count;
    int dimension;
    char dir_path[512];
    char out_path[512];
    char tmp_path[512];
    FILE *fp;
    int mode_flag;
    int spectral_active;

    /* ---- input validation ---- */
    if (!state || !out_dir || !session_id)
        return BANANA_K3H4_EXPORT_INVALID_SESSION;
    if (!session_id_valid(session_id))
        return BANANA_K3H4_EXPORT_INVALID_SESSION;

    /* ---- hardware preflight (must match expected LE layout) ---- */
    if ((uint32_t)state->envelope.byte_order_tag != K3H4_EXPORT_BYTE_ORDER_TAG)
        return BANANA_K3H4_EXPORT_PREFLIGHT_FAILED;

    /* ---- extract metadata ---- */
    cluster_count = state->cluster_count;
    dimension     = state->dimensions;
    if (cluster_count < 1 || cluster_count > RUNTIME_NETCODE_VECTOR_NODE_COUNT)
        return BANANA_K3H4_EXPORT_PREFLIGHT_FAILED;
    if (dimension < 1 || dimension > RUNTIME_NETCODE_VECTOR_MAX_DIMENSIONS)
        return BANANA_K3H4_EXPORT_PREFLIGHT_FAILED;

    /*
     * RuntimeNetcodeK3h4Output does not carry the assignment_family used to
     * produce it, so mode_flag defaults to MULTIPLICATIVE (0) as the
     * conservative baseline.  Callers that need the exact mode should
     * extend this via a separate config struct in a future ABI revision.
     */
    mode_flag       = RUNTIME_NETCODE_K3H4_ASSIGNMENT_MULTIPLICATIVE;
    spectral_active = (state->spectral_proxy[0].spectral_state != RUNTIME_NETCODE_SPECTRAL_DISABLED)
                          ? 1 : 0;

    /* ---- build binary payload ---- */
    memset(buf, 0, sizeof(buf));

    /* Header */
    pos = write_u8(buf, pos, BANANA_K3H4_TRAINING_ARTIFACT_TYPE);
    pos = write_le_u16(buf, pos, BANANA_K3H4_TRAINING_ARTIFACT_VERSION);
    pos = write_le_u32(buf, pos, K3H4_EXPORT_BYTE_ORDER_TAG);

    /* session_id: 16 bytes, null-padded */
    {
        char sid_field[16];
        memset(sid_field, 0, sizeof(sid_field));
        strncpy(sid_field, session_id, 15);
        memcpy(buf + pos, sid_field, 16);
        pos += 16u;
    }

    pos = write_le_i32(buf, pos, epoch_index);
    pos = write_le_i32(buf, pos, mode_flag);
    pos = write_le_i32(buf, pos, spectral_active);
    pos = write_le_i32(buf, pos, cluster_count);
    pos = write_le_i32(buf, pos, dimension);

    /* Cluster records */
    for (cluster = 0; cluster < cluster_count; cluster++)
    {
        const RuntimeNetcodeK3h4Center *c = &state->centers[cluster];
        const RuntimeNetcodeK3h4Radius *r = &state->radii[cluster];
        float radius_f32;
        int multiplicative_score_q16 = 0;
        int power_score_q16          = 0;
        int spectral_q16             = 0;
        int vec;

        /* center[d] is persisted as float32 even though the runtime contract is
         * Q16-based; the artifact preserves the decoded real-space coordinates.
         */
        for (dim = 0; dim < dimension; dim++)
        {
            float cv = (float)c->center_q16[dim] / 65536.0f;
            pos = write_le_f32(buf, pos, cv);
        }

        /* Radius is likewise emitted in decoded real-space units. */
        radius_f32 = (float)r->inscribed_radius_q16 / 65536.0f;
        pos = write_le_f32(buf, pos, radius_f32);

        /* The artifact stores representative cluster scores from the Q16 score
         * table; callers that need the full score lattice should read the full
         * RuntimeNetcodeK3h4Output instead of this summarized bundle.
         */
        for (vec = 0; vec < state->vector_count; vec++)
        {
            const RuntimeNetcodeWeightedVoronoiScore *s =
                &state->weighted_voronoi_scores[vec * RUNTIME_NETCODE_VECTOR_NODE_COUNT + cluster];
            if (s->cluster_id == cluster)
            {
                multiplicative_score_q16 = s->weighted_score_q16;
                power_score_q16          = s->weighted_score_q16;
                break;
            }
        }

        spectral_q16 = (spectral_active && cluster < RUNTIME_NETCODE_VECTOR_NODE_COUNT)
                           ? state->spectral_proxy[cluster].frequency_proxy_q16
                           : 0;

        pos = write_le_i32(buf, pos, multiplicative_score_q16);
        pos = write_le_i32(buf, pos, power_score_q16);
        pos = write_le_i32(buf, pos, spectral_q16);
    }

    /* Integrity marker */
    pos = write_le_u32(buf, pos, BANANA_K3H4_TRAINING_INTEGRITY_MARKER);

    /* CRC32 over everything so far */
    crc_end = pos;
    crc     = k3h4_export_crc32(buf, crc_end);
    pos     = write_le_u32(buf, pos, crc);

    /* ---- build output paths ---- */
    snprintf(dir_path, sizeof(dir_path), "%s/k3h4-training/%s", out_dir, session_id);
    snprintf(out_path, sizeof(out_path), "%s/epoch-%d.bin", dir_path, (int)epoch_index);
    snprintf(tmp_path, sizeof(tmp_path), "%s/epoch-%d.bin.tmp", dir_path, (int)epoch_index);

    /* ---- ensure directory tree exists ---- */
    if (ensure_dir_tree(dir_path) != 0)
        return BANANA_K3H4_EXPORT_WRITE_FAILED;

    /* ---- atomic write: tmp then rename ---- */
    fp = fopen(tmp_path, "wb");
    if (!fp)
        return BANANA_K3H4_EXPORT_WRITE_FAILED;

    if (fwrite(buf, 1, pos, fp) != pos)
    {
        fclose(fp);
        remove(tmp_path);
        return BANANA_K3H4_EXPORT_WRITE_FAILED;
    }
    fclose(fp);

    /*
     * POSIX rename(2) is atomic and overwrites the destination.
     * Windows rename() fails with EEXIST when the destination exists;
     * remove it first so the swap remains effectively atomic at the
     * application level (no partial file is ever left exposed).
     */
    remove(out_path);
    if (rename(tmp_path, out_path) != 0)
    {
        remove(tmp_path);
        return BANANA_K3H4_EXPORT_WRITE_FAILED;
    }

    return BANANA_K3H4_EXPORT_OK;
}
