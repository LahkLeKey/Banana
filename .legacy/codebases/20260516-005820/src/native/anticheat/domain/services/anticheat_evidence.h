// Evidence correlation domain
#ifndef BANANA_ANTICHEAT_EVIDENCE_H
#define BANANA_ANTICHEAT_EVIDENCE_H
#include <stdint.h>
#include "banana_anticheat.h"
void anticheat_evidence_record(const char *session_id, int evidence_type, int value);
int anticheat_evidence_correlate(const char *session_id);
#ifdef __cplusplus
extern "C" {
#endif
void anticheat_evidence_record(const char *session_id, int evidence_type, int value);
#ifdef __cplusplus
}
#endif
#endif