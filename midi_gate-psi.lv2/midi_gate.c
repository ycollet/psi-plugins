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

#include "ladspa-util.h"
#include <stdio.h>

#include "midi_gate.h"
#include <math.h>
#include <stdlib.h>

static LV2_Descriptor *stereoMidiGateDescriptor = NULL;

static void connect_port(LV2_Handle instance, uint32_t port, void *data) {
  StereoMidiGate *plugin = (StereoMidiGate *)instance;

  switch (port) {
  case MIDIGATE_MIDI_IN:
    plugin->midi_in = (const LV2_Atom_Sequence*)data;
    break;
  case MIDIGATE_LEFT_IN:
    plugin->left_in = (float*) data;
    break;
  case MIDIGATE_RIGHT_IN:
    plugin->right_in = (float*) data;
    break;
  case MIDIGATE_LEFT_OUT:
    plugin->left_out = (float*) data;
    break;
  case MIDIGATE_RIGHT_OUT:
    plugin->right_out = (float*) data;
    break;
  case MIDIGATE_ATTACK:
    plugin->attack = (float*) data;
    break;
  case MIDIGATE_HOLD:
    plugin->hold = (float*) data;
    break;
  case MIDIGATE_DECAY:
    plugin->decay = (float*) data;
    break;
  case MIDIGATE_RANGE:
    plugin->range = (float*) data;
    break;
  case MIDIGATE_PROGRAM:
    plugin->program = (float*) data;
    break;
  case MIDIGATE_SELECT: 
    plugin->select = (float*) data;
    break;
  case MIDIGATE_LEVEL:
    plugin->level = (float*) data;
    break;
  case MIDIGATE_GATE_STATE:
    plugin->gate_state = (float*) data;
    break;
    break;
  }
}

static LV2_Handle instantiate(const LV2_Descriptor *descriptor, double s_rate, const char *path, const LV2_Feature *const *features) {
  StereoMidiGate *self = (StereoMidiGate *)calloc(1,sizeof(StereoMidiGate));

  self->fs = s_rate;
  self->peak = 0.0f;
  self->gate = 0.0f;
  self->state = CLOSED;
  self->hold_count = 0;
  self->n_active_notes = 0;
  self->program_prev = 0; 
    
  /** Scan features array for the URID feature we need. */
  LV2_URID_Map* map = NULL;
  for (int i = 0; features[i]; ++i) {
    if (!strcmp(features[i]->URI, LV2_URID__map)) {
      map = (LV2_URID_Map*)features[i]->data;
      break;
    }
  }
    
  if (!map) {
    /**
       No URID feature given.  This is a host bug since we require this
       feature, but should be handled gracefully anyway.
    */
    return NULL;
  }
  self->map = map;
  self->uris.midi_MidiEvent = map->map(map->handle, LV2_MIDI__MidiEvent);
  return (LV2_Handle)self;
}


static void activate(LV2_Handle instance) {
  StereoMidiGate *self = (StereoMidiGate *)instance;
  self->peak = -70.0f;
  self->gate = 0.0f;
  self->state = CLOSED;
  self->hold_count = 0;
  self->n_active_notes = 0;
  self->program_prev = 0;
}

