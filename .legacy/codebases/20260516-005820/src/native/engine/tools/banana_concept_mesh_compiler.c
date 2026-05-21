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

typedef struct split_mesh_spec
{
    const char *id;
    const char *bone;
    double width;
    double height;
    double depth;
    const char *material;
} split_mesh_spec_t;

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

    int written = snprintf(buffer, size, "%s%s%s", dir, needs_sep ? (char[]){sep, '\0'} : "",
                           file);
    return written > 0 && (size_t)written < size;
}

static int write_split_mesh(const char *mesh_path, const split_mesh_spec_t *mesh)
{
    FILE *f = fopen(mesh_path, "w");
    if (!f || !mesh)
    {
        if (f)
        {
            fclose(f);
        }
        return 0;
    }

    const double hw = mesh->width * 0.5;
    const double hh = mesh->height * 0.5;

    fprintf(f,
            "{\n"
            "  \"kind\": \"banana-split-mesh-2p5d\",\n"
            "  \"id\": \"%s\",\n"
            "  \"bone\": \"%s\",\n"
            "  \"depth\": %.3f,\n"
            "  \"material\": \"%s\",\n"
            "  \"vertices\": [\n"
            "    {\"x\": %.4f, \"y\": %.4f, \"u\": 0.0, \"v\": 0.0},\n"
            "    {\"x\": %.4f, \"y\": %.4f, \"u\": 1.0, \"v\": 0.0},\n"
            "    {\"x\": %.4f, \"y\": %.4f, \"u\": 1.0, \"v\": 1.0},\n"
            "    {\"x\": %.4f, \"y\": %.4f, \"u\": 0.0, \"v\": 1.0}\n"
            "  ],\n"
            "  \"indices\": [0, 1, 2, 0, 2, 3]\n"
            "}\n",
            mesh->id, mesh->bone, mesh->depth, mesh->material, -hw, hh, hw, hh, hw, -hh, -hw,
            -hh);

    fclose(f);
    return 1;
}

static int write_manifest(const char *manifest_path, const split_mesh_spec_t *meshes,
                          size_t mesh_count)
{
    FILE *f = fopen(manifest_path, "w");
    if (!f)
    {
        return 0;
    }

    fprintf(f,
            "{\n"
            "  \"kind\": \"banana-engineer-concept-assets\",\n"
            "  \"theme\": \"Banana.Engineer\",\n"
            "  \"assetType\": \"split-meshes\",\n"
            "  \"meshes\": [\n");

    for (size_t i = 0; i < mesh_count; i++)
    {
        fprintf(f,
                "    {\"id\":\"%s\",\"file\":\"meshes/%s.mesh2p5d.json\",\"bone\":\"%s\"}%s\n",
                meshes[i].id, meshes[i].id, meshes[i].bone, (i + 1 == mesh_count) ? "" : ",");
    }

    fprintf(f,
            "  ],\n"
            "  \"tagline\": \"In this world, civilization is just another optimization problem.\"\n"
            "}\n");

    fclose(f);
    return 1;
}

int main(int argc, char **argv)
{
    const char *out_dir = "artifacts/generated-concepts";

    for (int i = 1; i < argc; i++)
    {
        const char *arg = argv[i];
        if (strncmp(arg, "--out-dir=", 10) == 0)
        {
            out_dir = arg + 10;
        }
        else if (strcmp(arg, "--out-dir") == 0 && i + 1 < argc)
        {
            out_dir = argv[++i];
        }
    }

    char mesh_dir[1200];
    char manifest_path[1200];
    if (!build_path(mesh_dir, sizeof(mesh_dir), out_dir, "meshes") ||
        !build_path(manifest_path, sizeof(manifest_path), out_dir,
                    "banana-engineer-concept-assets.manifest.json"))
    {
        fprintf(stderr, "[banana-concept-mesh-compiler] output path is too long\n");
        return 1;
    }

    if (!ensure_dir_exists(out_dir) || !ensure_dir_exists(mesh_dir))
    {
        fprintf(stderr, "[banana-concept-mesh-compiler] failed to create output dir %s\n",
                out_dir);
        return 1;
    }

    const split_mesh_spec_t meshes[] = {
        {"banana_head", "head", 0.38, 0.46, 0.58, "banana_skin"},
        {"banana_torso", "spine", 0.52, 0.58, 0.44, "cloth_primary"},
        {"banana_arm_l", "arm_l", 0.22, 0.40, 0.52, "cloth_secondary"},
        {"banana_arm_r", "arm_r", 0.22, 0.40, 0.52, "cloth_secondary"},
        {"banana_leg_l", "leg_l", 0.20, 0.42, 0.36, "leather"},
        {"banana_leg_r", "leg_r", 0.20, 0.42, 0.36, "leather"},
        {"banana_cape", "cape", 0.62, 0.62, 0.20, "cape_red"},
        {"banana_sword", "sword", 0.56, 0.10, 0.62, "metal_blade"},
    };

    for (size_t i = 0; i < sizeof(meshes) / sizeof(meshes[0]); i++)
    {
        char mesh_file_name[256];
        char mesh_path[1200];
        snprintf(mesh_file_name, sizeof(mesh_file_name), "%s.mesh2p5d.json", meshes[i].id);
        if (!build_path(mesh_path, sizeof(mesh_path), mesh_dir, mesh_file_name) ||
            !write_split_mesh(mesh_path, &meshes[i]))
        {
            fprintf(stderr, "[banana-concept-mesh-compiler] failed to write mesh %s\n",
                    meshes[i].id);
            return 1;
        }
    }

    if (!write_manifest(manifest_path, meshes, sizeof(meshes) / sizeof(meshes[0])))
    {
        fprintf(stderr, "[banana-concept-mesh-compiler] failed to write concept manifest\n");
        return 1;
    }

    printf("[banana-concept-mesh-compiler] generated split meshes in %s\n", out_dir);
    return 0;
}
