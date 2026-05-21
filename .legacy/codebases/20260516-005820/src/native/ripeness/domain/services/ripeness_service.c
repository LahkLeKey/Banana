#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int ripeness_predict(const char *input_json, char **out_json)
{
	const char *label = "ripe";
	const char *text = input_json ? input_json : "";
	size_t len;
	char *buf;
	int written;

	if (!out_json)
		return -1;

	if (strstr(text, "green") || strstr(text, "firm"))
		label = "unripe";
	else if (strstr(text, "brown") || strstr(text, "mushy") || strstr(text, "overripe"))
		label = "overripe";

	len = 96u + strlen(label);
	buf = (char *)malloc(len);
	if (!buf)
		return -1;

	written = snprintf(buf, len,
					   "{\"model\":\"ripeness\",\"label\":\"%s\",\"confidence\":0.72}",
					   label);
	if (written < 0)
	{
		free(buf);
		return -1;
	}

	*out_json = buf;
	return 0;
}

// TODO: Implement ripeness prediction logic migrated from core
