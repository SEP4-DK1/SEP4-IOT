#include "BreadConfig.h"
#include <stdlib.h>

breadConfig_t breadConfig_init() {
	breadConfig_t breadConfig;
	breadConfig = malloc(sizeof(*breadConfig));
	breadConfig->temperature = 0xffff;
	breadConfig->humidity = 0xff;
	return breadConfig;
}

void breadConfig_destroy(breadConfig_t breadConfig) {
	if (breadConfig != NULL)
	{
		free(breadConfig);
	}
}