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

#include "synth.h"
#include "common/bl_osc.h"
extern waveTableShape waveTableShapes[OSC_TABLES];
#include "common/roland.h"

void synth_init(SWSynth* const s, const float* const params[], const sample_t s_rate) {
  srand ( time(NULL) );
  s->roland           = calloc(1,sizeof(Roland));
    
  s->fs               = s_rate;
  s->peak             = 0.0;
  s->n_active_notes   = 0;
    
  s->filter_prev      = 0;
  s->voice_idx        = 0;
  s->polyphony        = 16.0;
  s->voice_last       = 0;
  s->clock            = 0;
    
  for (uint16_t i = 0;  i < NUM_OSCS; i++) 
    s->last_note[i] = 0;
    
  s->gbl.tempo        = 120.0;    
  s->gbl.bender       = 0.0;    
  s->gbl.modwheel     = 0.0;    
  s->gbl.tuning       = 0.0;
  s->frames           = 0;    
  s->table_len        = OSC_TABLE_LEN + 1;
  s->table_len        = rint(s_rate) * O_SMPL + 1;
  for (uint16_t i = 0;  i < OSC_TABLES; i++) {
    s->table[i]     = calloc(s->table_len, sizeof(sample_t));
  }
    
  delay_alloc(&s->delay_l, s_rate, s_rate);
  delay_alloc(&s->delay_r, s_rate, s_rate);
    
  filter_alloc(&s->filter[0], s_rate);
  filter_init(&s->filter[0], FILTER_NONE);
  filter_alloc(&s->filter[1], s_rate);
  filter_init(&s->filter[1], FILTER_NONE);
  filter_alloc(&s->filter[2], s_rate);
  filter_init(&s->filter[2], FILTER_NONE);
  filter_alloc(&s->filter[3], s_rate);
  filter_init(&s->filter[3], FILTER_NONE);
    
  biquad_init(&s->eq_filters[0]);
  eq_set_params(&s->eq_filters[0], 100.0, 0.0, PEAK_BW, s->fs);
  biquad_init(&s->eq_filters[1]);
  eq_set_params(&s->eq_filters[1], 1000.0, 0.0, PEAK_BW,  s->fs);
  biquad_init(&s->eq_filters[2]);
  hs_set_params(&s->eq_filters[2], 10000.0, 0.0, SHELF_SLOPE,  s->fs);
    
  roland_init(s->roland);
    
  adsr_init(&s->env[0],0.2f,0.0,1.0,0.1, s_rate);
  adsr_init(&s->env[1],0.2f,0.0,1.0,0.1, s_rate);
  adsr_init(&s->env[2],0.0,0.0,1.0,0.0, s_rate);
  adsr_init(&s->env[3],0.0,0.0,1.0,0.0, s_rate);
    
  // fill the root tables 
  osc_filltables(s->table, s_rate, s->table_len);
    
  // create the band limited tables
  blosc_make_tables(s_rate, BL_TABLE_LEN);
    
  osc_init(&s->lfo[0], LFO, 0, s_rate, s->table_len, 1.0, 0.0, 0.0, 0.0, (const float** const) s->table);
  osc_init(&s->lfo[1], LFO, 0, s_rate, s->table_len, 1.0, 0.0, 0.0, 0.0, (const float** const) s->table);
  osc_init(&s->lfo[2], LFO, 0, s_rate, s->table_len, 1.0, 0.0, 0.0, 0.0, (const float** const) s->table);
  
  voice_init_all(s->voices, s->fs, s->roland);
    
  for (int i = 0; i <V_PARAMS; ++i) s->prev[i] = 0.0;
    
  for (int i = 0; i <128; ++i) {
    s->nhistory[i].next       = 0;
    for (int j = 0; j < NUM_NOTES; ++j) 
      s->nhistory[i].voices[j] = 0;
  }  
  for (int i = 0; i <17; ++i) {
    for (int j = 0; j <128; ++j) 
      s->midi_cc[i][j]=0;
  }
}

