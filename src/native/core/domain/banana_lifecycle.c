#include "banana_lifecycle.h"

#include <string.h>

int banana_identifier_is_valid(const char* value) {
    size_t length = 0U;

    if (value == 0 || value[0] == '\0') {
        return 0;
    }

    length = strlen(value);
    return length > 0U && length < BANANA_ID_CAPACITY;
}

BananaStatus banana_identifier_copy(BananaIdentifier* identifier, const char* value) {
    size_t length = 0U;

    if (identifier == 0 || !banana_identifier_is_valid(value)) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    length = strlen(value);
    memset(identifier, 0, sizeof(*identifier));
    memcpy(identifier->value, value, length);
    return BANANA_OK;
}

const char* banana_species_name(BananaSpecies species) {
    switch (species) {
        case BANANA_SPECIES_CAVENDISH:
            return "Cavendish";
        case BANANA_SPECIES_PLANTAIN:
            return "Plantain";
        case BANANA_SPECIES_OTHER:
            return "Other";
    }

    return "Unknown";
}

const char* banana_ripeness_stage_name(BananaRipenessStage stage) {
    switch (stage) {
        case BANANA_STAGE_GREEN:
            return "GREEN";
        case BANANA_STAGE_BREAKING:
            return "BREAKING";
        case BANANA_STAGE_YELLOW:
            return "YELLOW";
        case BANANA_STAGE_SPOTTED:
            return "SPOTTED";
        case BANANA_STAGE_OVERRIPE:
            return "OVERRIPE";
        case BANANA_STAGE_BIODEGRADATION:
            return "BIODEGRADATION";
    }

    return "UNKNOWN";
}