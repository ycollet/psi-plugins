/*
  sidechain_gate.h
  
  Copyright 2014, Brendan Jones

  This file is part of psi-plugins/sidechain_gate, which is free software: you can redistribute
  it and/or modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  yoshimi is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.   See the GNU General Public License (version 2 or
  later) for more details.

  You should have received a copy of the GNU General Public License along with
  yoshimi; if not, write to the Free Software Foundation, Inc., 51 Franklin
  Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "ladspa-util.h"
#include "biquad.h"
#include <stdio.h>

#define ENV_TR 0.0001f

#define CLOSED  1
#define OPENING 2
#define OPEN    3
#define HOLD    4
#define CLOSING 5

#define PEAK_FRAMES 20
#define MAX_DB 6.0f
#define MIN_DB -70.0f
#define CLAMP_MAX_DB(v) (v > MAX_DB ? MAX_DB : v)
#define CLAMP_MIN_DB(v) (v < MIN_DB ? MIN_DB : v)
#define CLAMP_CO_DB(v) CLAMP_MIN_DB(CLAMP_MAX_DB((v > 0.0f ? 20.0f * log10f(v) : -70.0f)))

#define SCGATE_URI "http://bjones.it/psi-plugins/sidechain_gate"
#define SCGATE_UI_URI "http://bjones.it/psi-plugins/sidechain_gate/ui"

typedef enum {
  SCGATE_LEFT_IN      = 0,
  SCGATE_RIGHT_IN     = 1,
  SCGATE_SIDECHAIN_IN = 2,
  SCGATE_LEFT_OUT     = 3,
  SCGATE_RIGHT_OUT    = 4,
  SCGATE_LF           = 5,
  SCGATE_HF           = 6,
  SCGATE_THRESHOLD    = 7,
  SCGATE_HYSTER       = 8,
  SCGATE_ATTACK       = 9,
  SCGATE_HOLD         = 10,
  SCGATE_DECAY        = 11,
  SCGATE_RANGE        = 12,
  SCGATE_SELECT       = 13,
  SCGATE_LEVEL_LEFT   = 14,
  SCGATE_LEVEL_RIGHT  = 15,
  SCGATE_GATE_STATE   = 16,
  SCGATE_CHAIN_BAL    = 17,
  SCGATE_OUT_LEVEL_LEFT   = 18,
  SCGATE_OUT_LEVEL_RIGHT  = 19
} PortIndex;

#include "lv2.h"

typedef struct _SidechainGate {
  float *left_in;
  float *right_in;
  float *sidechain_in;
  float *left_out;
  float *right_out;
  float *lf_fc;
  float *hf_fc;
  float *threshold;
  float *hysteresis;
  float *attack;
  float *hold;
  float *decay;
  float *range;
  float *select;
  float *level_left;
  float *level_right;
  float *out_level_left;
  float *out_level_right;
  
  float *gate_state;
  float *chain_bal;
  float fs;
  float env;
  float gate;
  int state;
  int hold_count;
  biquad * lf;
  biquad * hf;
  float peak_left;
  float peak_right;
  float peak_out_left;
  float peak_out_right;
  int peak_frames;
  float threshold_prev;
  
} SidechainGate;
