#include "BreadConfig.h"
#include <stdlib.h>

breadConfig_t breadConfig_init() {
	breadConfig_t breadConfig;
	breadConfig = malloc(sizeof(*breadConfig));
	breadConfig->temperature = -1;
	breadConfig->humidity = -1;
	return breadConfig;
}

void breadConfig_destroy(breadConfig_t breadConfig) {
	if (breadConfig != NULL)
	{
		free(breadConfig);
	}
}