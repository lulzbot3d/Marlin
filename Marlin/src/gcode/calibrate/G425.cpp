/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../../Marlin.h"


#if ENABLED(LULZBOT_CALIBRATION_GCODE)

#include "../gcode.h"
#include "../../lcd/ultralcd.h"
#include "../../module/motion.h"
#include "../../module/planner.h"
#include "../../module/tool_change.h"
#include "../../module/endstops.h"

#define CALIBRATION_MEASUREMENT_RESOLUTION       0.01                    // mm
#define CALIBRATION_MEASUREMENT_LIMIT            5.0                     // mm

#define CALIBRATION_SLOW_FEEDRATE                Z_PROBE_SPEED_SLOW      // mm/m
#define CALIBRATION_FAST_FEEDRATE                Z_PROBE_SPEED_FAST      // mm/m
#define CALIBRATION_TRAVEL_FEEDRATE              HOMING_FEEDRATE_XY      // mm/m

#define CALIBRATION_NOZZLE_TIP_PLUNGE            0.7                     // mm
#define CALIBRATION_NOZZLE_TIP_DIAMETER          2.0                     // mm

// We keep a confidence interval of how close we are to the true measurement:
//
//   UNKNOWN   - No real notion on where the cube is on the bed
//   UNCERTAIN - Measurement may be uncertain due to backlash
//   CERTAIN   - Measurement obtained with backlash compensation
//
#define CALIBRATION_MEASUREMENT_UNKNOWN           5.0                    // mm
#define CALIBRATION_MEASUREMENT_UNCERTAIN         1.0                    // mm
#define CALIBRATION_MEASUREMENT_CERTAIN           0.5                    // mm

#if ENABLED(BACKLASH_GCODE)
  extern float backlash_distance_mm[];
  extern float backlash_correction, backlash_smoothing_mm;
#endif

struct measurements_t {
  static constexpr float true_center[XYZ] = CALIBRATION_CUBE_CENTER;
  static constexpr float dimensions[XYZ]  = CALIBRATION_CUBE_DIMENSIONS;

  float cube_top;
  float cube_left;
  float cube_right;
  float cube_front;
  float cube_back;

  float center[XYZ];

  float backlash_xl;
  float backlash_xr;
  float backlash_yf;
  float backlash_yb;
  float backlash_zt;

  float nozzle_x_width;
  float nozzle_y_depth;
  float nozzle_tip_radius;

  float error[XYZ];

  measurements_t() {
    center[X_AXIS] = true_center[X_AXIS];
    center[Y_AXIS] = true_center[Y_AXIS];
    center[Z_AXIS] = true_center[Z_AXIS];

    nozzle_tip_radius = CALIBRATION_NOZZLE_TIP_DIAMETER/2;
  }
};

#define HAS_X_CENTER defined(CALIBRATION_CUBE_LEFT_SIDE_MEASUREMENT)  && defined(CALIBRATION_CUBE_RIGHT_SIDE_MEASUREMENT)
#define HAS_Y_CENTER defined(CALIBRATION_CUBE_FRONT_SIDE_MEASUREMENT) && defined(CALIBRATION_CUBE_BACK_SIDE_MEASUREMENT)

static void calibrate_all();
static void calibrate_backlash(measurements_t &m, float confidence);
static void calibrate_toolhead(measurements_t &m, float confidence, uint8_t extruder);
static void calibrate_all_toolheads(measurements_t &m, float confidence);

static void report_measured_faces(const measurements_t &m);
static void report_measured_center(const measurements_t &m);
static void report_measured_backlash(const measurements_t &m);
static void report_measured_nozzle_error(const measurements_t &m);
static void report_measured_nozzle_dimensions(const measurements_t &m);
#if HOTENDS > 1
  static void report_relative_nozzle_offsets();
  static void normalize_nozzle_offsets();
  static void park_above_cube(measurements_t &m, float confidence);
