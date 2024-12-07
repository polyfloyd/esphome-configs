#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"


namespace esphome {
namespace hoera10jaar {

enum State {
    UNKNOWN,
    OPEN,
    CLOSED,
    DEFUNCT,
};

class Hoera10JaarComponent : public Component {
public:
    void setup() override;
    void set_led(const std::string &name, State state);
    void set_led_from_mqtt(const std::string &name, const std::string &state);

    void set_off();
};

} // esphome hoera10jaar
} // namespace esphome
