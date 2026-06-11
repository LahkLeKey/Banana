#include "runtime/engine/gameplay/service/gameplay_service.h"
#include "runtime/engine/state/engine_state.h"
#include "runtime/controller/attach/controller_attach.h"
#include "ai/combat_controller.h"
#include "ai/wildlife_controller.h"
#include "banana_native_v3.h"

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define setenv(name, value, overwrite) _putenv_s((name), (value))
#define unsetenv(name) _putenv_s((name), "")
#endif

typedef enum FeedbackScenarioKind
{
    FEEDBACK_SCENARIO_WARFRONT = 0,
    FEEDBACK_SCENARIO_NEGOTIATE = 1,
    FEEDBACK_SCENARIO_COMEBACK = 2,
} FeedbackScenarioKind;

typedef enum FeedbackComparisonOp
{
    FEEDBACK_COMPARE_EQ = 0,
    FEEDBACK_COMPARE_NE = 1,
    FEEDBACK_COMPARE_GTE = 2,
    FEEDBACK_COMPARE_LTE = 3,
    FEEDBACK_COMPARE_GT = 4,
    FEEDBACK_COMPARE_LT = 5,
} FeedbackComparisonOp;

typedef struct FeedbackLoopConfig
{
    const char *scenario_name;
    FeedbackScenarioKind scenario_kind;
    int ticks;
    int snapshot_interval;
    float war_radius;
    int reinforcements_per_tick;
    int population_cap;
    int escalation_tier;
    int intelligence_stage;
    int biome_stage;
    const char *script_path;
    const char *output_path;
    const char *feedback_note;
    int non_interactive;
} FeedbackLoopConfig;

typedef struct FeedbackLoopContext
{
    World *world;
    ControllerInstance *controllers[256];
    int max_controllers;
    int controller_count;
    EntityId player_id;
    int pickup_collected;
    EngineRuntimeState telemetry;
} FeedbackLoopContext;

typedef struct Dx12PlayloopRunner
{
    FeedbackLoopContext context;
    FeedbackLoopConfig config;
    FILE *artifact;
    int page_loaded;
    int tick_cursor;
    int expect_passed;
    int expect_failed;
    int scaffold_last_status;
    int scaffold_abi_version;
    int scaffold_chunk_estimate;
    int scaffold_chunk_serialized_bytes;
    banana_native_v3_launch_gate_decision scaffold_launch_decision;
    int scaffold_launch_decision_valid;
    banana_native_ui_frame scaffold_ui_frame;
    int scaffold_ui_frame_valid;
    char active_scenario_name[32];
} Dx12PlayloopRunner;

static void print_usage(const char *program)
{
    printf("Usage: %s [options]\n", program);
    printf("  --scenario <warfront|negotiate|comeback|flank|pressure|truce|rally>\n");
    printf("  --ticks <count>\n");
    printf("  --snapshot-interval <count>\n");
    printf("  --war-radius <float>\n");
    printf("  --reinforcements <count>\n");
    printf("  --population-cap <count>\n");
    printf("  --escalation-tier <count>\n");
    printf("  --intelligence-stage <count>\n");
    printf("  --biome-stage <count>\n");
    printf("  --script <path>\n");
    printf("  --output <path>\n");
    printf("  --feedback-note <text>\n");
    printf("  --non-interactive\n");
    printf("  --list-scenarios\n");
    printf("  --list-actions\n");
    printf("  --help\n");
}

static void print_scenarios(void)
{
    printf("Available feedback scenarios:\n");
    printf("  warfront   - symmetric lane pressure between banana and bean teams\n");
    printf("  flank      - warfront variant focused on flank behavior\n");
    printf("  pressure   - warfront variant emphasizing reinforcement pressure\n");
    printf("  negotiate  - high empathy/coordination negotiation lane with parity injections\n");
    printf("  truce      - negotiate variant with neutral observers present\n");
    printf("  comeback   - banana disadvantage scenario to inspect comeback bias and recovery\n");
    printf("  rally      - comeback variant verifying regroup recovery\n");
}

static void print_script_actions(void)
{
    printf("DX12 playloop script actions (Playwright-style):\n");
    printf("  test.step <label...>\n");
    printf("  test.use <dx12-client-scaffold>\n");
    printf("  page.goto <scenario>\n");
    printf("  page.reload\n");
    printf("  page.waitForTicks <count>\n");
    printf("  engine.config <war-radius|reinforcements|population-cap|escalation-tier|intelligence-stage|biome-stage|snapshot-interval> <value>\n");
    printf("  actor.spawn <banana|bean|neutral> <combat|wildlife> <x> <z>\n");
    printf("  state.sentience <humanoidIndex> <coordination> <empathy>\n");
    printf("  page.tick <count>\n");
    printf("  client.ping\n");
    printf("  client.world.init <seed> <cacheSize>\n");
    printf("  client.world.chunk-estimate <objectCount>\n");
    printf("  client.world.serialize <chunkX> <chunkZ> <bufferLen>\n");
    printf("  client.world.cleanup\n");
    printf("  client.launch.decide <mode> <steam> <linked> <goodStanding> <fresh> <available>\n");
    printf("  client.ui.reset\n");
    printf("  client.ui.write <host> <engineStatus> <movement> <moveX> <moveZ> <cssW> <cssH> <pixelW> <pixelH> <dpr> <timestampMs>\n");
    printf("  snapshot <label>\n");
    printf("  expect.toHaveMode <banana|bean> <hold-line|flank|regroup|negotiate>\n");
    printf("  expect.toHaveMetric <name> <eq|ne|gt|gte|lt|lte> <value>\n");
    printf("  expect.toHaveTeamCount <banana|bean|neutral> <eq|ne|gt|gte|lt|lte> <value>\n");
    printf("  expect.mode <banana|bean> <hold-line|flank|regroup|negotiate>\n");
    printf("  expect.metric <name> <eq|ne|gt|gte|lt|lte> <value>\n");
    printf("  expect.team-count <banana|bean|neutral> <eq|ne|gt|gte|lt|lte> <value>\n");
    printf("  note <text>\n");
}

static void trim_trailing_newline(char *line)
{
    size_t length = 0;

    if (!line)
        return;

    length = strlen(line);
    while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r'))
    {
        line[length - 1] = '\0';
        length--;
    }
}

static int parse_int_arg(const char *raw, int *out_value)
{
    char *end = NULL;
    long parsed = 0;

    if (!raw || !out_value)
        return 0;

    parsed = strtol(raw, &end, 10);
    if (raw == end || (end && *end != '\0'))
        return 0;

    if (parsed < INT_MIN || parsed > INT_MAX)
        return 0;

    *out_value = (int)parsed;
    return 1;
}

static int parse_float_arg(const char *raw, float *out_value)
{
    char *end = NULL;
    float parsed = 0.0f;

    if (!raw || !out_value)
        return 0;

    parsed = strtof(raw, &end);
    if (raw == end || (end && *end != '\0'))
        return 0;

    *out_value = parsed;
    return 1;
}

static int parse_uint32_arg(const char *raw, uint32_t *out_value)
{
    char *end = NULL;
    unsigned long parsed = 0;

    if (!raw || !out_value)
        return 0;

    parsed = strtoul(raw, &end, 10);
    if (raw == end || (end && *end != '\0'))
        return 0;

    if (parsed > UINT32_MAX)
        return 0;

    *out_value = (uint32_t)parsed;
    return 1;
}

static int parse_int64_arg(const char *raw, int64_t *out_value)
{
    char *end = NULL;
    long long parsed = 0;

    if (!raw || !out_value)
        return 0;

    parsed = strtoll(raw, &end, 10);
    if (raw == end || (end && *end != '\0'))
        return 0;

    *out_value = (int64_t)parsed;
    return 1;
}

static int resolve_scenario(const char *scenario_name, FeedbackScenarioKind *out_kind)
{
    if (!scenario_name || !out_kind)
        return 0;

    if (strcmp(scenario_name, "warfront") == 0)
    {
        *out_kind = FEEDBACK_SCENARIO_WARFRONT;
        return 1;
    }

    if (strcmp(scenario_name, "flank") == 0 || strcmp(scenario_name, "pressure") == 0)
    {
        *out_kind = FEEDBACK_SCENARIO_WARFRONT;
        return 1;
    }

    if (strcmp(scenario_name, "negotiate") == 0)
    {
        *out_kind = FEEDBACK_SCENARIO_NEGOTIATE;
        return 1;
    }

    if (strcmp(scenario_name, "truce") == 0)
    {
        *out_kind = FEEDBACK_SCENARIO_NEGOTIATE;
        return 1;
    }

    if (strcmp(scenario_name, "comeback") == 0)
    {
        *out_kind = FEEDBACK_SCENARIO_COMEBACK;
        return 1;
    }

    if (strcmp(scenario_name, "rally") == 0)
    {
        *out_kind = FEEDBACK_SCENARIO_COMEBACK;
        return 1;
    }

    return 0;
}

