#include "anticheat_evidence.h"
#include "../entities/anticheat_session.h"
#include <stdint.h>
#define MAX_EVIDENCE_TYPES 8
typedef struct {
    int type;
    int value;
} EvidenceEntry;
typedef struct {
    EvidenceEntry entries[MAX_EVIDENCE_TYPES];
    int count;
} EvidenceBuffer;
static EvidenceBuffer g_evidence_buffers[256];
void anticheat_evidence_record(const char *session_id, int evidence_type, int value) {
    unsigned hash = 0; for (const char *p = session_id; *p; ++p) hash = hash * 31 + (unsigned char)*p;
    EvidenceBuffer *buf = &g_evidence_buffers[hash % 256];
    if (buf->count < MAX_EVIDENCE_TYPES) {
        buf->entries[buf->count].type = evidence_type;
        buf->entries[buf->count].value = value;
        buf->count++;
    }
}
int anticheat_evidence_correlate(const char *session_id) {
    unsigned hash = 0; for (const char *p = session_id; *p; ++p) hash = hash * 31 + (unsigned char)*p;
    EvidenceBuffer *buf = &g_evidence_buffers[hash % 256];
    int types_seen[MAX_EVIDENCE_TYPES] = {0};
    int distinct = 0;
    for (int i = 0; i < buf->count; ++i) {
        if (!types_seen[buf->entries[i].type]) {
            types_seen[buf->entries[i].type] = 1;
            distinct++;
        }
    }
    return distinct >= 2;
}
