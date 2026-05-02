#include "banana_agent_pulse_model.h"

#include <string.h>

static const BananaAgentPulseLane BANANA_AGENT_PULSE_LANES[] = {
    {"banana-classifier-agent", 100, 1, "feedback-and-training"},
    {"api-pipeline-agent", 95, 2, "controller-service-pipeline"},
    {"csharp-api-agent", 93, 2, "api-services-and-interop"},
    {"native-core-agent", 91, 2, "native-core-domain"},
    {"react-ui-agent", 89, 2, "react-ui-flows"},
    {"electron-agent", 87, 2, "electron-runtime"},
    {"mobile-runtime-agent", 85, 2, "mobile-runtime"},
    {"integration-agent", 83, 2, "cross-domain-validation"},
    {"workflow-agent", 81, 2, "workflow-governance"}};

static const char *banana_strip_agent_prefix(const char *agent_label)
{
    static const char *prefix = "agent:";
    if (agent_label == 0)
    {
        return 0;
    }

    if (strncmp(agent_label, prefix, 6) == 0)
    {
        return agent_label + 6;
    }

    return agent_label;
}

size_t banana_agent_pulse_lane_count(void)
{
    return sizeof(BANANA_AGENT_PULSE_LANES) / sizeof(BANANA_AGENT_PULSE_LANES[0]);
}

const BananaAgentPulseLane *banana_agent_pulse_lanes(void)
{
    return BANANA_AGENT_PULSE_LANES;
}

int banana_agent_pulse_find_lane(const char *agent_label, BananaAgentPulseLane *out_lane)
{
    const char *normalized = banana_strip_agent_prefix(agent_label);
    size_t index = 0;
    size_t lane_count = banana_agent_pulse_lane_count();

    if (normalized == 0 || out_lane == 0)
    {
        return 0;
    }

    for (index = 0; index < lane_count; index++)
    {
        if (strcmp(normalized, BANANA_AGENT_PULSE_LANES[index].agent) == 0)
        {
            *out_lane = BANANA_AGENT_PULSE_LANES[index];
            return 1;
        }
    }

    return 0;
}
