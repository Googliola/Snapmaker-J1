#include "fdm.h"
#include "../../Marlin/src/gcode/gcode.h"
#include "../../Marlin/src/module/tool_change.h"
#include "../../Marlin/src/module/temperature.h"
#include "../../Marlin/src/module/motion.h"
FDM_Head fdm_head;

enum {
  FAN_TYPE_COLD_MODULE = 0,
  FAN_TYPE_COLD_EXTRUDER = 2,
};

enum {
  HEAD_STATUS_OK = 0,
  HEAD_STATUS_UPGRADE,
  HEAD_STATUS_INVALID,
  HEAD_STATUS_UPGRADE_FAILED,
};

ErrCode FDM_Head::set_temperature(uint8_t e, uint16_t temperature) {
  thermalManager.setTargetHotend(temperature, e);
  return E_SUCCESS;
}

float FDM_Head::get_temperature(uint8_t e) {
  return E_SUCCESS;
}

ErrCode FDM_Head::set_work_speed(float speed) {
  return E_SUCCESS;
}

ErrCode FDM_Head::get_filamenter(uint8_t e, bool state) {
  return E_SUCCESS;
}

ErrCode FDM_Head::change_tool(uint8_t e) {
  gcode.reset_stepper_timeout();
  tool_change(e, false);
  return E_SUCCESS;
}

ErrCode FDM_Head::set_fan_speed(uint8_t e, uint8_t fan_index, uint8_t speed) {
  if (e) {
    if (fan_index) {
      thermalManager.set_fan_speed(3, speed);
    } else {
      thermalManager.set_fan_speed(2, speed);
    }
  } else {
    if (fan_index) {
      thermalManager.set_fan_speed(1, speed);
    } else {
      thermalManager.set_fan_speed(0, speed);
    }
  }
  return E_SUCCESS;
}

ErrCode FDM_Head::get_fan_speed(uint8_t e, uint8_t fan_index, uint8_t &speed) {
  if (e) {
    if (fan_index) {
      thermalManager.fanSpeed(3);
    } else {
      thermalManager.fanSpeed(2);
    }
  } else {
    if (fan_index) {
      thermalManager.fanSpeed(1);
    } else {
      thermalManager.fanSpeed(0);
    }
  }
  return E_SUCCESS;
}

ErrCode FDM_Head::set_extruder_diff(uint8_t axis, float diff) {
  return E_SUCCESS;
}

ErrCode FDM_Head::get_fdm_info(uint8_t e, FDM_info *fdm) {
  fdm->key = MODULE_KEY(MODULE_PRINT, e);
  fdm->head_status = HEAD_STATUS_OK;
  fdm->head_active = e == active_extruder;
  fdm->extruder_count = 1;
  get_extruder_info(e, &fdm->extruder_info);
  
  uint8_t index=0, speed=0;
  fdm->fan_count = 2;
  get_fan_speed(e, index, speed);
  fdm->extruder_fan[e].index = index;
  fdm->extruder_fan[e].type = FAN_TYPE_COLD_EXTRUDER;
  fdm->extruder_fan[e].speed = speed;

  index = 1;
  get_fan_speed(e, index, speed);
  fdm->extruder_fan[e].index = index;
  fdm->extruder_fan[e].type = FAN_TYPE_COLD_MODULE;
  fdm->extruder_fan[e].speed = speed;
  
  return E_SUCCESS;
}

ErrCode FDM_Head::get_extruder_info(uint8_t e, extruder_info_t *info) {
  info->index = 0;
  info->is_available = 1;
  info->type = 0;
  info->diameter = FLOAT_TO_INT(0.3);
  info->cur_temp = FLOAT_TO_INT(thermalManager.degHotend(e));
  info->target_temp = FLOAT_TO_INT(thermalManager.degTargetHotend(e));
  return E_SUCCESS;
}

uint8_t FDM_Head::get_key(uint8_t e) {
  return MODULE_KEY(MODULE_PRINT, e);
}

ErrCode FDM_Head::get_module_info(uint8_t e, module_info_t &info) {
  info.key = get_key(e);
  info.module_id = MODULE_PRINT;
  info.module_index = e;
  info.module_state = 0;
  info.sn = 0;
  info.version_length = MODULE_VER_LEN;
  info.version[0] = 0;
  return E_SUCCESS;
}