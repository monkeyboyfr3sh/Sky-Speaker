#include "touch_handler.h"

#include <string.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_peripherals.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"
#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include "esp_hf_client_api.h"

#include "audio_element.h"

#include "i2s_stream.h"
#include "board.h"
#include "bluetooth_service.h"

#include "display_helper.h"

#include "audio_idf_version.h"

#include "esp_err.h"
#include "display_service.h"
#include "periph_touch.h"
#include "periph_button.h"
#include "periph_adc_button.h"

static const char *TAG = "BUTTON_HANDLER";

static esp_periph_handle_t * bt_periph_shared;
static display_service_handle_t * led_periph_shared;

esp_err_t init_touch_handler(esp_periph_handle_t * bt_periph, display_service_handle_t * led_periph)
{
    // Verify valid input
    if( (bt_periph == NULL) || (led_periph == NULL) ) {
        return ESP_ERR_INVALID_ARG;
    }
    // Update shared variables
    bt_periph_shared = bt_periph;
    led_periph_shared = led_periph;
    return ESP_OK;
}

esp_err_t touch_handler(audio_event_iface_msg_t msg)
{
    esp_err_t ret = ESP_OK;
    static int value = 0;
    static int pattern = DISPLAY_PATTERN_UNKNOWN;

    if ((msg.source_type == PERIPH_ID_TOUCH || msg.source_type == PERIPH_ID_BUTTON || msg.source_type == PERIPH_ID_ADC_BTN)
        && (msg.cmd == PERIPH_TOUCH_TAP || msg.cmd == PERIPH_BUTTON_PRESSED || msg.cmd == PERIPH_ADC_BUTTON_PRESSED)) {

        if ((int) msg.data == get_input_play_id()) {
            ESP_LOGI(TAG, "[ * ] [Play] touch tap event");
            periph_bluetooth_play((*bt_periph_shared));
        } else if ((int) msg.data == get_input_set_id()) {
            ESP_LOGI(TAG, "[ * ] [Set] touch tap event");
            periph_bluetooth_pause((*bt_periph_shared));
        } else if ((int) msg.data == get_input_volup_id()) {
            ESP_LOGI(TAG, "[ * ] [Vol+] touch tap event");
            periph_bluetooth_next((*bt_periph_shared));
        } else if ((int) msg.data == get_input_voldown_id()) {
            ESP_LOGI(TAG, "[ * ] [Vol-] touch tap event");
            periph_bluetooth_prev((*bt_periph_shared));
        } else if ((int) msg.data == get_input_mode_id()) {
            ESP_LOGI(TAG,"[ * ] [Mode] touch tap event");
            display_service_set_pattern((void *)(*led_periph_shared), DISPLAY_PATTERN_RECORDING_STOP, 100);
        } else if ((int) msg.data == get_input_rec_id()) {
            ESP_LOGI(TAG,"[ * ] [Rec] touch tap event");
            display_service_set_pattern((void *)(*led_periph_shared), DISPLAY_PATTERN_RECORDING_START, 100);
        }
    }

    return ret;
}