/**
 * @file
 * @author  Brendan Jones <brendan.jones.it@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 * 
 * Copyright 2000 Steve Harris, Nedko Arnaudov
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
 * 
 * A stereo gate with sidechain based on http://plugin.org.uk/swh-plugins/gate
 * 
 */

#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#include "sidechain_gate.h"

#include <math.h>
#include <stdlib.h>

static void connect_port(LV2_Handle instance, uint32_t port, void *data) {
  SidechainGate *plugin = (SidechainGate *)instance;

  switch (port) {
  case SCGATE_SIDECHAIN_IN:
    plugin->sidechain_in = (float*) data;
    break;
  case SCGATE_LEFT_IN:
    plugin->left_in = (float*) data;
    break;
  case SCGATE_RIGHT_IN:
    plugin->right_in = (float*) data;
    break;
  case SCGATE_LEFT_OUT:
    plugin->left_out = (float*) data;
    break;
  case SCGATE_RIGHT_OUT:
    plugin->right_out = (float*) data;
    break;
  case SCGATE_LF:
    plugin->lf_fc = (float*) data;
    break;
  case SCGATE_HF:
    plugin->hf_fc = (float*) data;
    break;
  case SCGATE_THRESHOLD:
    plugin->threshold = (float*) data;
    break;
  case SCGATE_HYSTER:
    plugin->hysteresis = (float*) data;
    break;
  case SCGATE_ATTACK:
    plugin->attack = (float*) data;
    break;
  case SCGATE_HOLD:
    plugin->hold = (float*) data;
    break;
  case SCGATE_DECAY:
    plugin->decay = (float*) data;
    break;
  case SCGATE_RANGE:
    plugin->range = (float*) data;
    break;
  case SCGATE_SELECT:
    plugin->select = (float*) data;
    break;
  case SCGATE_LEVEL_LEFT:
    plugin->level_left = (float*) data;
    break;
  case SCGATE_LEVEL_RIGHT:
    plugin->level_right = (float*) data;
    break;
  case SCGATE_OUT_LEVEL_LEFT:
    plugin->out_level_left = (float*) data;
    break;
  case SCGATE_OUT_LEVEL_RIGHT:
    plugin->out_level_right = (float*) data;
    break;
  case SCGATE_CHAIN_BAL:
    plugin->chain_bal = (float*) data;
    break;        
  case SCGATE_GATE_STATE:
    plugin->gate_state = (float*) data;
    break;
    break;
  }
}

static LV2_Handle instantiate(const LV2_Descriptor *descriptor,
                              double s_rate, const char *path,
                              const LV2_Feature *const *features) {
  SidechainGate *self = (SidechainGate *)malloc(sizeof(SidechainGate));
  biquad * lf = self->lf;
  biquad * hf = self->hf;
  lf = malloc(sizeof(biquad));
  hf = malloc(sizeof(biquad));
  biquad_init(lf);
  biquad_init(hf);
  self->lf = lf;
  self->hf = hf;
  self->fs = s_rate;
  self->env = 0.0f;
  self->gate = 0.0f;
  self->state = CLOSED;
  self->hold_count = 0;
    
  // monitor input / output peaks
  self->peak_frames = 0.0f;
  self->peak_left = 0.0f;
  self->peak_right = 0.0f;
  self->peak_out_left = 0.0f;
  self->peak_out_right = -0.0f;
  /**(self->level_left) = -70.0f;
   *(self->level_right) = -70.0f;
   *(self->out_level_left) = -70.0f;
   *(self->out_level_right) = -70.0f;*/
  return (LV2_Handle)self;
}

static void activate(LV2_Handle instance) {
  SidechainGate *self = (SidechainGate *)instance;
  biquad * lf = self->lf;
  biquad * hf = self->hf;
      
  biquad_init(lf);
  biquad_init(hf);
  self->env = 0.0f;
  self->gate = 0.0f;
  self->state = CLOSED;
  // monitor input / output peaks
    
  self->peak_frames = 0.0f;
  self->peak_left = 0.0f;
  self->peak_right = 0.0f;
  self->peak_out_left = 0.0f;
  self->peak_out_right = -0.0f;
  *(self->level_left) = -70.0f;
  *(self->level_right) = -70.0f;
  *(self->out_level_left) = -70.0f;
  *(self->out_level_right) = -70.0f;
  self->threshold_prev = *(self->threshold);  
}

