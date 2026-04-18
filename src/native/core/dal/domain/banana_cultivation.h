#ifndef BANANA_DAL_CULTIVATION_H
#define BANANA_DAL_CULTIVATION_H

#include "../../domain/banana_lifecycle.h"

int banana_cultivation_db_save_plant(const BananaPlant* plant);
int banana_cultivation_db_get_plant(const char* plant_id, BananaPlant* plant);
int banana_cultivation_db_clear_plants(void);

#endif