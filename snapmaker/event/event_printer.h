#ifndef EVENT_PRINTER_H
#define EVENT_PRINTER_H
#include "event_base.h"

enum {
  PRINTER_ID_REQ_FILE_INFO        = 0x00,
  PRINTER_ID_REPORT_STATUS        = 0x01,
  PRINTER_ID_REQ_GCODE            = 0x02,
  PRINTER_ID_START_WORK           = 0x03,
  PRINTER_ID_PAUSE_WORK           = 0x04,
  PRINTER_ID_RESUME_WORK          = 0x05,
  PRINTER_ID_STOP_WORK            = 0x06,
  PRINTER_ID_REQ_PL_STATUS        = 0x07,  // PL -> power loss
  PRINTER_ID_PL_RESUME            = 0x08,  // power loss resume
  PRINTER_ID_CLEAN_PL_DATA        = 0x09,
  PRINTER_ID_SET_MODE             = 0x0A,
  PRINTER_ID_REQ_AUTO_PARK_STATUS = 0x0B,
  PRINTER_ID_SET_AUTO_PARK_STATUS = 0x0C,
  PRINTER_ID_REQ_LINE             = 0xA0,
};

#define PRINTER_ID_CB_COUNT 13

extern event_cb_info_t printer_cb_info[PRINTER_ID_CB_COUNT];
void printer_event_init(void);
void printer_event_loop(void);
#endif