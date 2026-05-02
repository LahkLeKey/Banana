#include "banana_ml_shared.h"
#include "banana_status.h"

#include <ctype.h>
#include <stdint.h>
#include <string.h>

extern const char *const k_banana_signal_tokens[];
extern const size_t k_banana_signal_tokens_count;

typedef struct BananaMlBigramPattern
{
    const char *first;
    const char *second;
} BananaMlBigramPattern;

static const char *const k_not_banana_signal_tokens[] = {
    "plastic", "battery",  "engine",   "asphalt", "steel", "concrete", "tire",
    "diesel",  "gasoline", "chemical", "garbage", "waste", "junk",     "noise"};

static const BananaMlBigramPattern k_positive_bigrams[] = {
    {"ripe", "banana"},  {"banana", "bread"}, {"banana", "smoothie"},
    {"banana", "bunch"}, {"fresh", "banana"}, {"banana", "peel"}};

static const BananaMlBigramPattern k_negative_bigrams[] = {{"engine", "oil"},
                                                           {"plastic", "waste"},
                                                           {"chemical", "spill"},
                                                           {"junk", "data"},
                                                           {"random", "noise"}};

static uint32_t banana_ml_token_hash(const char *token)
{
    uint32_t hash = 2166136261u;
    while (*token)
    {
        hash ^= (unsigned char)*token;
        hash *= 16777619u;
        ++token;
    }
    return hash;
}

