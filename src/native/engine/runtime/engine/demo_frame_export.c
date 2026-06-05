#include "demo_frame_export.h"

#include "engine_host.h"

#include "../../engine.h"
#include "../../render/backend.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define DEMO_FRAME_EXPORT_DEFAULT_FORMAT "bmp"
#define DEMO_FRAME_EXPORT_MAX_FRAME_ID 128

typedef struct DemoFrameExportFrame
{
    char frame_id[DEMO_FRAME_EXPORT_MAX_FRAME_ID];
    char scenario[DEMO_FRAME_EXPORT_MAX_LABEL];
    int tick;
    int64_t timestamp_ms;
    char image_path[DEMO_FRAME_EXPORT_MAX_PATH];
    char metadata_path[DEMO_FRAME_EXPORT_MAX_PATH];
    int width;
    int height;
    char render_backend[32];
    int ui_overlay;
    int scene_variant;
    char scene_key[DEMO_FRAME_EXPORT_MAX_LABEL];
} DemoFrameExportFrame;

typedef struct DemoFrameExportFailure
{
    char frame_id[DEMO_FRAME_EXPORT_MAX_FRAME_ID];
    char scenario[DEMO_FRAME_EXPORT_MAX_LABEL];
    int tick;
    char error[256];
} DemoFrameExportFailure;

#pragma pack(push, 1)
typedef struct DemoFrameExportBmpFileHeader
{
    unsigned short bf_type;
    unsigned int bf_size;
    unsigned short bf_reserved1;
    unsigned short bf_reserved2;
    unsigned int bf_off_bits;
} DemoFrameExportBmpFileHeader;

typedef struct DemoFrameExportBmpInfoHeader
{
    unsigned int bi_size;
    int bi_width;
    int bi_height;
    unsigned short bi_planes;
    unsigned short bi_bit_count;
    unsigned int bi_compression;
    unsigned int bi_size_image;
    int bi_x_pels_per_meter;
    int bi_y_pels_per_meter;
    unsigned int bi_clr_used;
    unsigned int bi_clr_important;
} DemoFrameExportBmpInfoHeader;
#pragma pack(pop)

static int demo_frame_export_parse_env_flag(const char *name)
{
    const char *raw = getenv(name);

    if (!raw || raw[0] == '\0')
        return 0;

    if (strcmp(raw, "0") == 0 ||
        strcmp(raw, "false") == 0 ||
        strcmp(raw, "off") == 0 ||
        strcmp(raw, "no") == 0)
    {
        return 0;
    }

    return 1;
}

static int demo_frame_export_parse_env_int(const char *name,
                                           int fallback,
                                           int min_value,
                                           int max_value)
{
    const char *raw = getenv(name);
    char *end = NULL;
    long parsed = 0;

    if (!raw || raw[0] == '\0')
        return fallback;

    parsed = strtol(raw, &end, 10);
    if (raw == end || (end && *end != '\0'))
        return fallback;

    if (parsed < (long)min_value)
        return min_value;
    if (parsed > (long)max_value)
        return max_value;

    return (int)parsed;
}

