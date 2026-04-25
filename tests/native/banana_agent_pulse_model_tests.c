#include <stdio.h>
#include <stdlib.h>

#include "domain/banana_agent_pulse_model.h"

static void require_true(int condition, const char* message) {
    if (!condition) {
        fprintf(stderr, "banana_agent_pulse_model_tests failure: %s\n", message);
        exit(1);
    }
}

static void test_lane_sequence_is_deterministic(void) {
    const BananaAgentPulseLane* lanes = banana_agent_pulse_lanes();
    size_t lane_count = banana_agent_pulse_lane_count();
    size_t index = 0;

    require_true(lanes != 0, "expected non-null lane table");
    require_true(lane_count == 9, "expected exactly nine deterministic agent lanes");

    for (index = 0; index < lane_count; index++) {
        require_true(lanes[index].agent != 0, "expected non-null agent name");
        require_true(lanes[index].focus != 0, "expected non-null focus");
        require_true(lanes[index].deterministic_priority > 0, "expected positive deterministic priority");
        require_true(lanes[index].deterministic_snapshots > 0, "expected positive deterministic snapshot count");

        if (index > 0) {
            require_true(
                lanes[index - 1].deterministic_priority > lanes[index].deterministic_priority,
                "expected deterministic priorities to be strictly descending");
        }
    }
}

static void test_find_lane_supports_agent_prefixed_labels(void) {
    BananaAgentPulseLane lane;
    int found = banana_agent_pulse_find_lane("agent:react-ui-agent", &lane);

    require_true(found == 1, "expected lane lookup success for prefixed label");
    require_true(lane.deterministic_priority == 89, "expected deterministic priority for react-ui-agent");
    require_true(lane.deterministic_snapshots == 2, "expected deterministic snapshots for react-ui-agent");
}

static void test_find_lane_rejects_unknown_agent(void) {
    BananaAgentPulseLane lane;
    int found = banana_agent_pulse_find_lane("agent:unknown-agent", &lane);

    require_true(found == 0, "expected lane lookup miss for unknown agent");
}

int main(void) {
    test_lane_sequence_is_deterministic();
    test_find_lane_supports_agent_prefixed_labels();
    test_find_lane_rejects_unknown_agent();

    printf("banana_agent_pulse_model_tests passed\n");
    return 0;
}