/* 
 * Sync LFO's and delay
 */
void synth_sync(SWSynth* const s, uint32_t len) {
  const sample_t tempo = s->gbl.tempo;
    
  const sample_t lfo1f = s->lfo[0].freq;
  const sample_t lfo2f = s->lfo[1].freq;
  const sample_t lfo3f = s->lfo[2].freq;
    
  const sample_t dly_l = *(s->params[V_DLY_TML]);
  const sample_t dly_r = *(s->params[V_DLY_TMR]);
  const sample_t fbl    = *(s->params[V_DLY_FBL]);
  const sample_t fbr    = *(s->params[V_DLY_FBR]);
  const sample_t sync   = f_round(*(s->params[V_DLY_BPM]));
    
  if (s->lfo[0].sync) {
    const sample_t in_freq = lfo1f ? lfo1f : 0.000001;
    const int idx = f_round((V_TEMPO-1) * in_freq
                            / (parameters[V_LF1_FRQ].max - parameters[V_LF1_FRQ].min));
    s->lfo[0].freq = tempo / tempo_values[idx].factor;
  }
  if (s->lfo[1].sync) {
    const sample_t in_freq = lfo2f ? lfo2f : 0.000001;
    const int idx = f_round((V_TEMPO-1) * in_freq
                            / (parameters[V_LF2_FRQ].max - parameters[V_LF2_FRQ].min));
    s->lfo[1].freq = tempo / tempo_values[idx].factor;
  }
  if (s->lfo[2].sync) {
    const sample_t in_freq = lfo3f ? lfo3f : 0.000001;
    const int idx = f_round((V_TEMPO-1) * in_freq
                            / (parameters[V_LF3_FRQ].max - parameters[V_LF3_FRQ].min));
    s->lfo[2].freq = tempo / tempo_values[idx].factor;
  }
  if (sync) {
    const sample_t in_dly = dly_l == 0 ? 0.000001 : dly_l;
    const int idx = f_round((V_TEMPO_DLY-1) * in_dly); 
    sample_t new_delay = round(100.0 * 1.0 / (tempo / delay_values[idx].factor)) / 100.0;
    delay_update( &s->delay_l, fbl, new_delay, len);
  }
  if (sync) {
    const sample_t in_dly = (dly_r == 0 ) ? 0.000001 : dly_r;
    const int idx = f_round((V_TEMPO_DLY-1) * in_dly); 
    sample_t new_delay = round(100.0 * 1.0 / (tempo / delay_values[idx].factor)) / 100.0;
    delay_update( &s->delay_r, fbr, new_delay, len);
  }
}

void synth_all_notes_off(SWSynth* const s) {
  Voice* v;
  for (int i=NUM_VOICES; i--;) {
    v = s->voices[i];
    
    if ((v->env[0].state != adsr_idle)||(v->env[1].state != adsr_idle)) {
      adsr_gate(&v->env[0],false);
      adsr_gate(&v->env[1],false);
    }
  }
}

/*
 * parameters to be updated once per frame 
 */
