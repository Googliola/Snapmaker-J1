#include "event_base.h"
#include "event_fdm.h"
#include "subscribe.h"
#include "../module/fdm.h"
#include "../module/motion_control.h"

#pragma pack(1)
typedef struct {
  uint8_t key;
  uint8_t index;
  uint16_t temperature;
} set_temperature_t;

typedef struct {
  uint8_t key;
  uint8_t index;
  uint8_t speed;
} set_fan_t;

typedef enum : uint8_t {
  EXTRUDER_TO_RETRACK,
  RETRACK_TO_EXTRUDER,
} extrusion_type_e;

typedef struct {
  uint8_t key;
  extrusion_type_e move_type;
  float_to_int_t extruder_length;
  float_to_int_t extruder_speed;
  float_to_int_t retrack_length;
  float_to_int_t retrack_speed;
} extrusion_control_t;

#pragma pack()

static ErrCode fdm_get_info(event_param_t& event) {
  uint8_t e = MODULE_INDEX(event.data[0]);
  event.data[0] = E_SUCCESS;
  FDM_info * info = (FDM_info *)(event.data + 1);
  fdm_head.get_fdm_info(e, info);
  event.length = sizeof(FDM_info) + 1;
  send_event(event);
  return E_SUCCESS;
}

static ErrCode fdm_set_temperature(event_param_t& event) {
  set_temperature_t *t = (set_temperature_t *)event.data;
  uint8_t e = MODULE_INDEX(t->key);
  SERIAL_ECHOLNPAIR("set temperature e:", e, " t:", t->temperature);
  fdm_head.set_temperature(e, t->temperature);
  event.data[0] = E_SUCCESS;
  event.length = 1;
  return send_event(event);
}

static ErrCode fdm_set_work_speed(event_param_t& event) {
  return E_SUCCESS;
}

static ErrCode fdm_enable_filament_sensor(event_param_t& event) {
  return E_SUCCESS;
}

static ErrCode fdm_change_tool_head(event_param_t& event) {
  fdm_head.change_tool(MODULE_INDEX(event.data[0]));
  event.data[0] = E_SUCCESS;
  event.length = 1;
  return send_event(event);
}

static ErrCode fdm_set_fan_speed(event_param_t& event) {
  set_fan_t *fan = (set_fan_t *)event.data;
  uint8_t e = MODULE_INDEX(fan->key);
  SERIAL_ECHOLNPAIR("set fan e:", e , " index:", fan->index, " speed:", fan->speed);
  fdm_head.set_fan_speed(e, fan->index, fan->speed);
  event.data[0] = E_SUCCESS;
  event.length = 1;
  return send_event(event);
}

static ErrCode fdm_set_nozzle_spacing(event_param_t& event) {
  return E_SUCCESS;
}

static ErrCode fdm_get_nozzle_spacing(event_param_t& event) {
  return E_SUCCESS;
}


static ErrCode fdm_extrusion_control(event_param_t& event) {
  extrusion_control_t * extrusion = (extrusion_control_t *)event.data;
  float_to_int_t  retrack_length = INT_TO_FLOAT(extrusion->retrack_length);
  float_to_int_t  retrack_speed = INT_TO_FLOAT(extrusion->retrack_speed);
  float_to_int_t  extruder_length = INT_TO_FLOAT(extrusion->extruder_length);
  float_to_int_t  extruder_speed = INT_TO_FLOAT(extrusion->extruder_speed);
  if (extrusion->move_type == EXTRUDER_TO_RETRACK) {
    motion_control.extrude_e(extruder_length, extruder_speed);
    motion_control.synchronize();
    motion_control.retrack_e(retrack_length, retrack_speed);
    motion_control.synchronize();
  } else {
    motion_control.retrack_e(retrack_length, retrack_speed);
    motion_control.synchronize();
    motion_control.extrude_e(extruder_length, extruder_speed);
    motion_control.synchronize();
  }
  event.data[0] = E_SUCCESS;
  event.length = 1;
  return send_event(event);
}

static ErrCode fdm_report_subscribe_info(event_param_t& event) {
  event.data[0] = E_SUCCESS;
  event.data[2] = 1;  // extruder count
  extruder_info_t *info = (extruder_info_t *)(event.data + 3);
  HOTEND_LOOP() {
    event.data[1] = fdm_head.get_key(e);
    fdm_head.get_extruder_info(e, info);
    event.length = sizeof(extruder_info_t) + 3;
    send_event(event);
  }
  return E_SUCCESS;
}



event_cb_info_t fdm_cb_info[FDM_ID_CB_COUNT] = {
  {FDM_ID_GET_INFO              , EVENT_CB_DIRECT_RUN, fdm_get_info},
  {FDM_ID_SET_TEMPERATURE       , EVENT_CB_DIRECT_RUN, fdm_set_temperature},
  {FDM_ID_SET_WORK_SPEED        , EVENT_CB_DIRECT_RUN, fdm_set_work_speed},
  {FDM_ID_ENABLE_FILAMENT_SENSOR, EVENT_CB_DIRECT_RUN, fdm_enable_filament_sensor},
  {FDM_ID_CHANGE_TOOL_HEAD      , EVENT_CB_DIRECT_RUN, fdm_change_tool_head},
  {FDM_ID_SET_FAN_SPEED         , EVENT_CB_DIRECT_RUN, fdm_set_fan_speed},
  {FDM_ID_SET_NOZZLE_SPACING    , EVENT_CB_DIRECT_RUN, fdm_set_nozzle_spacing},
  {FDM_ID_GET_NOZZLE_SPACING    , EVENT_CB_DIRECT_RUN, fdm_get_nozzle_spacing},
  {FDM_ID_EXTRUSION_CONTROL     , EVENT_CB_DIRECT_RUN, fdm_extrusion_control},
  {FDM_ID_REPORT_SUBSCRIBE_INFO , EVENT_CB_DIRECT_RUN, fdm_report_subscribe_info},
};