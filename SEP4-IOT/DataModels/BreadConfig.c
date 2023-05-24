#include "BreadConfig.h"
#include <stdlib.h>

breadConfig_t breadConfig_init() {
	breadConfig_t breadConfig;
	breadConfig = malloc(sizeof(*breadConfig));
	breadConfig->temperature = 0;
	breadConfig->humidity = 0;
	return breadConfig;
}

void breadConfig_destroy(breadConfig_t breadConfig) {
	if (breadConfig != NULL)
	{
		free(breadConfig);
	}
}