static void run(LV2_Handle instance, uint32_t sample_count) {
  SidechainGate *self = (SidechainGate *)instance;

  const float lf_fc = *(self->lf_fc);
  const float hf_fc = *(self->hf_fc);
  const float threshold = *(self->threshold);
  const float attack = *(self->attack);
  const float hold = *(self->hold);
  const float decay = *(self->decay);
  const float range = *(self->range);
  float hysteresis = *(self->hysteresis);
  const float select = *(self->select);
  const float * const left_in = self->left_in;
  const float * const right_in = self->right_in;
  float * const left_out = self->left_out;
  float * const right_out = self->right_out;
  const float * const sidechain_in = self->sidechain_in;
  const float chain_bal = *(self->chain_bal);
  const float chain_bali = 1.0f - chain_bal;
        
  float fs = self->fs;
  float env = self->env;
  float gate = self->gate;
  int state = self->state;
  int hold_count = self->hold_count;
  biquad * lf = self->lf;
  biquad * hf = self->hf;

  unsigned long pos;
  float cut = DB_CO(range);
  float t_level = DB_CO(threshold);
  float h_level = DB_CO(hysteresis);
  float a_rate = 1000.0f / (attack * fs);
  float d_rate = 1000.0f / (decay * fs);
  float post_filter_l, apost_filter_l;
  float post_filter_r, apost_filter_r;
  float apost_filter;
  float peak_left = self->peak_left;
  float peak_right = self->peak_right;
  float peak_out_left = self->peak_out_left;
  float peak_out_right = self->peak_out_right;
    
  int op = f_round(select);

  ls_set_params(lf, lf_fc, -40.0f, 0.6f, fs);
  hs_set_params(hf, hf_fc, -50.0f, 0.6f, fs);

  // we want to keep the threshold and hyster a uniform distance apart when
  // the threshold changes
  if ((threshold != self->threshold_prev )) {
    hysteresis = hysteresis + (threshold - self->threshold_prev);
    self->threshold_prev = threshold;
  }
    
  // hysteresis cannot be greater than the threshold
  if (hysteresis > threshold) hysteresis = threshold;
    
  h_level = DB_CO(hysteresis);
    
  for (pos = 0; pos < sample_count; pos++) {

    post_filter_l = chain_bali * left_in[pos] + chain_bal * sidechain_in[pos];
    post_filter_r = chain_bali * right_in[pos] + chain_bal * sidechain_in[pos]; 
        
    post_filter_l = biquad_run(lf, post_filter_l);
    post_filter_r = biquad_run(lf, post_filter_r);
    post_filter_l = biquad_run(hf, post_filter_l);
    post_filter_r = biquad_run(hf, post_filter_r);
    apost_filter_l = fabs(post_filter_l);
    apost_filter_r = fabs(post_filter_r);

    // we take the greater of left and right
    apost_filter = (apost_filter_r + apost_filter_l)  * 0.5f;
                 
    if (apost_filter > env) {
      env = apost_filter;
    } else {
      env = apost_filter * ENV_TR + env * (1.0f - ENV_TR);
    }

    if (state == CLOSED) {
      if (env >= t_level) {
        state = OPENING;
      }
    } else if (state == OPENING) {
      gate += a_rate;
      if (gate >= 1.0f) {
        gate = 1.0f;
        state = OPEN;
      }
    } else if (state == OPEN) {
      if (env < h_level) {
        state = HOLD;
        hold_count = f_round(hold * fs * 0.001f);
      }
    } else if (state == HOLD) {
      if ((env >= t_level)) {
        state = OPEN;
      } else if (hold_count <= 0) {
        state = CLOSING;
      } else {
        hold_count--;
      }
    } else if (state == CLOSING) {
      gate -= d_rate;
      if (env >= t_level) {
        state = OPENING;
      } else if (gate <= 0.0f) {
        gate = 0.0f;
        state = CLOSED;
      }
    }

    if (op == 0) {
      // gate
      buffer_write(left_out[pos], left_in[pos] * (cut * (1.0f - gate) + gate));
      buffer_write(right_out[pos], right_in[pos] * (cut * (1.0f - gate) + gate));
      if (peak_left < apost_filter_l) peak_left = apost_filter_l;
      if (peak_right < apost_filter_r) peak_right = apost_filter_r; 
    } else if (op == -1) {
      // input listen (filter only)
      buffer_write(left_out[pos], post_filter_l);
      buffer_write(right_out[pos], post_filter_r);
      if (peak_left < apost_filter_l) peak_left = apost_filter_l;
      if (peak_right < apost_filter_r) peak_right = apost_filter_r; 
    } else {
      // pass-through
      buffer_write(left_out[pos], left_in[pos]);
      buffer_write(right_out[pos], right_in[pos]);
      if (peak_left < fabs(left_in[pos])) peak_left = fabs(left_in[pos]);
      if (peak_right < fabs(right_in[pos])) peak_right = fabs(right_in[pos]);
    }
    // track output peaks
    if (peak_out_left < fabs(left_out[pos])) peak_out_left = fabs(left_out[pos]);
    if (peak_out_right < fabs(right_out[pos])) peak_out_right = fabs(right_out[pos]);
  }
  self->peak_frames++;
  if (self->peak_frames > PEAK_FRAMES ) {
    *(self->level_left) = CLAMP_CO_DB(peak_left);
    *(self->level_right) =CLAMP_CO_DB( peak_right);
    *(self->out_level_left) = CLAMP_CO_DB(peak_out_left);
    *(self->out_level_right) = CLAMP_CO_DB(peak_out_right);
    //reset peaks
    self->peak_frames = 0;
    self->peak_frames = 0.0f;
    self->peak_left = 0.0f;
    self->peak_right = 0.0f;
    self->peak_out_left = 0.0f;
    self->peak_out_right = -0.0f;
  } else {
    // carry the peak to the next frame
    self->peak_left = peak_left;
    self->peak_right = peak_right;
    self->peak_out_left = peak_out_left;
    self->peak_out_right = peak_out_right;
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

  self->env = env;
  self->gate = gate;
  self->state = state;
  self->hold_count = hold_count;
  *(self->hysteresis) = hysteresis;
}

static void deactivate(LV2_Handle instance) {
}

static void cleanup(LV2_Handle instance) {
  SidechainGate *self = (SidechainGate *)instance;
  free(self->lf);
  free(self->hf);
  free(instance);
}

static const void* extension_data(const char* uri) {
  return NULL;
}

static const LV2_Descriptor scgateDescriptor = {
  SCGATE_URI,
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
    return &scgateDescriptor;
  default:
    return NULL;
  }
}
