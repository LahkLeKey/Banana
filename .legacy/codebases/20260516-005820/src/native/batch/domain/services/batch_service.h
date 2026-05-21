#pragma once
#include "../entities/batch.h"

int batch_create(const char *input_json, char **out_json);
int batch_get_status(const char *batch_id, char **out_json);
int batch_predict_ripeness(const char *batch_id, char **out_json);
int harvest_create(const char *input_json, char **out_json);
int harvest_add_bunch(const char *batch_id, const char *input_json, char **out_json);
int harvest_get_status(const char *batch_id, char **out_json);