static float iec_scale(float db) {
  float def = 0.0f; /* Meter deflection %age */
 
  if (db < -70.0f) {
    def = 0.0f;
  } else if (db < -60.0f) {
    def = (db + 70.0f) * 0.25f;
  } else if (db < -50.0f) {
    def = (db + 60.0f) * 0.5f + 5.0f;
  } else if (db < -40.0f) {
    def = (db + 50.0f) * 0.75f + 7.5;
  } else if (db < -30.0f) {
    def = (db + 40.0f) * 1.5f + 15.0f;
  } else if (db < -20.0f) {
    def = (db + 30.0f) * 2.0f + 30.0f;
  } else if (db < 0.0f) {
    def = (db + 20.0f) * 2.5f + 50.0f;
  } else {
    def = 100.0f;
  }
 
  return (def * 2.0f);
}
/**
   A function to write a chunk of output, to be called from run(). The 
*/
static void write_output(StereoMidiGate* self, uint32_t offset, uint32_t len) {
  const float attack = *(self->attack);
  const float hold = *(self->hold);
  const float decay = *(self->decay);
  const float range = *(self->range);
  const float select = *(self->select);
  float const program = *(self->program);
   
  float gate_state;
  const float * const left_in = self->left_in;
  const float * const right_in = self->right_in;
  float * const left_out = self->left_out;
  float * const right_out = self->right_out;
  unsigned long pos = 0;
    
  float fs = self->fs;
  float peak = self->peak;
  float gate = self->gate;
  int state = self->state;
  int hold_count = self->hold_count;
  float cut = DB_CO(range);
  float a_rate = 1000.0f / (attack * fs);
  float d_rate = 1000.0f / (decay * fs);
  float post_filter_l, apost_filter_l;
  float post_filter_r, apost_filter_r;
  float apost_filter, post_filter;
  float peak_m = 0;
    
  int op = f_round(select);

  const bool active = (program == 0)
    ? (self->n_active_notes > 0)
    : (self->n_active_notes == 0);

  for (pos = offset ; pos < (len + offset); pos++) {
       
    if (state == CLOSED) {
      if (active) {
        state = OPENING;
      }
    } else if (state == OPENING) {
      gate += a_rate;
      if (gate >= 1.0f) {
        gate = 1.0f;
        state = OPEN;
      }
    } else if (state == OPEN) {
      if (!active) {
        state = HOLD;
        hold_count = f_round(hold * fs * 0.001f);
      }
    } else if (state == HOLD) {
      if (active) {
        state = OPEN;
      } else if (hold_count <= 0) {
        state = CLOSING;
      } else {
        hold_count--;
      }
    } else if (state == CLOSING) {
      gate -= d_rate;
      if (active) {
        state = OPENING;
      } else if (gate <= 0.0f) {
        gate = 0.0f;
        state = CLOSED;
      }
    }

    if (op == 0) {
      buffer_write(left_out[pos]
                   ,left_in[pos] * (cut * (1.0f - gate) + gate));
      buffer_write(right_out[pos]
                   ,right_in[pos] * (cut * (1.0f - gate) + gate));
    } else {
      buffer_write(left_out[pos], left_in[pos]);
      buffer_write(right_out[pos], right_in[pos]);
    }
    peak_m = max(fabs(left_out[pos]),fabs(right_out[pos]));
        
    if (peak < peak_m) peak = peak_m;
  }
    
  self->peak_frames++;
  if (self->peak_frames > PEAK_FRAMES ) {
    *(self->level) = peak > 0.0f ? 20.0f * log10f(peak) : -70.0f;
    self->peak_frames = 0;
    self->peak = -70.0f;
  } else {
    self->peak = peak;
  }
    
  switch (state) {
  case OPEN:
    *(self->gate_state) = 1.0;
    break;
  case HOLD:
    *(self->gate_state) = 0.5;
    break;
  default:
    *(self->gate_state) = 0.0;
  }
  *(self->program) = program;
  self->gate = gate;
  self->state = state;
  self->hold_count = hold_count;

}

static void run(LV2_Handle instance, uint32_t sample_count) {
  StereoMidiGate *self = (StereoMidiGate *)instance;
  uint32_t  offset = 0;
  const float select = *(self->select);
  const float * const left_in = self->left_in;
  const float * const right_in = self->right_in;
  float * const left_out = self->left_out;
  float * const right_out = self->right_out;
    
  unsigned long pos;
  int op = f_round(select);
    
  if (op == 0) {
    if (self->program_prev != *(self->program)) {
      // reset on direction change
      activate(self);
      self->program_prev = *(self->program);
    }
    LV2_ATOM_SEQUENCE_FOREACH(self->midi_in, ev) {
      if (ev->body.type == self->uris.midi_MidiEvent) {
        const uint8_t* const msg = (const uint8_t*)(ev + 1);
        switch (lv2_midi_message_type(msg)) {
        case LV2_MIDI_MSG_NOTE_ON:
          ++self->n_active_notes;
          break;
        case LV2_MIDI_MSG_NOTE_OFF:
          --self->n_active_notes;
        case LV2_MIDI_MSG_PGM_CHANGE:
          if (msg[1] == 0 || msg[1] == 1) {
            // reset and change direction
            activate(self);
            self->program_prev = *(self->program);
            *(self->program) = msg[1];
          }
          break;
                    
        default: break;
        }

        write_output(self, offset, ev->time.frames - offset);
        offset = ev->time.frames;
      }
    }
    write_output(self, offset, sample_count - offset);
  } else {
    write_output(self, 0, sample_count);
  }
}

static void deactivate(LV2_Handle instance) {
}

static void cleanup(LV2_Handle instance) {
  free(instance);
}

static const void* extension_data(const char* uri) {
  return NULL;
}

static const LV2_Descriptor descriptor = {
  MIDIGATE_URI,
  instantiate,
  connect_port,
  activate,
  run,
  deactivate,
  cleanup,
  extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index) {
  switch (index) {
  case 0:
    return &descriptor;
  default:
    return NULL;
  }
}