static void demo_frame_export_copy_string(char *dest, size_t dest_size, const char *value)
{
    if (!dest || dest_size == 0)
        return;

    if (!value || value[0] == '\0')
    {
        dest[0] = '\0';
        return;
    }

    strncpy(dest, value, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

static void demo_frame_export_normalize_path(char *path)
{
    size_t i = 0;

    if (!path)
        return;

    for (i = 0; path[i] != '\0'; i++)
    {
        if (path[i] == '\\')
            path[i] = '/';
    }
}

static void demo_frame_export_sanitize_label(const char *input, char *output, size_t output_size)
{
    size_t i = 0;
    size_t used = 0;

    if (!output || output_size == 0)
        return;

    output[0] = '\0';

    if (!input || input[0] == '\0')
        return;

    for (i = 0; input[i] != '\0' && used + 1 < output_size; i++)
    {
        char c = input[i];
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_')
        {
            output[used++] = c;
        }
        else
        {
            output[used++] = '_';
        }
    }

    output[used] = '\0';
}

static int64_t demo_frame_export_now_ms(void)
{
#ifdef _WIN32
    return (int64_t)GetTickCount64();
#else
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        return 0;
    return (int64_t)ts.tv_sec * 1000 + (int64_t)(ts.tv_nsec / 1000000);
#endif
}

static void demo_frame_export_timestamp_utc(char *output, size_t output_size)
{
    time_t now = time(NULL);
    struct tm utc_now;
    size_t written = 0;

    if (!output || output_size == 0)
        return;

#ifdef _WIN32
    gmtime_s(&utc_now, &now);
#else
    gmtime_r(&now, &utc_now);
#endif

    written = strftime(output, output_size, "%Y-%m-%dT%H:%M:%SZ", &utc_now);
    if (written == 0 && output_size > 0)
        output[0] = '\0';
}

static void demo_frame_export_run_id(char *output,
                                     size_t output_size,
                                     const char *timestamp,
                                     const char *label)
{
    char clean_label[DEMO_FRAME_EXPORT_MAX_LABEL] = {0};
    char safe_timestamp[32] = {0};
    size_t i = 0;

    if (!output || output_size == 0)
        return;

    demo_frame_export_copy_string(safe_timestamp, sizeof(safe_timestamp), timestamp ? timestamp : "run");
    for (i = 0; safe_timestamp[i] != '\0'; i++)
    {
        if (safe_timestamp[i] == ':' || safe_timestamp[i] == '-')
            safe_timestamp[i] = '_';
    }

    demo_frame_export_sanitize_label(label, clean_label, sizeof(clean_label));
    if (clean_label[0] == '\0')
    {
        snprintf(output, output_size, "%s", safe_timestamp);
        return;
    }

    snprintf(output, output_size, "%s-%s", safe_timestamp, clean_label);
}

static int demo_frame_export_mkdir_single(const char *path)
{
#ifdef _WIN32
    if (_mkdir(path) == 0)
        return 1;
#else
    if (mkdir(path, 0755) == 0)
        return 1;
#endif

    return (errno == EEXIST) ? 1 : 0;
}

static int demo_frame_export_mkdirs(const char *path)
{
    char buffer[DEMO_FRAME_EXPORT_MAX_PATH];
    size_t len = 0;
    size_t i = 0;
    size_t start = 1;

    if (!path || path[0] == '\0')
        return 0;

    len = strlen(path);
    if (len >= sizeof(buffer))
        return 0;

    strncpy(buffer, path, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    demo_frame_export_normalize_path(buffer);

    if (len >= 3 && buffer[1] == ':' && (buffer[2] == '/' || buffer[2] == '\\'))
        start = 3;

    for (i = start; i < len; i++)
    {
        if (buffer[i] == '/' || buffer[i] == '\\')
        {
            buffer[i] = '\0';
            if (buffer[0] != '\0' && !demo_frame_export_mkdir_single(buffer))
                return 0;
            buffer[i] = '/';
        }
    }

    return demo_frame_export_mkdir_single(buffer);
}

static void demo_frame_export_write_json_string(FILE *file, const char *value)
{
    const char *cursor = value ? value : "";

    fputc('"', file);
    for (; *cursor != '\0'; ++cursor)
    {
        switch (*cursor)
        {
        case '\\':
            fputs("\\\\", file);
            break;
        case '"':
            fputs("\\\"", file);
            break;
        case '\n':
            fputs("\\n", file);
            break;
        case '\r':
            fputs("\\r", file);
            break;
        case '\t':
            fputs("\\t", file);
            break;
        default:
            fputc(*cursor, file);
            break;
        }
    }
    fputc('"', file);
}

static int demo_frame_export_reserve_frames(DemoFrameExportState *state, int desired)
{
    DemoFrameExportFrame *frames = NULL;
    int capacity = 0;

    if (!state)
        return 0;

    if (state->frame_capacity >= desired)
        return 1;

    capacity = state->frame_capacity > 0 ? state->frame_capacity : 8;
    while (capacity < desired)
        capacity *= 2;

    frames = (DemoFrameExportFrame *)realloc(state->frames, (size_t)capacity * sizeof(DemoFrameExportFrame));
    if (!frames)
        return 0;

    state->frames = frames;
    state->frame_capacity = capacity;
    return 1;
}

static int demo_frame_export_reserve_failures(DemoFrameExportState *state, int desired)
{
    DemoFrameExportFailure *failures = NULL;
    int capacity = 0;

    if (!state)
        return 0;

    if (state->failure_capacity >= desired)
        return 1;

    capacity = state->failure_capacity > 0 ? state->failure_capacity : 4;
    while (capacity < desired)
        capacity *= 2;

    failures = (DemoFrameExportFailure *)realloc(state->failures, (size_t)capacity * sizeof(DemoFrameExportFailure));
    if (!failures)
        return 0;

    state->failures = failures;
    state->failure_capacity = capacity;
    return 1;
}

static void demo_frame_export_track_scenario(DemoFrameExportState *state, const char *scenario)
{
    int i = 0;

    if (!state || !scenario || scenario[0] == '\0')
        return;

    for (i = 0; i < state->scenario_count; i++)
    {
        if (strcmp(state->scenario_labels[i], scenario) == 0)
            return;
    }

    if (state->scenario_count >= DEMO_FRAME_EXPORT_MAX_SCENARIOS)
        return;

    demo_frame_export_copy_string(state->scenario_labels[state->scenario_count],
                                  sizeof(state->scenario_labels[state->scenario_count]),
                                  scenario);
    state->scenario_count++;
}

static void demo_frame_export_record_failure(DemoFrameExportState *state,
                                             const char *scenario,
                                             int tick,
                                             const char *frame_id,
                                             const char *error)
{
    DemoFrameExportFailure *failures = NULL;

    if (!state)
        return;

    if (!demo_frame_export_reserve_failures(state, state->failure_count + 1))
        return;

    failures = (DemoFrameExportFailure *)state->failures;
    memset(&failures[state->failure_count], 0, sizeof(DemoFrameExportFailure));
    demo_frame_export_copy_string(failures[state->failure_count].scenario,
                                  sizeof(failures[state->failure_count].scenario),
                                  scenario ? scenario : "unknown");
    demo_frame_export_copy_string(failures[state->failure_count].frame_id,
                                  sizeof(failures[state->failure_count].frame_id),
                                  frame_id ? frame_id : "");
    demo_frame_export_copy_string(failures[state->failure_count].error,
                                  sizeof(failures[state->failure_count].error),
                                  error ? error : "export failed");
    failures[state->failure_count].tick = tick;
    state->failure_count++;
}

static int demo_frame_export_write_bmp(DemoFrameExportState *state,
                                       const char *path,
                                       const unsigned char *rgba,
                                       int width,
                                       int height)
{
    DemoFrameExportBmpFileHeader file_header;
    DemoFrameExportBmpInfoHeader info_header;
    FILE *file = NULL;
    size_t pixel_bytes = 0;
    size_t i = 0;
    unsigned char *bgra = NULL;

    if (!state || !path || !rgba || width <= 0 || height <= 0)
        return 0;

    pixel_bytes = (size_t)width * (size_t)height * 4u;
    if (pixel_bytes == 0)
        return 0;

    if (state->scratch_bgra_capacity < pixel_bytes)
    {
        bgra = (unsigned char *)realloc(state->scratch_bgra, pixel_bytes);
        if (!bgra)
            return 0;
        state->scratch_bgra = bgra;
        state->scratch_bgra_capacity = pixel_bytes;
    }
    else
    {
        bgra = state->scratch_bgra;
    }

    for (i = 0; i < pixel_bytes; i += 4)
    {
        bgra[i + 0] = rgba[i + 2];
        bgra[i + 1] = rgba[i + 1];
        bgra[i + 2] = rgba[i + 0];
        bgra[i + 3] = rgba[i + 3];
    }

    memset(&file_header, 0, sizeof(file_header));
    memset(&info_header, 0, sizeof(info_header));

    file_header.bf_type = 0x4D42;
    file_header.bf_off_bits = sizeof(DemoFrameExportBmpFileHeader) + sizeof(DemoFrameExportBmpInfoHeader);
    file_header.bf_size = (unsigned int)(file_header.bf_off_bits + pixel_bytes);

    info_header.bi_size = sizeof(DemoFrameExportBmpInfoHeader);
    info_header.bi_width = width;
    info_header.bi_height = -height;
    info_header.bi_planes = 1;
    info_header.bi_bit_count = 32;
    info_header.bi_compression = 0;
    info_header.bi_size_image = (unsigned int)pixel_bytes;

    file = fopen(path, "wb");
    if (!file)
        return 0;

    fwrite(&file_header, sizeof(file_header), 1, file);
    fwrite(&info_header, sizeof(info_header), 1, file);
    fwrite(bgra, 1, pixel_bytes, file);
    fclose(file);
    return 1;
}

static int demo_frame_export_write_metadata(const DemoFrameExportState *state,
                                            const DemoFrameExportFrame *frame)
{
    char metadata_path[DEMO_FRAME_EXPORT_MAX_PATH];
    FILE *file = NULL;

    if (!state || !frame)
        return 0;

    snprintf(metadata_path, sizeof(metadata_path), "%s/%s", state->run_dir, frame->metadata_path);

    file = fopen(metadata_path, "w");
    if (!file)
        return 0;

    fprintf(file, "{\n");
    fprintf(file, "  \"frame_id\": ");
    demo_frame_export_write_json_string(file, frame->frame_id);
    fprintf(file, ",\n  \"run_id\": ");
    demo_frame_export_write_json_string(file, state->run_id);
    fprintf(file, ",\n  \"scenario\": ");
    demo_frame_export_write_json_string(file, frame->scenario);
    fprintf(file, ",\n  \"tick\": %d,\n", frame->tick);
    fprintf(file, "  \"timestamp_ms\": %lld,\n", (long long)frame->timestamp_ms);
    fprintf(file, "  \"image_path\": ");
    demo_frame_export_write_json_string(file, frame->image_path);
    fprintf(file, ",\n  \"metadata_path\": ");
    demo_frame_export_write_json_string(file, frame->metadata_path);
    fprintf(file, ",\n  \"width\": %d,\n", frame->width);
    fprintf(file, "  \"height\": %d,\n", frame->height);
    fprintf(file, "  \"render_backend\": ");
    demo_frame_export_write_json_string(file, frame->render_backend);
    fprintf(file, ",\n  \"ui_overlay\": %s,\n", frame->ui_overlay ? "true" : "false");
    fprintf(file, "  \"scene_variant\": %d,\n", frame->scene_variant);
    fprintf(file, "  \"scene_key\": ");
    demo_frame_export_write_json_string(file, frame->scene_key);
    fprintf(file, "\n}\n");

    fclose(file);
    return 1;
}

static int demo_frame_export_write_manifest(const DemoFrameExportState *state)
{
    DemoFrameExportFrame *frames = NULL;
    DemoFrameExportFailure *failures = NULL;
    char manifest_path[DEMO_FRAME_EXPORT_MAX_PATH];
    FILE *file = NULL;
    int i = 0;
    const char *status = "complete";

    if (!state)
        return 0;

    if (state->failure_count > 0)
        status = "failed";

    snprintf(manifest_path, sizeof(manifest_path), "%s/manifest.json", state->run_dir);

    file = fopen(manifest_path, "w");
    if (!file)
        return 0;

    fprintf(file, "{\n");
    fprintf(file, "  \"run_id\": ");
    demo_frame_export_write_json_string(file, state->run_id);
    fprintf(file, ",\n  \"created_at\": ");
    demo_frame_export_write_json_string(file, state->created_at);
    fprintf(file, ",\n  \"suite\": ");
    demo_frame_export_write_json_string(file, state->suite_label);
    fprintf(file, ",\n  \"scenarios\": [");
    for (i = 0; i < state->scenario_count; i++)
    {
        if (i > 0)
            fprintf(file, ", ");
        demo_frame_export_write_json_string(file, state->scenario_labels[i]);
    }
    fprintf(file, "],\n");
    fprintf(file, "  \"capture_interval_ticks\": %d,\n", state->capture_interval_ticks);
    fprintf(file, "  \"format\": ");
    demo_frame_export_write_json_string(file, state->format);
    fprintf(file, ",\n  \"output_dir\": ");
    demo_frame_export_write_json_string(file, state->output_dir);
    fprintf(file, ",\n  \"status\": ");
    demo_frame_export_write_json_string(file, status);
    fprintf(file, ",\n  \"frames\": [\n");

    frames = (DemoFrameExportFrame *)state->frames;
    for (i = 0; i < state->frame_count; i++)
    {
        const DemoFrameExportFrame *frame = &frames[i];
        fprintf(file, "    {\n");
        fprintf(file, "      \"frame_id\": ");
        demo_frame_export_write_json_string(file, frame->frame_id);
        fprintf(file, ",\n      \"scenario\": ");
        demo_frame_export_write_json_string(file, frame->scenario);
        fprintf(file, ",\n      \"tick\": %d,\n", frame->tick);
        fprintf(file, "      \"timestamp_ms\": %lld,\n", (long long)frame->timestamp_ms);
        fprintf(file, "      \"image_path\": ");
        demo_frame_export_write_json_string(file, frame->image_path);
        fprintf(file, ",\n      \"metadata_path\": ");
        demo_frame_export_write_json_string(file, frame->metadata_path);
        fprintf(file, ",\n      \"width\": %d,\n", frame->width);
        fprintf(file, "      \"height\": %d,\n", frame->height);
        fprintf(file, "      \"render_backend\": ");
        demo_frame_export_write_json_string(file, frame->render_backend);
        fprintf(file, ",\n      \"ui_overlay\": %s,\n", frame->ui_overlay ? "true" : "false");
        fprintf(file, "      \"scene_variant\": %d,\n", frame->scene_variant);
        fprintf(file, "      \"scene_key\": ");
        demo_frame_export_write_json_string(file, frame->scene_key);
        fprintf(file, "\n    }");
        if (i + 1 < state->frame_count)
            fprintf(file, ",");
        fprintf(file, "\n");
    }

    fprintf(file, "  ],\n  \"failures\": [\n");

    failures = (DemoFrameExportFailure *)state->failures;
    for (i = 0; i < state->failure_count; i++)
    {
        const DemoFrameExportFailure *failure = &failures[i];
        fprintf(file, "    {\n");
        fprintf(file, "      \"frame_id\": ");
        demo_frame_export_write_json_string(file, failure->frame_id);
        fprintf(file, ",\n      \"scenario\": ");
        demo_frame_export_write_json_string(file, failure->scenario);
        fprintf(file, ",\n      \"tick\": %d,\n", failure->tick);
        fprintf(file, "      \"error\": ");
        demo_frame_export_write_json_string(file, failure->error);
        fprintf(file, "\n    }");
        if (i + 1 < state->failure_count)
            fprintf(file, ",");
        fprintf(file, "\n");
    }

    fprintf(file, "  ]\n}\n");
    fclose(file);
    return 1;
}

void demo_frame_export_config_init(DemoFrameExportConfig *config)
{
    if (!config)
        return;

    memset(config, 0, sizeof(*config));
    demo_frame_export_copy_string(config->format, sizeof(config->format), DEMO_FRAME_EXPORT_DEFAULT_FORMAT);
}

void demo_frame_export_config_apply_env(DemoFrameExportConfig *config,
                                        int default_interval_ticks,
                                        const char *default_output_dir)
{
    const char *output_dir = NULL;
    const char *format = NULL;
    const char *suite = NULL;
    const char *run_label = NULL;
    int interval_ticks = 0;

    if (!config)
        return;

    if (demo_frame_export_parse_env_flag("BANANA_DEMO_FRAME_EXPORT"))
        config->enabled = 1;

    config->capture_interval_ticks = default_interval_ticks > 0 ? default_interval_ticks : 1;

    output_dir = getenv("BANANA_DEMO_FRAME_OUTPUT_DIR");
    if (!output_dir || output_dir[0] == '\0')
        output_dir = default_output_dir;

    if (output_dir && output_dir[0] != '\0')
    {
        demo_frame_export_copy_string(config->output_dir, sizeof(config->output_dir), output_dir);
        demo_frame_export_normalize_path(config->output_dir);
    }

    interval_ticks = demo_frame_export_parse_env_int("BANANA_DEMO_FRAME_INTERVAL",
                                                     config->capture_interval_ticks,
                                                     1,
                                                     60000);
    if (interval_ticks > 0)
        config->capture_interval_ticks = interval_ticks;

    format = getenv("BANANA_DEMO_FRAME_FORMAT");
    if (format && format[0] != '\0')
    {
        if (strcmp(format, "bmp") == 0 || strcmp(format, "BMP") == 0)
            demo_frame_export_copy_string(config->format, sizeof(config->format), "bmp");
        else
            demo_frame_export_copy_string(config->format, sizeof(config->format), DEMO_FRAME_EXPORT_DEFAULT_FORMAT);
    }

    suite = getenv("BANANA_DEMO_FRAME_SUITE");
    if (suite && suite[0] != '\0')
    {
        demo_frame_export_copy_string(config->suite_label, sizeof(config->suite_label), suite);
    }

    run_label = getenv("BANANA_DEMO_FRAME_RUN_LABEL");
    if (run_label && run_label[0] != '\0')
    {
        demo_frame_export_copy_string(config->run_label, sizeof(config->run_label), run_label);
    }
}

void demo_frame_export_state_init(DemoFrameExportState *state)
{
    if (!state)
        return;

    memset(state, 0, sizeof(*state));
}

int demo_frame_export_begin(DemoFrameExportState *state,
                            const DemoFrameExportConfig *config)
{
    const char *run_label = NULL;

    if (!state || !config)
        return 0;

    demo_frame_export_state_init(state);

    state->enabled = config->enabled ? 1 : 0;
    if (!state->enabled)
        return 1;

    state->capture_interval_ticks = config->capture_interval_ticks > 0 ? config->capture_interval_ticks : 1;
    demo_frame_export_copy_string(state->format,
                                  sizeof(state->format),
                                  config->format[0] != '\0' ? config->format : DEMO_FRAME_EXPORT_DEFAULT_FORMAT);
    demo_frame_export_copy_string(state->output_dir, sizeof(state->output_dir), config->output_dir);
    demo_frame_export_copy_string(state->suite_label, sizeof(state->suite_label), config->suite_label);
    demo_frame_export_copy_string(state->run_label, sizeof(state->run_label), config->run_label);

    demo_frame_export_timestamp_utc(state->created_at, sizeof(state->created_at));

    run_label = state->run_label[0] != '\0' ? state->run_label : state->suite_label;
    if (!run_label || run_label[0] == '\0')
        run_label = "run";

    demo_frame_export_run_id(state->run_id, sizeof(state->run_id), state->created_at, run_label);

    if (state->output_dir[0] == '\0')
    {
        demo_frame_export_record_failure(state, "", 0, "", "output_dir missing");
        return 0;
    }

    demo_frame_export_normalize_path(state->output_dir);

    if (!demo_frame_export_mkdirs(state->output_dir))
    {
        demo_frame_export_record_failure(state, "", 0, "", "failed to create output root");
        return 0;
    }

    snprintf(state->run_dir, sizeof(state->run_dir), "%s/%s", state->output_dir, state->run_id);
    demo_frame_export_normalize_path(state->run_dir);

    if (!demo_frame_export_mkdirs(state->run_dir))
    {
        demo_frame_export_record_failure(state, "", 0, "", "failed to create run directory");
        return 0;
    }

    {
        char frames_dir[DEMO_FRAME_EXPORT_MAX_PATH];
        char review_dir[DEMO_FRAME_EXPORT_MAX_PATH];

        snprintf(frames_dir, sizeof(frames_dir), "%s/frames", state->run_dir);
        snprintf(review_dir, sizeof(review_dir), "%s/review", state->run_dir);
        if (!demo_frame_export_mkdirs(frames_dir) || !demo_frame_export_mkdirs(review_dir))
        {
            demo_frame_export_record_failure(state, "", 0, "", "failed to create bundle folders");
            return 0;
        }
    }

    return 1;
}

int demo_frame_export_should_capture(const DemoFrameExportState *state, int tick)
{
    if (!state || !state->enabled)
        return 0;

    if (tick <= 0)
        return 0;

    if (state->capture_interval_ticks <= 0)
        return 0;

    if (tick == 1)
        return 1;

    return (tick % state->capture_interval_ticks) == 0;
}

int demo_frame_export_capture_engine_frame(DemoFrameExportState *state,
                                           int64_t timestamp_ms)
{
    int width = 0;
    int height = 0;
    const unsigned char *rgba = NULL;
    const char *backend = banana_render_backend_name(banana_render_backend_active());

    if (!state || !state->enabled)
        return 1;

    rgba = engine_get_frame_buffer();
    engine_get_frame_dimensions(&width, &height);

    return demo_frame_export_capture_rgba(state,
                                          rgba,
                                          width,
                                          height,
                                          timestamp_ms,
                                          backend,
                                          0);
}

int demo_frame_export_capture_rgba(DemoFrameExportState *state,
                                   const unsigned char *rgba,
                                   int width,
                                   int height,
                                   int64_t timestamp_ms,
                                   const char *render_backend,
                                   int ui_overlay)
{
    RuntimeEngineCaptureContext capture_context;
    DemoFrameExportFrame *frames = NULL;
    DemoFrameExportFrame frame;
    char scenario_dir[DEMO_FRAME_EXPORT_MAX_PATH];
    char sanitized_scenario[DEMO_FRAME_EXPORT_MAX_LABEL] = {0};
    char scenario_label[DEMO_FRAME_EXPORT_MAX_LABEL] = {0};
    char image_path_full[DEMO_FRAME_EXPORT_MAX_PATH];
    int64_t resolved_timestamp = timestamp_ms;
    int tick = 0;

    if (!state || !state->enabled)
        return 1;

    if (!rgba || width <= 0 || height <= 0)
    {
        demo_frame_export_record_failure(state, "unknown", 0, "", "missing frame buffer");
        return 0;
    }

    runtime_engine_host_get_capture_context(&capture_context);
    demo_frame_export_copy_string(scenario_label, sizeof(scenario_label), capture_context.scenario_name);
    if (scenario_label[0] == '\0')
        demo_frame_export_copy_string(scenario_label, sizeof(scenario_label), "unknown");

    demo_frame_export_sanitize_label(scenario_label, sanitized_scenario, sizeof(sanitized_scenario));
    if (sanitized_scenario[0] == '\0')
        demo_frame_export_copy_string(sanitized_scenario, sizeof(sanitized_scenario), "unknown");

    tick = capture_context.tick;

    if (resolved_timestamp <= 0)
        resolved_timestamp = demo_frame_export_now_ms();

    memset(&frame, 0, sizeof(frame));
    snprintf(frame.frame_id, sizeof(frame.frame_id), "%s-%d-%lld", sanitized_scenario, tick, (long long)resolved_timestamp);
    demo_frame_export_copy_string(frame.scenario, sizeof(frame.scenario), scenario_label);
    frame.tick = tick;
    frame.timestamp_ms = resolved_timestamp;
    snprintf(frame.image_path, sizeof(frame.image_path), "frames/%s/%d-%lld.%s",
             sanitized_scenario,
             tick,
             (long long)resolved_timestamp,
             state->format[0] != '\0' ? state->format : DEMO_FRAME_EXPORT_DEFAULT_FORMAT);
    snprintf(frame.metadata_path, sizeof(frame.metadata_path), "frames/%s/%d-%lld.json",
             sanitized_scenario,
             tick,
             (long long)resolved_timestamp);
    frame.width = width;
    frame.height = height;
    demo_frame_export_copy_string(frame.render_backend,
                                  sizeof(frame.render_backend),
                                  render_backend ? render_backend : "unknown");
    frame.ui_overlay = ui_overlay;
    frame.scene_variant = capture_context.scene_variant;
    demo_frame_export_copy_string(frame.scene_key,
                                  sizeof(frame.scene_key),
                                  capture_context.scene_key);

    snprintf(scenario_dir, sizeof(scenario_dir), "%s/frames/%s", state->run_dir, sanitized_scenario);
    if (!demo_frame_export_mkdirs(scenario_dir))
    {
        demo_frame_export_record_failure(state, scenario_label, tick, frame.frame_id, "failed to create scenario directory");
        return 0;
    }

    snprintf(image_path_full, sizeof(image_path_full), "%s/%s", state->run_dir, frame.image_path);
    if (!demo_frame_export_write_bmp(state, image_path_full, rgba, width, height))
    {
        demo_frame_export_record_failure(state, scenario_label, tick, frame.frame_id, "failed to write bmp");
        return 0;
    }

    if (!demo_frame_export_write_metadata(state, &frame))
    {
        demo_frame_export_record_failure(state, scenario_label, tick, frame.frame_id, "failed to write metadata");
        return 0;
    }

    if (!demo_frame_export_reserve_frames(state, state->frame_count + 1))
    {
        demo_frame_export_record_failure(state, scenario_label, tick, frame.frame_id, "failed to reserve frame list");
        return 0;
    }

    frames = (DemoFrameExportFrame *)state->frames;
    frames[state->frame_count] = frame;
    state->frame_count++;
    demo_frame_export_track_scenario(state, scenario_label);
    return 1;
}

int demo_frame_export_finalize(DemoFrameExportState *state)
{
    int result = 1;

    if (!state)
        return 0;

    if (!state->enabled)
        return 1;

    if (state->frame_count == 0 && state->failure_count == 0)
    {
        demo_frame_export_record_failure(state, "", 0, "", "no frames captured");
    }

    if (!demo_frame_export_write_manifest(state))
        result = 0;

    if (state->failure_count > 0)
        result = 0;

    free(state->frames);
    free(state->failures);
    free(state->scratch_bgra);

    state->frames = NULL;
    state->failures = NULL;
    state->scratch_bgra = NULL;
    state->frame_count = 0;
    state->frame_capacity = 0;
    state->failure_count = 0;
    state->failure_capacity = 0;
    state->scratch_bgra_capacity = 0;
    state->enabled = 0;

    return result;
}

int demo_frame_export_has_failures(const DemoFrameExportState *state)
{
    if (!state)
        return 0;

    return (state->failure_count > 0) ? 1 : 0;
}

int demo_frame_export_is_enabled(const DemoFrameExportState *state)
{
    if (!state)
        return 0;

    return state->enabled;
}