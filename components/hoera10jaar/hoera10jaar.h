#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"
#include "esphome/components/light/light_output.h"


namespace esphome {
namespace hoera10jaar {

enum State {
    UNKNOWN,
    OPEN,
    CLOSED,
    DEFUNCT,
};

const int num_levels = 32;

class Hoera10JaarComponent : public Component {
public:
    void setup() override;
    void set_led(const std::string &name, State state);
    void set_led_from_mqtt(const std::string &name, const std::string &state);
    void set_brightness(float b);

private:
    int levels0[num_levels] = {0};
    int levels1[num_levels] = {0};
    const int *levels;
    static void led_matrix_loop(void *param);
};

class Hoera10JaarLight : public light::LightOutput {
public:
    void set_parent(Hoera10JaarComponent *out) { this->out = out; }

    light::LightTraits get_traits() {
        auto traits = light::LightTraits();
        traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS});
        return traits;
    }

    void write_state(light::LightState *state) {
        float bright;
        state->current_values_as_brightness(&bright);
        this->out->set_brightness(bright);
    }

private:
    Hoera10JaarComponent *out;
};

} // esphome hoera10jaar
} // namespace esphome
