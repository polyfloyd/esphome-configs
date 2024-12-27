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
from esphome.components.number import Number
from esphome.components.text_sensor import TextSensor
from esphome.components.template import template_ns

DEPENDENCIES = ["climate"]

CONF_AUTO_ACTION = "auto_action"
CONF_GET_MODE = "get_mode"
CONF_CURRENT_TEMPERATURE_ID = "current_temperature_id"
CONF_TARGET_TEMPERATURE_ID = "target_temperature_id"
CONF_ACTION_ID = "action_id"

TemplateClimate = template_ns.class_("TemplateClimate",  climate.Climate, cg.Component)

CONFIG_SCHEMA = (
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID(CONF_ID): cv.declare_id(TemplateClimate),
            cv.Required(CONF_CURRENT_TEMPERATURE_ID): cv.use_id(Number),
            cv.Required(CONF_TARGET_TEMPERATURE_ID): cv.use_id(Number),
            cv.Optional(CONF_ACTION_ID): cv.use_id(TextSensor),
            cv.Optional(CONF_TEMPERATURE_STEP, default=1): cv.temperature_delta,
            cv.Required(CONF_IDLE_ACTION): automation.validate_automation(single=True),
            cv.Optional(CONF_COOL_ACTION): automation.validate_automation(single=True),
            cv.Optional(CONF_HEAT_ACTION): automation.validate_automation(single=True),
            cv.Optional(CONF_FAN_ONLY_ACTION): automation.validate_automation(single=True),
            cv.Optional(CONF_DRY_ACTION): automation.validate_automation(single=True),
            cv.Optional(CONF_AUTO_ACTION): automation.validate_automation(single=True),
            cv.Required(CONF_GET_MODE): cv.returning_lambda,
        }
    )
)

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
    if CONF_ACTION_ID in config:
        cg.add(var.set_action(
            await cg.get_variable(config[CONF_ACTION_ID])
        ))
    cg.add(var.set_temperature_step(config[CONF_TEMPERATURE_STEP]))

    get_mode = await cg.process_lambda(
        config[CONF_GET_MODE], [], return_type=climate.ClimateMode
    )
    cg.add(var.set_get_mode(get_mode))

    await automation.build_automation(
        var.get_idle_action_trigger(), [], config[CONF_IDLE_ACTION]
    )
    if CONF_COOL_ACTION in config:
        await automation.build_automation(
            var.get_cool_action_trigger(), [], config[CONF_COOL_ACTION]
        )
    if CONF_HEAT_ACTION in config:
        await automation.build_automation(
            var.get_heat_action_trigger(), [], config[CONF_HEAT_ACTION]
        )
    if CONF_FAN_ONLY_ACTION in config:
        await automation.build_automation(
            var.get_fan_only_action_trigger(), [], config[CONF_FAN_ONLY_ACTION]
        )
    if CONF_DRY_ACTION in config:
        await automation.build_automation(
            var.get_dry_action_trigger(), [], config[CONF_DRY_ACTION]
        )
    if CONF_AUTO_ACTION in config:
        await automation.build_automation(
            var.get_auto_action_trigger(), [], config[CONF_AUTO_ACTION]
        )