void synth_frame(SWSynth* const s, uint32_t len) {
  if (s->polyphony   != f_round(*(s->params[V_PLY]))) {
    s->voice_idx            = 0;
    s->polyphony            = f_round(*(s->params[V_PLY]));
  }
    
  s->panning                  = *(s->params[V_PAN]);
    
  eq_set_params(&s->eq_filters[0], 100.0, *(s->params[V_LOW]), PEAK_BW, s->fs);
  eq_set_params(&s->eq_filters[1], 1000.0, *(s->params[V_MID]), PEAK_BW,  s->fs);
  hs_set_params(&s->eq_filters[2], 10000.0, *(s->params[V_HGH]), SHELF_SLOPE,  s->fs);
    
  // pitch
  s->mod_osc1_pitch.mod1      =  f_round(*(s->params[V_OS1_PIT_M1]));
  s->mod_osc1_pitch.mod2      =  f_round(*(s->params[V_OS1_PIT_M2]));
  s->mod_osc1_pitch.mod3      =  f_round(*(s->params[V_OS1_PIT_M3]));
  s->mod_osc2_pitch.mod1      =  f_round(*(s->params[V_OS2_PIT_M1]));
  s->mod_osc2_pitch.mod2      =  f_round(*(s->params[V_OS2_PIT_M2]));
  s->mod_osc2_pitch.mod3      =  f_round(*(s->params[V_OS2_PIT_M3]));
  s->mod_osc1_pitch.mod1_amt  =  *(s->params[V_OS1_PIT_M1A]);
  s->mod_osc1_pitch.mod2_amt  =  *(s->params[V_OS1_PIT_M2A]);
  s->mod_osc1_pitch.mod3_amt  =  *(s->params[V_OS1_PIT_M3A]);
  s->mod_osc2_pitch.mod1_amt  =  *(s->params[V_OS2_PIT_M1A]);
  s->mod_osc2_pitch.mod2_amt  =  *(s->params[V_OS2_PIT_M2A]);
  s->mod_osc2_pitch.mod3_amt  =  *(s->params[V_OS2_PIT_M3A]);
    
  // filter 1
  s->mod_cutoff1.mod1         = f_round(*(s->params[V_FL1_M1]));
  s->mod_cutoff1.mod2         = f_round(*(s->params[V_FL1_M2]));
  s->mod_cutoff1.mod3         = f_round(*(s->params[V_FL1_M3]));
  s->mod_cutoff1.mod1_amt     = *(s->params[V_FL1_M1A]);
  s->mod_cutoff1.mod2_amt     = *(s->params[V_FL1_M2A]);
  s->mod_cutoff1.mod3_amt     = *(s->params[V_FL1_M3A]);
    
  s->filter[0].cutoff         = *(s->params[V_FL1_CUT]);
  s->filter[0].resonance      = *(s->params[V_FL1_RES]);
  s->filter[0].filter_type    = f_round(*(s->params[V_FL1_TYP]));    
  s->filter_input[0]          = f_round(*(s->params[V_FL1_IN]));
  s->filter[0].retrigger      = f_round(*(s->params[V_FL1_TRG]));
    
  // filter 2
  s->mod_cutoff2.mod1         = f_round(*(s->params[V_FL2_M1]));
  s->mod_cutoff2.mod2         = f_round(*(s->params[V_FL2_M2]));
  s->mod_cutoff2.mod3         = f_round(*(s->params[V_FL2_M3]));
  s->mod_cutoff2.mod1_amt     = *(s->params[V_FL2_M1A]);
  s->mod_cutoff2.mod2_amt     = *(s->params[V_FL2_M2A]);
  s->mod_cutoff2.mod3_amt     = *(s->params[V_FL2_M3A]);
    
  s->filter[1].cutoff         = *(s->params[V_FL2_CUT]);
  s->filter[1].resonance      = *(s->params[V_FL2_RES]);
  s->filter[1].filter_type    = f_round(*(s->params[V_FL2_TYP]));
    
  s->filter_input[1]          = f_round(*(s->params[V_FL2_IN]));
  s->filter[1].retrigger      = f_round(*(s->params[V_FL2_TRG]));
    
  // filter 3
  s->filter[2].cutoff         = *(s->params[V_FL3_CUT]);
  s->filter[2].resonance      = *(s->params[V_FL3_RES]);
    
  if ( *(s->params[V_FL3_TYP]) != s->filter[2].filter_type ) {
    s->prev[V_FL3_TYP]      = *(s->params[V_FL3_TYP]);
    filter_init(&s->filter[2], f_round(*(s->params[V_FL3_TYP])));
    filter_reset(&s->filter[2], s->filter[2].cutoff, s->filter[2].resonance,1.0, s->filter[2].bandwidth);    
  }
  s->filter_input[2]          = f_round(*(s->params[V_FL3_IN]));
  s->mod_cutoff3.mod1         = f_round(*(s->params[V_FL3_M1]));
  s->mod_cutoff3.mod2         = f_round(*(s->params[V_FL3_M2]));
  s->mod_cutoff3.mod3         = f_round(*(s->params[V_FL3_M3]));
  s->mod_cutoff3.mod1_amt     = *(s->params[V_FL3_M1A]);
  s->mod_cutoff3.mod2_amt     = *(s->params[V_FL3_M2A]);
  s->mod_cutoff3.mod3_amt     = *(s->params[V_FL3_M3A]);
    
  // filter 4
  s->filter[3].cutoff         = *(s->params[V_FL4_CUT]);
  s->filter[3].resonance      = *(s->params[V_FL4_RES]);
  if ( *(s->params[V_FL4_TYP]) != s->filter[3].filter_type ) {
    s->prev[V_FL4_TYP]      = *(s->params[V_FL4_TYP]);
    filter_init(&s->filter[3], f_round(*(s->params[V_FL4_TYP])));
    filter_reset(&s->filter[3], s->filter[3].cutoff, s->filter[3].resonance,1.0, s->filter[3].bandwidth);    
  }
  s->filter_input[3]          = f_round(*(s->params[V_FL4_IN]));
  s->mod_cutoff4.mod1         = f_round(*(s->params[V_FL4_M1]));
  s->mod_cutoff4.mod2         = f_round(*(s->params[V_FL4_M2]));
  s->mod_cutoff4.mod3         = f_round(*(s->params[V_FL4_M3]));
  s->mod_cutoff4.mod1_amt     = *(s->params[V_FL4_M1A]);
  s->mod_cutoff4.mod2_amt     = *(s->params[V_FL4_M2A]);
  s->mod_cutoff4.mod3_amt     = *(s->params[V_FL4_M3A]);
    
  // lfo'S
  const int lf1_sync          = f_round(*(s->params[V_LF1_SYN]));
  const int lf2_sync          = f_round(*(s->params[V_LF2_SYN]));
  const int lf3_sync          = f_round(*(s->params[V_LF3_SYN]));
    
  if ( (lf1_sync == 0) || (s->prev[V_LF1_SYN] != lf1_sync) ) 
    s->lfo[0].freq          = *(s->params[V_LF1_FRQ]);
  s->lfo[0].amp               = *(s->params[V_LF1_AMP]);
  s->lfo[0].shape             = f_round(*(s->params[V_LF1_SHP]));
  s->lfo[0].sync              = f_round(*(s->params[V_LF1_SYN]));
    
  if ( (lf2_sync == 0) || (s->prev[V_LF2_SYN] != lf2_sync) ) 
    s->lfo[1].freq          = *(s->params[V_LF2_FRQ]);
  s->lfo[1].amp               = *(s->params[V_LF2_AMP]);
  s->lfo[1].shape             = f_round(*(s->params[V_LF2_SHP]));
  s->lfo[1].sync              = f_round(*(s->params[V_LF2_SYN]));
    
  if ( (lf3_sync == 0) || (s->prev[V_LF3_SYN] != lf3_sync) ) 
    s->lfo[2].freq          = *(s->params[V_LF3_FRQ]);
  s->lfo[2].amp               = *(s->params[V_LF3_AMP]);
  s->lfo[2].shape             = f_round(*(s->params[V_LF3_SHP]));
  s->lfo[2].sync              = f_round(*(s->params[V_LF3_SYN]));
    
  s->env[0].attackRate        = *(s->params[V_EN1_ATT]) ;
  s->env[0].decayRate         = *(s->params[V_EN1_DEC]) ;
  s->env[0].sustainLevel      = *(s->params[V_EN1_SUS]);
  s->env[0].sustainLevel      = *(s->params[V_EN1_REL]) ;
  s->env[0].loop              = f_round(*(s->params[V_EN1_TRG]));
  s->env[1].attackRate        = *(s->params[V_EN2_ATT]) ;
  s->env[1].decayRate         = *(s->params[V_EN2_DEC]) ;
  s->env[1].sustainLevel      = *(s->params[V_EN2_SUS]);
  s->env[1].sustainLevel      = *(s->params[V_EN2_REL]) ;
  s->env[1].loop              = f_round(*(s->params[V_EN2_TRG]));
  s->env[2].attackRate        = *(s->params[V_EN3_ATT]) ;
  s->env[2].decayRate         = *(s->params[V_EN3_DEC]) ;
  s->env[2].sustainLevel      = *(s->params[V_EN3_SUS]);
  s->env[2].sustainLevel      = *(s->params[V_EN3_REL]) ;
  s->env[2].loop              = f_round(*(s->params[V_EN3_TRG]));
  s->env[3].attackRate        = *(s->params[V_EN4_ATT]) ;
  s->env[3].decayRate         = *(s->params[V_EN4_DEC]) ;
  s->env[3].sustainLevel      = *(s->params[V_EN4_SUS]);
  s->env[3].sustainLevel      = *(s->params[V_EN4_REL]) ;
  s->env[3].loop              = f_round(*(s->params[V_EN4_TRG]));
    
  s->midi_learn_on            = f_round(*(s->params[V_MID_ON]));
  s->key_split                = f_round(*(s->params[V_MID_KBD_SPT]));
  s->midi_channel             = f_round(*(s->params[V_MID_CHN]));
  s->last_port                = f_round(*(s->params[V_LAST_PORT]));
    
  // update incoming midi learn ports
  for (int i = V_MID_L01-1; i< (V_MID_L16); i++ ) {
    if (*(s->params[i])!=0) {
      uint8_t chn         = (f_round(*(s->params[i]) ) >> 16 )& 0x7f;
      uint8_t cc          = ((f_round(*(s->params[i])) )>> 8 ) & 0x7f;
      uint8_t port        = (f_round(*(s->params[i])) )& 0x7f;
            
      s->midi_cc[chn][cc] = port;
    }
  }
    
  if (f_round(*(s->params[V_NOT_OFF]))!=0) {
    *(s->params[V_NOT_OFF]) = 0.0;
    synth_all_notes_off(s);
  }
  synth_sync( s, len);
    
  for (int i = 0; i <V_PARAMS; ++i) {
    s->prev[i]              = *(s->params[i]);
  }    
    
  interpolate(&s->mix, s->mix.value, *(s->params[V_OSC_MIX]), len);
  interpolate(&s->filter_mix, s->filter_mix.value, *(s->params[V_F_MIX]), len);
    
  interpolate(&s->filter3_co, s->filter3_co.value, *(s->params[V_FL3_CUT]), len);
  interpolate(&s->filter4_co, s->filter4_co.value, *(s->params[V_FL4_CUT]), len);
    
  interpolate(&s->filter_mix, s->filter_mix.value, *(s->params[V_F_MIX]), len);

  voice_update(s->voices, &s->gbl, &s->filter[0], &s->filter[1], (const float * const*) s->params, len); 

  s->delay_split              = f_round(*(s->params[V_DLY_SPT]));
    
  if (*(s->params[V_DLY_BPM]) == 0.0) {
    const sample_t dl   = *(s->params[V_DLY_TML]);
    const sample_t dr   = *(s->params[V_DLY_TMR]);
    const sample_t fbl  = *(s->params[V_DLY_FBL]);
    const sample_t fbr  = *(s->params[V_DLY_FBR]);
    
    if ( dl != s->delay_l.delay || fbl != s->delay_l.feedback) 
      delay_update(&s->delay_l, fbl, dl, len);
    
    if ( dr !=s->delay_r.delay || fbr != s->delay_r.feedback)
      delay_update(&s->delay_r, fbr, dr, len);
  }
}

