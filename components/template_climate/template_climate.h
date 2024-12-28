#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/number/number.h"
#include "esphome/components/select/select.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace template_ {

using namespace climate;
using namespace number;
using namespace select;
using namespace sensor;
using namespace text_sensor;

class TemplateClimate : public Climate, public Component {
public:
    ClimateTraits traits_;

    void setup() override;

    ClimateTraits traits() override {
        return this->traits_;
    }

    void set_current_temperature(Sensor *sensor) {
        this->_current_temperature = sensor;
        this->traits_.set_supports_current_temperature(true);
    }

    void set_target_temperature(Number *num) {
        this->_target_temperature = num;
    }

    void set_mode(Select *s) {
        this->_mode = s;
    }

    void set_action(TextSensor *s) {
        this->_action = s;
        this->traits_.set_supports_action(true);
    }

protected:
    void control(const ClimateCall &call) override;

    Sensor *_current_temperature{nullptr};
    Number *_target_temperature{nullptr};
    Select *_mode{nullptr};
    TextSensor *_action{nullptr};
};

} // namespace template_
} // namespace esphome