#endif
static void probe_cube(measurements_t &m, float confidence);
static void move_to(
  const AxisEnum a1,           const float p1,
  const AxisEnum a2 = NO_AXIS, const float p2 = 0,
  const AxisEnum a3 = NO_AXIS, const float p3 = 0
);
static float measure(const AxisEnum axis, const int dir, const bool stopping_state, float *backlash_ptr, bool fast);

class TemporaryBacklashCompensation {
    #if ENABLED(BACKLASH_GCODE)
      float saved_backlash_correction;
      #ifdef BACKLASH_SMOOTHING_MM
        float saved_backlash_smoothing_mm;
      #endif
    #endif

  public:
    // Saves the current backlash compensation state and sets it to
    // the value of enable
    TemporaryBacklashCompensation(bool enable) {
      #if ENABLED(BACKLASH_GCODE)
        saved_backlash_correction   = backlash_correction;
        #ifdef BACKLASH_SMOOTHING_MM
          saved_backlash_smoothing_mm = backlash_smoothing_mm;
          backlash_smoothing_mm = 0;
        #endif
        backlash_correction = enable;
      #endif
    }

    // Restore user specified backlash options
    ~TemporaryBacklashCompensation() {
      #if ENABLED(BACKLASH_GCODE)
        backlash_correction = saved_backlash_correction;
        #ifdef BACKLASH_SMOOTHING_MM
          backlash_smoothing_mm = saved_backlash_smoothing_mm;
        #endif
      #endif
    }
};

class TemporaryEndstopsState {
  private:
    bool saved_endstops_enabled_globally, saved_soft_endstops_enabled;

  public:
    // Saves the software endstops state and sets it to
    // the value of enable
    TemporaryEndstopsState(bool enable) {
      saved_endstops_enabled_globally = endstops.are_endstops_enabled_globally();
      endstops.enable_globally(enable);

      saved_soft_endstops_enabled = soft_endstops_enabled;
      soft_endstops_enabled = enable;
    }

    // Restore user specified software endstop state
    ~TemporaryEndstopsState() {
      soft_endstops_enabled = saved_soft_endstops_enabled;
      endstops.enable_globally(saved_endstops_enabled_globally);
    }
};

/**
 * G425: Perform calibration with calibration cube.
 *
 *   B           - Perform calibration of backlash only.
 *   T<extruder> - Perform calibration of toolhead only.
 *   V           - Probe cube and print position, error and backlash.
 *   U           - Uncertainty, how far to start probe away from the cube (mm)
 *
 *   no args     - Perform entire calibration sequence (backlash + all toolheads)
 */
void GcodeSuite::G425() {
  if (axis_unhomed_error()) return;

  measurements_t m;

  float confidence = parser.seenval('U') ? parser.value_float() : CALIBRATION_MEASUREMENT_UNCERTAIN;

  if (parser.seen('V')) {
    ui.set_status_P(PSTR("Measuring nozzle center"));
    probe_cube(m, confidence);
    SERIAL_ECHOLNPGM("Measurement results: ");
    SERIAL_EOL();
    report_measured_faces(m);
    report_measured_center(m);
    SERIAL_EOL();
    report_measured_backlash(m);
    SERIAL_EOL();
    report_measured_nozzle_error(m);
    SERIAL_EOL();
    report_measured_nozzle_dimensions(m);
    ui.set_status_P(PSTR("Measurements finished."));
  } else if(parser.seen('B')) {
    calibrate_backlash(m, confidence);
    ui.set_status_P(PSTR("Calibration done."));
  } else if(parser.seen('T')) {
    calibrate_toolhead(m, confidence, parser.has_value() ? parser.value_int() : active_extruder);
    ui.set_status_P(PSTR("Calibration done."));
  }
  else {
    calibrate_all();
  }
}

