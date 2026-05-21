#include "engine_serialize.h"

#include "engine.h"
#include "world/world.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define BANANA_ENGINE_SNAPSHOT_BUFFER_SIZE 65536

static char s_snapshot_buffer[BANANA_ENGINE_SNAPSHOT_BUFFER_SIZE];
static unsigned long long s_snapshot_tick = 0;

static unsigned long long banana_now_ms(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (unsigned long long)ts.tv_sec * 1000ull + (unsigned long long)ts.tv_nsec / 1000000ull;
}

static int appendf(char *buffer, size_t buffer_size, size_t *offset, const char *format, ...)
{
    va_list args;
    int written = 0;

    if (*offset >= buffer_size)
        return 0;

    va_start(args, format);
    written = vsnprintf(buffer + *offset, buffer_size - *offset, format, args);
    va_end(args);

    if (written < 0)
        return 0;

    if ((size_t)written >= buffer_size - *offset)
    {
        buffer[0] = '\0';
        return 0;
    }

    *offset += (size_t)written;
    return 1;
}

static const char *entity_type_name(EntityType type)
{
    switch (type)
    {
    case ENTITY_TYPE_STATIC:
        return "static";
    case ENTITY_TYPE_DYNAMIC:
        return "dynamic";
    case ENTITY_TYPE_NPC:
        return "npc";
    case ENTITY_TYPE_PLAYER:
        return "player";
    case ENTITY_TYPE_TRIGGER:
        return "trigger";
    default:
        return "unknown";
    }
}

void engine_serialize_reset(void)
{
    s_snapshot_tick = 0;
    snprintf(s_snapshot_buffer, sizeof(s_snapshot_buffer),
             "{\"tick\":0,\"timestamp\":0,\"isAuthoritative\":true,\"entities\":{}}");
}

const char *engine_serialize_world(void)
{
    const World *world = world_get();
    size_t offset = 0;
    const char *separator = "";

    if (!world)
    {
        snprintf(s_snapshot_buffer, sizeof(s_snapshot_buffer),
                 "{\"error\":\"engine_not_initialized\"}");
        return s_snapshot_buffer;
    }

    if (!appendf(s_snapshot_buffer, sizeof(s_snapshot_buffer), &offset,
                 "{\"tick\":%llu,\"timestamp\":%llu,\"isAuthoritative\":true,\"entities\":{",
                 s_snapshot_tick, banana_now_ms()))
    {
        snprintf(s_snapshot_buffer, sizeof(s_snapshot_buffer),
                 "{\"error\":\"snapshot_serialization_failed\"}");
        return s_snapshot_buffer;
    }

    for (int i = 0; i < world->entity_count; i++)
    {
        const Entity *entity = world->entities[i];
        const char *metadata_suffix = "";
        if (!entity)
            continue;

        if (entity->type == ENTITY_TYPE_PLAYER && entity->player_guid[0] != '\0')
        {
            if (entity->controller_kind[0] != '\0')
            {
                metadata_suffix =
                    "\",\"playerGuid\":\"%s\",\"controllerKind\":\"%s\"";
            }
            else
            {
                metadata_suffix = "\",\"playerGuid\":\"%s\"";
            }
        }

        if (metadata_suffix[0] == '\0')
        {
            if (!appendf(s_snapshot_buffer, sizeof(s_snapshot_buffer), &offset,
                         "%s\"entity-%u\":{\"id\":%u,\"type\":\"%s\",\"active\":%s,\"x\":%.3f,\"y\":%.3f,"
                         "\"z\":%.3f,\"state\":%d}",
                         separator, entity->id, entity->id, entity_type_name(entity->type),
                         entity->active ? "true" : "false", entity->position[0],
                         entity->position[1], entity->position[2],
                         entity->active ? engine_get_entity_state(i) : 0))
            {
                snprintf(s_snapshot_buffer, sizeof(s_snapshot_buffer),
                         "{\"error\":\"snapshot_too_large\"}");
                return s_snapshot_buffer;
            }
        }
        else if (entity->controller_kind[0] != '\0')
        {
            if (!appendf(s_snapshot_buffer, sizeof(s_snapshot_buffer), &offset,
                         "%s\"entity-%u\":{\"id\":%u,\"type\":\"%s\",\"active\":%s,\"x\":%.3f,\"y\":%.3f,"
                         "\"z\":%.3f,\"state\":%d,\"playerGuid\":\"%s\",\"controllerKind\":\"%s\"}",
                         separator, entity->id, entity->id, entity_type_name(entity->type),
                         entity->active ? "true" : "false", entity->position[0],
                         entity->position[1], entity->position[2],
                         entity->active ? engine_get_entity_state(i) : 0, entity->player_guid,
                         entity->controller_kind))
            {
                snprintf(s_snapshot_buffer, sizeof(s_snapshot_buffer),
                         "{\"error\":\"snapshot_too_large\"}");
                return s_snapshot_buffer;
            }
        }
        else if (!appendf(s_snapshot_buffer, sizeof(s_snapshot_buffer), &offset,
                          "%s\"entity-%u\":{\"id\":%u,\"type\":\"%s\",\"active\":%s,\"x\":%.3f,\"y\":%.3f,"
                          "\"z\":%.3f,\"state\":%d,\"playerGuid\":\"%s\"}",
                          separator, entity->id, entity->id, entity_type_name(entity->type),
                          entity->active ? "true" : "false", entity->position[0],
                          entity->position[1], entity->position[2],
                          entity->active ? engine_get_entity_state(i) : 0,
                          entity->player_guid))
        {
            snprintf(s_snapshot_buffer, sizeof(s_snapshot_buffer),
                     "{\"error\":\"snapshot_too_large\"}");
            return s_snapshot_buffer;
        }

        separator = ",";
    }

    if (!appendf(s_snapshot_buffer, sizeof(s_snapshot_buffer), &offset, "}}"))
    {
        snprintf(s_snapshot_buffer, sizeof(s_snapshot_buffer),
                 "{\"error\":\"snapshot_serialization_failed\"}");
    }

    return s_snapshot_buffer;
}

const char *engine_tick_serialize(float dt)
{
    if (engine_tick(dt) != 0)
    {
        snprintf(s_snapshot_buffer, sizeof(s_snapshot_buffer),
                 "{\"error\":\"engine_tick_failed\"}");
        return s_snapshot_buffer;
    }

    s_snapshot_tick += 1;
    return engine_serialize_world();
}
