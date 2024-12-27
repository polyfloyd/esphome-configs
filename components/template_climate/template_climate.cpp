#include "template_climate.h"

namespace esphome {
namespace template_ {

static const char *const TAG = "template_climate";

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

TemplateClimate::TemplateClimate()
    : _idle_action_trigger(new Trigger<>()),
      _cool_action_trigger(new Trigger<>()),
      _heat_action_trigger(new Trigger<>()),
      _fan_only_action_trigger(new Trigger<>()),
      _dry_action_trigger(new Trigger<>()),
      _auto_action_trigger(new Trigger<>()) {}

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

    if (this->_action != nullptr) {
        this->_action->add_on_state_callback([this](const std::string &state) {
            this->action = action_from_string(state);
            this->publish_state();
        });
        this->action = action_from_string(this->_action->state);
    }
}

void TemplateClimate::loop() {
    this->refresh();
}

void TemplateClimate::refresh() {
    this->mode = this->_get_mode.value()();
}

ClimateTraits TemplateClimate::traits() {
    ClimateTraits traits;

    traits.set_supports_current_temperature(true);

    std::set<ClimateMode> supported_modes{CLIMATE_MODE_OFF};
    if (this->_cool_action_trigger) supported_modes.insert(CLIMATE_MODE_COOL);
    if (this->_heat_action_trigger) supported_modes.insert(CLIMATE_MODE_HEAT);
    if (this->_fan_only_action_trigger) supported_modes.insert(CLIMATE_MODE_FAN_ONLY);
    if (this->_dry_action_trigger) supported_modes.insert(CLIMATE_MODE_DRY);
    if (this->_auto_action_trigger) supported_modes.insert(CLIMATE_MODE_AUTO);
    traits.set_supported_modes(supported_modes);

    traits.set_supports_action(this->_action != nullptr);
    traits.set_visual_current_temperature_step(this->_temperature_step);

    return traits;
}

void TemplateClimate::control(const ClimateCall &call) {
    auto mode = call.get_mode();
    if (mode) {
        switch (mode.value()) {
        case CLIMATE_MODE_OFF:
            this->_idle_action_trigger->trigger();
        case CLIMATE_MODE_COOL:
            if (this->_cool_action_trigger) this->_cool_action_trigger->trigger();
        case CLIMATE_MODE_HEAT:
            if (this->_heat_action_trigger) this->_heat_action_trigger->trigger();
        case CLIMATE_MODE_FAN_ONLY:
            if (this->_fan_only_action_trigger) this->_fan_only_action_trigger->trigger();
        case CLIMATE_MODE_DRY:
            if (this->_dry_action_trigger) this->_dry_action_trigger->trigger();
        case CLIMATE_MODE_AUTO:
            if (this->_auto_action_trigger) this->_auto_action_trigger->trigger();
        }
    }

    auto target_temperature = call.get_target_temperature();
    if (target_temperature) {
        this->_target_temperature->state = target_temperature.value();
    }

    this->refresh();

    this->publish_state();
}

} // namespace template_
} // namespace esphome
