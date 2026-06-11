#include "netcode_link.h"

#include <math.h>

static int clamp_percent(int value)
{
    if (value < 0) return 0;
    if (value > 100) return 100;
    return value;
}

int runtime_netcode_link_build(const RuntimeNetcodeLinkInput *input,
                               RuntimeNetcodeLinkOutput *out_output)
{
    int interaction_signal;
    float player_signal;

    if (!input || !out_output)
        return -1;

    interaction_signal = clamp_percent(input->interaction_signal);
    player_signal = (float)input->player_level * 14.0f +
                    (float)(input->combo_streak < 1 ? 1 : input->combo_streak) * 8.0f +
                    (float)interaction_signal * 0.06f;

    out_output->intel = clamp_percent((int)lroundf((float)input->call_density * 1.02f +
                                                   (float)interaction_signal * 0.08f));
    out_output->objectives = clamp_percent((int)lroundf((float)input->quest_percent * 1.04f +
                                                        (float)interaction_signal * 0.10f));
    out_output->player = clamp_percent((int)lroundf(player_signal));
    out_output->ops = clamp_percent((int)lroundf(((float)input->branch_pressure +
                                                  (float)input->dependency_pulse) / 2.0f +
                                                 (float)interaction_signal * 0.10f));

    return 0;
}