#if HOTENDS > 1
  static void park_above_cube(measurements_t &m, float confidence) {
    // Move to safe distance above cube/washer
    move_to(Z_AXIS, m.center[Z_AXIS] + m.dimensions[Z_AXIS]/2 + confidence);

    // Move to center of cube in XY
    move_to(X_AXIS, m.center[X_AXIS], Y_AXIS, m.center[Y_AXIS]);
  }

  static void set_nozzle(measurements_t &m, uint8_t extruder) {
      if(extruder != active_extruder) {
        park_above_cube(m, CALIBRATION_MEASUREMENT_UNKNOWN);
        tool_change(extruder);
      }
  }

  static void reset_nozzle_offsets() {
      hotend_offset[X_AXIS][0] = 0;
      hotend_offset[Y_AXIS][0] = 0;
      hotend_offset[Z_AXIS][0] = 0;
      constexpr float tmp4[XYZ][HOTENDS] = { HOTEND_OFFSET_X, HOTEND_OFFSET_Y, HOTEND_OFFSET_Z };
      LOOP_XYZ(i) HOTEND_LOOP() hotend_offset[i][e] = tmp4[i][e];
  }
#endif

static void calibrate_all() {
  measurements_t m;

  SERIAL_EOL();

  #if HOTENDS > 1
    reset_nozzle_offsets();
  #endif

  // Make sure backlash compensation is enabled for the subsequent steps
  TemporaryBacklashCompensation s(true);

  // Do a fast and rough calibration of the toolheads
  ui.set_status_P(PSTR("Finding calibration cube"));
  calibrate_all_toolheads(m, CALIBRATION_MEASUREMENT_UNKNOWN);

  #if ENABLED(BACKLASH_GCODE)
    calibrate_backlash(m, CALIBRATION_MEASUREMENT_UNCERTAIN);
    SERIAL_ECHOLNPGM("Backlash before correction:");
    report_measured_backlash(m);
  #endif

  // Cycle the toolheads so the servos settle into their "natural" positions
  #if HOTENDS > 1
    HOTEND_LOOP() set_nozzle(m, e);
  #endif

  // Do a slow and precise calibration of the toolheads
  calibrate_all_toolheads(m, CALIBRATION_MEASUREMENT_UNCERTAIN);

  ui.set_status_P(PSTR("Calibration done."));
  move_to(X_AXIS, 150); // Park nozzle away from cube
}

/* Probes around the calibration cube measuring the backlash */
static void calibrate_backlash(measurements_t &m, float confidence) {
  #if ENABLED(BACKLASH_GCODE)
    {
      // Backlash compensation should be off while measuring backlash
      TemporaryBacklashCompensation s(false);

      ui.set_status_P(PSTR("Measuring backlash"));
      probe_cube(m, confidence);

      #if HAS_X_CENTER
        backlash_distance_mm[X_AXIS] = (m.backlash_xl + m.backlash_xr)/2;
      #elif defined(CALIBRATION_CUBE_LEFT_SIDE_MEASUREMENT)
        backlash_distance_mm[X_AXIS] = m.backlash_xl;
      #elif defined(CALIBRATION_CUBE_RIGHT_SIDE_MEASUREMENT)
        backlash_distance_mm[X_AXIS] = m.backlash_xr;
      #endif

      #if HAS_Y_CENTER
        backlash_distance_mm[Y_AXIS] = (m.backlash_yf + m.backlash_yb)/2;
      #elif defined(CALIBRATION_CUBE_FRONT_SIDE_MEASUREMENT)
        backlash_distance_mm[Y_AXIS] = m.backlash_yf;
      #elif defined(CALIBRATION_CUBE_BACK_SIDE_MEASUREMENT)
        backlash_distance_mm[Y_AXIS] = m.backlash_yb;
      #endif

      backlash_distance_mm[Z_AXIS] = m.backlash_zt;
    }

    // Turn on backlash compensation and move in all
    // directions to take up any backlash
    {
      TemporaryBacklashCompensation s(true);
      planner.synchronize();
      move_to(
        X_AXIS, current_position[X_AXIS] + 3,
        Y_AXIS, current_position[Y_AXIS] + 3,
        Z_AXIS, current_position[Z_AXIS] + 3
      );
      planner.synchronize();
      move_to(
        X_AXIS, current_position[X_AXIS] - 3,
        Y_AXIS, current_position[Y_AXIS] - 3,
        Z_AXIS, current_position[Z_AXIS] - 3
      );
      planner.synchronize();
    }
  #endif
}

