/*
  midi_rnd.c
  A simple randomized MIDI sequencer.
  
  Copyright 2013, Brendan Jones

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "lv2.h"

#include "lv2/lv2plug.in/ns/ext/atom/forge.h"
#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#include "ladspa-util.h"

#define MIDIRND_URI "http://bjones.it/psi-plugins/midi_rnd"

#define max(a,b)                                \
  ({ __typeof__ (a) _a = (a);                   \
    __typeof__ (b) _b = (b);                    \
    _a > _b ? _a : _b; })

typedef enum {
  MIDIRND_CHROM      = 0,
  MIDIRND_MAJOR      = 1,
  MIDIRND_MINOR      = 2,
  MIDIRND_PENT       = 3,
  MIDIRND_PENTMN     = 4
} ScaleType;

typedef enum {
  MIDIRND_CHROM_MSK  = 4095,
  MIDIRND_MAJOR_MSK  = 2741,
  MIDIRND_MINOR_MSK  = 1453,
  MIDIRND_PENT_MSK   = 661,
  MIDIRND_PENTMN_MSK = 1193
} ScaleTypeMask;

typedef struct _MidiRnd {
  float* channel;
  float* lower;
  float* upper;
  float* range;
  float* r_lower;
  float* r_upper;
  float* scale_root;
  float* scale_type;
    
  const LV2_Atom_Sequence* midi_in;
  LV2_Atom_Sequence* midi_out;
  LV2_Atom_Forge forge;
  LV2_Atom_Forge_Frame frame;
    
  uint8_t history[128];
  unsigned program;
  unsigned velocity;
  // Features
  LV2_URID_Map* map;

  struct {
    LV2_URID midi_MidiEvent;
  } uris;
} MidiRnd;

static void connect_port(LV2_Handle instance, uint32_t port, void *data) {
  MidiRnd *plugin = (MidiRnd *)instance;

  switch (port) {
  case 0:
    plugin->midi_in = (const LV2_Atom_Sequence*)data;
    break;
  case 1:
    plugin->midi_out = (LV2_Atom_Sequence*)data;
    break;
  case 2:
    plugin->channel = (float*) data;
    break;
  case 3:
    plugin->lower = (float*) data;
    break;
  case 4:
    plugin->upper = (float*) data;
    break;
  case 5:
    plugin->range = (float*) data;
    break;
  case 6:
    plugin->r_lower = (float*) data;
    break;
  case 7:
    plugin->r_upper = (float*) data;
    break;
  case 8:
    plugin->scale_type = (float*) data;
    break;
  case 9:
    plugin->scale_root = (float*) data;
    break;
    break;
  }
}

static LV2_Handle instantiate(const LV2_Descriptor *descriptor,
                              double s_rate, const char *path,
                              const LV2_Feature *const *features) {
  MidiRnd *self = (MidiRnd *)calloc(1,sizeof(MidiRnd));

  /** Scan features array for the URID feature we need. */
  self->velocity = 63;
  self->program = 0;
    
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
  lv2_atom_forge_init(&self->forge, self->map);
  return (LV2_Handle)self;
}


static void activate(LV2_Handle instance) {
  MidiRnd *self = (MidiRnd *)instance;
  self->program = 0;
  self->velocity = 1;
  srand ( time(NULL) );
}

/**
 * add a midi message to the output port
 */
void forge_midimessage(MidiRnd* self,
                       uint32_t tme,
                       const uint8_t* const buffer,
                       uint32_t size) {
  LV2_Atom midiatom;
  midiatom.type = self->uris.midi_MidiEvent;
  midiatom.size = size;

  lv2_atom_forge_frame_time(&self->forge, tme);
  lv2_atom_forge_raw(&self->forge, &midiatom, sizeof(LV2_Atom));
  lv2_atom_forge_raw(&self->forge, buffer, size);
  lv2_atom_forge_pad(&self->forge, sizeof(LV2_Atom) + size);
}

/* return an equally likely random number beteen min and max */
uint16_t random_in_range (uint16_t min, uint16_t max) {
  int base_random = rand(); 
  if (RAND_MAX == base_random) return random_in_range(min, max);
  
  int range       = max - min,
    remainder   = RAND_MAX % range,
    bucket      = RAND_MAX / range;
  if (base_random < RAND_MAX - remainder) {
    return min + base_random/bucket;
  } else {
    return random_in_range (min, max);
  }
}

/* return a random note as described by the scale mask */
uint16_t random_in_mask (uint16_t mask) {
  int i, iter, num_notes = 0;
  unsigned key[11];
  for (i=1, iter=1;(i<4096); i<<=1, iter++)
    if (mask & i) {
      key[num_notes] = (iter-1);
      num_notes++;
    }
  int rnd_note = random_in_range( 0, num_notes-1);
  return key[rnd_note];
}