static int parse_args(int argc, char **argv, FeedbackLoopConfig *config)
{
    int i = 0;

    if (!config)
        return 0;

    memset(config, 0, sizeof(*config));
    config->scenario_name = "warfront";
    config->ticks = 24;
    config->snapshot_interval = 4;
    config->war_radius = 6.0f;
    config->reinforcements_per_tick = 2;
    config->population_cap = 128;
    config->escalation_tier = 1;
    config->intelligence_stage = 4;
    config->biome_stage = 0;

    for (i = 1; i < argc; i++)
    {
        const char *arg = argv[i];

        if (strcmp(arg, "--scenario") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "missing value for --scenario\n");
                return 0;
            }
            config->scenario_name = argv[++i];
            continue;
        }

        if (strcmp(arg, "--ticks") == 0)
        {
            if (i + 1 >= argc || !parse_int_arg(argv[++i], &config->ticks))
            {
                fprintf(stderr, "invalid value for --ticks\n");
                return 0;
            }
            continue;
        }

        if (strcmp(arg, "--snapshot-interval") == 0)
        {
            if (i + 1 >= argc || !parse_int_arg(argv[++i], &config->snapshot_interval))
            {
                fprintf(stderr, "invalid value for --snapshot-interval\n");
                return 0;
            }
            continue;
        }

        if (strcmp(arg, "--war-radius") == 0)
        {
            if (i + 1 >= argc || !parse_float_arg(argv[++i], &config->war_radius))
            {
                fprintf(stderr, "invalid value for --war-radius\n");
                return 0;
            }
            continue;
        }

        if (strcmp(arg, "--reinforcements") == 0)
        {
            if (i + 1 >= argc || !parse_int_arg(argv[++i], &config->reinforcements_per_tick))
            {
                fprintf(stderr, "invalid value for --reinforcements\n");
                return 0;
            }
            continue;
        }

        if (strcmp(arg, "--population-cap") == 0)
        {
            if (i + 1 >= argc || !parse_int_arg(argv[++i], &config->population_cap))
            {
                fprintf(stderr, "invalid value for --population-cap\n");
                return 0;
            }
            continue;
        }

        if (strcmp(arg, "--escalation-tier") == 0)
        {
            if (i + 1 >= argc || !parse_int_arg(argv[++i], &config->escalation_tier))
            {
                fprintf(stderr, "invalid value for --escalation-tier\n");
                return 0;
            }
            continue;
        }

        if (strcmp(arg, "--intelligence-stage") == 0)
        {
            if (i + 1 >= argc || !parse_int_arg(argv[++i], &config->intelligence_stage))
            {
                fprintf(stderr, "invalid value for --intelligence-stage\n");
                return 0;
            }
            continue;
        }

        if (strcmp(arg, "--biome-stage") == 0)
        {
            if (i + 1 >= argc || !parse_int_arg(argv[++i], &config->biome_stage))
            {
                fprintf(stderr, "invalid value for --biome-stage\n");
                return 0;
            }
            continue;
        }

        if (strcmp(arg, "--script") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "missing value for --script\n");
                return 0;
            }
            config->script_path = argv[++i];
            continue;
        }

        if (strcmp(arg, "--output") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "missing value for --output\n");
                return 0;
            }
            config->output_path = argv[++i];
            continue;
        }

        if (strcmp(arg, "--feedback-note") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "missing value for --feedback-note\n");
                return 0;
            }
            config->feedback_note = argv[++i];
            continue;
        }

        if (strcmp(arg, "--non-interactive") == 0)
        {
            config->non_interactive = 1;
            continue;
        }

        if (strcmp(arg, "--list-scenarios") == 0)
        {
            print_scenarios();
            return 2;
        }

        if (strcmp(arg, "--list-actions") == 0)
        {
            print_script_actions();
            return 2;
        }

        if (strcmp(arg, "--help") == 0)
        {
            print_usage(argv[0]);
            return 2;
        }

        fprintf(stderr, "unknown argument: %s\n", arg);
        return 0;
    }

    if (!resolve_scenario(config->scenario_name, &config->scenario_kind))
    {
        fprintf(stderr, "unknown scenario: %s\n", config->scenario_name);
        return 0;
    }

    if (config->ticks <= 0)
    {
        fprintf(stderr, "--ticks must be > 0\n");
        return 0;
    }

    if (config->snapshot_interval <= 0)
    {
        fprintf(stderr, "--snapshot-interval must be > 0\n");
        return 0;
    }

    if (config->reinforcements_per_tick <= 0)
    {
        fprintf(stderr, "--reinforcements must be > 0\n");
        return 0;
    }

    if (config->population_cap <= 0)
    {
        fprintf(stderr, "--population-cap must be > 0\n");
        return 0;
    }

    return 1;
}

static void feedback_log(FILE *artifact, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);

    if (artifact)
    {
        va_start(args, fmt);
        vfprintf(artifact, fmt, args);
        va_end(args);
    }
}

static int feedback_loop_context_init(FeedbackLoopContext *context, float player_x, float player_z)
{
    if (!context)
        return 0;

    memset(context, 0, sizeof(*context));

    context->max_controllers = (int)(sizeof(context->controllers) / sizeof(context->controllers[0]));
    context->world = world_create();
    context->pickup_collected = 1;

    if (!context->world)
    {
        fprintf(stderr, "unable to create world for feedback loop\n");
        return 0;
    }

    wildlife_controller_register();
    combat_controller_register();

    context->player_id = world_spawn_entity(context->world, ENTITY_TYPE_PLAYER, player_x, 0.0f, player_z);
    if (context->player_id == 0)
    {
        fprintf(stderr, "unable to create player anchor for feedback loop\n");
        return 0;
    }

    return 1;
}

static void feedback_loop_context_destroy(FeedbackLoopContext *context)
{
    int i = 0;

    if (!context)
        return;

    for (i = 0; i < context->controller_count; i++)
    {
        if (context->controllers[i])
            controller_destroy(context->controllers[i]);
    }

    if (context->world)
        world_destroy(context->world);

    memset(context, 0, sizeof(*context));
}

static int feedback_loop_spawn_controller(FeedbackLoopContext *context,
                                          ControllerTeam team,
                                          const char *controller_kind,
                                          float x,
                                          float z,
                                          EntityId *out_entity_id)
{
    EntityId entity_id = 0;
    Entity *entity = NULL;

    if (!context || !context->world || !controller_kind)
        return 0;

    entity_id = world_spawn_entity(context->world, ENTITY_TYPE_NPC, x, 0.0f, z);
    if (entity_id == 0)
        return 0;

    if (runtime_controller_attach_to_entity_with_team(context->world,
                                                      context->controllers,
                                                      context->max_controllers,
                                                      &context->controller_count,
                                                      entity_id,
                                                      controller_kind,
                                                      team) == 0u)
    {
        return 0;
    }

    entity = world_get_entity(context->world, entity_id);
    if (entity)
    {
        strncpy(entity->controller_kind, controller_kind, sizeof(entity->controller_kind) - 1);
        entity->controller_kind[sizeof(entity->controller_kind) - 1] = '\0';
    }

    if (out_entity_id)
        *out_entity_id = entity_id;

    return 1;
}

static int seed_scenario(FeedbackLoopContext *context, FeedbackScenarioKind scenario_kind)
{
    if (!context)
        return 0;

    context->telemetry.war_sentience_humanoid_index = 10;
    context->telemetry.war_sentience_coordination_level = 5;
    context->telemetry.war_sentience_empathy_level = 4;

    if (scenario_kind == FEEDBACK_SCENARIO_WARFRONT)
    {
        if (!feedback_loop_spawn_controller(context, CONTROLLER_TEAM_BANANA, "combat", 0.0f, 0.0f, NULL))
            return 0;
        if (!feedback_loop_spawn_controller(context, CONTROLLER_TEAM_BEAN, "wildlife", 2.0f, 0.0f, NULL))
            return 0;
        return 1;
    }

    if (scenario_kind == FEEDBACK_SCENARIO_NEGOTIATE)
    {
        context->telemetry.war_sentience_humanoid_index = 14;
        context->telemetry.war_sentience_coordination_level = 9;
        context->telemetry.war_sentience_empathy_level = 9;

        if (!feedback_loop_spawn_controller(context, CONTROLLER_TEAM_BANANA, "combat", 0.0f, 0.0f, NULL))
            return 0;
        if (!feedback_loop_spawn_controller(context, CONTROLLER_TEAM_BEAN, "wildlife", 2.0f, 0.0f, NULL))
            return 0;
        if (!feedback_loop_spawn_controller(context, CONTROLLER_TEAM_BEAN, "wildlife", 3.0f, 0.25f, NULL))
            return 0;
        return 1;
    }

    if (scenario_kind == FEEDBACK_SCENARIO_COMEBACK)
    {
        context->telemetry.war_sentience_humanoid_index = 11;
        context->telemetry.war_sentience_coordination_level = 4;
        context->telemetry.war_sentience_empathy_level = 2;

        if (!feedback_loop_spawn_controller(context, CONTROLLER_TEAM_BANANA, "combat", 0.0f, 0.0f, NULL))
            return 0;
        if (!feedback_loop_spawn_controller(context, CONTROLLER_TEAM_BEAN, "wildlife", 2.0f, 0.0f, NULL))
            return 0;
        if (!feedback_loop_spawn_controller(context, CONTROLLER_TEAM_BEAN, "wildlife", 3.0f, -0.25f, NULL))
            return 0;
        if (!feedback_loop_spawn_controller(context, CONTROLLER_TEAM_BEAN, "wildlife", 3.5f, 0.35f, NULL))
            return 0;
        return 1;
    }

    return 0;
}