sample_t synth_process(SWSynth* const s, const uint32_t pos, sample_t* const left, sample_t* const right) {
  sample_t note1      = 0.0, note2 = 0.0;
  sample_t vleft2     = 0.0;
  sample_t vright2    = 0.0;
  sample_t filt1_out  = 0.0;
  sample_t filt2_out  = 0.0;
  sample_t mono_out   = 0.0;
    
  const sample_t osc_mix      = interp_value(&s->mix);
  const sample_t filter_mix   = interp_value(&s->filter_mix);
    
  Voice*   v          = 0;
  Filter* f3    = &s->filter[2];
  Filter* f4    = &s->filter[3];
    
  f3->cutoff    = interp_value(&s->filter3_co);
  f4->cutoff    = interp_value(&s->filter4_co);
    
    
  sample_t pan_mod    = s->panning;
  uint8_t pmodtype    = f_round(*(s->params[V_PAN_M1]));
    
  osc_tic(&s->lfo[0]);
  osc_tic(&s->lfo[1]);
  osc_tic(&s->lfo[2]);
  adsr_process(&s->env[0]);
  adsr_process(&s->env[1]);
  adsr_process(&s->env[2]);
  adsr_process(&s->env[3]);        
        
  for (int i=NUM_VOICES; i--;  ) {
    v = s->voices[i];
        
    if ((v->env[0].state != adsr_idle)||(v->env[1].state != adsr_idle)) {
      v->gain = *(s->params[V_VOLUME]);
            
      voice_process(v, pos, &vleft2, &vright2);
      note1 += vleft2;
      note2 += vright2;
            
      if (v->glide_step > 0) {
        v->glide_step--;
        v->note += v->glide_factor;
      }            
    }
  }
    
  if ( s->delay_split == 1 ) {
    note1 = note1 + delay_feed(&s->delay_l, note1);
    note2 = note2 + delay_feed(&s->delay_r, note2);
  } 
    
  f3->cutoff          = mod_cutoff(s, &s->mod_cutoff3, *(s->params[V_FL3_CUT]));
    
  f4->cutoff          = mod_cutoff(s, &s->mod_cutoff4, *(s->params[V_FL4_CUT]));
  if (s->filter_input[2]) {
    switch (s->filter_input[2]) {
    case FI_NONE:
      break;
    case FI_OSC1:
      filt1_out += filter_run(f3, note1);
      break;
    case FI_OSC2:
      filt1_out += filter_run(f3, note2);
      break;
    case FI_OSC1OSC2:
      filt1_out += filter_run(f3, note2 * COS(M_PI*osc_mix/2) + note1 * SIN(M_PI*osc_mix/2));
      break;
    case FI_FL1:
      // won't happen
      break;
    default:
      break;
    }
    filt1_out *= SIN(M_PI*filter_mix/2);
    
  }
    
  if (s->filter_input[3]) {
    switch (s->filter_input[3]) {
    case FI_NONE:
      break;
    case FI_OSC1:
      filt2_out += filter_run(f4, note1);
      break;
    case FI_OSC2:
      filt2_out += filter_run(f4, note2);
      break;
    case FI_OSC1OSC2:
      filt2_out += filter_run(f4, note2 * COS(M_PI*osc_mix/2) + note1 * SIN(M_PI*osc_mix/2));
      break;
    case FI_FL1:
      filt2_out += filter_run(f4, filt1_out);
      break;
    default:
      break;
    }
    filt2_out *= COS(M_PI*filter_mix/2);
  }
    
  mono_out  = filt1_out + filt2_out;
    
  mono_out = biquad_run(&s->eq_filters[0], mono_out);
  mono_out = biquad_run(&s->eq_filters[1], mono_out);
  mono_out = biquad_run(&s->eq_filters[2], mono_out);
    
  *left = *right = mono_out;
  if (s->delay_split == 0) {
    if (s->delay_l.delay == s->delay_r.delay) {
      if ( s->delay_l.delay > 0.0) {
        *left = *right = * left +  delay_feed(&s->delay_l, mono_out);
      }
    } else {
      if ( s->delay_l.delay > 0.0 ) {
        *left   = *left +  delay_feed(&s->delay_l, mono_out);
      }
      if ( s->delay_r.delay> 0.0 ) {
        *right   = *right +  delay_feed(&s->delay_r, mono_out);
      }
    }
  }
  pan_mod = s->panning;
  if (pmodtype) {
    switch(pmodtype) {
    case M_L1:
      pan_mod = s->lfo[0].out * 0.5;
      break;
    case M_L2:
      pan_mod = s->lfo[1].out * 0.5;
      break;
    case M_L3:
      pan_mod =  s->lfo[2].out * 0.5;
      break;
    default:
      break;
    }
  }
  if (pan_mod!= 0.0) {
    *left   = *left * sqrt( 0.5 - (pan_mod));
    *right  = *right * sqrt( 0.5 + (pan_mod));
  } 
  return mono_out;
}

