import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    CONF_ID
)

hoera10jaar_ns = cg.esphome_ns.namespace("hoera10jaar")
Hoera10JaarComponent = hoera10jaar_ns.class_("Hoera10JaarComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Hoera10JaarComponent),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
