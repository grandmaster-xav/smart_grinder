#ifndef _NVS_HANDLER_H_
# define _NVS_HANDLER_H_

# include "nvs_flash.h"

esp_err_t init_nvs(void);
void set_str_value_nvs(char *name_space, char *key, char *value);
bool get_str_value_nvs(char *name_space, char *key, char *value, size_t l);

#endif