/* Probes around the calibration cube for all toolheads, adjusting the coordinate
 * system for the first nozzle and the nozzle offset for subsequent nozzles.
 */
static void calibrate_all_toolheads(measurements_t &m, float confidence) {
  // For all subsequent steps, make sure backlash compensation is on.
  TemporaryBacklashCompensation s(true);

  HOTEND_LOOP() calibrate_toolhead(m, confidence, e);

  SERIAL_EOL();
  #if HOTENDS > 1
    normalize_nozzle_offsets();
    set_nozzle(m, 0);
    report_relative_nozzle_offsets();
  #endif
}

/* Probes around the calibration cube and then adjusts the position and
 * toolhead offset using the deviation from the known position of the
 * calibration cube.
 *
 * Prerequisites:
 *    - calibrate_backlash()
 */
static void calibrate_toolhead(measurements_t &m, float confidence, uint8_t extruder) {
  // For all subsequent steps, make sure backlash compensation is on.
  TemporaryBacklashCompensation s(true);

  const bool fast = confidence == CALIBRATION_MEASUREMENT_UNKNOWN;

  #if HOTENDS > 1
    set_nozzle(m, extruder);
  #endif

  ui.set_status_P(fast ? PSTR("Finding calibration cube") : PSTR("Centering nozzle"));
  probe_cube(m, confidence);
  if(!fast) {
    #if ENABLED(BACKLASH_GCODE)
      SERIAL_ECHOPAIR("Backlash (T", extruder);
      SERIAL_ECHOLNPGM("):");
      report_measured_backlash(m);
    #endif
  }

  /* Adjust the hotend offset */
  #if HOTENDS > 1
    #if HAS_X_CENTER
      hotend_offset[X_AXIS][extruder] += m.error[X_AXIS];
    #endif
    #if HAS_Y_CENTER
      hotend_offset[Y_AXIS][extruder] += m.error[Y_AXIS];
    #endif
    hotend_offset[Z_AXIS][extruder] += m.error[Z_AXIS];
  #endif

  /* Correct for positional error, so the cube
  /* is at the known actual spot */
  planner.synchronize();
  #if HAS_X_CENTER
    current_position[X_AXIS] += m.error[X_AXIS];
    m.center        [X_AXIS]  = m.true_center[X_AXIS];
    m.error         [X_AXIS]  = 0;
  #endif
  #if HAS_Y_CENTER
    current_position[Y_AXIS] += m.error[Y_AXIS];
    m.center        [Y_AXIS]  = m.true_center[Y_AXIS];
    m.error         [Y_AXIS]  = 0;
  #endif
  current_position  [Z_AXIS] += m.error[Z_AXIS];
  m.center          [Z_AXIS]  = m.true_center[Z_AXIS];
  m.error           [Z_AXIS]  = 0;
  sync_plan_position();
}

static void report_measured_faces(const measurements_t &m) {
    SERIAL_ECHOLNPAIR("  Top   Z:            ", m.cube_top);
  #if defined(CALIBRATION_CUBE_LEFT_SIDE_MEASUREMENT)
    SERIAL_ECHOLNPAIR("  Left  X:            ", m.cube_left);
  #endif
  #if defined(CALIBRATION_CUBE_RIGHT_SIDE_MEASUREMENT)
    SERIAL_ECHOLNPAIR("  Right X:            ", m.cube_right);
  #endif
  #if defined(CALIBRATION_CUBE_FRONT_SIDE_MEASUREMENT)
    SERIAL_ECHOLNPAIR("  Front Y:            ", m.cube_front);
  #endif
  #if defined(CALIBRATION_CUBE_BACK_SIDE_MEASUREMENT)
    SERIAL_ECHOLNPAIR("  Back  Y:            ", m.cube_back);
  #endif
}