/* 
 * Assign an incoming note to a voice
 */
Voice* synth_note_on(SWSynth* const s, const uint8_t note, const uint8_t vel) { 
  Voice* v = s->voices[s->voice_idx];
  int cnt = 0;
  if (s->polyphony==1) {
    v = s->voices[s->voice_idx];
    if (s->voice_idx++ > 3) s->voice_idx = 0;
    
    if ( s->voice_last && (s->voice_last->env[0].state != adsr_idle
                           || s->voice_last->env[1].state != adsr_idle )) {
      adsr_setReleaseRate(&s->voice_last->env[0],0.05 * s->fs );
      adsr_gate(&s->voice_last->env[0],0);
      adsr_setReleaseRate(&s->voice_last->env[1],0.05 * s->fs );
      adsr_gate(&s->voice_last->env[1],0);
    } 
  } else {
    bool done = false;
    // find next free skipping over those in use
    while ((cnt < s->polyphony) && (!done)) {
      
      if (++s->voice_idx >= s->polyphony) s->voice_idx = 0;
      
      if ( (s->voices[s->voice_idx]->env[0].state == adsr_idle )
           && (s->voices[s->voice_idx]->env[1].state == adsr_idle ) ) {
        done = true;
      }
      ++cnt;
    }
  }
  
  sample_t last_note = s->last_note[0];
  if (!s->key_split) {
    last_note = s->last_note[0];
    s->last_note[0] = (sample_t) note;
  } else {
    if ( note>59 ) {
      last_note = s->last_note[1];
      s->last_note[1] = (sample_t) note;
    }
    if ( note<60 ) {
      last_note = s->last_note[0];
      s->last_note[0] = (sample_t) note;
    }
  }    
  v = voice_set(note, vel, v, &s->filter[0], &s->filter[1]
                , (const float** const) s->table
                , (const float * const*) s->params, last_note);
  
  s->voice_last = v;
  
  s->nhistory[note].voices[s->nhistory[note].next] = v;
  s->nhistory[note].next++;
  if ( s->nhistory[note].next >= NUM_NOTES) s->nhistory[note].next=0;
    
  if (s->n_active_notes == 0) {
      // reset global envelopes
      adsr_init(&s->env[0], *(s->params[V_EN1_ATT]) 
                , *(s->params[V_EN1_DEC]) 
                , *(s->params[V_EN1_SUS])
                , *(s->params[V_EN1_REL]) , v->srate);
        
      adsr_init(&s->env[1], *(s->params[V_EN2_ATT]) 
                , *(s->params[V_EN2_DEC]) 
                , *(s->params[V_EN2_SUS])
                , *(s->params[V_EN2_REL]) , v->srate);
        
      adsr_init(&s->env[2], *(s->params[V_EN3_ATT]) 
                , *(s->params[V_EN3_DEC]) 
                , *(s->params[V_EN3_SUS])
                , *(s->params[V_EN3_REL]) , v->srate);
    
      adsr_init(&s->env[3], *(s->params[V_EN4_ATT]) 
                , *(s->params[V_EN4_DEC]) 
                , *(s->params[V_EN4_SUS])
                , *(s->params[V_EN4_REL]) , v->srate);
    
      adsr_gate(&s->env[0],1);
      adsr_gate(&s->env[1],1);
      adsr_gate(&s->env[2],1);
      adsr_gate(&s->env[3],1);
    }
  s->n_active_notes++;
  return v;
}

