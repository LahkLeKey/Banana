#ifndef BANANA_DAL_PROCESSING_H
#define BANANA_DAL_PROCESSING_H

#include "../../domain/banana_processing.h"

int banana_processing_db_save_bunch(const BananaBunchRecord* bunch);
int banana_processing_db_get_bunch(const char* bunch_id, BananaBunchRecord* bunch);
int banana_processing_db_clear_bunches(void);

#endif