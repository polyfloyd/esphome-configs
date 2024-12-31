#include "template_climate.h"

namespace esphome {
namespace template_ {

static const char *const TAG = "template_climate";

ClimateMode mode_from_string(const std::string &s) {
  if (str_equals_case_insensitive(s, "OFF")) {
    return CLIMATE_MODE_OFF;
  } else if (str_equals_case_insensitive(s, "HEAT_COOL")) {
    return CLIMATE_MODE_HEAT_COOL;
  } else if (str_equals_case_insensitive(s, "COOL")) {
    return CLIMATE_MODE_COOL;
  } else if (str_equals_case_insensitive(s, "HEAT")) {
    return CLIMATE_MODE_HEAT;
  } else if (str_equals_case_insensitive(s, "FAN_ONLY")) {
    return CLIMATE_MODE_FAN_ONLY;
  } else if (str_equals_case_insensitive(s, "DRY")) {
    return CLIMATE_MODE_DRY;
  } else if (str_equals_case_insensitive(s, "AUTO")) {
    return CLIMATE_MODE_AUTO;
  } else {
    ESP_LOGW(TAG, "Unrecognized mode %s", s.c_str());
    return CLIMATE_MODE_OFF;
  }
}

ClimateFanMode fan_mode_from_string(const std::string &s) {
  if (str_equals_case_insensitive(s, "ON")) {
      return CLIMATE_FAN_ON;
  } else if (str_equals_case_insensitive(s, "OFF")) {
      return CLIMATE_FAN_OFF;
  } else if (str_equals_case_insensitive(s, "AUTO")) {
      return CLIMATE_FAN_AUTO;
  } else if (str_equals_case_insensitive(s, "LOW")) {
      return CLIMATE_FAN_LOW;
  } else if (str_equals_case_insensitive(s, "MEDIUM")) {
      return CLIMATE_FAN_MEDIUM;
  } else if (str_equals_case_insensitive(s, "HIGH")) {
      return CLIMATE_FAN_HIGH;
  } else if (str_equals_case_insensitive(s, "MIDDLE")) {
      return CLIMATE_FAN_MIDDLE;
  } else if (str_equals_case_insensitive(s, "FOCUS")) {
      return CLIMATE_FAN_FOCUS;
  } else if (str_equals_case_insensitive(s, "DIFFUSE")) {
      return CLIMATE_FAN_DIFFUSE;
  } else if (str_equals_case_insensitive(s, "QUIET")) {
      return CLIMATE_FAN_QUIET;
  } else {
    ESP_LOGW(TAG, "Unrecognized fan mode %s", s.c_str());
    return CLIMATE_FAN_AUTO;
  }
}

ClimateAction action_from_string(const std::string &s) {
  if (str_equals_case_insensitive(s, "OFF")) {
    return CLIMATE_ACTION_OFF;
  } else if (str_equals_case_insensitive(s, "COOLING")) {
    return CLIMATE_ACTION_COOLING;
  } else if (str_equals_case_insensitive(s, "HEATING")) {
    return CLIMATE_ACTION_HEATING;
  } else if (str_equals_case_insensitive(s, "IDLE")) {
    return CLIMATE_ACTION_IDLE;
  } else if (str_equals_case_insensitive(s, "DRYING")) {
    return CLIMATE_ACTION_DRYING;
  } else if (str_equals_case_insensitive(s, "FAN")) {
    return CLIMATE_ACTION_FAN;
  } else {
    ESP_LOGW(TAG, "Unrecognized action %s", s.c_str());
    return CLIMATE_ACTION_OFF;
  }
}

void TemplateClimate::setup() {
  this->_current_temperature->add_on_state_callback([this](float x) {
      this->current_temperature = x;
      this->publish_state();
    });
  this->current_temperature = this->_current_temperature->state;

  this->_target_temperature->add_on_state_callback([this](float x) {
      this->target_temperature = x;
      this->publish_state();
    });
  this->current_temperature = this->_target_temperature->state;

  this->_mode->add_on_state_callback([this](const std::string &x, size_t i) {
      this->mode = mode_from_string(x);
      this->publish_state();
    });
  this->mode = mode_from_string(this->_mode->state);

  if (this->_fan_mode != nullptr) {
    this->_fan_mode->add_on_state_callback([this](const std::string &x, size_t i) {
        this->fan_mode = fan_mode_from_string(x);
        this->publish_state();
      });
    this->fan_mode = fan_mode_from_string(this->_fan_mode->state);
  }

  if (this->_action != nullptr) {
    this->_action->add_on_state_callback([this](const std::string &x) {
        this->action = action_from_string(x);
        this->publish_state();
      });
    this->action = action_from_string(this->_action->state);
  }
}

void TemplateClimate::control(const ClimateCall &call) {
  auto mode = call.get_mode();
  if (mode) {
    auto call = this->_mode->make_call();
    switch (mode.value()) {
      case CLIMATE_MODE_OFF:
        call.set_option("OFF");
        break;
      case CLIMATE_MODE_COOL:
        call.set_option("COOL");
        break;
      case CLIMATE_MODE_HEAT:
        call.set_option("HEAT");
        break;
      case CLIMATE_MODE_FAN_ONLY:
        call.set_option("FAN_ONLY");
        break;
      case CLIMATE_MODE_DRY:
        call.set_option("DRY");
        break;
      case CLIMATE_MODE_AUTO:
        call.set_option("AUTO");
        break;
    }
    call.perform();
  }

  auto fan_mode = call.get_fan_mode();
  if (fan_mode && this->_fan_mode != nullptr) {
    auto call = this->_fan_mode->make_call();
    switch (mode.value()) {
      case CLIMATE_FAN_ON:
        call.set_option("ON");
        break;
      case CLIMATE_FAN_OFF:
        call.set_option("OFF");
        break;
      case CLIMATE_FAN_AUTO:
        call.set_option("AUTO");
        break;
      case CLIMATE_FAN_LOW:
        call.set_option("LOW");
        break;
      case CLIMATE_FAN_MEDIUM:
        call.set_option("MEDIUM");
        break;
      case CLIMATE_FAN_HIGH:
        call.set_option("HIGH");
        break;
      case CLIMATE_FAN_MIDDLE:
        call.set_option("MIDDLE");
        break;
      case CLIMATE_FAN_FOCUS:
        call.set_option("FOCUS");
        break;
      case CLIMATE_FAN_DIFFUSE:
        call.set_option("DIFFUSE");
        break;
      case CLIMATE_FAN_QUIET:
        call.set_option("QUIET");
        break;
    }
    call.perform();
  }

  auto target_temperature = call.get_target_temperature();
  if (target_temperature) {
    this->_target_temperature->make_call()
      .set_value(target_temperature.value())
      .perform();
  }
}

} // namespace template_
} // namespace esphome
