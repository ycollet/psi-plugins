/**
 * @file
 * @author  Brendan Jones <brendan.jones.it@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 * 
 * Copyright 2014, Brendan Jones
 *
 * This program is free software: you can redistribute
 * it and/or modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.   See the GNU General Public License (version 2 or
 * later) for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * the program; if not, write to the Free Software Foundation, Inc., 51 Franklin
 * Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * @section DESCRIPTION
 */


#define MIDIGATE_URI "http://bjones.it/psi-plugins/midi_gate"
#define MIDIGATE_UI_URI "http://bjones.it/psi-plugins/midi_gate/ui"

#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define ENV_TR 0.0001f

#define PEAK_FRAMES 50

#define CLOSED  1
#define OPENING 2
#define OPEN    3
#define HOLD    4
#define CLOSING 5

#define max(a,b)                                \
  ({ __typeof__ (a) _a = (a);                   \
    __typeof__ (b) _b = (b);                    \
    _a > _b ? _a : _b; })
     
typedef enum {
  MIDIGATE_MIDI_IN    = 0,
  MIDIGATE_LEFT_IN    = 1,
  MIDIGATE_RIGHT_IN   = 2,
  MIDIGATE_LEFT_OUT   = 3,
  MIDIGATE_RIGHT_OUT  = 4,
  MIDIGATE_ATTACK     = 5,
  MIDIGATE_HOLD       = 6,
  MIDIGATE_DECAY      = 7,
  MIDIGATE_RANGE      = 8,
  MIDIGATE_PROGRAM    = 9,
  MIDIGATE_SELECT     = 10,
  MIDIGATE_LEVEL      = 11,
  MIDIGATE_GATE_STATE = 12
    
} PortIndex;

typedef struct _StereoMidiGate {
  float *attack;
  float *hold;
  float *decay;
  float *range;
  float *select;
  float *level;
  float *gate_state;
  float *left_in;
  float *right_in;
  float *left_out;
  float *right_out;
  float *program;

  const LV2_Atom_Sequence* midi_in;
  float fs;
  float peak;
  float gate;
  int state;
  int hold_count;
  int peak_frames;
  unsigned n_active_notes;
    
  unsigned program_prev;

  // Features
  LV2_URID_Map* map;

  struct {
    LV2_URID midi_MidiEvent;
  } uris;
} StereoMidiGate;
