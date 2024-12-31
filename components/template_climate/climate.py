import esphome.config_validation as cv
import esphome.codegen as cg
from esphome import automation
from esphome.const import (
    CONF_COOL_ACTION,
    CONF_DRY_ACTION,
    CONF_FAN_ONLY_ACTION,
    CONF_HEAT_ACTION,
    CONF_ID,
    CONF_IDLE_ACTION,
    CONF_TEMPERATURE_STEP,
)
from esphome.components import climate
from esphome.components.climate import ClimateMode, CLIMATE_MODES, CLIMATE_FAN_MODES
from esphome.components.number import Number
from esphome.components.select import Select
from esphome.components.sensor import Sensor
from esphome.components.template import template_ns
from esphome.components.text_sensor import TextSensor

DEPENDENCIES = ["climate"]

CONF_AUTO_ACTION = "auto_action"
CONF_CURRENT_TEMPERATURE_ID = "current_temperature_id"
CONF_TARGET_TEMPERATURE_ID = "target_temperature_id"
CONF_MODE_ID = "mode_id"
CONF_FAN_MODE_ID = "fan_mode_id"
CONF_ACTION_ID = "action_id"
CONF_SUPPORTED_MODES = "supported_modes"
CONF_SUPPORTED_FAN_MODES = "supported_fan_modes"

TemplateClimate = template_ns.class_("TemplateClimate",  climate.Climate, cg.Component)

CONFIG_SCHEMA = (
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID(CONF_ID): cv.declare_id(TemplateClimate),
            cv.Required(CONF_CURRENT_TEMPERATURE_ID): cv.use_id(Sensor),
            cv.Required(CONF_TARGET_TEMPERATURE_ID): cv.use_id(Number),
            cv.Required(CONF_MODE_ID): cv.use_id(Select),
            cv.Optional(CONF_FAN_MODE_ID): cv.use_id(Select),
            cv.Required(CONF_SUPPORTED_MODES): cv.ensure_list(cv.enum(CLIMATE_MODES)),
            cv.Optional(CONF_SUPPORTED_FAN_MODES): cv.ensure_list(cv.enum(CLIMATE_FAN_MODES)),
            cv.Optional(CONF_ACTION_ID): cv.use_id(TextSensor),
            cv.Optional(CONF_TEMPERATURE_STEP, default=1): cv.temperature_delta,
        }
    )
)

def FINAL_VALIDATE_SCHEMA(config):
    if CONF_FAN_MODE_ID in config and CONF_SUPPORTED_FAN_MODES not in config:
        raise cv.Invalid(f"{CONF_FAN_MODE_ID} requires {CONF_SUPPORTED_FAN_MODES}")
    if CONF_FAN_MODE_ID not in config and CONF_SUPPORTED_FAN_MODES in config:
        raise cv.Invalid(f"{CONF_SUPPORTED_FAN_MODES} is invalid without {CONF_FAN_MODE_ID}")


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    cg.add(var.set_current_temperature(
        await cg.get_variable(config[CONF_CURRENT_TEMPERATURE_ID])
    ))

    cg.add(var.set_target_temperature(
        await cg.get_variable(config[CONF_TARGET_TEMPERATURE_ID])
    ))

    cg.add(var.set_mode(
        await cg.get_variable(config[CONF_MODE_ID])
    ))
    cg.add(var.traits_.set_supported_modes(
        [CLIMATE_MODES[m] for m in config[CONF_SUPPORTED_MODES]]
    ))

    if v := config.get(CONF_FAN_MODE_ID):
        cg.add(var.set_fan_mode(await cg.get_variable(v)))
        cg.add(var.traits_.set_supported_fan_modes(
            [CLIMATE_FAN_MODES[m] for m in config[CONF_SUPPORTED_FAN_MODES]]
        ))

    if v := config.get(CONF_ACTION_ID):
        cg.add(var.set_action(await cg.get_variable(v)))

    cg.add(var.traits_.set_visual_current_temperature_step(
        config[CONF_TEMPERATURE_STEP])
    )