static int count_controllers_for_team(const FeedbackLoopContext *context, ControllerTeam team)
{
    int count = 0;
    int i = 0;

    if (!context)
        return 0;

    for (i = 0; i < context->controller_count; i++)
    {
        if (context->controllers[i] && context->controllers[i]->team == team)
            count++;
    }

    return count;
}

static const char *mode_label(RuntimeWarSentienceBehaviorMode mode)
{
    switch (mode)
    {
    case RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE:
        return "hold-line";
    case RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK:
        return "flank";
    case RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP:
        return "regroup";
    case RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE:
        return "negotiate";
    default:
        return "unknown";
    }
}

static int resolve_mode(const char *raw, RuntimeWarSentienceBehaviorMode *out_mode)
{
    if (!raw || !out_mode)
        return 0;

    if (strcmp(raw, "hold-line") == 0 || strcmp(raw, "holdline") == 0 || strcmp(raw, "hold_line") == 0)
    {
        *out_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
        return 1;
    }

    if (strcmp(raw, "flank") == 0)
    {
        *out_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK;
        return 1;
    }

    if (strcmp(raw, "regroup") == 0)
    {
        *out_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP;
        return 1;
    }

    if (strcmp(raw, "negotiate") == 0)
    {
        *out_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE;
        return 1;
    }

    return 0;
}

static int resolve_team(const char *raw, ControllerTeam *out_team)
{
    if (!raw || !out_team)
        return 0;

    if (strcmp(raw, "banana") == 0)
    {
        *out_team = CONTROLLER_TEAM_BANANA;
        return 1;
    }

    if (strcmp(raw, "bean") == 0)
    {
        *out_team = CONTROLLER_TEAM_BEAN;
        return 1;
    }

    if (strcmp(raw, "neutral") == 0)
    {
        *out_team = CONTROLLER_TEAM_NEUTRAL;
        return 1;
    }

    return 0;
}

static int parse_flag_arg(const char *raw, int *out_value)
{
    int parsed = 0;

    if (!raw || !out_value)
        return 0;

    if (strcmp(raw, "true") == 0 || strcmp(raw, "on") == 0 || strcmp(raw, "yes") == 0)
    {
        *out_value = 1;
        return 1;
    }

    if (strcmp(raw, "false") == 0 || strcmp(raw, "off") == 0 || strcmp(raw, "no") == 0)
    {
        *out_value = 0;
        return 1;
    }

    if (!parse_int_arg(raw, &parsed))
        return 0;

    if (parsed != 0 && parsed != 1)
        return 0;

    *out_value = parsed;
    return 1;
}

static int resolve_ui_host(const char *raw, banana_native_ui_host_kind *out_host)
{
    if (!raw || !out_host)
        return 0;

    if (strcmp(raw, "web") == 0)
    {
        *out_host = BANANA_NATIVE_UI_HOST_WEB;
        return 1;
    }

    if (strcmp(raw, "react-native") == 0 || strcmp(raw, "react_native") == 0)
    {
        *out_host = BANANA_NATIVE_UI_HOST_REACT_NATIVE;
        return 1;
    }

    if (strcmp(raw, "native") == 0 || strcmp(raw, "dx12") == 0)
    {
        *out_host = BANANA_NATIVE_UI_HOST_NATIVE;
        return 1;
    }

    return 0;
}

static int resolve_ui_engine_status(const char *raw, banana_native_ui_engine_status *out_status)
{
    if (!raw || !out_status)
        return 0;

    if (strcmp(raw, "idle") == 0)
    {
        *out_status = BANANA_NATIVE_UI_STATUS_IDLE;
        return 1;
    }

    if (strcmp(raw, "loading") == 0)
    {
        *out_status = BANANA_NATIVE_UI_STATUS_LOADING;
        return 1;
    }

    if (strcmp(raw, "running") == 0)
    {
        *out_status = BANANA_NATIVE_UI_STATUS_RUNNING;
        return 1;
    }

    if (strcmp(raw, "error") == 0)
    {
        *out_status = BANANA_NATIVE_UI_STATUS_ERROR;
        return 1;
    }

    if (strcmp(raw, "unavailable") == 0)
    {
        *out_status = BANANA_NATIVE_UI_STATUS_UNAVAILABLE;
        return 1;
    }

    return 0;
}

static int resolve_ui_movement_source(const char *raw, banana_native_ui_movement_source *out_movement)
{
    if (!raw || !out_movement)
        return 0;

    if (strcmp(raw, "none") == 0)
    {
        *out_movement = BANANA_NATIVE_UI_MOVEMENT_NONE;
        return 1;
    }

    if (strcmp(raw, "keyboard") == 0)
    {
        *out_movement = BANANA_NATIVE_UI_MOVEMENT_KEYBOARD;
        return 1;
    }

    if (strcmp(raw, "radial") == 0)
    {
        *out_movement = BANANA_NATIVE_UI_MOVEMENT_RADIAL;
        return 1;
    }

    return 0;
}

static void emit_snapshot(FILE *stream,
                          const char *scenario_name,
                          const char *label,
                          const FeedbackLoopContext *context,
                          int tick)
{
    if (!stream || !scenario_name || !context)
        return;

    fprintf(stream,
            "tick=%d scenario=%s label=%s entities=%d controllers=%d banana=%d bean=%d "
            "bananaMode=%s beanMode=%s negotiateStreak=%d trim=%d comebackBonus=%d "
            "bananaHits={hold:%d flank:%d regroup:%d negotiate:%d} "
            "beanHits={hold:%d flank:%d regroup:%d negotiate:%d} "
            "truceHits={total:%d banana:%d bean:%d base:%d apex:%d mythic:%d} "
            "truceGate={checks:%d ok:%d stageChecks:%d stageOk:%d behavior:%d empathy:%d coord:%d streak:%d intel:%d}\n",
            tick,
            scenario_name,
            label ? label : "-",
            context->world ? context->world->entity_count : 0,
            context->controller_count,
            count_controllers_for_team(context, CONTROLLER_TEAM_BANANA),
            count_controllers_for_team(context, CONTROLLER_TEAM_BEAN),
            mode_label(context->telemetry.war_sentience_behavior_banana),
            mode_label(context->telemetry.war_sentience_behavior_bean),
            context->telemetry.war_sentience_negotiate_streak_ticks,
            context->telemetry.war_sentience_negotiate_deescalation_trim_last_tick,
            context->telemetry.war_sentience_comeback_bonus_last_tick,
            context->telemetry.war_sentience_spawn_mode_hits_banana[RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE],
            context->telemetry.war_sentience_spawn_mode_hits_banana[RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK],
            context->telemetry.war_sentience_spawn_mode_hits_banana[RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP],
            context->telemetry.war_sentience_spawn_mode_hits_banana[RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE],
            context->telemetry.war_sentience_spawn_mode_hits_bean[RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE],
            context->telemetry.war_sentience_spawn_mode_hits_bean[RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK],
            context->telemetry.war_sentience_spawn_mode_hits_bean[RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP],
            context->telemetry.war_sentience_spawn_mode_hits_bean[RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE],
            context->telemetry.war_sentience_truce_variant_hits_total,
            context->telemetry.war_sentience_truce_variant_hits_banana,
            context->telemetry.war_sentience_truce_variant_hits_bean,
            context->telemetry.war_sentience_truce_variant_hits_base,
            context->telemetry.war_sentience_truce_variant_hits_apex,
            context->telemetry.war_sentience_truce_variant_hits_mythic,
            context->telemetry.war_sentience_truce_gate_checks_total,
            context->telemetry.war_sentience_truce_gate_checks_granted,
                 context->telemetry.war_sentience_truce_gate_checks_stage[context->telemetry.war_intelligence_stage < 0
                                                   ? 0
                                                   : (context->telemetry.war_intelligence_stage >= BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS
                                                       ? BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS - 1
                                                       : context->telemetry.war_intelligence_stage)],
                 context->telemetry.war_sentience_truce_gate_granted_stage[context->telemetry.war_intelligence_stage < 0
                                                     ? 0
                                                     : (context->telemetry.war_intelligence_stage >= BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS
                                                         ? BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS - 1
                                                         : context->telemetry.war_intelligence_stage)],
            context->telemetry.war_sentience_truce_gate_block_behavior,
            context->telemetry.war_sentience_truce_gate_block_empathy,
            context->telemetry.war_sentience_truce_gate_block_coordination,
            context->telemetry.war_sentience_truce_gate_block_streak,
            context->telemetry.war_sentience_truce_gate_block_intelligence);
}

static void emit_snapshot_dual(Dx12PlayloopRunner *runner, const char *label)
{
    if (!runner || !runner->page_loaded)
        return;

    emit_snapshot(stdout,
                  runner->active_scenario_name,
                  label,
                  &runner->context,
                  runner->tick_cursor);

    if (runner->artifact)
    {
        emit_snapshot(runner->artifact,
                      runner->active_scenario_name,
                      label,
                      &runner->context,
                      runner->tick_cursor);
    }
}

static void feedback_loop_tick_once(FeedbackLoopContext *context, const FeedbackLoopConfig *config)
{
    if (!context || !config)
        return;

    runtime_gameplay_service_tick(context->world,
                                  &context->telemetry,
                                  context->controllers,
                                  context->max_controllers,
                                  &context->controller_count,
                                  context->player_id,
                                  &context->pickup_collected,
                                  0.0f,
                                  1.0f,
                                  config->war_radius,
                                  config->reinforcements_per_tick,
                                  config->population_cap,
                                  config->escalation_tier,
                                  config->intelligence_stage,
                                  config->biome_stage);
}

