#ifdef USE_ESP8266

#include "adc_sensor.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

#ifdef USE_ADC_SENSOR_VCC
#include <Esp.h>
ADC_MODE(ADC_VCC)
#else
#include <Arduino.h>
#endif  // USE_ADC_SENSOR_VCC

namespace esphome {
namespace adc {

static const char *const TAG = "adc.esp8266";

void ADCSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADC '%s'...", this->get_name().c_str());
#ifndef USE_ADC_SENSOR_VCC
  this->pin_->setup();
#endif
}

void ADCSensor::dump_config() {
  LOG_SENSOR("", "ADC Sensor", this);
#ifdef USE_ADC_SENSOR_VCC
  ESP_LOGCONFIG(TAG, "  Pin: VCC");
#else
  LOG_PIN("  Pin: ", this->pin_);
#endif  // USE_ADC_SENSOR_VCC
  ESP_LOGCONFIG(TAG, "  Samples: %i", this->sample_count_);
  LOG_UPDATE_INTERVAL(this);
}

float ADCSensor::sample() {
  uint32_t raw = 0;
  for (uint8_t sample = 0; sample < this->sample_count_; sample++) {
#ifdef USE_ADC_SENSOR_VCC
    raw += ESP.getVcc();  // NOLINT(readability-static-accessed-through-instance)
#else
    raw += analogRead(this->pin_->get_pin());  // NOLINT
#endif  // USE_ADC_SENSOR_VCC
  }
  raw = (raw + (this->sample_count_ >> 1)) / this->sample_count_;  // NOLINT(clang-analyzer-core.DivideZero)
  if (this->output_raw_) {
    return raw;
  }
  return raw / 1024.0f;
}

std::string ADCSensor::unique_id() { return get_mac_address() + "-adc"; }

}  // namespace adc
}  // namespace esphome

#endif  // USE_ESP8266
