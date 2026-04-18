#ifndef BANANA_DB_H
#define BANANA_DB_H

#include <stddef.h>

int banana_db_query(int purchases, int multiplier, char** out_payload, int* out_row_count);

#endif
