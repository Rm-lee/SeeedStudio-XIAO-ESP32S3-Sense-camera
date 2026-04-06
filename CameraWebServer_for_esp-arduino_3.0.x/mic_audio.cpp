// PDM microphone driver for XIAO ESP32S3 Sense
// Uses the IDF 5.x (Arduino ESP32 3.0.x) I2S PDM RX channel API.
//
// Wiring (onboard MSM261D4030H1CPM or equivalent):
//   PDM_CLK  → GPIO 42   (I2S_PDM_RX clk)
//   PDM_DATA → GPIO 41   (I2S_PDM_RX din)

#include "mic_audio.h"
#include <Arduino.h>
#include <driver/i2s_pdm.h>
#include <esp_log.h>

#define PDM_CLK_GPIO     ((gpio_num_t)42)
#define PDM_DATA_GPIO    ((gpio_num_t)41)
#define PDM_SAMPLE_RATE  16000
#define PDM_READ_TIMEOUT pdMS_TO_TICKS(300)

static const char* TAG = "MIC";
static i2s_chan_handle_t s_rx_chan = NULL;

void initMic() {
  // Create an I2S RX channel on I2S_NUM_0
  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
  chan_cfg.auto_clear = false;

  esp_err_t err = i2s_new_channel(&chan_cfg, NULL, &s_rx_chan);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "i2s_new_channel failed: 0x%x", err);
    return;
  }

  // Configure PDM RX mode
  i2s_pdm_rx_config_t pdm_rx_cfg = {
    .clk_cfg  = I2S_PDM_RX_CLK_DEFAULT_CONFIG(PDM_SAMPLE_RATE),
    .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
    .gpio_cfg = {
      .clk = PDM_CLK_GPIO,
      .din = PDM_DATA_GPIO,
      .invert_flags = { .clk_inv = false }
    },
  };

  err = i2s_channel_init_pdm_rx_mode(s_rx_chan, &pdm_rx_cfg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "i2s_channel_init_pdm_rx_mode failed: 0x%x", err);
    i2s_del_channel(s_rx_chan);
    s_rx_chan = NULL;
    return;
  }

  err = i2s_channel_enable(s_rx_chan);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "i2s_channel_enable failed: 0x%x", err);
    i2s_del_channel(s_rx_chan);
    s_rx_chan = NULL;
    return;
  }

  Serial.println("[MIC] PDM mic ready: CLK=42, DAT=41, 16kHz mono 16-bit PCM");
}

bool readMicSamples(uint8_t* buf, size_t bufLen, size_t* bytesRead) {
  if (!s_rx_chan) {
    *bytesRead = 0;
    return false;
  }
  esp_err_t err = i2s_channel_read(s_rx_chan, buf, bufLen, bytesRead, PDM_READ_TIMEOUT);
  return err == ESP_OK;
}
