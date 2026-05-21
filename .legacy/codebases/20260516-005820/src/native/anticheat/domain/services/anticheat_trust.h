// Trust lifecycle domain (DDD, SOLID)
#ifndef BANANA_ANTICHEAT_TRUST_H
#define BANANA_ANTICHEAT_TRUST_H
#include <stdint.h>
#include "banana_anticheat.h"
BananaTrustState anticheat_trust_get_state(const char *session_id);
void anticheat_trust_update(const char *session_id, int event);
#endif