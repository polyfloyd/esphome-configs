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
    this->_current_temperature->add_on_state_callback([this](float state) {
        this->current_temperature = state;
        this->publish_state();
    });
    this->current_temperature = this->_current_temperature->state;

    this->_target_temperature->add_on_state_callback([this](float state) {
        this->target_temperature = state;
        this->publish_state();
    });
    this->current_temperature = this->_target_temperature->state;

    this->_mode->add_on_state_callback([this](const std::string &state, size_t i) {
        this->mode = mode_from_string(state);
        this->publish_state();
    });
    this->mode = mode_from_string(this->_mode->state);

    if (this->_action != nullptr) {
        this->_action->add_on_state_callback([this](const std::string &state) {
            this->action = action_from_string(state);
            this->publish_state();
        });
        this->action = action_from_string(this->_action->state);
    }
}

void TemplateClimate::control(const ClimateCall &call) {
    auto mode = call.get_mode();
    if (mode) {
        switch (mode.value()) {
        case CLIMATE_MODE_OFF:
            this->_mode->state = "OFF";
        case CLIMATE_MODE_COOL:
            this->_mode->state = "COOL";
        case CLIMATE_MODE_HEAT:
            this->_mode->state = "HEAT";
        case CLIMATE_MODE_FAN_ONLY:
            this->_mode->state = "FAN_ONLY";
        case CLIMATE_MODE_DRY:
            this->_mode->state = "DRY";
        case CLIMATE_MODE_AUTO:
            this->_mode->state = "AUTO";
        }
    }

    auto target_temperature = call.get_target_temperature();
    if (target_temperature) {
        this->_target_temperature->state = target_temperature.value();
    }

    this->publish_state();
}

} // namespace template_
} // namespace esphome
