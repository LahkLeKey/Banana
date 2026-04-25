/* banana_dal.h — Postgres-gated DAL surface.
 * Loud-gate contract (spec 006 User Story 4):
 *   BANANA_PG_CONNECTION unset => BANANA_DB_NOT_CONFIGURED with remediation.
 */
#ifndef BANANA_DAL_H
#define BANANA_DAL_H

#ifdef __cplusplus
extern "C" {
#endif

int banana_dal_query_profile(const char* profile_id, char** out_json);

#ifdef __cplusplus
}
#endif

#endif
