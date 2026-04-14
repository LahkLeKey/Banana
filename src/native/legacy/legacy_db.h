#ifndef LEGACY_DB_H
#define LEGACY_DB_H

#include <stddef.h>

int legacy_db_query_banana(int purchases, int multiplier, char** out_payload, int* out_row_count);

#endif
