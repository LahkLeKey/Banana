#ifndef BANANA_ENGINE_RUNTIME_NETCODE_K3H4_EXPORT_H
#define BANANA_ENGINE_RUNTIME_NETCODE_K3H4_EXPORT_H

#include <stdint.h>
#include "netcode_k3h4_metrics.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* -----------------------------------------------------------------------
     * Training artifact ABI constants
     * ----------------------------------------------------------------------- */

    #define BANANA_K3H4_TRAINING_ARTIFACT_VERSION  ((uint16_t)1)
    #define BANANA_K3H4_TRAINING_ARTIFACT_TYPE     ((uint8_t)0x01)
    #define BANANA_K3H4_TRAINING_INTEGRITY_MARKER  ((uint32_t)0xDEADBEEFu)
    #define BANANA_K3H4_SESSION_ID_MAX_LEN         15

    /* -----------------------------------------------------------------------
     * Export status codes
     * ----------------------------------------------------------------------- */

    typedef enum
    {
        BANANA_K3H4_EXPORT_OK                  = 0,
        BANANA_K3H4_EXPORT_PREFLIGHT_FAILED    = 1,
        BANANA_K3H4_EXPORT_WRITE_FAILED        = 2,
        BANANA_K3H4_EXPORT_INVALID_SESSION     = 3,
        BANANA_K3H4_EXPORT_VERSION_UNSUPPORTED = 4,
    } BananaK3h4ExportStatus;

    /* -----------------------------------------------------------------------
     * banana_k3h4_export_training_artifact
     *
     * Serialises the supplied k3h4 pipeline output into a versioned binary
     * training artifact file and writes it atomically to:
     *   <out_dir>/k3h4-training/<session_id>/epoch-<epoch_index>.bin
     *
     * Hardware preflight (byte-order tag, dtype tag, alignment) is validated
     * before any I/O is attempted.  On failure no file is created.
     *
     * Parameters:
     *   session_id  – null-terminated, max BANANA_K3H4_SESSION_ID_MAX_LEN chars.
     *   epoch_index – zero-based epoch counter.
     *   state       – completed RuntimeNetcodeK3h4Output from the pipeline.
     *   out_dir     – base output directory (must already exist).
     *
     * Returns: BananaK3h4ExportStatus
     * ----------------------------------------------------------------------- */
    BananaK3h4ExportStatus banana_k3h4_export_training_artifact(
        const char *session_id,
        int32_t epoch_index,
        const RuntimeNetcodeK3h4Output *state,
        const char *out_dir
    );

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_RUNTIME_NETCODE_K3H4_EXPORT_H */
