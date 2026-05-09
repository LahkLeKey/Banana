#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

#include "application/banana_asset_generation_service.h"
#include "banana_asset_types.h"
#include "infrastructure/banana_asset_json_writer.h"

static int parse_int_arg(const char *value, int fallback)
{
    if (!value || !*value)
    {
        return fallback;
    }

    char *end = NULL;
    long parsed = strtol(value, &end, 10);
    if (end == value || *end != '\0')
    {
        return fallback;
    }

    if (parsed < 1)
    {
        return fallback;
    }

    return (int)parsed;
}

static int ensure_dir_exists(const char *path)
{
    if (!path || !*path)
    {
        return 0;
    }

    char buffer[1024];
    size_t length = strlen(path);
    if (length >= sizeof(buffer))
    {
        return 0;
    }

    memcpy(buffer, path, length + 1);

    for (size_t i = 1; i < length; i++)
    {
        if (buffer[i] == '/' || buffer[i] == '\\')
        {
            char separator = buffer[i];
            buffer[i] = '\0';
            if (strlen(buffer) > 0)
            {
                if (MKDIR(buffer) != 0 && errno != EEXIST)
                {
                    return 0;
                }
            }
            buffer[i] = separator;
        }
    }

    if (MKDIR(buffer) != 0 && errno != EEXIST)
    {
        return 0;
    }

    return 1;
}

static int build_path(char *buffer, size_t size, const char *dir, const char *file)
{
    if (!buffer || !dir || !file)
    {
        return 0;
    }

    size_t dir_len = strlen(dir);
    int needs_sep = (dir_len > 0 && dir[dir_len - 1] != '/' && dir[dir_len - 1] != '\\');

#ifdef _WIN32
    const char sep = '\\';
#else
    const char sep = '/';
#endif

    int written = snprintf(buffer, size, "%s%s%s", dir, needs_sep ? (char[]){sep, '\0'} : "", file);
    return written > 0 && (size_t)written < size;
}

int main(int argc, char **argv)
{
    banana_compiler_config_t config = {
        .seed = 1337u,
        .profile = "default",
        .out_dir = "artifacts/generated-assets",
        .width = 32,
        .height = 32,
    };

    int cellular_iterations = 3;

    for (int i = 1; i < argc; i++)
    {
        const char *arg = argv[i];
        if (strncmp(arg, "--seed=", 7) == 0)
        {
            config.seed = (unsigned int)strtoul(arg + 7, NULL, 10);
        }
        else if (strcmp(arg, "--seed") == 0 && i + 1 < argc)
        {
            config.seed = (unsigned int)strtoul(argv[++i], NULL, 10);
        }
        else if (strncmp(arg, "--profile=", 10) == 0)
        {
            config.profile = arg + 10;
        }
        else if (strcmp(arg, "--profile") == 0 && i + 1 < argc)
        {
            config.profile = argv[++i];
        }
        else if (strncmp(arg, "--out=", 6) == 0)
        {
            config.out_dir = arg + 6;
        }
        else if (strncmp(arg, "--out-dir=", 10) == 0)
        {
            config.out_dir = arg + 10;
        }
        else if ((strcmp(arg, "--out") == 0 || strcmp(arg, "--out-dir") == 0) && i + 1 < argc)
        {
            config.out_dir = argv[++i];
        }
        else if (strncmp(arg, "--width=", 8) == 0)
        {
            config.width = parse_int_arg(arg + 8, config.width);
        }
        else if (strcmp(arg, "--width") == 0 && i + 1 < argc)
        {
            config.width = parse_int_arg(argv[++i], config.width);
        }
        else if (strncmp(arg, "--height=", 9) == 0)
        {
            config.height = parse_int_arg(arg + 9, config.height);
        }
        else if (strcmp(arg, "--height") == 0 && i + 1 < argc)
        {
            config.height = parse_int_arg(argv[++i], config.height);
        }
        else if (strncmp(arg, "--ca-iterations=", 16) == 0)
        {
            cellular_iterations = parse_int_arg(arg + 16, cellular_iterations);
        }
        else if (strcmp(arg, "--ca-iterations") == 0 && i + 1 < argc)
        {
            cellular_iterations = parse_int_arg(argv[++i], cellular_iterations);
        }
    }

    if (!ensure_dir_exists(config.out_dir))
    {
        fprintf(stderr, "[banana-asset-compiler] failed to create output directory: %s\n",
                config.out_dir);
        return 1;
    }

    int cell_count = config.width * config.height;
    banana_terrain_cell_t *cells =
        (banana_terrain_cell_t *)malloc((size_t)cell_count * sizeof(banana_terrain_cell_t));
    if (!cells)
    {
        fprintf(stderr, "[banana-asset-compiler] out of memory\n");
        return 1;
    }

    uint64_t checksum = 0ull;
    if (!banana_generate_terrain_cells(&config, cellular_iterations, cells, &checksum))
    {
        free(cells);
        fprintf(stderr, "[banana-asset-compiler] terrain generation failed\n");
        return 1;
    }

    char palette_path[1200];
    char terrain_path[1200];
    char manifest_path[1200];

    if (!build_path(palette_path, sizeof(palette_path), config.out_dir,
                    "banana-generated-palette.json") ||
        !build_path(terrain_path, sizeof(terrain_path), config.out_dir,
                    "banana-generated-terrain.json") ||
        !build_path(manifest_path, sizeof(manifest_path), config.out_dir,
                    "banana-generated-assets.manifest.json"))
    {
        free(cells);
        fprintf(stderr, "[banana-asset-compiler] output path is too long\n");
        return 1;
    }

    if (!banana_write_palette_json(palette_path) ||
        !banana_write_terrain_json(terrain_path, &config, cells, checksum, cellular_iterations) ||
        !banana_write_manifest_json(manifest_path, &config, checksum, cellular_iterations))
    {
        free(cells);
        fprintf(stderr, "[banana-asset-compiler] failed to write one or more output files\n");
        return 1;
    }

    free(cells);

    printf("[banana-asset-compiler] Generated assets in %s (seed=%u, profile=%s, size=%dx%d)\n",
           config.out_dir, config.seed, config.profile, config.width, config.height);

    return 0;
}
