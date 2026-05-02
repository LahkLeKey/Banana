/* banana_status.h / banana_status.c — single canonical error reporting
 * (spec 006 FR-004). Numeric values mirror BananaStatusCode in the public
 * header so internal modules and managed callers agree.
 */
#ifndef BANANA_STATUS_H
#define BANANA_STATUS_H

#include "banana_wrapper.h" /* re-uses the public enum */

#ifdef __cplusplus
extern "C"
{
#endif

    /* Canonical lookup of a remediation hint string for a status code.
     * Returns a static C string; do NOT free. */
    const char *banana_status_remediation(int status);

#ifdef __cplusplus
}
#endif

#endif
