#include <errno.h>
#include <math.h>
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

typedef struct clip_spec
{
    const char *name;
    int frame_count;
    int fps;
    int loop;
} clip_spec_t;

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

static int write_vector_shapes(const char *file_path)
{
    FILE *f = fopen(file_path, "w");
    if (!f)
    {
        return 0;
    }

    fprintf(f,
            "{\n"
            "  \"kind\": \"banana-character-vectors\",\n"
            "  \"layers\": [\n"
            "    {\"id\":\"head\",\"shape\":\"rounded-rect\",\"size\":[0.38,0.46],\"color\":\"#ffd84a\"},\n"
            "    {\"id\":\"body\",\"shape\":\"rounded-rect\",\"size\":[0.52,0.58],\"color\":\"#f2c56a\"},\n"
            "    {\"id\":\"cape\",\"shape\":\"poly\",\"points\":[[-0.24,0.18],[0.0,-0.44],[0.24,0.18]],\"color\":\"#cf3047\"},\n"
            "    {\"id\":\"sword\",\"shape\":\"poly\",\"points\":[[-0.04,0.0],[0.48,0.02],[0.52,-0.02],[-0.04,-0.02]],\"color\":\"#eaf3ff\"}\n"
            "  ]\n"
            "}\n");

    fclose(f);
    return 1;
}

static int write_clip_json(const char *file_path, const clip_spec_t *clip)
{
    FILE *f = fopen(file_path, "w");
    if (!f || !clip)
    {
        if (f)
        {
            fclose(f);
        }
        return 0;
    }

    fprintf(f,
            "{\n"
            "  \"kind\": \"banana-character-clip\",\n"
            "  \"name\": \"%s\",\n"
            "  \"fps\": %d,\n"
            "  \"loop\": %s,\n"
            "  \"frames\": [\n",
            clip->name, clip->fps, clip->loop ? "true" : "false");

    for (int i = 0; i < clip->frame_count; i++)
    {
        double phase = (double)i / (double)(clip->frame_count > 1 ? clip->frame_count - 1 : 1);
        double arm_swing = sin(phase * 6.283185307179586) * 24.0;
        double leg_swing = sin(phase * 6.283185307179586 + 3.141592653589793) * 18.0;
        double bob = sin(phase * 6.283185307179586) * 0.035;
        double cape_drag = sin(phase * 6.283185307179586 + 1.047197551196598) * 12.0;

        if (strcmp(clip->name, "idle") == 0)
        {
            arm_swing *= 0.28;
            leg_swing *= 0.18;
            bob *= 0.35;
            cape_drag *= 0.22;
        }
        else if (strcmp(clip->name, "jump") == 0)
        {
            arm_swing *= 0.5;
            leg_swing *= -0.4;
            bob = sin(phase * 3.141592653589793) * 0.22;
            cape_drag *= 0.6;
        }
        else if (strcmp(clip->name, "slash") == 0)
        {
            arm_swing = -18.0 + phase * 120.0;
            leg_swing *= 0.35;
            bob *= 0.4;
            cape_drag = 6.0 + sin(phase * 12.566370614359172) * 8.0;
        }
        else if (strcmp(clip->name, "cast") == 0)
        {
            arm_swing = 8.0 + sin(phase * 9.424777960769379) * 26.0;
            leg_swing *= 0.1;
            bob *= 0.55;
            cape_drag *= 0.9;
        }
        else if (strcmp(clip->name, "hurt") == 0)
        {
            arm_swing = sin(phase * 18.849555921538759) * 9.0;
            leg_swing = sin(phase * 12.566370614359172) * 7.0;
            bob = sin(phase * 6.283185307179586) * -0.06;
            cape_drag *= 0.25;
        }

        fprintf(f,
                "    {\"index\":%d,\"bones\":{\"hips\":{\"ty\":%.4f},\"arm_l\":{\"rot\":%.2f},\"arm_r\":{\"rot\":%.2f},\"leg_l\":{\"rot\":%.2f},\"leg_r\":{\"rot\":%.2f},\"cape\":{\"rot\":%.2f}}}%s\n",
                i, bob, -arm_swing, arm_swing, leg_swing, -leg_swing, cape_drag,
                (i == clip->frame_count - 1) ? "" : ",");
    }

    fprintf(f, "  ]\n}\n");
    fclose(f);
    return 1;
}