static void report_measured_center(const measurements_t &m) {
  #if HAS_X_CENTER
    SERIAL_ECHOLNPAIR("  Center X:           ", m.center[X_AXIS]);
  #endif
  #if HAS_Y_CENTER
    SERIAL_ECHOLNPAIR("  Center Y:           ", m.center[Y_AXIS]);
  #endif
  SERIAL_ECHOLNPAIR("  Center Z:           ", m.center[Z_AXIS]);
}

static void report_measured_backlash(const measurements_t &m) {
  #if defined(CALIBRATION_CUBE_LEFT_SIDE_MEASUREMENT)
    SERIAL_ECHOLNPAIR("  Backlash X (left):  ", m.backlash_xl);
  #endif
  #if defined(CALIBRATION_CUBE_RIGHT_SIDE_MEASUREMENT)
    SERIAL_ECHOLNPAIR("  Backlash X (right): ", m.backlash_xr);
  #endif
  #if defined(CALIBRATION_CUBE_FRONT_SIDE_MEASUREMENT)
    SERIAL_ECHOLNPAIR("  Backlash Y (front): ", m.backlash_yf);
  #endif
  #if defined(CALIBRATION_CUBE_BACK_SIDE_MEASUREMENT)
    SERIAL_ECHOLNPAIR("  Backlash Y (back):  ", m.backlash_yb);
  #endif
  SERIAL_ECHOLNPAIR(  "  Backlash Z (top):   ", m.backlash_zt);
}

static void report_measured_nozzle_error(const measurements_t &m) {
  SERIAL_ECHOLNPAIR(  "  Active Nozzle:      ", active_extruder);
  #if HAS_X_CENTER
    SERIAL_ECHOLNPAIR("  Positional Error X: ", m.error[X_AXIS]);
  #endif
  #if HAS_Y_CENTER
    SERIAL_ECHOLNPAIR("  Positional Error Y: ", m.error[Y_AXIS]);
  #endif
  SERIAL_ECHOLNPAIR(  "  Positional Error Z: ", m.error[Z_AXIS]);
}

#if HOTENDS > 1
static void normalize_nozzle_offsets() {
  for (uint8_t e = 1; e < HOTENDS; e++) {
    hotend_offset[X_AXIS][e] -= hotend_offset[X_AXIS][0];
    hotend_offset[Y_AXIS][e] -= hotend_offset[Y_AXIS][0];
    hotend_offset[Z_AXIS][e] -= hotend_offset[Z_AXIS][0];
  }
  hotend_offset[X_AXIS][0] = 0;
  hotend_offset[Y_AXIS][0] = 0;
  hotend_offset[Z_AXIS][0] = 0;
}

static void report_relative_nozzle_offsets() {
  for (uint8_t e = 1; e < HOTENDS; e++) {
    SERIAL_ECHOPAIR("Nozzle offset for T", e);
    SERIAL_ECHOLNPGM(":");
    SERIAL_ECHOLNPAIR("  Offset X:       ", hotend_offset[X_AXIS][e] - hotend_offset[X_AXIS][0]);
    SERIAL_ECHOLNPAIR("  Offset Y:       ", hotend_offset[Y_AXIS][e] - hotend_offset[Y_AXIS][0]);
    SERIAL_ECHOLNPAIR("  Offset Z:       ", hotend_offset[Z_AXIS][e] - hotend_offset[Z_AXIS][0]);
  }
}
#endif

static void report_measured_nozzle_dimensions(const measurements_t &m) {
  #if HAS_X_CENTER
    SERIAL_ECHOLNPAIR("  Nozzle Width:       ", m.nozzle_x_width);
  #endif
  #if HAS_Y_CENTER
    SERIAL_ECHOLNPAIR("  Nozzle Depth:       ", m.nozzle_y_depth);
  #endif
}

