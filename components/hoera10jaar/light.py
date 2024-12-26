import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    CONF_OUTPUT_ID
)
from esphome.components import light

from . import Hoera10JaarComponent, hoera10jaar_ns

DEPENDENCIES = ["light"]

CONF_HOERA10JAAR_ID = 'hoera10jaar_id'

Hoera10JaarLight = hoera10jaar_ns.class_("Hoera10JaarLight",  light.LightOutput)

CONFIG_SCHEMA = (
    light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend(
        {
            cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(Hoera10JaarLight),
            cv.Required(CONF_HOERA10JAAR_ID): cv.use_id(Hoera10JaarComponent),
        }
    )
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    parent = await cg.get_variable(config[CONF_HOERA10JAAR_ID])
    cg.add(var.set_parent(parent))
    await light.register_light(var, config)