/**
   Intercept MIDI and send to the output buffer 
*/
static void write_output(MidiRnd* self, uint32_t frames,
                         const uint8_t* const buffer,
                         uint32_t size) {
  int channel = f_round(*(self->channel));
  const float range = *(self->range);
  const float scale_type = *(self->scale_type);
  const float scale_root = *(self->scale_root);
  int upper = f_round(*(self->upper));
  int lower = f_round(*(self->lower));
  int r_upper = f_round(*(self->r_upper));
  int r_lower = f_round(*(self->r_lower));
  uint16_t scale_mask = 0;
    
  const int chn = buffer[0] & 0x0f;
  int mst = buffer[0] & 0xf0;
    
    
  if (size != 3 || ( (lv2_midi_message_type(buffer) != LV2_MIDI_MSG_NOTE_ON) 
                     && (lv2_midi_message_type(buffer) != LV2_MIDI_MSG_NOTE_OFF)) ) {
    forge_midimessage(self, frames, buffer, size);
    return;
  }
    
  const uint8_t vel  = buffer[2] & 0x7f;

  if (mst == LV2_MIDI_MSG_NOTE_ON && vel ==0 ) {
    mst = LV2_MIDI_MSG_NOTE_OFF;
  }

  uint8_t output[3], out_note;
    
  int rnd_key;
  output[0] = buffer[0];
  output[1] = buffer[1];
  output[2] = buffer[2];
    
  if ( (f_round(channel) == 0 || f_round(channel) == (int)(chn+1))
       && !(buffer[1] < f_round(lower)) 
       && !(buffer[1] > f_round(upper)) ) {
    if (range != 0)
      rnd_key = random_in_range(0,f_round(range*12)-1) - (f_round(range*6));
    else
      rnd_key = random_in_range(r_lower,r_upper);
    
    // chromatic
    if (f_round(scale_type) == 0) {
      out_note = buffer[1] + rnd_key;
    } else {
      switch (f_round(scale_type)) {
      case MIDIRND_MAJOR:
        scale_mask = MIDIRND_MAJOR_MSK;
        break;
      case MIDIRND_MINOR:
        scale_mask = MIDIRND_MINOR_MSK;
        break;
      case MIDIRND_PENT:
        scale_mask = MIDIRND_PENT_MSK;
        break;
      case MIDIRND_PENTMN:
        scale_mask = MIDIRND_PENTMN_MSK;
        break;
      default: break;
      }
           
      if (range!=0) out_note = buffer[1] + rnd_key;
      else out_note = rnd_key;
                 
      uint16_t note = 1 << (out_note % 12);
      while (!(note & scale_mask)||((out_note+scale_root)>r_upper)) {
        // out of scale or range, generating ia new one 
        if (range != 0)
          out_note = random_in_range(0,f_round(range*12)-1) + buffer[1]
            - (f_round(range*6));
        else {
          out_note = random_in_range(r_lower,r_upper);
        }
        note = 1 << (out_note % 12);
      }
      out_note=out_note + scale_root;
    }    

    switch (lv2_midi_message_type(buffer)) {
    case LV2_MIDI_MSG_NOTE_ON:
      output[1] = out_note;
      self->history[buffer[1]] = out_note;
      break;
    case LV2_MIDI_MSG_NOTE_OFF:
      // attempt to map note off to previously generated note-on
      output[1] = self->history[buffer[1]];
      self->history[buffer[1]] = 0;
      break;
    case LV2_MIDI_MSG_INVALID:
    case LV2_MIDI_MSG_NOTE_PRESSURE:
    case LV2_MIDI_MSG_CONTROLLER:
    case LV2_MIDI_MSG_PGM_CHANGE:
    case LV2_MIDI_MSG_CHANNEL_PRESSURE:
    case LV2_MIDI_MSG_BENDER:
    case LV2_MIDI_MSG_SYSTEM_EXCLUSIVE:
    case LV2_MIDI_MSG_MTC_QUARTER:
    case LV2_MIDI_MSG_SONG_POS:
    case LV2_MIDI_MSG_SONG_SELECT:
    case LV2_MIDI_MSG_TUNE_REQUEST:
    case LV2_MIDI_MSG_CLOCK:
    case LV2_MIDI_MSG_START:
    case LV2_MIDI_MSG_CONTINUE:
    case LV2_MIDI_MSG_STOP:
    case LV2_MIDI_MSG_ACTIVE_SENSE:
    case LV2_MIDI_MSG_RESET:
      break;
    }
  }
  forge_midimessage(self, frames, output, size);
}

static void run(LV2_Handle instance, uint32_t sample_count) {
  MidiRnd *self = (MidiRnd *)instance;
    
  /* prepare midi_out port */
  const uint32_t capacity = self->midi_out->atom.size;
  lv2_atom_forge_set_buffer(&self->forge, (uint8_t*)self->midi_out, capacity);
  lv2_atom_forge_sequence_head(&self->forge, &self->frame, 0);
    
  LV2_ATOM_SEQUENCE_FOREACH(self->midi_in, ev) {
    if (ev->body.type == self->uris.midi_MidiEvent) {
      const uint8_t* const msg = (const uint8_t*)(ev + 1);
      switch (lv2_midi_message_type(msg)) {
      case LV2_MIDI_MSG_NOTE_ON:
        write_output(self, ev->time.frames, msg, ev->body.size);
        break;
      case LV2_MIDI_MSG_NOTE_OFF:
        write_output(self, ev->time.frames, msg, ev->body.size);
        break;
      case LV2_MIDI_MSG_PGM_CHANGE:
        if (msg[1] == 0 || msg[1] == 1) {
          self->program = msg[1];
        }
        break;
                
      default: break;
      }
    }
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
  MIDIRND_URI,
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
