#include "banana_agent_pulse_model.h"

#include <stdio.h>

int main(void) {
    const BananaAgentPulseLane* lanes = banana_agent_pulse_lanes();
    size_t lane_count = banana_agent_pulse_lane_count();
    size_t index = 0;

    printf("{\n");
    printf("  \"model\": \"banana-agent-pulse-deterministic-v1\",\n");
    printf("  \"lanes\": [\n");

    for (index = 0; index < lane_count; index++) {
        printf("    {\"agent\":\"%s\",\"priority\":%d,\"deterministic_snapshots\":%d,\"focus\":\"%s\"}",
            lanes[index].agent,
            lanes[index].deterministic_priority,
            lanes[index].deterministic_snapshots,
            lanes[index].focus);

        if (index + 1U < lane_count) {
            printf(",");
        }

        printf("\n");
    }

    printf("  ]\n");
    printf("}\n");

    return 0;
}
