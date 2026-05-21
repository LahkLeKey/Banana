// Scoring and confidence domain
#ifndef BANANA_ANTICHEAT_SCORING_H
#define BANANA_ANTICHEAT_SCORING_H
#include <stdint.h>
#include "banana_anticheat.h"
void anticheat_scoring_update(const char *session_id, int risk_delta, int confidence_delta);
BananaRiskConfidence anticheat_scoring_get(const char *session_id);
#endif