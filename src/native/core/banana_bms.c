#include "banana_bms.h"

const char* banana_species_name(BananaSpecies species) {
    switch (species) {
        case BANANA_SPECIES_CAVENDISH:
            return "Cavendish";
        case BANANA_SPECIES_PLANTAIN:
            return "Plantain";
        case BANANA_SPECIES_OTHER:
        default:
            return "Other";
    }
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
        default:
            return "BIODEGRADATION";
    }
}
