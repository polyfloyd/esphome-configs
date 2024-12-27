#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/number/number.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace template_ {

using namespace climate;
using namespace number;
using namespace text_sensor;

class TemplateClimate : public Climate, public Component {
public:
    TemplateClimate();

    void setup() override;
    void loop() override;
    void refresh();
    ClimateTraits traits() override;

    void set_current_temperature(Number *num) {
        this->_current_temperature = num;
    }
    void set_target_temperature(Number *num) {
        this->_target_temperature = num;
    }
    void set_action(TextSensor *s) {
        this->_action = s;
    }
    Trigger<> *get_idle_action_trigger() const {
        return this->_idle_action_trigger;
    }
    Trigger<> *get_cool_action_trigger() const {
        return this->_cool_action_trigger;
    }
    Trigger<> *get_heat_action_trigger() const {
        return this->_heat_action_trigger;
    }
    Trigger<> *get_fan_only_action_trigger() const {
        return this->_fan_only_action_trigger;
    }
    Trigger<> *get_dry_action_trigger() const {
        return this->_dry_action_trigger;
    }
    Trigger<> *get_auto_action_trigger() const {
        return this->_auto_action_trigger;
    }
    void set_temperature_step(float t) {
        this->_temperature_step = t;
    }
    void set_get_mode(std::function<ClimateMode()> &&f) {
        this->_get_mode = f;
    }

protected:
    void control(const ClimateCall &call) override;

    float _temperature_step;
    Number *_current_temperature{nullptr};
    Number *_target_temperature{nullptr};
    TextSensor *_action{nullptr};
    Trigger<> *_idle_action_trigger{nullptr};
    Trigger<> *_cool_action_trigger{nullptr};
    Trigger<> *_heat_action_trigger{nullptr};
    Trigger<> *_fan_only_action_trigger{nullptr};
    Trigger<> *_dry_action_trigger{nullptr};
    Trigger<> *_auto_action_trigger{nullptr};
    optional<std::function<ClimateMode()>> _get_mode;
};

} // namespace template_
} // namespace esphome
