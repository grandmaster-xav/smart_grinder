#include <NvsHandler.h>

esp_err_t init_nvs()
{
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);
	return (err);
}

void set_str_value_nvs(char *name_space, char *key, char *value)
{
	/*
	Set 'value' with l character in the NVS 'key' in partition 'name_space'
	*/
	nvs_handle my_handle;
	esp_err_t err = nvs_open(name_space, NVS_READWRITE, &my_handle);
	if (err == ESP_OK)
	{
		nvs_set_str(my_handle, key, value);
		nvs_commit(my_handle);
		nvs_close(my_handle);
	}
}

bool get_str_value_nvs(char *name_space, char *key, char *value, size_t l)
{
	/*
	Get 'value' with l character from the NVS 'key' in partition 'name_space'
	*/

	nvs_handle my_handle;
	esp_err_t err = nvs_open(name_space, NVS_READWRITE, &my_handle);
	if (err == ESP_OK)
	{
		err = nvs_get_str(my_handle, key, value, &l);
		if (err != ESP_OK)
			return (false);
		nvs_close(my_handle);
		return (true);
	}
	return (false);
}
