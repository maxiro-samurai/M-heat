#include "nvs_data.h"

/**
 * @brief NVS 初始化
 * 
 */

void nvs_init(void) 
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}

/**
 * @brief NVS设定参数
 * * @param key 键值
 * * @param value 值 
 * * @note 该函数用于设置NVS中的参数值
 */

esp_err_t nvs_set_parameter(const char* key, float value)
{
    //初始化句柄
    int32_t temp;
    memcpy(&temp, &value, sizeof(float));
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");
    }
    err = nvs_set_i32(nvs_handle, key, temp);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) setting NVS parameter!", esp_err_to_name(err));
    } else {
        ESP_LOGI("NVS", "NVS parameter set successfully");
    }
    printf("Committing updates in NVS ... ");
    err = nvs_commit(nvs_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    //关闭句柄
    nvs_close(nvs_handle);
    return err;

}
/**
 * @brief NVS获取参数
 * * @param key 键值
 * * @param value 值 
 * * @note 该函数用于获取NVS中的参数值
 */
esp_err_t nvs_get_parameter(const char* key, float *value)
{
    int32_t temp;
    //初始化句柄
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");
    }
    err = nvs_get_i32(nvs_handle, key, &temp);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) getting NVS parameter!", esp_err_to_name(err));
    } else {
        ESP_LOGI("NVS", "NVS parameter get successfully");
        memcpy(value,&temp,sizeof(float));
    }
    //关闭句柄
    nvs_close(nvs_handle);
    return err;
}

/**
 * @brief NVS清除参数
 * * @param key 键值
 * * @note 该函数用于清除NVS中的参数值
 */
void nvs_clear_parameter(const char* key)
{
    //初始化句柄
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error opening NVS handle: %s", esp_err_to_name(err));
    } 
    err = nvs_erase_key(nvs_handle, key);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) getting NVS parameter!", esp_err_to_name(err));
    } else {
        ESP_LOGI("NVS", "NVS parameter get successfully");
    }
    //关闭句柄
    nvs_close(nvs_handle);
}