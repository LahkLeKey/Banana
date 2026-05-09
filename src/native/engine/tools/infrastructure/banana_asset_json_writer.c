#include "banana_asset_json_writer.h"

#include <stdio.h>
#include <string.h>

#include "../domain/banana_asset_domain.h"

int banana_write_palette_json(const char *path)
{
    FILE *f = fopen(path, "w");
    if (!f)
    {
        return 0;
    }

    const banana_tile_def_t *defs = banana_tile_defs();
    fprintf(f, "{\n  \"tiles\": [\n");
    for (int i = 0; i < banana_tile_count(); i++)
    {
        fprintf(f, "    {\"id\":\"%s\",\"biome\":%d,\"elevationBase\":%d,\"resourceBias\":%d}%s\n",
                defs[i].id, defs[i].biome, defs[i].elevation_base, defs[i].resource_bias,
                (i + 1 < banana_tile_count()) ? "," : "");
    }
    fprintf(f, "  ]\n}\n");

    fclose(f);
    return 1;
}

int banana_write_terrain_json(const char *path, const banana_compiler_config_t *config,
                              const banana_terrain_cell_t *cells, uint64_t checksum,
                              int cellular_iterations)
{
    FILE *f = fopen(path, "w");
    if (!f)
    {
        return 0;
    }

    const banana_tile_def_t *defs = banana_tile_defs();
    int cell_count = config->width * config->height;

    fprintf(f,
            "{\n"
            "  \"seed\": %u,\n"
            "  \"profile\": \"%s\",\n"
            "  \"algorithm\": \"wave-function-collapse+cellular-automata\",\n"
            "  \"postProcess\": {\"cellularAutomataIterations\": %d},\n"
            "  \"width\": %d,\n"
            "  \"height\": %d,\n"
            "  \"cells\": [\n",
            config->seed, config->profile, cellular_iterations, config->width, config->height);

    for (int i = 0; i < cell_count; i++)
    {
        int x = i % config->width;
        int y = i / config->width;

        fprintf(f, "    {\"x\":%d,\"y\":%d,\"tile\":\"%s\",\"elevation\":%d,\"resource\":%d}%s\n",
                x, y, defs[cells[i].tile_index].id, cells[i].elevation, cells[i].resource,
                (i + 1 < cell_count) ? "," : "");
    }

    fprintf(f,
            "  ],\n"
            "  \"checksum\": \"0x%016llX\"\n"
            "}\n",
            (unsigned long long)checksum);

    fclose(f);
    return 1;
}

int banana_write_manifest_json(const char *path, const banana_compiler_config_t *config,
                               uint64_t checksum, int cellular_iterations)
{
    FILE *f = fopen(path, "w");
    if (!f)
    {
        return 0;
    }

    fprintf(f,
            "{\n"
            "  \"kind\": \"banana-generated-assets\",\n"
            "  \"compiler\": \"banana_asset_compiler\",\n"
            "  \"profile\": \"%s\",\n"
            "  \"seed\": %u,\n"
            "  \"width\": %d,\n"
            "  \"height\": %d,\n"
            "  \"algorithm\": \"wave-function-collapse+cellular-automata\",\n"
            "  \"postProcess\": {\"cellularAutomataIterations\": %d},\n"
            "  \"rules\": {\n"
            "    \"tileVocabulary\": \"banana_tile_defs\",\n"
            "    \"adjacency\": \"neighbor biomes differ by at most 1\",\n"
            "    \"tileWeighting\": \"weighted random selection from tile definitions\",\n"
            "    \"elevationNoiseRange\": [-5, 5],\n"
            "    \"resourceNoiseRange\": [-3, 3]\n"
            "  },\n"
            "  \"files\": [\n"
            "    {\"name\":\"palette\",\"file\":\"banana-generated-palette.json\"},\n"
            "    "
            "{\"name\":\"terrain\",\"file\":\"banana-generated-terrain.json\",\"checksum\":\"0x%"
            "016llX\"}\n"
            "  ]\n"
            "}\n",
            config->profile, config->seed, config->width, config->height, cellular_iterations,
            (unsigned long long)checksum);

    fclose(f);
    return 1;
}
