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
#pragma once

/**
 * sanitycheck.h
 *
 * Test configuration values for errors at compile-time.
 */

#if ENABLED(COLOR_MIXING_EXTRUDER)
  #if EXTRUDERS > 1
    #error "DEPENDENCY ERROR: COLOR_MIXING_EXTRUDER supports plus one extruder."
  #endif
  #if MIXING_STEPPERS < 2
    #error "DEPENDENCY ERROR: You must set MIXING_STEPPERS >= 2 for a mixing extruder."
  #endif
  #if ENABLED(FILAMENT_WIDTH_SENSOR)
    #error "DEPENDENCY ERROR: COLOR_MIXING_EXTRUDER is incompatible with FILAMENT_WIDTH_SENSOR. Comment out this line to use it anyway."
  #endif
#endif
