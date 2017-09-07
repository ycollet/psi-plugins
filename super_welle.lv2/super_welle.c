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

#include "super_welle.h"
#include "common/ladspa-util.h"
#include "common/voice_params.h"
#include "common/voice.h"
#include "synth.h"

#define LSF_BW 0.9
#define LSF_FB 0.9f

static void connect_port(LV2_Handle instance, uint32_t port, void *data) {
  SuperWelle *plugin = (SuperWelle *)instance;

  switch (port) {
  case V_MIDI_IN:
    plugin->midi_in     = (const LV2_Atom_Sequence*)data;
    break;
  case V_LEFT_IN:
    plugin->left_in     = (float*) data;
    break;
  case V_RIGHT_IN:
    plugin->right_in    = (float*) data;
    break;
  case V_LEFT_OUT:
    plugin->left_out    = (float*) data;
    break;
  case V_RIGHT_OUT:
    plugin->right_out   = (float*) data;
    break;
  case V_NOTIFY:
    plugin->notify      = (LV2_Atom_Sequence*) data;
    break;
  case V_CONTROL:
    plugin->control     = (const LV2_Atom_Sequence*) data;
    break;
  default:
    break;
  }
  // input ports
  plugin->params[port] = (float*) data;
}

static LV2_Handle instantiate(const LV2_Descriptor *descriptor,
                              double s_rate, const char *path,
                              const LV2_Feature *const *features) {
  SuperWelle *self = (SuperWelle *)calloc(1,sizeof(SuperWelle));
  self->fs = s_rate;
  self->synth = calloc(1, sizeof(SWSynth));
    
  srand(time(NULL));
    
  // Get host features
  for (int i = 0; features[i]; ++i) {
    if (!strcmp(features[i]->URI, LV2_URID__map)) {
      self->map = (LV2_URID_Map*)features[i]->data;
    } else if (!strcmp(features[i]->URI, LV2_LOG__log)) {
      self->log = (LV2_Log_Log*)features[i]->data;
    }
  }

  if (!self->map) {
    /**
       No URID feature given.  This is a host bug since we require this
       feature, but should be handled gracefully anyway.
    */
    return NULL;
  }
  map_uris(self->map, &self->uris);
  lv2_atom_forge_init(&self->forge, self->map);
  lv2_log_logger_init(&self->logger, self->map, self->log);
  return (LV2_Handle)self;
}

static void activate(LV2_Handle instance) {
  SuperWelle *self = (SuperWelle *)instance;
  synth_init(self->synth, (const float * const*) self->params, self->fs);
}

/* copy ports back to the UI if they have been changed via midi_in */
static void update_ui(SuperWelle* self, const uint32_t port, const float value) {
  LV2_Atom_Forge_Frame frame;
  lv2_atom_forge_frame_time(&self->forge, 0);
  lv2_atom_forge_object(&self->forge, &frame, 0, self->uris.UIData);

  /// Add UI state as properties
  lv2_atom_forge_key(&self->forge, self->uris.ui_portNumber);
  lv2_atom_forge_int(&self->forge, port);
  lv2_atom_forge_key(&self->forge, self->uris.ui_portValue);
  lv2_atom_forge_float(&self->forge, value);
  lv2_atom_forge_pad(&self->forge, sizeof(LV2_Atom_Int)+sizeof(LV2_Atom_Float)+sizeof(LV2_Atom));
  // Close off object
  lv2_atom_forge_pop(&self->forge, &frame);
  
}

/**
   A function to write a chunk of output, to be called from run(). The
*/
static void write_output(SuperWelle* self, uint32_t offset, uint32_t len) {

  const float amp = *(self->params[V_VOLUME]);
  float tempo = *(self->params[V_TMP]);
  float * const left_out = self->left_out;
  float * const right_out = self->right_out;
  float cut = amp;
    
  sample_t left, right;
  sample_t post_filter_l;
  sample_t post_filter_r;
    
  if (tempo != self->synth->gbl.tempo) {
    self->synth->gbl.tempo = tempo;
    update_ui(self, V_TMP,lrint(tempo));
  }
    
  synth_frame(self->synth, len);
    
  for (int pos = offset ; pos < (len + offset); pos++) {
    left     = 0.0f;
    right    = 0.0f;
    
    synth_process(self->synth, pos, &left, &right);
    
    post_filter_l = left;
    post_filter_r = right;
    
    buffer_write(left_out[pos], (float) post_filter_l);
    buffer_write(right_out[pos], (float) post_filter_r);
  }
}

