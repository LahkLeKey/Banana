/*
 * Not-banana signal tokens used as the native fallback reference.
 * The training workflow compares learned vocabulary against this list.
 */

#include <stddef.h>

const char *const k_banana_signal_tokens[] = {"banana",     "ripe",    "peel",  "smoothie",
                                              "plantation", "harvest", "bunch", "cavendish",
                                              "fruit",      "crate",   "bread", "cartons"};

const size_t k_banana_signal_tokens_count =
    sizeof(k_banana_signal_tokens) / sizeof(k_banana_signal_tokens[0]);