/* LIFO note off */
void synth_note_off(SWSynth* const s, const uint8_t note, const uint8_t vel) {
  uint8_t start = s->nhistory[note].next;
  uint8_t cnt = 0;
  Voice* v = 0;
  while ((v == 0 ) && (cnt < NUM_NOTES)) {
    v = s->nhistory[note].voices[start];
    
    if (v==0) {
      cnt++;
      start++;
      if (start == NUM_NOTES) start = 0;
    }
  }
  
  if (v!=0) {
    if (s->polyphony > 1) {
      adsr_gate(&v->env[0],false);
      adsr_gate(&v->env[1],false);
    } else {
      if (v->key_note == note ) {
        adsr_gate(&v->env[0],false);
        adsr_gate(&v->env[1],false);
      }
      
    }
    v = s->nhistory[note].voices[start] = 0;
  }
  if (s->n_active_notes > 0) s->n_active_notes--;
  
  if (s->n_active_notes == 0) {
    adsr_gate(&s->env[0],0);
    adsr_gate(&s->env[1],0);
    if (!s->env[2].loop) adsr_gate(&s->env[2],0);
    if (!s->env[3].loop) adsr_gate(&s->env[3],0);
  }
}

void synth_free(SWSynth* s) {
  for (int i = 0; i < NUM_VOICES; i++) {
    if (s->voices[i]) {
      Voice* v = s->voices[i];
      if (v->filter1) {
        free(v->filter1);
        v->filter1 = 0;
      }
      
      if (v->filter2) {
        free(v->filter2);
        v->filter2 = 0;
      }
      
      if (v->lfo1) {
        free(v->lfo1);
        v->lfo1 = 0;
      }
      
      if (v->lfo2) {
        free(v->lfo2);
        v->lfo2 = 0;
      }
      
      if (v->lfo3) {
        free(v->lfo3);
        v->lfo3 = 0;
      }
      
      if (v->noise) {
        free(v->noise);
        v->noise = 0;
      }
      
      if (v->sub_osc) {
        free(v->sub_osc);
        v->sub_osc = 0;
      }
      
      if (v->osc_parms[0].lp_filter) {
        free(v->osc_parms[0].lp_filter);
        v->osc_parms[0].lp_filter = 0;
      }
      
      if (v->osc_parms[0].hp_filter) {
        free(v->osc_parms[0].hp_filter);
        v->osc_parms[0].hp_filter = 0;
      }
      
      if (v->osc_parms[1].lp_filter) {
        free(v->osc_parms[1].lp_filter);
        v->osc_parms[1].lp_filter = 0;
      }
      
      if (v->osc_parms[1].hp_filter) {
        free(v->osc_parms[1].hp_filter);
        v->osc_parms[1].hp_filter = 0;
      }
      free(s->voices[i]);
      s->voices[i] = 0;
    }
  }
  for (uint16_t i = 0;  i < OSC_TABLES; i++) {
    if (s->table[i]) {
      free(s->table[i]);
      s->table[i] = 0;
    }
  }
  if (s->roland) {    
    free(s->roland);
    s->roland = 0;
  }
}