static void run(LV2_Handle instance, uint32_t sample_count) {
  SuperWelle *self = (SuperWelle *)instance;
  const SW_LV2_URIS* uris = &self->uris;
  SWSynth    *s    = self->synth;   
  uint32_t  offset = 0;
  uint32_t  port = 0;
  uint8_t   chn, note, vel;
  float     val = 0;
    
  memcpy( self->synth->params, self->params, sizeof(self->params));
    
  const size_t   size  = 1024;
    
  const uint32_t space = self->notify->atom.size;
  uint32_t rframes = 0;
    
  if (space < size + 128) {
    /* Insufficient space, report error and do nothing.  Note that a
       real-time production plugin mustn't call log functions in run(), but
       this can be useful for debugging and example purposes.
    */
    lv2_log_error(&self->logger, "Buffer size is insufficient\n");
    return;
  }
  // Prepare forge buffer and initialize atom-sequence
  lv2_atom_forge_set_buffer(&self->forge, (uint8_t*)self->notify, space);
  lv2_atom_forge_sequence_head(&self->forge, &self->frame, 0);
    
  LV2_ATOM_SEQUENCE_FOREACH(self->control, ev) {
        
    if ( ev->body.type == self->uris.atom_Blank 
         || ev->body.type == self->uris.atom_Object) {
      const LV2_Atom_Object* obj = (LV2_Atom_Object*)&ev->body;
            
      if (obj->body.otype == uris->time_Position) {
                
        LV2_Atom *beat = NULL, *bpm = NULL, *speed = NULL;
        LV2_Atom *fps = NULL, *frame = NULL;
        lv2_atom_object_get(obj,
                            uris->time_barBeat, &beat,
                            uris->time_beatsPerMinute, &bpm,
                            uris->time_speed, &speed,
                            uris->time_frame, &frame,
                            uris->time_fps, &fps,
                            NULL);
        if (bpm && bpm->type == uris->atom_Float) {
                    
          float bpmValue = ((LV2_Atom_Float*)bpm)->body;
          s->gbl.tempo = bpmValue;   
          *(self->params[V_TMP]) = bpmValue;
          update_ui(self, V_TMP,lrint(s->gbl.tempo));
        }
      }
    }   
  }
    
  LV2_ATOM_SEQUENCE_FOREACH(self->midi_in, ev) {
    if ( ev->body.type == self->uris.atom_Blank 
         || ev->body.type == self->uris.atom_Object) {
      const LV2_Atom_Object* obj = (LV2_Atom_Object*)&ev->body;
      if (obj->body.otype == uris->time_Position) {
                
        LV2_Atom *beat = NULL, *bpm = NULL, *speed = NULL;
        LV2_Atom *fps = NULL, *frame = NULL;
        lv2_atom_object_get(obj,
                            uris->time_barBeat, &beat,
                            uris->time_beatsPerMinute, &bpm,
                            uris->time_speed, &speed,
                            uris->time_frame, &frame,
                            uris->time_fps, &fps,
                            NULL);
        if (bpm && bpm->type == uris->atom_Float) {
          float bpmValue = ((LV2_Atom_Float*)bpm)->body;
          s->gbl.tempo = bpmValue;        
          *(self->params[V_TMP]) = bpmValue;
          update_ui(self, V_TMP,lrint(s->gbl.tempo));
        }
      }
      write_output(self, offset, ev->time.frames - offset);
      offset = ev->time.frames;
      rframes += ev->time.frames;
    }   

    if (ev->body.type == uris->midi_MidiEvent) {
      const uint8_t* const msg = (const uint8_t*)(ev + 1);
      s->frames += ev->time.frames;
      switch (lv2_midi_message_type(msg)) {
      case LV2_MIDI_MSG_CLOCK:
        s->clock++;
        if (s->clock == 96) {
          s->fpb = s->frames / 4.0;
          s->frames = 0;
          s->gbl.tempo = (self->fs / s->fpb) * 60;
          s->clock = 0;
          update_ui(self, V_TMP,lrint(s->gbl.tempo));
        }
        break;
                
      case LV2_MIDI_MSG_NOTE_ON:
        chn  = msg[0] & 0xf;
        note = msg[1] & 0x7f;
        vel  = msg[2] & 0x7f;
        if ( (s->midi_channel==0)
             ||(chn == (uint8_t)(s->midi_channel - 1))) {
          if (vel!=0) {
            synth_note_on(s,note,vel);
          } else {
            // note off vel 0
            synth_note_off(s,note,vel);
          }
        }
        break;
                
      case LV2_MIDI_MSG_NOTE_OFF:
        chn  = msg[0] & 0xf;
        note = msg[1] & 0x7f;
        vel  = msg[2] & 0x7f;
        if ( (s->midi_channel==0)
             ||(chn == (uint8_t)(s->midi_channel - 1))) {
          // note off vel 0
          synth_note_off(s,note,vel);
        }
        synth_note_off(s,note,vel);
        break;
                
      case LV2_MIDI_MSG_BENDER:
        s->gbl.tuning = (msg[2]-64)/32.0;
        s->gbl.bender = ((sample_t)msg[2])/127.0;
        break;
                
      case LV2_MIDI_MSG_NOTE_PRESSURE:

        break;
                
      case LV2_MIDI_MSG_CONTROLLER:
        if (s->midi_learn_on == 0) {
          uint8_t chn = (msg[0] & 0xf) + 1;
          if ( (s->midi_cc[chn][msg[1]] != 0) || 
               (s->midi_cc[0][msg[1]] != 0) ) {
            port = s->midi_cc[chn][msg[1]];
            val = ((sample_t) msg[2]) 
              * ((parameters[port].max 
                  - parameters[port].min)/127.0)
              + parameters[port].min;
                        
            if (parameters[port].round) val = f_round(val);
            *(s->params[port]) = val;
            update_ui(self, port,(float) val);
            update_ui(self, V_MID_ON, 0.0f);
          } else 
            switch(msg[1]) {
              // we'll handle this via midi mapping
              /*case LV2_MIDI_CTL_SC2_TIMBRE:
               *(s->params[V_FL1_CUT]) 
               = ((sample_t)msg[2])/127.0 + 0.0001;
               s->midi_timbre = (float)msg[2]/127.0;
               break;
                                    
               case LV2_MIDI_CTL_SC5_BRIGHTNESS:
               *(s->params[V_FL1_RES]) = 
               ((float)msg[2]/127.0);
               s->midi_brightness      = 
               (float)msg[2]/127.0;
               break;*/
            case LV2_MIDI_CTL_ALL_SOUNDS_OFF:
            case LV2_MIDI_CTL_ALL_NOTES_OFF:
              synth_all_notes_off(s);
              break;
            case LV2_MIDI_CTL_MSB_MODWHEEL:
              *(s->params[V_LF1_FRQ]) 
                = 20.0 * ((sample_t)msg[2])/127.0
                + 0.0001;
              s->gbl.modwheel = ((sample_t)msg[2])/127.0;
            default:
              break;
            }
        } else {
          uint8_t chn = (msg[0] & 0xf) + 1;
          s->midi_cc[chn][(int)msg[1]] = s->last_port;
          int done = 0;
          for (int i = V_MID_L01; (i< (V_MID_L16+1))&(!done); i++ ) {
            if (*(s->params[i])==0) {
              // free port
              // channel / ontroller / port
              int32_t learn = (((uint32_t)chn & 0xf  )  << 16) ;
              learn = learn | (((uint32_t)(msg[1])) << 8 );
              learn = learn | (((uint32_t)(s->last_port)) & 0x7f  );
              *(s->params[i]) = (float) learn;
              update_ui(self, i, learn);
              done = 1;
            }
                            
          }
          s->midi_learn_on = 0;
          *(s->params[V_MID_ON]) = 0;
          update_ui(self, V_MID_ON, 0.0f);
        }
        break;
      default:
        break;
      }
      write_output(self, offset, ev->time.frames - offset);
      offset = ev->time.frames;
      rframes += ev->time.frames;
            
    }
        
  }
  s->frames += sample_count - rframes;
  write_output(self, offset, sample_count - offset);
  lv2_atom_forge_pop(&self->forge, &self->frame);
    
}

static void deactivate(LV2_Handle instance) {
}

static void cleanup(LV2_Handle instance) {
  SuperWelle *self = (SuperWelle *)instance;
  synth_free(self->synth);
  free(instance);
}

static const void* extension_data(const char* uri) {
  return NULL;
}

static const LV2_Descriptor descriptor = {
  SW_URI,
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
