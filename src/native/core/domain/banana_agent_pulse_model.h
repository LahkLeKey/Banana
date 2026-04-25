#ifndef BANANA_AGENT_PULSE_MODEL_H
#define BANANA_AGENT_PULSE_MODEL_H

#include <stddef.h>

typedef struct BananaAgentPulseLane {
    const char* agent;
    int deterministic_priority;
    int deterministic_snapshots;
    const char* focus;
} BananaAgentPulseLane;

size_t banana_agent_pulse_lane_count(void);
const BananaAgentPulseLane* banana_agent_pulse_lanes(void);
int banana_agent_pulse_find_lane(const char* agent_label, BananaAgentPulseLane* out_lane);

#endif