static int write_character_json(const char *file_path, const char *character_id)
{
    FILE *f = fopen(file_path, "w");
    if (!f)
    {
        return 0;
    }

    fprintf(
        f,
        "{\n"
        "  \"kind\": \"banana-character-2p5d\",\n"
        "  \"id\": \"%s\",\n"
        "  \"displayName\": \"Banana Tempest\",\n"
        "  \"renderModel\": {\"type\":\"billboard-bone-layers\",\"cameraFacing\":true,\"depthParallax\":0.22},\n"
        "  \"vectorSource\": \"vectors/rig.vec2.json\",\n"
        "  \"clips\": [\n"
        "    {\"name\":\"idle\",\"file\":\"clips/idle.vec2anim.json\",\"fps\":12},\n"
        "    {\"name\":\"run\",\"file\":\"clips/run.vec2anim.json\",\"fps\":18},\n"
        "    {\"name\":\"jump\",\"file\":\"clips/jump.vec2anim.json\",\"fps\":18},\n"
        "    {\"name\":\"slash\",\"file\":\"clips/slash.vec2anim.json\",\"fps\":20},\n"
        "    {\"name\":\"cast\",\"file\":\"clips/cast.vec2anim.json\",\"fps\":16},\n"
        "    {\"name\":\"hurt\",\"file\":\"clips/hurt.vec2anim.json\",\"fps\":14}\n"
        "  ]\n"
        "}\n",
        character_id);

    fclose(f);
    return 1;
}

int main(int argc, char **argv)
{
    const char *out_dir = "artifacts/generated-characters";
    const char *character_id = "banana-tempest";

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
        else if (strncmp(arg, "--character-id=", 15) == 0)
        {
            character_id = arg + 15;
        }
        else if (strcmp(arg, "--character-id") == 0 && i + 1 < argc)
        {
            character_id = argv[++i];
        }
    }

    char character_dir[1200];
    char vectors_dir[1200];
    char clips_dir[1200];
    char character_json[1200];
    char vectors_json[1200];

    if (!build_path(character_dir, sizeof(character_dir), out_dir, character_id) ||
        !build_path(vectors_dir, sizeof(vectors_dir), character_dir, "vectors") ||
        !build_path(clips_dir, sizeof(clips_dir), character_dir, "clips") ||
        !build_path(character_json, sizeof(character_json), character_dir, "character.2p5d.json") ||
        !build_path(vectors_json, sizeof(vectors_json), vectors_dir, "rig.vec2.json"))
    {
        fprintf(stderr, "[banana-character-vector-compiler] output path is too long\n");
        return 1;
    }

    if (!ensure_dir_exists(character_dir) || !ensure_dir_exists(vectors_dir) ||
        !ensure_dir_exists(clips_dir))
    {
        fprintf(stderr,
                "[banana-character-vector-compiler] failed to create output directories under %s\n",
                out_dir);
        return 1;
    }

    if (!write_character_json(character_json, character_id) || !write_vector_shapes(vectors_json))
    {
        fprintf(stderr, "[banana-character-vector-compiler] failed to write character core files\n");
        return 1;
    }

    const clip_spec_t clips[] = {
        {"idle", 24, 12, 1}, {"run", 20, 18, 1}, {"jump", 14, 18, 0},
        {"slash", 16, 20, 0}, {"cast", 22, 16, 0}, {"hurt", 10, 14, 0},
    };

    for (size_t i = 0; i < sizeof(clips) / sizeof(clips[0]); i++)
    {
        char clip_file_name[128];
        char clip_file_path[1200];
        snprintf(clip_file_name, sizeof(clip_file_name), "%s.vec2anim.json", clips[i].name);
        if (!build_path(clip_file_path, sizeof(clip_file_path), clips_dir, clip_file_name) ||
            !write_clip_json(clip_file_path, &clips[i]))
        {
            fprintf(stderr,
                    "[banana-character-vector-compiler] failed to write clip %s for %s\n",
                    clips[i].name, character_id);
            return 1;
        }
    }

    printf("[banana-character-vector-compiler] generated 2.5D vector character pack in %s\n",
           character_dir);
    return 0;
}
