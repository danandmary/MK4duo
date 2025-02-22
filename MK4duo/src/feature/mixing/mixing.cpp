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
 * mixing.cpp
 *
 * Copyright (c) 2019 Alberto Cotronei @MagoKimbra
 */

#include "../../../MK4duo.h"
#include "sanitycheck.h"

#if ENABLED(COLOR_MIXING_EXTRUDER)

Mixer mixer;

/** Public Parameters */
float Mixer::collector[MIXING_STEPPERS]; // mix proportion. 0.0 = off, otherwise <= COLOR_A_MASK.

#if HAS_GRADIENT_MIX

  mixer_perc_t Mixer::mix[MIXING_STEPPERS];

  gradient_t Mixer::gradient = {
    false,        // enabled
    { 0 },        // color (array)
    0, 0,         // start_z, end_z
    0, 1,         // start_vtool, end_vtool
    { 0 }, { 0 }  // start_mix, end_mix (array)
  };

  float Mixer::prev_z = 0;

#endif

/** Private Parameters */
// Used up to Planner level
uint_fast8_t  Mixer::selected_vtool = 0;
mixer_color_t Mixer::color[MIXING_VIRTUAL_TOOLS][MIXING_STEPPERS];

// Used in Stepper
int_fast8_t   Mixer::runner = 0;
mixer_color_t Mixer::s_color[MIXING_STEPPERS];
mixer_accu_t  Mixer::accu[MIXING_STEPPERS] = { 0 };

void Mixer::normalize(const uint8_t tool_index) {
  float cmax = 0;
  #if ENABLED(MIXING_DEBUG)
    float csum = 0;
  #endif

  MIXING_STEPPER_LOOP(i) {
    const float v = collector[i];
    NOLESS(cmax, v);
    #if ENABLED(MIXING_DEBUG)
      csum += v;
    #endif
  }
  #if ENABLED(MIXING_DEBUG)
    SERIAL_MSG("Mixer: Old relation : [ ");
    MIXING_STEPPER_LOOP(i) {
      SERIAL_VAL(collector[i] / csum, 3);
      SERIAL_CHR(' ');
    }
    SERIAL_EM("]");
  #endif

  // Scale all values so their maximum is COLOR_A_MASK
  const float scale = float(COLOR_A_MASK) / cmax;
  MIXING_STEPPER_LOOP(i)
    color[tool_index][i] = collector[i] * scale;

  #if ENABLED(MIXING_DEBUG)
    csum = 0;
    SERIAL_MSG("Mixer: Normalize to : [ ");
    MIXING_STEPPER_LOOP(i) {
      SERIAL_VAL(uint16_t(color[tool_index][i]));
      SERIAL_CHR(' ');
      csum += color[tool_index][i];
    }
    SERIAL_EM("]");
    SERIAL_MSG("Mixer: New relation : [ ");
    MIXING_STEPPER_LOOP(i) {
      SERIAL_VAL(uint16_t(color[tool_index][i]) / csum, 3);
      SERIAL_CHR(' ');
    }
    SERIAL_EM("]");
  #endif

  #if HAS_GRADIENT_MIX
    refresh_gradient();
  #endif

}

void Mixer::reset_vtools() {
  // Virtual Tools 0, 1, 2, 3 = Filament 1, 2, 3, 4, etc.
  // Every virtual tool gets a pure filament
  for (uint8_t t = 0; t < MIXING_VIRTUAL_TOOLS && t < MIXING_STEPPERS; t++)
    MIXING_STEPPER_LOOP(i)
      color[t][i] = (t == i) ? COLOR_A_MASK : 0;

  // Remaining virtual tools are 100% filament 1
  #if MIXING_VIRTUAL_TOOLS > MIXING_STEPPERS
    for (uint8_t t = MIXING_STEPPERS; t < MIXING_VIRTUAL_TOOLS; t++)
      MIXING_STEPPER_LOOP(i)
        color[t][i] = (i == 0) ? COLOR_A_MASK : 0;
  #endif
}

// called at boot
void Mixer::init() {
  reset_vtools();
  ZERO(collector);
  #if HAS_GRADIENT_MIX
    update_mix_from_vtool();
    update_gradient_for_planner_z();
  #endif
}

void Mixer::refresh_collector(const float proportion/*=1.0*/, const uint8_t t/*=selected_vtool*/) {
  float csum = 0, cmax = 0;
  MIXING_STEPPER_LOOP(i) {
    const float v = color[t][i];
    cmax = MAX(cmax, v);
    csum += v;
  }
  const float inv_prop = proportion / csum;
  MIXING_STEPPER_LOOP(i) collector[i] = color[t][i] * inv_prop;
}

#if HAS_GRADIENT_MIX

  void Mixer::update_gradient_for_z(const float z) {
    if (z == prev_z) return;
    prev_z = z;

    const float slice = gradient.end_z - gradient.start_z;

    float pct = float(z - gradient.start_z) / slice;
    NOLESS(pct, 0.0f); NOMORE(pct, 1.0f);

    MIXING_STEPPER_LOOP(i) {
      const mixer_perc_t sm = gradient.start_mix[i];
      mix[i] = sm + (gradient.end_mix[i] - sm) * pct;
    }

    copy_mix_to_color(gradient.color);
  }

  void Mixer::update_gradient_for_planner_z() {
    update_gradient_for_z(mechanics.current_position[Z_AXIS]);
  }

#endif

#endif // ENABLED(COLOR_MIXING_EXTRUDER)
