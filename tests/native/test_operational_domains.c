// Native operational domain tests for ripeness, batch, harvest, and truck.
#include "../../src/native/wrapper/banana_wrapper.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void assert_contains(const char* text, const char* needle) {
    assert(text != NULL);
    assert(needle != NULL);
    assert(strstr(text, needle) != NULL);
}

static void extract_json_string_field(
    const char* json, const char* field, char* out_value, size_t out_size) {
    char pattern[64];
    const char* start;
    const char* end;
    size_t n;

    assert(json != NULL);
    assert(field != NULL);
    assert(out_value != NULL);
    assert(out_size > 0);

    snprintf(pattern, sizeof(pattern), "\"%s\":\"", field);
    start = strstr(json, pattern);
    assert(start != NULL);
    start += strlen(pattern);
    end = strchr(start, '"');
    assert(end != NULL);

    n = (size_t)(end - start);
    if (n >= out_size) n = out_size - 1;
    memcpy(out_value, start, n);
    out_value[n] = '\0';
}

int main(void) {
    int rc;
    char* json = NULL;
    char batch_id[32] = {0};
    char harvest_id[32] = {0};
    char truck_id[32] = {0};

    rc = banana_predict_banana_ripeness("{\"notes\":\"green peel and firm body\"}", &json);
    assert(rc == BANANA_OK);
    assert_contains(json, "\"label\":\"unripe\"");
    banana_free(json);

    rc = banana_create_batch("{\"manifest\":\"ripe banana bunch crate\"}", &json);
    assert(rc == BANANA_OK);
    assert_contains(json, "\"status\":\"created\"");
    extract_json_string_field(json, "batch_id", batch_id, sizeof(batch_id));
    banana_free(json);

    rc = banana_get_batch_status(batch_id, &json);
    assert(rc == BANANA_OK);
    assert_contains(json, "\"status\":\"active\"");
    assert_contains(json, batch_id);
    banana_free(json);

    rc = banana_predict_batch_ripeness(batch_id, &json);
    assert(rc == BANANA_OK);
    assert_contains(json, "\"confidence\":");
    banana_free(json);

    rc = banana_create_harvest_batch("{}", &json);
    assert(rc == BANANA_OK);
    extract_json_string_field(json, "harvest_batch_id", harvest_id, sizeof(harvest_id));
    banana_free(json);

    rc = banana_add_bunch_to_harvest_batch(harvest_id, "{\"count\":\"bunch crate\"}", &json);
    assert(rc == BANANA_OK);
    assert_contains(json, "\"status\":\"updated\"");
    banana_free(json);

    rc = banana_get_harvest_batch_status(harvest_id, &json);
    assert(rc == BANANA_OK);
    assert_contains(json, "\"status\":\"active\"");
    banana_free(json);

    rc = banana_register_truck("{\"destination\":\"warehouse\"}", &json);
    assert(rc == BANANA_OK);
    extract_json_string_field(json, "truck_id", truck_id, sizeof(truck_id));
    assert_contains(json, "\"location\":\"warehouse\"");
    banana_free(json);

    rc = banana_load_truck_container(truck_id, "{\"container\":\"c1\"}", &json);
    assert(rc == BANANA_OK);
    assert_contains(json, "\"status\":\"loaded\"");
    banana_free(json);

    rc = banana_relocate_truck(truck_id, "{\"destination\":\"port\"}", &json);
    assert(rc == BANANA_OK);
    assert_contains(json, "\"status\":\"relocated\"");
    assert_contains(json, "\"location\":\"port\"");
    banana_free(json);

    rc = banana_get_truck_status(truck_id, &json);
    assert(rc == BANANA_OK);
    assert_contains(json, "\"status\":\"active\"");
    banana_free(json);

    rc = banana_unload_truck_container(truck_id, "container-1", &json);
    assert(rc == BANANA_OK);
    assert_contains(json, "\"status\":\"unloaded\"");
    banana_free(json);

    rc = banana_get_batch_status("batch-9999", &json);
    assert(rc == BANANA_NOT_FOUND);

    printf("[native-unit] operational domains deterministic behavior ok\n");
    return 0;
}
