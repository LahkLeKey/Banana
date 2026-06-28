#include "netcode_link.h"

#include <math.h>
#include <stddef.h>

static int clamp_percent(int value)
{
    if (value < 0) return 0;
    if (value > 100) return 100;
    return value;
}

int runtime_netcode_link_build(const RuntimeNetcodeLinkInput *input,
                               RuntimeNetcodeLinkOutput *out_output)
{
    RuntimeNetcodeLinkInput fallback_input;
    RuntimeNetcodeLinkOutput fallback_output;
    const RuntimeNetcodeLinkInput *effective_input = NULL;
    RuntimeNetcodeLinkOutput *effective_output = NULL;
    int inputs_valid = 0;
    int interaction_signal;
    float player_signal;

    fallback_input.call_density = 0;
    fallback_input.quest_percent = 0;
    fallback_input.player_level = 0;
    fallback_input.combo_streak = 0;
    fallback_input.branch_pressure = 0;
    fallback_input.dependency_pulse = 0;
    fallback_input.interaction_signal = 0;

    fallback_output.intel = 0;
    fallback_output.objectives = 0;
    fallback_output.player = 0;
    fallback_output.ops = 0;

    inputs_valid = (input != NULL && out_output != NULL);
    effective_input = input ? input : &fallback_input;
    effective_output = inputs_valid ? out_output : &fallback_output;
    interaction_signal = clamp_percent(effective_input->interaction_signal);
    player_signal = (float)effective_input->player_level * 14.0f +
                    (float)(effective_input->combo_streak < 1 ? 1 : effective_input->combo_streak) * 8.0f +
                    (float)interaction_signal * 0.06f;

    effective_output->intel = clamp_percent((int)lroundf((float)effective_input->call_density * 1.02f +
                                                   (float)interaction_signal * 0.08f));
    effective_output->objectives = clamp_percent((int)lroundf((float)effective_input->quest_percent * 1.04f +
                                                        (float)interaction_signal * 0.10f));
    effective_output->player = clamp_percent((int)lroundf(player_signal));
    effective_output->ops = clamp_percent((int)lroundf(((float)effective_input->branch_pressure +
                                                  (float)effective_input->dependency_pulse) / 2.0f +
                                                 (float)interaction_signal * 0.10f));

    return inputs_valid ? 0 : -1;
}