static int resolve_comparison(const char *raw, FeedbackComparisonOp *out_op)
{
    if (!raw || !out_op)
        return 0;

    if (strcmp(raw, "eq") == 0 || strcmp(raw, "=") == 0 || strcmp(raw, "==") == 0)
    {
        *out_op = FEEDBACK_COMPARE_EQ;
        return 1;
    }

    if (strcmp(raw, "ne") == 0 || strcmp(raw, "!=") == 0)
    {
        *out_op = FEEDBACK_COMPARE_NE;
        return 1;
    }

    if (strcmp(raw, "gte") == 0 || strcmp(raw, ">=") == 0)
    {
        *out_op = FEEDBACK_COMPARE_GTE;
        return 1;
    }

    if (strcmp(raw, "lte") == 0 || strcmp(raw, "<=") == 0)
    {
        *out_op = FEEDBACK_COMPARE_LTE;
        return 1;
    }

    if (strcmp(raw, "gt") == 0 || strcmp(raw, ">") == 0)
    {
        *out_op = FEEDBACK_COMPARE_GT;
        return 1;
    }

    if (strcmp(raw, "lt") == 0 || strcmp(raw, "<") == 0)
    {
        *out_op = FEEDBACK_COMPARE_LT;
        return 1;
    }

    return 0;
}

static int compare_values(int actual, FeedbackComparisonOp op, int expected)
{
    switch (op)
    {
    case FEEDBACK_COMPARE_EQ:
        return actual == expected;
    case FEEDBACK_COMPARE_NE:
        return actual != expected;
    case FEEDBACK_COMPARE_GTE:
        return actual >= expected;
    case FEEDBACK_COMPARE_LTE:
        return actual <= expected;
    case FEEDBACK_COMPARE_GT:
        return actual > expected;
    case FEEDBACK_COMPARE_LT:
        return actual < expected;
    default:
        return 0;
    }
}

