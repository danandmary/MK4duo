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
 * mcode
 *
 * Copyright (c) 2019 Alberto Cotronei @MagoKimbra
 */

#if HAS_FANS

  #define CODE_M106
  #define CODE_M107

  /**
   * M106: Set Fan Speed
   *
   *  P<index>  Fan index, if more than one fan
   *  S<int>    Speed between 0-255
   *  F<int>    Set PWM frequency
   *  H<int>    Set Auto mode - H=7 for controller - H-1 for disabled
   *  T<int>    Set Triggered temperature
   *  U<int>    Fan Pin
   *  L<int>    Min Speed
   *  X<int>    Max Speed
   *  I<bool>   Inverted pin output
   */
  inline void gcode_M106(void) {

    uint8_t f = 0;

    if (printer.debugSimulation() || !commands.get_target_fan(f)) return;

    const uint8_t new_speed = parser.byteval('S', 255);

    Fan * const fan = &fans[f];

    if (parser.seen('U')) {
      // Put off the fan
      fan->speed = 0;
      fan->data.pin = parser.value_pin();
      SERIAL_LM(ECHO, MSG_CHANGE_PIN);
    }

    if (parser.seen('I'))
      fan->setHWinvert(parser.value_bool());

    if (parser.seen('H'))
      fan->set_auto_monitor(parser.value_int());

    fan->data.min_speed           = parser.byteval('L', fan->data.min_speed);
    fan->data.max_speed           = parser.byteval('X', fan->data.max_speed);
    fan->data.freq                = parser.ushortval('F', fan->data.freq);
    fan->data.trigger_temperature = parser.ushortval('T', fan->data.trigger_temperature);

    fan->set_speed(new_speed);

    #if ENABLED(DUAL_X_CARRIAGE) && FAN_COUNT > 1
      // Check for Clone fan
      if (f == 0 && mechanics.dxc_is_duplicating() && TEST(fans[1].data.auto_monitor, 6))
        fans[1].set_speed(new_speed);
    #endif

    #if DISABLED(DISABLE_M503)
      // No arguments? Show M106 report.
      if (!parser.seen("SUIHLXFT")) fan->print_M106();
    #endif

  }

  /**
   * M107: Fan Off
   */
  inline void gcode_M107(void) {
    uint8_t f = 0;
    if (printer.debugSimulation() || !commands.get_target_fan(f)) return;
    fans[f].speed = 0;
  }

#endif // HAS_FANS
