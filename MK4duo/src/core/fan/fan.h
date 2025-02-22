/**
 * MK4duo Firmware for 3D Printer, Laser and CNC
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (c) 2019 Alberto Cotronei @MagoKimbra
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * fan.h
 *
 * Copyright (c) 2019 Alberto Cotronei @MagoKimbra
 */

#pragma once

#if ENABLED(TACHOMETRIC)
  #include "tachometric.h"
#endif

union fan_flag_t {
  bool all;
  struct {
    bool  HWInvert  : 1;
    bool  Idle      : 1;
    bool  bit2      : 1;
    bool  bit3      : 1;
    bool  bit4      : 1;
    bool  bit5      : 1;
    bool  bit6      : 1;
    bool  bit7      : 1;
  };
  fan_flag_t() { all = false; }
};

// Struct Fan data
struct fan_data_t {
  pin_t           pin;
  fan_flag_t      flag;
  uint8_t         ID,
                  min_speed,
                  max_speed,
                  auto_monitor;
  uint16_t        trigger_temperature,
                  freq;
  #if ENABLED(TACHOMETRIC)
    tacho_data_t  tacho;
  #endif
};

class Fan {

  public: /** Constructor */

    Fan() {}

  public: /** Public Parameters */

    fan_data_t  data;

    uint8_t     speed,
                paused_speed,
                scaled_speed,
                kickstart;

  public: /** Public Function */

    void init();
    void set_speed(const uint8_t new_speed);
    void set_auto_monitor(const int8_t h);
    void set_output_pwm();
    void spin();
    void print_M106();

    inline uint8_t actual_speed() { return ((kickstart ? data.max_speed : speed) * scaled_speed) >> 7; }
    inline uint8_t percent()      { return ui8topercent(actual_speed()); }

    // Fan flag bit 0 Hardware inverted
    FORCE_INLINE void setHWinvert(const bool onoff) { data.flag.HWInvert = onoff; }
    FORCE_INLINE bool isHWinvert()                  { return data.flag.HWInvert; }

    // Fan flag bit 1 Idle
    FORCE_INLINE void setIdle(const bool onoff) {
      if (onoff != isIdle()) {
        data.flag.Idle = onoff;
        if (onoff) {
          paused_speed = speed;
          speed = 0;
        }
        else
          speed = paused_speed;
      }
    }
    FORCE_INLINE bool isIdle() { return data.flag.Idle; }

};

#if HAS_FANS
  extern Fan fans[FAN_COUNT];
#endif // HAS_FANS