static int read_metric_value(const Dx12PlayloopRunner *runner, const char *metric_name, int *out_value)
{
    if (!runner || !metric_name || !out_value)
        return 0;

    if (strcmp(metric_name, "ticks") == 0)
    {
        *out_value = runner->tick_cursor;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-last-status") == 0)
    {
        *out_value = runner->scaffold_last_status;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-abi-version") == 0)
    {
        *out_value = runner->scaffold_abi_version;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-chunk-estimate") == 0)
    {
        *out_value = runner->scaffold_chunk_estimate;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-chunk-serialized") == 0)
    {
        *out_value = runner->scaffold_chunk_serialized_bytes;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-launch-decided") == 0)
    {
        *out_value = runner->scaffold_launch_decision_valid;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-launch-allow") == 0)
    {
        *out_value = runner->scaffold_launch_decision.allow;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-launch-reason") == 0)
    {
        *out_value = (int)runner->scaffold_launch_decision.reason_code;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-launch-remediable") == 0)
    {
        *out_value = runner->scaffold_launch_decision.is_remediable;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-ui-written") == 0)
    {
        *out_value = runner->scaffold_ui_frame_valid;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-ui-host") == 0)
    {
        *out_value = runner->scaffold_ui_frame.host;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-ui-status") == 0)
    {
        *out_value = runner->scaffold_ui_frame.engine_status;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-ui-movement") == 0)
    {
        *out_value = runner->scaffold_ui_frame.movement_source;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-ui-css-width") == 0)
    {
        *out_value = runner->scaffold_ui_frame.viewport.css_width;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-ui-css-height") == 0)
    {
        *out_value = runner->scaffold_ui_frame.viewport.css_height;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-ui-pixel-width") == 0)
    {
        *out_value = runner->scaffold_ui_frame.viewport.pixel_width;
        return 1;
    }

    if (strcmp(metric_name, "scaffold-ui-pixel-height") == 0)
    {
        *out_value = runner->scaffold_ui_frame.viewport.pixel_height;
        return 1;
    }

    if (!runner->page_loaded)
        return 0;

    if (strcmp(metric_name, "entities") == 0)
    {
        *out_value = runner->context.world ? runner->context.world->entity_count : 0;
        return 1;
    }

    if (strcmp(metric_name, "controllers") == 0)
    {
        *out_value = runner->context.controller_count;
        return 1;
    }

    if (strcmp(metric_name, "banana-count") == 0)
    {
        *out_value = count_controllers_for_team(&runner->context, CONTROLLER_TEAM_BANANA);
        return 1;
    }

    if (strcmp(metric_name, "bean-count") == 0)
    {
        *out_value = count_controllers_for_team(&runner->context, CONTROLLER_TEAM_BEAN);
        return 1;
    }

    if (strcmp(metric_name, "neutral-count") == 0)
    {
        *out_value = count_controllers_for_team(&runner->context, CONTROLLER_TEAM_NEUTRAL);
        return 1;
    }

    if (strcmp(metric_name, "negotiate-streak") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_negotiate_streak_ticks;
        return 1;
    }

    if (strcmp(metric_name, "trim") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_negotiate_deescalation_trim_last_tick;
        return 1;
    }

    if (strcmp(metric_name, "comeback-bonus") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_comeback_bonus_last_tick;
        return 1;
    }

    if (strcmp(metric_name, "banana-negotiate-hits") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_spawn_mode_hits_banana[RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE];
        return 1;
    }

    if (strcmp(metric_name, "bean-negotiate-hits") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_spawn_mode_hits_bean[RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE];
        return 1;
    }

    if (strcmp(metric_name, "truce-hits-total") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_variant_hits_total;
        return 1;
    }

    if (strcmp(metric_name, "truce-hits-banana") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_variant_hits_banana;
        return 1;
    }

    if (strcmp(metric_name, "truce-hits-bean") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_variant_hits_bean;
        return 1;
    }

    if (strcmp(metric_name, "truce-hits-base") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_variant_hits_base;
        return 1;
    }

    if (strcmp(metric_name, "truce-hits-apex") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_variant_hits_apex;
        return 1;
    }

    if (strcmp(metric_name, "truce-hits-mythic") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_variant_hits_mythic;
        return 1;
    }

    if (strcmp(metric_name, "truce-hits-stage-current") == 0)
    {
        int stage_index = runner->context.telemetry.war_intelligence_stage;

        if (stage_index < 0)
            stage_index = 0;
        if (stage_index >= BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS)
            stage_index = BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS - 1;

        *out_value = runner->context.telemetry.war_sentience_truce_variant_hits_stage[stage_index];
        return 1;
    }

    if (strcmp(metric_name, "truce-gate-checks") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_gate_checks_total;
        return 1;
    }

    if (strcmp(metric_name, "truce-gate-checks-stage-current") == 0)
    {
        int stage_index = runner->context.telemetry.war_intelligence_stage;

        if (stage_index < 0)
            stage_index = 0;
        if (stage_index >= BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS)
            stage_index = BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS - 1;

        *out_value = runner->context.telemetry.war_sentience_truce_gate_checks_stage[stage_index];
        return 1;
    }

    if (strcmp(metric_name, "truce-gate-granted") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_gate_checks_granted;
        return 1;
    }

    if (strcmp(metric_name, "truce-gate-granted-stage-current") == 0)
    {
        int stage_index = runner->context.telemetry.war_intelligence_stage;

        if (stage_index < 0)
            stage_index = 0;
        if (stage_index >= BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS)
            stage_index = BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS - 1;

        *out_value = runner->context.telemetry.war_sentience_truce_gate_granted_stage[stage_index];
        return 1;
    }

    if (strcmp(metric_name, "truce-gate-block-behavior") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_gate_block_behavior;
        return 1;
    }

    if (strcmp(metric_name, "truce-gate-block-empathy") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_gate_block_empathy;
        return 1;
    }

    if (strcmp(metric_name, "truce-gate-block-coordination") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_gate_block_coordination;
        return 1;
    }

    if (strcmp(metric_name, "truce-gate-block-streak") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_gate_block_streak;
        return 1;
    }

    if (strcmp(metric_name, "truce-gate-block-intelligence") == 0)
    {
        *out_value = runner->context.telemetry.war_sentience_truce_gate_block_intelligence;
        return 1;
    }

    return 0;
}

static void record_expect(Dx12PlayloopRunner *runner, int pass, const char *fmt, ...)
{
    char message[512] = {0};
    va_list args;

    if (!runner || !fmt)
        return;

    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    if (pass)
    {
        runner->expect_passed++;
        feedback_log(runner->artifact, "[dx12-playloop][PASS] %s\n", message);
        return;
    }

    runner->expect_failed++;
    feedback_log(runner->artifact, "[dx12-playloop][FAIL] %s\n", message);
}

static int split_tokens(char *line, char **tokens, int max_tokens)
{
    int count = 0;
    char *cursor = line;

    if (!line || !tokens || max_tokens <= 0)
        return 0;

    while (*cursor != '\0' && count < max_tokens)
    {
        while (*cursor != '\0' && isspace((unsigned char)*cursor))
            cursor++;

        if (*cursor == '\0')
            break;

        tokens[count++] = cursor;

        while (*cursor != '\0' && !isspace((unsigned char)*cursor))
            cursor++;

        if (*cursor == '\0')
            break;

        *cursor = '\0';
        cursor++;
    }

    return count;
}

static void join_tokens(char **tokens, int start_index, int token_count, char *output, size_t output_len)
{
    int i = 0;
    size_t used = 0;

    if (!tokens || !output || output_len == 0)
        return;

    output[0] = '\0';

    for (i = start_index; i < token_count; i++)
    {
        size_t piece_len = strlen(tokens[i]);

        if (used > 0)
        {
            if (used + 1 >= output_len)
                break;
            output[used++] = ' ';
            output[used] = '\0';
        }

        if (used + piece_len >= output_len)
            break;

        memcpy(output + used, tokens[i], piece_len);
        used += piece_len;
        output[used] = '\0';
    }
}

static int load_page(Dx12PlayloopRunner *runner, const char *scenario_name, FeedbackScenarioKind scenario_kind)
{
    size_t scenario_len = 0;

    if (!runner || !scenario_name)
        return 0;

    feedback_loop_context_destroy(&runner->context);

    if (!feedback_loop_context_init(&runner->context, -12.0f, -12.0f))
        return 0;

    if (!seed_scenario(&runner->context, scenario_kind))
        return 0;

    runner->tick_cursor = 0;
    runner->page_loaded = 1;

    scenario_len = strlen(scenario_name);
    if (scenario_len >= sizeof(runner->active_scenario_name))
        scenario_len = sizeof(runner->active_scenario_name) - 1;

    memcpy(runner->active_scenario_name, scenario_name, scenario_len);
    runner->active_scenario_name[scenario_len] = '\0';

    feedback_log(runner->artifact, "[dx12-playloop] page.goto %s\n", runner->active_scenario_name);
    return 1;
}

static int ensure_page_loaded(Dx12PlayloopRunner *runner)
{
    if (!runner)
        return 0;

    if (runner->page_loaded)
        return 1;

    return load_page(runner, runner->config.scenario_name, runner->config.scenario_kind);
}

static int set_engine_config(Dx12PlayloopRunner *runner, const char *key, const char *value_token)
{
    int parsed_int = 0;
    float parsed_float = 0.0f;

    if (!runner || !key || !value_token)
        return 0;

    if (strcmp(key, "war-radius") == 0)
    {
        if (!parse_float_arg(value_token, &parsed_float))
            return 0;
        runner->config.war_radius = parsed_float;
        return 1;
    }

    if (!parse_int_arg(value_token, &parsed_int))
        return 0;

    if (strcmp(key, "reinforcements") == 0)
    {
        if (parsed_int <= 0)
            return 0;
        runner->config.reinforcements_per_tick = parsed_int;
        return 1;
    }

    if (strcmp(key, "population-cap") == 0)
    {
        if (parsed_int <= 0)
            return 0;
        runner->config.population_cap = parsed_int;
        return 1;
    }

    if (strcmp(key, "escalation-tier") == 0)
    {
        runner->config.escalation_tier = parsed_int;
        return 1;
    }

    if (strcmp(key, "intelligence-stage") == 0)
    {
        runner->config.intelligence_stage = parsed_int;
        return 1;
    }

    if (strcmp(key, "biome-stage") == 0)
    {
        runner->config.biome_stage = parsed_int;
        return 1;
    }

    if (strcmp(key, "snapshot-interval") == 0)
    {
        if (parsed_int <= 0)
            return 0;
        runner->config.snapshot_interval = parsed_int;
        return 1;
    }

    return 0;
}

static int execute_ticks(Dx12PlayloopRunner *runner, int tick_count)
{
    int i = 0;

    if (!runner || tick_count <= 0)
        return 0;

    if (!ensure_page_loaded(runner))
        return 0;

    for (i = 0; i < tick_count; i++)
    {
        feedback_loop_tick_once(&runner->context, &runner->config);
        runner->tick_cursor++;

        if ((runner->tick_cursor % runner->config.snapshot_interval) == 0)
            emit_snapshot_dual(runner, "auto");
    }

    feedback_log(runner->artifact,
                 "[dx12-playloop] page.tick count=%d totalTicks=%d\n",
                 tick_count,
                 runner->tick_cursor);
    return 1;
}

static int run_dx12_client_scaffold_fixture(Dx12PlayloopRunner *runner, int line_number)
{
    const uint32_t seed_value = 1337u;
    const int cache_size = 256;
    const int object_count = 128;
    const int chunk_x = 0;
    const int chunk_z = 0;
    const int buffer_len = 12288;
    const int flag_steam = 0;
    const int flag_linked = 1;
    const int flag_good_standing = 1;
    const int flag_fresh = 1;
    const int flag_available = 1;
    const int64_t timestamp_ms = 4242;
    banana_native_ui_viewport viewport;
    uint8_t *serialization_buffer = NULL;
    int serialized_size = 0;

    if (!runner)
        return 0;

    memset(&viewport, 0, sizeof(viewport));
    viewport.css_width = 1920;
    viewport.css_height = 1080;
    viewport.pixel_width = 1920;
    viewport.pixel_height = 1080;
    viewport.device_pixel_ratio = 1.0f;

    runner->scaffold_abi_version = banana_native_v3_abi_version();
    runner->scaffold_last_status = banana_native_v3_ping();
    if (runner->scaffold_last_status != 0)
    {
        fprintf(stderr,
                "script line %d: test.use dx12-client-scaffold failed at client.ping status=%d\n",
                line_number,
                runner->scaffold_last_status);
        return 0;
    }

    banana_native_v3_world_cleanup();
    runner->scaffold_last_status = banana_native_v3_world_init(seed_value, cache_size);
    if (runner->scaffold_last_status != 0)
    {
        fprintf(stderr,
                "script line %d: test.use dx12-client-scaffold failed at client.world.init status=%d\n",
                line_number,
                runner->scaffold_last_status);
        return 0;
    }

    runner->scaffold_chunk_estimate = banana_native_v3_world_chunk_estimate_size(object_count);
    if (runner->scaffold_chunk_estimate < 0)
    {
        runner->scaffold_last_status = -1;
        fprintf(stderr,
                "script line %d: test.use dx12-client-scaffold failed at client.world.chunk-estimate status=%d\n",
                line_number,
                runner->scaffold_chunk_estimate);
        return 0;
    }

    serialization_buffer = (uint8_t *)malloc((size_t)buffer_len);
    if (!serialization_buffer)
    {
        fprintf(stderr,
                "script line %d: test.use dx12-client-scaffold allocation failed\n",
                line_number);
        return 0;
    }

    serialized_size = banana_native_v3_world_chunk_serialize(chunk_x,
                                                             chunk_z,
                                                             serialization_buffer,
                                                             buffer_len);
    free(serialization_buffer);
    serialization_buffer = NULL;

    runner->scaffold_chunk_serialized_bytes = serialized_size;
    if (serialized_size < 0)
    {
        runner->scaffold_last_status = serialized_size;
        fprintf(stderr,
                "script line %d: test.use dx12-client-scaffold failed at client.world.serialize status=%d\n",
                line_number,
                serialized_size);
        return 0;
    }

    memset(&runner->scaffold_launch_decision, 0, sizeof(runner->scaffold_launch_decision));
    runner->scaffold_last_status = banana_native_v3_launch_gate_decide("development",
                                                                        flag_steam,
                                                                        flag_linked,
                                                                        flag_good_standing,
                                                                        flag_fresh,
                                                                        flag_available,
                                                                        &runner->scaffold_launch_decision);
    runner->scaffold_launch_decision_valid = (runner->scaffold_last_status == 0) ? 1 : 0;
    if (runner->scaffold_last_status != 0)
    {
        fprintf(stderr,
                "script line %d: test.use dx12-client-scaffold failed at client.launch.decide status=%d\n",
                line_number,
                runner->scaffold_last_status);
        return 0;
    }

    banana_native_v3_ui_frame_reset(&runner->scaffold_ui_frame);
    banana_native_v3_ui_frame_write(&runner->scaffold_ui_frame,
                                    BANANA_NATIVE_UI_HOST_NATIVE,
                                    BANANA_NATIVE_UI_SURFACE_GAME_ENGINE,
                                    BANANA_NATIVE_UI_STATUS_LOADING,
                                    NULL,
                                    BANANA_NATIVE_UI_MOVEMENT_NONE,
                                    0.0f,
                                    0.0f,
                                    viewport,
                                    "dx12-playloop",
                                    timestamp_ms);
    runner->scaffold_ui_frame_valid = 1;
    runner->scaffold_last_status = 0;

    feedback_log(runner->artifact,
                 "[dx12-playloop] test.use dx12-client-scaffold seed=%u cacheSize=%d chunkEstimate=%d chunkBytes=%d launchAllow=%d\n",
                 (unsigned int)seed_value,
                 cache_size,
                 runner->scaffold_chunk_estimate,
                 runner->scaffold_chunk_serialized_bytes,
                 runner->scaffold_launch_decision.allow);
    return 1;
}

static int execute_script_line(Dx12PlayloopRunner *runner, char **tokens, int token_count, int line_number)
{
    const char *command = NULL;
    const char *reload_scenario_name = NULL;
    uint32_t seed_value = 0u;
    int cache_size = 0;
    int chunk_x = 0;
    int chunk_z = 0;
    int buffer_len = 0;
    int flag_steam = 0;
    int flag_linked = 0;
    int flag_good_standing = 0;
    int flag_fresh = 0;
    int flag_available = 0;
    int int_value = 0;
    int int_value2 = 0;
    int int_value3 = 0;
    int int_value4 = 0;
    int serialized_size = 0;
    int64_t timestamp_ms = 0;
    float x = 0.0f;
    float z = 0.0f;
    float dpr = 1.0f;
    uint8_t *serialization_buffer = NULL;
    banana_native_ui_host_kind ui_host = BANANA_NATIVE_UI_HOST_NATIVE;
    banana_native_ui_engine_status ui_status = BANANA_NATIVE_UI_STATUS_IDLE;
    banana_native_ui_movement_source ui_movement = BANANA_NATIVE_UI_MOVEMENT_NONE;
    banana_native_ui_viewport viewport;
    ControllerTeam team = CONTROLLER_TEAM_NEUTRAL;
    FeedbackScenarioKind scenario_kind = FEEDBACK_SCENARIO_WARFRONT;
    RuntimeWarSentienceBehaviorMode expected_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
    RuntimeWarSentienceBehaviorMode actual_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
    FeedbackComparisonOp comparison = FEEDBACK_COMPARE_EQ;
    int expected_metric = 0;
    int actual_metric = 0;
    char step_name[512] = {0};
    int is_page_tick_alias = 0;
    int is_expect_mode_alias = 0;
    int is_expect_metric_alias = 0;
    int is_expect_team_count_alias = 0;

    if (!runner || !tokens || token_count <= 0)
        return 0;

    command = tokens[0];
    is_page_tick_alias = (strcmp(command, "page.waitForTicks") == 0);
    is_expect_mode_alias = (strcmp(command, "expect.toHaveMode") == 0);
    is_expect_metric_alias = (strcmp(command, "expect.toHaveMetric") == 0);
    is_expect_team_count_alias = (strcmp(command, "expect.toHaveTeamCount") == 0);

    if (strcmp(command, "test.step") == 0)
    {
        if (token_count < 2)
        {
            fprintf(stderr, "script line %d: test.step requires a label\n", line_number);
            return 0;
        }

        join_tokens(tokens, 1, token_count, step_name, sizeof(step_name));
        feedback_log(runner->artifact, "[dx12-playloop] test.step %s\n", step_name);
        return 1;
    }

    if (strcmp(command, "test.use") == 0)
    {
        if (token_count != 2)
        {
            fprintf(stderr, "script line %d: test.use requires one fixture name\n", line_number);
            return 0;
        }

        if (strcmp(tokens[1], "dx12-client-scaffold") == 0)
            return run_dx12_client_scaffold_fixture(runner, line_number);

        fprintf(stderr, "script line %d: unknown fixture '%s'\n", line_number, tokens[1]);
        return 0;
    }

    if (strcmp(command, "page.goto") == 0)
    {
        if (token_count != 2)
        {
            fprintf(stderr, "script line %d: page.goto requires exactly one scenario argument\n", line_number);
            return 0;
        }

        if (!resolve_scenario(tokens[1], &scenario_kind))
        {
            fprintf(stderr, "script line %d: unknown scenario '%s'\n", line_number, tokens[1]);
            return 0;
        }

        if (!load_page(runner, tokens[1], scenario_kind))
        {
            fprintf(stderr, "script line %d: failed to load scenario '%s'\n", line_number, tokens[1]);
            return 0;
        }

        return 1;
    }

    if (strcmp(command, "page.reload") == 0)
    {
        if (token_count != 1)
        {
            fprintf(stderr, "script line %d: page.reload takes no arguments\n", line_number);
            return 0;
        }

        reload_scenario_name = (runner->active_scenario_name[0] != '\0') ? runner->active_scenario_name : runner->config.scenario_name;
        if (!resolve_scenario(reload_scenario_name, &scenario_kind))
            scenario_kind = runner->config.scenario_kind;

        if (!load_page(runner, reload_scenario_name, scenario_kind))
        {
            fprintf(stderr, "script line %d: failed to reload scenario '%s'\n", line_number, reload_scenario_name);
            return 0;
        }

        feedback_log(runner->artifact, "[dx12-playloop] page.reload %s\n", reload_scenario_name);
        return 1;
    }

    if (strcmp(command, "engine.config") == 0)
    {
        if (token_count != 3)
        {
            fprintf(stderr, "script line %d: engine.config requires <key> <value>\n", line_number);
            return 0;
        }

        if (!set_engine_config(runner, tokens[1], tokens[2]))
        {
            fprintf(stderr,
                    "script line %d: invalid engine.config assignment '%s %s'\n",
                    line_number,
                    tokens[1],
                    tokens[2]);
            return 0;
        }

        feedback_log(runner->artifact, "[dx12-playloop] engine.config %s=%s\n", tokens[1], tokens[2]);
        return 1;
    }

    if (strcmp(command, "actor.spawn") == 0)
    {
        if (token_count != 5)
        {
            fprintf(stderr,
                    "script line %d: actor.spawn requires <team> <controllerKind> <x> <z>\n",
                    line_number);
            return 0;
        }

        if (!ensure_page_loaded(runner))
        {
            fprintf(stderr, "script line %d: unable to initialize scenario context\n", line_number);
            return 0;
        }

        if (!resolve_team(tokens[1], &team))
        {
            fprintf(stderr, "script line %d: unknown team '%s'\n", line_number, tokens[1]);
            return 0;
        }

        if (!parse_float_arg(tokens[3], &x) || !parse_float_arg(tokens[4], &z))
        {
            fprintf(stderr, "script line %d: actor.spawn x/z must be numeric\n", line_number);
            return 0;
        }

        if (!feedback_loop_spawn_controller(&runner->context, team, tokens[2], x, z, NULL))
        {
            fprintf(stderr, "script line %d: actor.spawn failed\n", line_number);
            return 0;
        }

        feedback_log(runner->artifact,
                     "[dx12-playloop] actor.spawn team=%s kind=%s x=%.3f z=%.3f\n",
                     tokens[1],
                     tokens[2],
                     x,
                     z);
        return 1;
    }

    if (strcmp(command, "state.sentience") == 0)
    {
        if (token_count != 4)
        {
            fprintf(stderr,
                    "script line %d: state.sentience requires <humanoid> <coordination> <empathy>\n",
                    line_number);
            return 0;
        }

        if (!ensure_page_loaded(runner))
        {
            fprintf(stderr, "script line %d: unable to initialize scenario context\n", line_number);
            return 0;
        }

        if (!parse_int_arg(tokens[1], &runner->context.telemetry.war_sentience_humanoid_index) ||
            !parse_int_arg(tokens[2], &runner->context.telemetry.war_sentience_coordination_level) ||
            !parse_int_arg(tokens[3], &runner->context.telemetry.war_sentience_empathy_level))
        {
            fprintf(stderr, "script line %d: state.sentience values must be integers\n", line_number);
            return 0;
        }

        feedback_log(runner->artifact,
                     "[dx12-playloop] state.sentience humanoid=%d coordination=%d empathy=%d\n",
                     runner->context.telemetry.war_sentience_humanoid_index,
                     runner->context.telemetry.war_sentience_coordination_level,
                     runner->context.telemetry.war_sentience_empathy_level);
        return 1;
    }

    if (strcmp(command, "page.tick") == 0 || is_page_tick_alias)
    {
        if (token_count != 2 || !parse_int_arg(tokens[1], &int_value) || int_value <= 0)
        {
            fprintf(stderr,
                    "script line %d: %s requires a positive integer count\n",
                    line_number,
                    command);
            return 0;
        }

        return execute_ticks(runner, int_value);
    }

    if (strcmp(command, "client.ping") == 0)
    {
        if (token_count != 1)
        {
            fprintf(stderr, "script line %d: client.ping takes no arguments\n", line_number);
            return 0;
        }

        runner->scaffold_abi_version = banana_native_v3_abi_version();
        runner->scaffold_last_status = banana_native_v3_ping();

        feedback_log(runner->artifact,
                     "[dx12-playloop] client.ping status=%d abiVersion=%d\n",
                     runner->scaffold_last_status,
                     runner->scaffold_abi_version);
        return 1;
    }

    if (strcmp(command, "client.world.init") == 0)
    {
        if (token_count != 3)
        {
            fprintf(stderr, "script line %d: client.world.init requires <seed> <cacheSize>\n", line_number);
            return 0;
        }

        if (!parse_uint32_arg(tokens[1], &seed_value) || !parse_int_arg(tokens[2], &cache_size) || cache_size <= 0)
        {
            fprintf(stderr, "script line %d: client.world.init seed/cacheSize must be valid numbers\n", line_number);
            return 0;
        }

        runner->scaffold_last_status = banana_native_v3_world_init(seed_value, cache_size);

        feedback_log(runner->artifact,
                     "[dx12-playloop] client.world.init seed=%u cacheSize=%d status=%d\n",
                     (unsigned int)seed_value,
                     cache_size,
                     runner->scaffold_last_status);
        return 1;
    }

    if (strcmp(command, "client.world.chunk-estimate") == 0)
    {
        if (token_count != 2)
        {
            fprintf(stderr, "script line %d: client.world.chunk-estimate requires <objectCount>\n", line_number);
            return 0;
        }

        if (!parse_int_arg(tokens[1], &int_value) || int_value < 0)
        {
            fprintf(stderr, "script line %d: client.world.chunk-estimate objectCount must be >= 0\n", line_number);
            return 0;
        }

        runner->scaffold_chunk_estimate = banana_native_v3_world_chunk_estimate_size(int_value);
        runner->scaffold_last_status = (runner->scaffold_chunk_estimate >= 0) ? 0 : -1;

        feedback_log(runner->artifact,
                     "[dx12-playloop] client.world.chunk-estimate objects=%d estimate=%d\n",
                     int_value,
                     runner->scaffold_chunk_estimate);
        return 1;
    }

    if (strcmp(command, "client.world.serialize") == 0)
    {
        if (token_count != 4)
        {
            fprintf(stderr, "script line %d: client.world.serialize requires <chunkX> <chunkZ> <bufferLen>\n", line_number);
            return 0;
        }

        if (!parse_int_arg(tokens[1], &chunk_x) || !parse_int_arg(tokens[2], &chunk_z) ||
            !parse_int_arg(tokens[3], &buffer_len) || buffer_len <= 0)
        {
            fprintf(stderr, "script line %d: client.world.serialize expects numeric chunk coordinates and positive buffer length\n", line_number);
            return 0;
        }

        serialization_buffer = (uint8_t *)malloc((size_t)buffer_len);
        if (!serialization_buffer)
        {
            fprintf(stderr, "script line %d: client.world.serialize buffer allocation failed\n", line_number);
            return 0;
        }

        serialized_size = banana_native_v3_world_chunk_serialize(chunk_x,
                                                                 chunk_z,
                                                                 serialization_buffer,
                                                                 buffer_len);
        free(serialization_buffer);
        serialization_buffer = NULL;

        runner->scaffold_chunk_serialized_bytes = serialized_size;
        runner->scaffold_last_status = (serialized_size >= 0) ? 0 : serialized_size;

        if (serialized_size < 0)
        {
            fprintf(stderr,
                    "script line %d: client.world.serialize failed for chunk=(%d,%d) status=%d\n",
                    line_number,
                    chunk_x,
                    chunk_z,
                    serialized_size);
            return 0;
        }

        feedback_log(runner->artifact,
                     "[dx12-playloop] client.world.serialize chunk=(%d,%d) bytes=%d\n",
                     chunk_x,
                     chunk_z,
                     serialized_size);
        return 1;
    }

    if (strcmp(command, "client.world.cleanup") == 0)
    {
        if (token_count != 1)
        {
            fprintf(stderr, "script line %d: client.world.cleanup takes no arguments\n", line_number);
            return 0;
        }

        banana_native_v3_world_cleanup();
        runner->scaffold_last_status = 0;
        feedback_log(runner->artifact, "[dx12-playloop] client.world.cleanup\n");
        return 1;
    }

    if (strcmp(command, "client.launch.decide") == 0)
    {
        if (token_count != 7)
        {
            fprintf(stderr,
                    "script line %d: client.launch.decide requires <mode> <steam> <linked> <goodStanding> <fresh> <available>\n",
                    line_number);
            return 0;
        }

        if (!parse_flag_arg(tokens[2], &flag_steam) ||
            !parse_flag_arg(tokens[3], &flag_linked) ||
            !parse_flag_arg(tokens[4], &flag_good_standing) ||
            !parse_flag_arg(tokens[5], &flag_fresh) ||
            !parse_flag_arg(tokens[6], &flag_available))
        {
            fprintf(stderr,
                    "script line %d: client.launch.decide flag values must be 0/1 or true/false\n",
                    line_number);
            return 0;
        }

        memset(&runner->scaffold_launch_decision, 0, sizeof(runner->scaffold_launch_decision));
        runner->scaffold_last_status = banana_native_v3_launch_gate_decide(tokens[1],
                                                                            flag_steam,
                                                                            flag_linked,
                                                                            flag_good_standing,
                                                                            flag_fresh,
                                                                            flag_available,
                                                                            &runner->scaffold_launch_decision);
        runner->scaffold_launch_decision_valid = (runner->scaffold_last_status == 0) ? 1 : 0;

        if (runner->scaffold_last_status != 0)
        {
            fprintf(stderr,
                    "script line %d: client.launch.decide failed mode='%s' status=%d\n",
                    line_number,
                    tokens[1],
                    runner->scaffold_last_status);
            return 0;
        }

        feedback_log(runner->artifact,
                     "[dx12-playloop] client.launch.decide mode=%s allow=%d reason=%d remediable=%d\n",
                     tokens[1],
                     runner->scaffold_launch_decision.allow,
                     (int)runner->scaffold_launch_decision.reason_code,
                     runner->scaffold_launch_decision.is_remediable);
        return 1;
    }

    if (strcmp(command, "client.ui.reset") == 0)
    {
        if (token_count != 1)
        {
            fprintf(stderr, "script line %d: client.ui.reset takes no arguments\n", line_number);
            return 0;
        }

        banana_native_v3_ui_frame_reset(&runner->scaffold_ui_frame);
        runner->scaffold_ui_frame_valid = 1;
        runner->scaffold_last_status = 0;

        feedback_log(runner->artifact, "[dx12-playloop] client.ui.reset\n");
        return 1;
    }

    if (strcmp(command, "client.ui.write") == 0)
    {
        memset(&viewport, 0, sizeof(viewport));

        if (token_count != 12)
        {
            fprintf(stderr,
                    "script line %d: client.ui.write requires <host> <engineStatus> <movement> <moveX> <moveZ> <cssW> <cssH> <pixelW> <pixelH> <dpr> <timestampMs>\n",
                    line_number);
            return 0;
        }

        if (!resolve_ui_host(tokens[1], &ui_host) ||
            !resolve_ui_engine_status(tokens[2], &ui_status) ||
            !resolve_ui_movement_source(tokens[3], &ui_movement))
        {
            fprintf(stderr,
                    "script line %d: client.ui.write host/status/movement must be known labels\n",
                    line_number);
            return 0;
        }

        if (!parse_float_arg(tokens[4], &x) || !parse_float_arg(tokens[5], &z) ||
            !parse_int_arg(tokens[6], &int_value) || !parse_int_arg(tokens[7], &int_value2) ||
            !parse_int_arg(tokens[8], &int_value3) || !parse_int_arg(tokens[9], &int_value4) ||
            !parse_float_arg(tokens[10], &dpr) || !parse_int64_arg(tokens[11], &timestamp_ms))
        {
            fprintf(stderr,
                    "script line %d: client.ui.write move/viewport/timestamp values are invalid\n",
                    line_number);
            return 0;
        }

        viewport.css_width = int_value;
        viewport.css_height = int_value2;
        viewport.pixel_width = int_value3;
        viewport.pixel_height = int_value4;
        viewport.device_pixel_ratio = dpr;

        banana_native_v3_ui_frame_write(&runner->scaffold_ui_frame,
                                        ui_host,
                                        BANANA_NATIVE_UI_SURFACE_GAME_ENGINE,
                                        ui_status,
                                        NULL,
                                        ui_movement,
                                        x,
                                        z,
                                        viewport,
                                        "dx12-playloop",
                                        timestamp_ms);

        runner->scaffold_ui_frame_valid = 1;
        runner->scaffold_last_status = 0;

        feedback_log(runner->artifact,
                     "[dx12-playloop] client.ui.write host=%s status=%s movement=%s css=%dx%d px=%dx%d\n",
                     tokens[1],
                     tokens[2],
                     tokens[3],
                     viewport.css_width,
                     viewport.css_height,
                     viewport.pixel_width,
                     viewport.pixel_height);
        return 1;
    }

    if (strcmp(command, "snapshot") == 0)
    {
        if (token_count > 2)
        {
            fprintf(stderr, "script line %d: snapshot takes zero or one label token\n", line_number);
            return 0;
        }

        if (!ensure_page_loaded(runner))
        {
            fprintf(stderr, "script line %d: unable to initialize scenario context\n", line_number);
            return 0;
        }

        emit_snapshot_dual(runner, token_count == 2 ? tokens[1] : "manual");
        return 1;
    }

    if (strcmp(command, "expect.mode") == 0 || is_expect_mode_alias)
    {
        if (token_count != 3)
        {
            fprintf(stderr,
                    "script line %d: %s requires <banana|bean> <mode>\n",
                    line_number,
                    command);
            return 0;
        }

        if (!ensure_page_loaded(runner))
        {
            fprintf(stderr, "script line %d: unable to initialize scenario context\n", line_number);
            return 0;
        }

        if (!resolve_mode(tokens[2], &expected_mode))
        {
            fprintf(stderr, "script line %d: unknown mode '%s'\n", line_number, tokens[2]);
            return 0;
        }

        if (strcmp(tokens[1], "banana") == 0)
            actual_mode = runner->context.telemetry.war_sentience_behavior_banana;
        else if (strcmp(tokens[1], "bean") == 0)
            actual_mode = runner->context.telemetry.war_sentience_behavior_bean;
        else
        {
            fprintf(stderr, "script line %d: expect.mode supports only banana or bean\n", line_number);
            return 0;
        }

        record_expect(runner,
                      actual_mode == expected_mode,
                      "expect.mode team=%s expected=%s actual=%s",
                      tokens[1],
                      mode_label(expected_mode),
                      mode_label(actual_mode));
        return 1;
    }

    if (strcmp(command, "expect.metric") == 0 || is_expect_metric_alias)
    {
        if (token_count != 4)
        {
            fprintf(stderr,
                    "script line %d: %s requires <name> <eq|ne|gt|gte|lt|lte> <value>\n",
                    line_number,
                    command);
            return 0;
        }

        if (!resolve_comparison(tokens[2], &comparison) || !parse_int_arg(tokens[3], &expected_metric))
        {
            fprintf(stderr, "script line %d: invalid comparison/value for expect.metric\n", line_number);
            return 0;
        }

        if (!read_metric_value(runner, tokens[1], &actual_metric))
        {
            fprintf(stderr, "script line %d: unknown metric '%s'\n", line_number, tokens[1]);
            return 0;
        }

        record_expect(runner,
                      compare_values(actual_metric, comparison, expected_metric),
                      "expect.metric name=%s op=%s expected=%d actual=%d",
                      tokens[1],
                      tokens[2],
                      expected_metric,
                      actual_metric);
        return 1;
    }

    if (strcmp(command, "expect.team-count") == 0 || is_expect_team_count_alias)
    {
        if (token_count != 4)
        {
            fprintf(stderr,
                    "script line %d: %s requires <team> <eq|ne|gt|gte|lt|lte> <value>\n",
                    line_number,
                    command);
            return 0;
        }

        if (!ensure_page_loaded(runner))
        {
            fprintf(stderr, "script line %d: unable to initialize scenario context\n", line_number);
            return 0;
        }

        if (!resolve_team(tokens[1], &team))
        {
            fprintf(stderr, "script line %d: unknown team '%s'\n", line_number, tokens[1]);
            return 0;
        }

        if (!resolve_comparison(tokens[2], &comparison) || !parse_int_arg(tokens[3], &expected_metric))
        {
            fprintf(stderr, "script line %d: invalid comparison/value for expect.team-count\n", line_number);
            return 0;
        }

        actual_metric = count_controllers_for_team(&runner->context, team);

        record_expect(runner,
                      compare_values(actual_metric, comparison, expected_metric),
                      "expect.team-count team=%s op=%s expected=%d actual=%d",
                      tokens[1],
                      tokens[2],
                      expected_metric,
                      actual_metric);
        return 1;
    }

    if (strcmp(command, "note") == 0)
    {
        char note_text[512] = {0};
        join_tokens(tokens, 1, token_count, note_text, sizeof(note_text));

        if (note_text[0] != '\0')
            feedback_log(runner->artifact, "[dx12-playloop] note=%s\n", note_text);
        return 1;
    }

    fprintf(stderr, "script line %d: unknown command '%s'\n", line_number, command);
    return 0;
}

static int run_scripted_playloop(const FeedbackLoopConfig *config)
{
    Dx12PlayloopRunner runner;
    FILE *script = NULL;
    char line[1024] = {0};
    char operator_note[512] = {0};
    const char *note_to_emit = NULL;
    int line_number = 0;
    int status = 1;

    if (!config || !config->script_path)
        return 1;

    memset(&runner, 0, sizeof(runner));
    runner.config = *config;
    strncpy(runner.active_scenario_name, config->scenario_name, sizeof(runner.active_scenario_name) - 1);
    runner.active_scenario_name[sizeof(runner.active_scenario_name) - 1] = '\0';

    script = fopen(config->script_path, "r");
    if (!script)
    {
        fprintf(stderr, "failed to open script '%s'\n", config->script_path);
        goto cleanup;
    }

    if (config->output_path && config->output_path[0] != '\0')
    {
        runner.artifact = fopen(config->output_path, "w");
        if (!runner.artifact)
        {
            fprintf(stderr, "failed to open output file '%s'\n", config->output_path);
            goto cleanup;
        }
    }

    feedback_log(runner.artifact,
                 "[dx12-playloop] script=%s fallbackScenario=%s\n",
                 config->script_path,
                 config->scenario_name);

    while (fgets(line, sizeof(line), script))
    {
        char *tokens[16] = {0};
        char *cursor = line;
        int token_count = 0;

        line_number++;
        trim_trailing_newline(line);

        while (*cursor != '\0' && isspace((unsigned char)*cursor))
            cursor++;

        if (*cursor == '\0' || *cursor == '#')
            continue;

        token_count = split_tokens(cursor, tokens, (int)(sizeof(tokens) / sizeof(tokens[0])));
        if (token_count == 0)
            continue;

        if (!execute_script_line(&runner, tokens, token_count, line_number))
            goto cleanup;
    }

    if (runner.page_loaded)
    {
        emit_snapshot_dual(&runner, "final");
    }
    else
    {
        feedback_log(runner.artifact,
                     "[dx12-playloop] no gameplay page loaded; skipping final snapshot\n");
    }

    note_to_emit = config->feedback_note;

    if (!config->non_interactive)
    {
        printf("[dx12-playloop] operator note (optional): ");
        fflush(stdout);

        if (fgets(operator_note, sizeof(operator_note), stdin))
        {
            trim_trailing_newline(operator_note);
            if (operator_note[0] != '\0')
                note_to_emit = operator_note;
        }
    }

    if (note_to_emit && note_to_emit[0] != '\0')
        feedback_log(runner.artifact, "[dx12-playloop] note=%s\n", note_to_emit);

    feedback_log(runner.artifact,
                 "[dx12-playloop] summary pass=%d fail=%d ticks=%d\n",
                 runner.expect_passed,
                 runner.expect_failed,
                 runner.tick_cursor);

    status = (runner.expect_failed == 0) ? 0 : 1;

cleanup:
    if (script)
        fclose(script);

    banana_native_v3_world_cleanup();

    feedback_loop_context_destroy(&runner.context);

    if (runner.artifact)
        fclose(runner.artifact);

    return status;
}

static int run_feedback_loop(const FeedbackLoopConfig *config)
{
    FeedbackLoopContext context;
    FILE *artifact = NULL;
    char operator_note[512] = {0};
    const char *note_to_emit = NULL;
    int tick = 0;
    int status = 1;

    memset(&context, 0, sizeof(context));

    if (!feedback_loop_context_init(&context, -12.0f, -12.0f))
        goto cleanup;

    if (!seed_scenario(&context, config->scenario_kind))
    {
        fprintf(stderr, "failed to seed scenario '%s'\n", config->scenario_name);
        goto cleanup;
    }

    if (config->output_path && config->output_path[0] != '\0')
    {
        artifact = fopen(config->output_path, "w");
        if (!artifact)
        {
            fprintf(stderr, "failed to open output file '%s'\n", config->output_path);
            goto cleanup;
        }
    }

    feedback_log(artifact,
                 "[feedback-loop] scenario=%s ticks=%d snapshotInterval=%d\n",
                 config->scenario_name,
                 config->ticks,
                 config->snapshot_interval);

    for (tick = 1; tick <= config->ticks; tick++)
    {
        if (config->scenario_kind == FEEDBACK_SCENARIO_NEGOTIATE && (tick % 2) == 0)
        {
            float parity_z = ((tick % 4) == 0) ? -0.25f : 0.25f;
            feedback_loop_spawn_controller(&context,
                                           CONTROLLER_TEAM_BEAN,
                                           "wildlife",
                                           2.0f + (float)tick * 0.25f,
                                           parity_z,
                                           NULL);
        }

        if (config->scenario_kind == FEEDBACK_SCENARIO_COMEBACK && tick == 3)
        {
            feedback_loop_spawn_controller(&context,
                                           CONTROLLER_TEAM_BANANA,
                                           "combat",
                                           -1.25f,
                                           0.15f,
                                           NULL);
        }

        feedback_loop_tick_once(&context, config);

        if (tick == 1 || (tick % config->snapshot_interval) == 0 || tick == config->ticks)
        {
            emit_snapshot(stdout, config->scenario_name, "legacy", &context, tick);
            if (artifact)
                emit_snapshot(artifact, config->scenario_name, "legacy", &context, tick);
        }
    }

    note_to_emit = config->feedback_note;

    if (!config->non_interactive)
    {
        printf("[feedback-loop] operator note (optional): ");
        fflush(stdout);

        if (fgets(operator_note, sizeof(operator_note), stdin))
        {
            trim_trailing_newline(operator_note);
            if (operator_note[0] != '\0')
                note_to_emit = operator_note;
        }
    }

    if (note_to_emit && note_to_emit[0] != '\0')
    {
        printf("[feedback-loop] note=%s\n", note_to_emit);
        if (artifact)
            fprintf(artifact, "[feedback-loop] note=%s\n", note_to_emit);
    }

    status = 0;

cleanup:
    if (artifact)
        fclose(artifact);

    feedback_loop_context_destroy(&context);
    return status;
}

int main(int argc, char **argv)
{
    FeedbackLoopConfig config;
    int parse_result = 0;

    parse_result = parse_args(argc, argv, &config);
    if (parse_result == 2)
        return 0;

    if (parse_result == 0)
    {
        print_usage(argv[0]);
        return 1;
    }

    if (config.script_path && config.script_path[0] != '\0')
        return run_scripted_playloop(&config);

    return run_feedback_loop(&config);
}