static int banana_ml_token_in_list(const char *token, const char *const *list, size_t count)
{
    size_t i;
    for (i = 0; i < count; ++i)
    {
        if (strcmp(token, list[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

static int banana_ml_match_bigram(const char *prev, const char *current,
                                  const BananaMlBigramPattern *patterns, size_t count)
{
    size_t i;
    for (i = 0; i < count; ++i)
    {
        if (strcmp(prev, patterns[i].first) == 0 && strcmp(current, patterns[i].second) == 0)
        {
            return 1;
        }
    }
    return 0;
}

static int banana_ml_extract_key_text(const char *input_json, const char *key, char *out_text,
                                      size_t out_text_size)
{
    size_t key_len;
    const char *pos;
    const char *scan;
    size_t out_index;

    if (!input_json || !key || !out_text || out_text_size == 0)
    {
        return 0;
    }

    key_len = strlen(key);
    scan = input_json;

    while ((pos = strstr(scan, key)) != NULL)
    {
        const char *cursor = pos + key_len;
        while (*cursor == ' ' || *cursor == '\t' || *cursor == '\r' || *cursor == '\n')
        {
            ++cursor;
        }
        if (*cursor != ':')
        {
            scan = pos + 1;
            continue;
        }
        ++cursor;
        while (*cursor == ' ' || *cursor == '\t' || *cursor == '\r' || *cursor == '\n')
        {
            ++cursor;
        }
        if (*cursor != '"')
        {
            scan = pos + 1;
            continue;
        }
        ++cursor;

        out_index = 0;
        while (*cursor && *cursor != '"' && out_index + 1 < out_text_size)
        {
            if (*cursor == '\\' && cursor[1])
            {
                char escaped = cursor[1];
                if (escaped == 'n' || escaped == 'r' || escaped == 't')
                {
                    out_text[out_index++] = ' ';
                }
                else
                {
                    out_text[out_index++] = escaped;
                }
                cursor += 2;
                continue;
            }
            out_text[out_index++] = *cursor;
            ++cursor;
        }
        out_text[out_index] = '\0';
        return out_index > 0;
    }

    return 0;
}

static void banana_ml_normalize_text(const char *source, char *out_text, size_t out_text_size)
{
    size_t i;
    size_t out_index = 0;
    int last_was_space = 1;

    if (!source || !out_text || out_text_size == 0)
    {
        return;
    }

    for (i = 0; source[i] != '\0' && out_index + 1 < out_text_size; ++i)
    {
        unsigned char c = (unsigned char)source[i];
        if (isalnum(c))
        {
            out_text[out_index++] = (char)tolower(c);
            last_was_space = 0;
        }
        else if (!last_was_space)
        {
            out_text[out_index++] = ' ';
            last_was_space = 1;
        }
    }

    if (out_index > 0 && out_text[out_index - 1] == ' ')
    {
        --out_index;
    }

    out_text[out_index] = '\0';
}

static int banana_ml_load_candidate_text(const char *input_json, char *candidate,
                                         size_t candidate_size)
{
    const char *key_patterns[] = {"\"text\"", "\"message\"", "\"prompt\"", "\"input\""};
    size_t i;

    if (!input_json || !candidate || candidate_size == 0)
    {
        return 0;
    }

    for (i = 0; i < sizeof(key_patterns) / sizeof(key_patterns[0]); ++i)
    {
        if (banana_ml_extract_key_text(input_json, key_patterns[i], candidate, candidate_size))
        {
            return 1;
        }
    }

    strncpy(candidate, input_json, candidate_size - 1);
    candidate[candidate_size - 1] = '\0';
    return candidate[0] != '\0';
}

double banana_ml_clamp01(double value)
{
    if (value < 0.0)
        return 0.0;
    if (value > 1.0)
        return 1.0;
    return value;
}

double banana_ml_sigmoid_approx(double value)
{
    double abs_value = (value < 0.0) ? -value : value;
    return 0.5 * (value / (1.0 + abs_value)) + 0.5;
}

void banana_ml_fill_confusion(int predicted_is_banana, int actual_is_banana,
                              BananaMlConfusionMatrix *out_confusion)
{
    if (!out_confusion)
    {
        return;
    }

    out_confusion->tp = (predicted_is_banana && actual_is_banana) ? 1 : 0;
    out_confusion->fp = (predicted_is_banana && !actual_is_banana) ? 1 : 0;
    out_confusion->fn = (!predicted_is_banana && actual_is_banana) ? 1 : 0;
    out_confusion->tn = (!predicted_is_banana && !actual_is_banana) ? 1 : 0;
}

double banana_ml_jaccard_for_banana(const BananaMlConfusionMatrix *confusion)
{
    int denominator;
    if (!confusion)
    {
        return 0.0;
    }

    denominator = confusion->tp + confusion->fp + confusion->fn;
    if (denominator == 0)
    {
        return 1.0;
    }
    return (double)confusion->tp / (double)denominator;
}

int banana_ml_build_feature_vector(const char *input_json, BananaMlFeatureVector *out_features)
{
    char candidate[2048];
    char normalized[2048];
    char prev_token[64];
    uint64_t unique_bloom[4] = {0, 0, 0, 0};
    size_t position = 0;
    double harmonic_sum = 0.0;
    double banana_attention_sum = 0.0;
    double not_banana_attention_sum = 0.0;
    size_t i = 0;

    if (!input_json || !out_features)
    {
        return BANANA_INVALID_ARGUMENT;
    }

    memset(out_features, 0, sizeof(*out_features));
    prev_token[0] = '\0';

    if (!banana_ml_load_candidate_text(input_json, candidate, sizeof(candidate)))
    {
        return BANANA_INVALID_ARGUMENT;
    }

    banana_ml_normalize_text(candidate, normalized, sizeof(normalized));
    if (normalized[0] == '\0')
    {
        return BANANA_INVALID_ARGUMENT;
    }

    while (normalized[i] != '\0')
    {
        char token[64];
        size_t token_len = 0;
        uint32_t hash;
        size_t bloom_index;
        uint64_t bloom_mask;
        int banana_hit;
        int not_banana_hit;

        while (normalized[i] == ' ')
        {
            ++i;
        }
        if (normalized[i] == '\0')
        {
            break;
        }

        while (normalized[i] != '\0' && normalized[i] != ' ')
        {
            if (token_len + 1 < sizeof(token))
            {
                token[token_len++] = normalized[i];
            }
            ++i;
        }
        token[token_len] = '\0';
        if (token_len == 0)
        {
            continue;
        }

        ++out_features->token_count;
        ++position;

        hash = banana_ml_token_hash(token);
        bloom_index = (size_t)(hash & 255u) / 64u;
        bloom_mask = 1ull << ((hash & 255u) % 64u);
        if ((unique_bloom[bloom_index] & bloom_mask) == 0)
        {
            unique_bloom[bloom_index] |= bloom_mask;
            ++out_features->unique_token_count;
        }

        banana_hit =
            banana_ml_token_in_list(token, k_banana_signal_tokens, k_banana_signal_tokens_count);
        not_banana_hit = banana_ml_token_in_list(token, k_not_banana_signal_tokens,
                                                 sizeof(k_not_banana_signal_tokens) /
                                                     sizeof(k_not_banana_signal_tokens[0]));

        if (banana_hit)
        {
            ++out_features->banana_hits;
            banana_attention_sum += 1.0 / (double)position;
        }
        if (not_banana_hit)
        {
            ++out_features->not_banana_hits;
            not_banana_attention_sum += 1.0 / (double)position;
        }

        harmonic_sum += 1.0 / (double)position;

        if (prev_token[0] != '\0')
        {
            if (banana_ml_match_bigram(prev_token, token, k_positive_bigrams,
                                       sizeof(k_positive_bigrams) / sizeof(k_positive_bigrams[0])))
            {
                ++out_features->positive_bigram_hits;
            }
            if (banana_ml_match_bigram(prev_token, token, k_negative_bigrams,
                                       sizeof(k_negative_bigrams) / sizeof(k_negative_bigrams[0])))
            {
                ++out_features->negative_bigram_hits;
            }
        }

        strncpy(prev_token, token, sizeof(prev_token) - 1);
        prev_token[sizeof(prev_token) - 1] = '\0';
    }

    if (out_features->token_count == 0)
    {
        return BANANA_INVALID_ARGUMENT;
    }

    out_features->banana_signal_ratio =
        (double)out_features->banana_hits / (double)out_features->token_count;
    out_features->not_banana_signal_ratio =
        (double)out_features->not_banana_hits / (double)out_features->token_count;
    out_features->unique_token_ratio =
        (double)out_features->unique_token_count / (double)out_features->token_count;
    out_features->length_ratio = banana_ml_clamp01((double)out_features->token_count / 24.0);

    if (out_features->token_count > 1)
    {
        double bigram_denominator = (double)(out_features->token_count - 1);
        out_features->positive_bigram_ratio =
            (double)out_features->positive_bigram_hits / bigram_denominator;
        out_features->negative_bigram_ratio =
            (double)out_features->negative_bigram_hits / bigram_denominator;
    }
    else
    {
        out_features->positive_bigram_ratio = 0.0;
        out_features->negative_bigram_ratio = 0.0;
    }

    if (harmonic_sum > 0.0)
    {
        out_features->banana_attention_ratio = banana_attention_sum / harmonic_sum;
        out_features->not_banana_attention_ratio = not_banana_attention_sum / harmonic_sum;
    }

    out_features->banana_signal_ratio = banana_ml_clamp01(out_features->banana_signal_ratio);
    out_features->not_banana_signal_ratio =
        banana_ml_clamp01(out_features->not_banana_signal_ratio);
    out_features->unique_token_ratio = banana_ml_clamp01(out_features->unique_token_ratio);
    out_features->positive_bigram_ratio = banana_ml_clamp01(out_features->positive_bigram_ratio);
    out_features->negative_bigram_ratio = banana_ml_clamp01(out_features->negative_bigram_ratio);
    out_features->banana_attention_ratio = banana_ml_clamp01(out_features->banana_attention_ratio);
    out_features->not_banana_attention_ratio =
        banana_ml_clamp01(out_features->not_banana_attention_ratio);

    return BANANA_OK;
}
