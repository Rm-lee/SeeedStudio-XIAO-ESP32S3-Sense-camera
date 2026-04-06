#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// XIAO ESP32S3 Sense onboard PDM microphone
// CLK → GPIO 42   (I2S WS / PDM CLK)
// DAT → GPIO 41   (I2S SD / PDM DAT)
// Streams 16-bit signed PCM, mono, 16 kHz

void initMic();

// Fill buf with raw PCM bytes; returns true on success.
// bytesRead is set to the actual number of bytes placed in buf.
bool readMicSamples(uint8_t* buf, size_t bufLen, size_t* bytesRead);
