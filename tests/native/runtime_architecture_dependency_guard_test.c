#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime_architecture_guard_rules.h"

#ifndef BANANA_ENGINE_SOURCE_ROOT
#define BANANA_ENGINE_SOURCE_ROOT "."
#endif

static int read_text_file(const char *path, char **out_content)
{
    FILE *file = NULL;
    long length = 0;
    size_t read_bytes = 0;
    char *buffer = NULL;

    if (!path || !out_content)
        return 0;

    *out_content = NULL;
    file = fopen(path, "rb");
    if (!file)
        return 0;

    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        return 0;
    }

    length = ftell(file);
    if (length < 0)
    {
        fclose(file);
        return 0;
    }

    if (fseek(file, 0, SEEK_SET) != 0)
    {
        fclose(file);
        return 0;
    }

    buffer = (char *)malloc((size_t)length + 1u);
    if (!buffer)
    {
        fclose(file);
        return 0;
    }

    read_bytes = fread(buffer, 1u, (size_t)length, file);
    fclose(file);

    if ((long)read_bytes != length)
    {
        free(buffer);
        return 0;
    }

    buffer[length] = '\0';
    *out_content = buffer;
    return 1;
}

static int check_rule(const FileRule *rule)
{
    char full_path[1024] = {0};
    char *content = NULL;
    int i = 0;
    int failures = 0;

    if (!rule)
        return 1;

    (void)snprintf(full_path,
                   sizeof(full_path),
                   "%s/%s",
                   BANANA_ENGINE_SOURCE_ROOT,
                   rule->relative_path);

    if (!read_text_file(full_path, &content))
    {
        fprintf(stderr, "architecture-guard: unable to read %s (%s)\n",
                rule->relative_path,
                rule->description);
        return 0;
    }

    for (i = 0; i < rule->forbidden_count; ++i)
    {
        const char *pattern = rule->forbidden_patterns[i];
        if (pattern && strstr(content, pattern))
        {
            fprintf(stderr,
                    "architecture-guard: %s violated by pattern '%s' in %s\n",
                    rule->description,
                    pattern,
                    rule->relative_path);
            failures += 1;
        }
    }

    free(content);
    return failures == 0;
}

int main(void)
{
    int ok = 1;
    const int rule_count = runtime_architecture_guard_rule_count();
    int i = 0;

    for (i = 0; i < rule_count; ++i)
    {
        if (!check_rule(&kRuntimeArchitectureGuardRules[i]))
            ok = 0;
    }

    if (!ok)
        return 1;

    printf("runtime architecture dependency guards validated\n");
    return 0;
}