static void probe_cube(measurements_t &m, float confidence) {
  constexpr float plunge           = CALIBRATION_NOZZLE_TIP_PLUNGE;
  const bool fast                  = confidence == CALIBRATION_MEASUREMENT_UNKNOWN;
  #ifndef CALIBRATION_CUBE_TOP_CENTER_MEASUREMENT
    constexpr float inset          = 2;
  #endif

  TemporaryEndstopsState s(false);

  #ifdef CALIBRATION_CUBE_TOP_CENTER_MEASUREMENT
    // Move to safe distance above cube/washer
    move_to(Z_AXIS, m.center[Z_AXIS] + m.dimensions[Z_AXIS]/2 + confidence);

    // Move to center of cube in XY
    move_to(X_AXIS, m.center[X_AXIS], Y_AXIS, m.center[Y_AXIS]);

    // Probe exact top of cube
    m.cube_top = measure(Z_AXIS, -1, true, &m.backlash_zt, fast);
    m.center    [Z_AXIS] = m.cube_top - m.dimensions[Z_AXIS]/2;
  #endif

  #ifdef CALIBRATION_CUBE_RIGHT_SIDE_MEASUREMENT
    // Move to safe distance above cube/washer
    move_to(Z_AXIS, m.center[Z_AXIS] + m.dimensions[Z_AXIS]/2 + confidence);

    #ifdef CALIBRATION_CUBE_TOP_CENTER_MEASUREMENT
      // Move to center of cube in XY
      move_to(X_AXIS, m.center[X_AXIS], Y_AXIS, m.center[Y_AXIS]);
    #else
      // Probe top nearest right side
      move_to(X_AXIS, m.center[X_AXIS] + m.dimensions[X_AXIS]/2 - inset, Y_AXIS, m.center[Y_AXIS]);
      m.cube_top       = measure(Z_AXIS, -1, true, &m.backlash_zt, fast);
      m.center[Z_AXIS] = m.cube_top - m.dimension[Z_AXIS]/2;
    #endif

    // Move to safe distance to the size of the cube
    move_to(X_AXIS, m.center[X_AXIS] + m.dimensions[X_AXIS]/2 + m.nozzle_tip_radius + confidence);

    // Plunge below the side of the cube and measure
    move_to(Z_AXIS, m.cube_top - plunge);
    m.cube_right = measure(X_AXIS, -1, true, &m.backlash_xr, fast);
    m.center    [X_AXIS] = m.cube_right - m.dimensions[X_AXIS]/2 - m.nozzle_tip_radius;
  #endif

  #ifdef CALIBRATION_CUBE_FRONT_SIDE_MEASUREMENT
    // Move to safe distance above cube/washer
    move_to(Z_AXIS, m.center[Z_AXIS] + m.dimensions[Z_AXIS]/2 + confidence);

    #ifdef CALIBRATION_CUBE_TOP_CENTER_MEASUREMENT
      // Move to center of cube in XY
      move_to(X_AXIS, m.center[X_AXIS], Y_AXIS, m.center[Y_AXIS]);
    #else // Probe top nearest front side
      move_to(X_AXIS, m.center[X_AXIS], Y_AXIS, m.center[Y_AXIS] - m.dimensions[Y_AXIS]/2 + inset);
      m.cube_top = measure(Z_AXIS, -1, true, &m.backlash_zt, fast);
      m.center[Z_AXIS] = m.cube_top - m.dimensions[Z_AXIS]/2;
    #endif

    // Move to safe distance to the size of the cube
    move_to(Y_AXIS, m.center[Y_AXIS] - m.dimensions[Y_AXIS]/2 - m.nozzle_tip_radius - confidence);

    // Plunge below the side of the cube and measure
    move_to(Z_AXIS, m.cube_top - plunge);
    m.cube_front = measure(Y_AXIS, 1, true, &m.backlash_yf, fast);
    m.center    [Y_AXIS] = m.cube_front + m.dimensions[Y_AXIS]/2 + m.nozzle_tip_radius;
  #endif

  #ifdef CALIBRATION_CUBE_LEFT_SIDE_MEASUREMENT
    // Move to safe distance above cube/washer
    move_to(Z_AXIS, m.center[Z_AXIS] + m.dimensions[Z_AXIS]/2 + confidence);

    #ifdef CALIBRATION_CUBE_TOP_CENTER_MEASUREMENT
      // Move to center of cube in XY
      move_to(X_AXIS, m.center[X_AXIS], Y_AXIS, m.center[Y_AXIS]);
    #else
      // Probe top nearest left side
      move_to(X_AXIS, m.center[X_AXIS] - m.dimensions[X_AXIS]/2 + inset, Y_AXIS, m.center[Y_AXIS]);
      m.cube_top = measure(Z_AXIS, -1, true, &m.backlash_zt, fast);
      m.center[Z_AXIS] = m.cube_top - m.dimensions[Z_AXIS]/2;
    #endif

    // Move to safe distance to the size of the cube
    move_to(X_AXIS, m.center[X_AXIS] - m.dimensions[X_AXIS]/2 - m.nozzle_tip_radius - confidence);

    // Plunge below the side of the cube and measure
    move_to(Z_AXIS, m.cube_top - plunge);
    m.cube_left = measure(X_AXIS, 1, true, &m.backlash_xl, fast);
    m.center    [X_AXIS] = m.cube_left + m.dimensions[X_AXIS]/2 + m.nozzle_tip_radius;
  #endif

  #ifdef CALIBRATION_CUBE_BACK_SIDE_MEASUREMENT
    // Move to safe distance above cube/washer
    move_to(Z_AXIS, m.center[Z_AXIS] + m.dimensions[Z_AXIS]/2 + confidence);

    #ifdef CALIBRATION_CUBE_TOP_CENTER_MEASUREMENT
      // Move to center of cube in XY
      move_to(X_AXIS, m.center[X_AXIS], Y_AXIS, m.center[Y_AXIS]);
    #else
      // Probe top nearest back side
      move_to(X_AXIS, m.center[X_AXIS], Y_AXIS, m.center[Y_AXIS] + m.dimension[Y_AXIS]/2 - inset);
      m.cube_top = measure(Z_AXIS, -1, true, &m.backlash_zt, fast);
      m.center[Z_AXIS] = m.cube_top - m.dimensions[Z_AXIS]/2;
    #endif

    // Move to safe distance to the size of the cube
    move_to(Y_AXIS, m.center[Y_AXIS] + m.dimensions[Y_AXIS]/2 + m.nozzle_tip_radius + confidence);

    // Plunge below the side of the cube and measure
    move_to(Z_AXIS, m.cube_top - plunge);
    m.cube_back = measure(Y_AXIS, -1, true, &m.backlash_yb, fast);
    m.center    [Y_AXIS] = m.cube_back - m.dimensions[Y_AXIS]/2 - m.nozzle_tip_radius;
  #endif

  #if HAS_X_CENTER
    m.center[X_AXIS] = (m.cube_left+m.cube_right)/2;
  #endif
  #if HAS_Y_CENTER
    m.center[Y_AXIS] = (m.cube_front+m.cube_back)/2;
  #endif

  #if defined(CALIBRATION_CUBE_LEFT_SIDE_MEASUREMENT) && defined(CALIBRATION_CUBE_RIGHT_SIDE_MEASUREMENT)
    m.nozzle_x_width = (m.cube_right - m.cube_left)  - m.backlash_xl - m.backlash_xr - m.dimensions[X_AXIS];
  #endif
  #if defined(CALIBRATION_CUBE_FRONT_SIDE_MEASUREMENT) && defined(CALIBRATION_CUBE_BACK_SIDE_MEASUREMENT)
    m.nozzle_y_depth = (m.cube_back  - m.cube_front) - m.backlash_yf - m.backlash_yb - m.dimensions[Y_AXIS];
  #endif

  // Move to safe distance above cube/washer
  move_to(Z_AXIS, m.center[Z_AXIS] + m.dimensions[Z_AXIS]/2 + confidence);

  m.error[X_AXIS] = 0;
  m.error[Y_AXIS] = 0;
  m.error[Z_AXIS] = 0;

  #if HAS_X_CENTER
    m.error[X_AXIS] = m.true_center[X_AXIS] - m.center[X_AXIS];
  #endif
  #if HAS_Y_CENTER
    m.error[Y_AXIS] = m.true_center[Y_AXIS] - m.center[Y_AXIS];
  #endif
  m.error[Z_AXIS] = m.true_center[Z_AXIS] - m.center[Z_AXIS];
}

