// Session management domain
#ifndef BANANA_ANTICHEAT_SESSION_H
#define BANANA_ANTICHEAT_SESSION_H
#include <stdint.h>
#define BANANA_ANTICHEAT_MAX_SESSIONS 256
#define BANANA_ANTICHEAT_SESSION_ID_MAX 64
typedef struct BananaAnticheatSession {
	int in_use;
	char session_id[BANANA_ANTICHEAT_SESSION_ID_MAX];
	int score;
	int confidence_score;
	int trust_state;
    uint32_t integrity_hash;
} BananaAnticheatSession;
#ifdef __cplusplus
extern "C" {
#endif
BananaAnticheatSession *anticheat_session_find(const char *session_id, int create);
#ifdef __cplusplus
}
#endif
#endif