static bool read_probe_value() {
  #if ENABLED(Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN)
    return ((READ(Z_MIN_PIN))       != Z_MIN_ENDSTOP_INVERTING);
  #else
    return ((READ(Z_MIN_PROBE_PIN)) != Z_MIN_PROBE_ENDSTOP_INVERTING);
  #endif
}

// Moves along axis in the specified dir, until the probe value becomes stopping_state,
// then return the axis value.
float measuring_movement(const AxisEnum axis, const int dir, const bool stopping_state, float resolution, float feedrate, float limit) {
  float travel = 0;
  while (travel < limit) {
    set_destination_from_current();
    destination[axis] += float(dir) * resolution;
    do_blocking_move_to(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], MMM_TO_MMS(feedrate));
    planner.synchronize();

    if(read_probe_value() != stopping_state)
      travel += resolution;
    else
      break;
  }
  return destination[axis] - (float(dir) * resolution);
}

// Moves along axis in the specified dir, until the probe value becomes stopping_state.
// Returns the axis value at that point and reverses to starting point. When
// backlash_ptr != 0, then the axis backlash is recorded while reversing.
float measure(const AxisEnum axis, const int dir, const bool stopping_state, float *backlash_ptr, bool fast) {
  float resolution, feedrate, limit;

  if(fast) {
    resolution = 0.25;
    feedrate   = CALIBRATION_FAST_FEEDRATE;
    limit      = 50;
  } else {
    resolution = CALIBRATION_MEASUREMENT_RESOLUTION;
    feedrate   = CALIBRATION_SLOW_FEEDRATE;
    limit      = CALIBRATION_MEASUREMENT_LIMIT;
  }

  // Save position
  set_destination_from_current();
  float start_pos = destination[axis];
  const float measured_position = measuring_movement(axis, dir, stopping_state, resolution, feedrate, limit);
  // Measure backlash
  if(backlash_ptr && !fast) {
    const float release_pos = measuring_movement(axis, -1 * dir, !stopping_state, resolution, feedrate, limit);
    *backlash_ptr = ABS(release_pos - measured_position);
  }
  // Return to starting position
  destination[axis] = start_pos;
  do_blocking_move_to(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], MMM_TO_MMS(CALIBRATION_TRAVEL_FEEDRATE));
  return measured_position;
}

// Move to a particular location. Up to three individual axis
// and their destinations can be specified, in any order.
void move_to(
  const AxisEnum a1, const float p1,
  const AxisEnum a2, const float p2,
  const AxisEnum a3, const float p3
) {
  set_destination_from_current();

  // Note: The order of p1, p2, p3 may not correspond to X, Y, Z
  if(a1 != NO_AXIS) destination[a1] = p1;
  if(a2 != NO_AXIS) destination[a2] = p2;
  if(a3 != NO_AXIS) destination[a3] = p3;

  // Make sure coordinates are within bounds
  destination[X_AXIS] = MAX(MIN(destination[X_AXIS],X_MAX_POS),X_MIN_POS);
  destination[Y_AXIS] = MAX(MIN(destination[Y_AXIS],Y_MAX_POS),Y_MIN_POS);
  destination[Z_AXIS] = MAX(MIN(destination[Z_AXIS],Z_MAX_POS),Z_MIN_POS);

  // Move to commanded destination
  do_blocking_move_to(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], MMM_TO_MMS(CALIBRATION_TRAVEL_FEEDRATE));
}

#endif // CALIBRATION_GCODE
