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

#include <string>
#include <iostream>

#include <FL/x.H>
#include <FL/Fl_Theme.H>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"

#include "super_welle_widget.h"

extern "C" {
#include "super_welle.h"
#include "synth.h"
#include "common/voice_params.h"
}

using namespace std;

typedef struct {
  SuperWelleUI*  widget;
  LV2_Atom_Forge forge;
  LV2_URID_Map*  map;
  SW_LV2_URIS    uris;

  LV2UI_Write_Function write_function;
  LV2UI_Controller controller;
} SuperWelleGUI;

uint8_t update_ui = 0;

static LV2UI_Handle instantiate(const struct _LV2UI_Descriptor * descriptor,
                                const char * plugin_uri,
                                const char * bundle_path,
                                LV2UI_Write_Function write_function,
                                LV2UI_Controller controller,
                                LV2UI_Widget * widget,
                                const LV2_Feature * const * features) {
  void* parentXwindow = 0;
  LV2UI_Resize* resize = NULL;
  
  if (strcmp(plugin_uri, SW_URI) != 0) {
    fprintf(stderr, "SW_URI error: invalid URI %s\n", plugin_uri);
    return NULL;
  }

  SuperWelleGUI* self = (SuperWelleGUI*)malloc(sizeof(SuperWelleGUI));
  for (int i = 0; features[i]; ++i) {
    if (!strcmp(features[i]->URI, LV2_UI__parent)) {
      parentXwindow = features[i]->data;
    } else if (!strcmp(features[i]->URI, LV2_UI__resize)) {
      resize = (LV2UI_Resize*)features[i]->data;
    } else if (!strcmp(features[i]->URI, LV2_URID_URI "#map")) {
      self->map = (LV2_URID_Map*)features[i]->data;
    }
  }

  if (!self->map) {
    fprintf(stderr, "SW_URI UI: Host does not support urid:map\n");
    free(self);
    return NULL;
  }


  map_uris(self->map, &self->uris);
  lv2_atom_forge_init(&self->forge, self->map);
  
  if (self == NULL) return NULL;
  
  self->controller     = controller;
  self->write_function = write_function;
  
  Fl_Theme::set( "Vector");
  fl_open_display();
  
  self->widget = new SuperWelleUI();
  self->widget->window->border(0);
  self->widget->controller = controller;
  self->widget->write_function = write_function;
  if (resize) {
    resize->ui_resize(resize->handle, self->widget->getWidth(), self->widget->getHeight());
  } else {
    cout << "SuperWelleUI: TODO: Resize disabled."<< endl;
  }
  fl_embed( self->widget->window, (Window)parentXwindow );
  
  return (LV2UI_Handle)self;
}

static void update_ports( SuperWelleGUI* self, int port, float value);

static void cleanup(LV2UI_Handle ui) {
  SuperWelleGUI *pluginGui = (SuperWelleGUI *) ui;
  delete pluginGui->widget;
  free( pluginGui);
}

static int recv_ui_state(SuperWelleGUI* self, const LV2_Atom_Object* obj) {
  const LV2_Atom* ui_portNumber  = NULL;
  const LV2_Atom* ui_portValue  = NULL;
    
  const int n_props  = lv2_atom_object_get(
                                           obj,
                                           self->uris.ui_portNumber, &ui_portNumber,
                                           self->uris.ui_portValue, &ui_portValue,
                                           NULL);

  if (n_props != 2 ||
      ui_portNumber->type != self->uris.atom_Int ||
      ui_portValue->type != self->uris.atom_Float ) {
    // Object does not have the required properties with correct types
    fprintf(stderr, "SW_URI UI error: Corrupt state message %d %d\n",ui_portNumber->type,ui_portValue->type);
    return 1;
  }
    
  const int32_t portValue  = ((const LV2_Atom_Int*)ui_portNumber)->body;
  const float   portNumber = ((const LV2_Atom_Float*)ui_portValue)->body;
  update_ports(self, portValue, portNumber);
  return 0;
}

static void port_event(LV2UI_Handle ui,
                       uint32_t port_index,
                       uint32_t buffer_size,
                       uint32_t format,
                       const void * buffer) {
  SuperWelleGUI *self = (SuperWelleGUI *) ui;
  
  const LV2_Atom* atom = (const LV2_Atom*)buffer;
  
  if (format == self->uris.atom_eventTransfer 
      && ( atom->type == self->uris.atom_Blank
           || atom->type == self->uris.atom_Object )) {
    
    const LV2_Atom_Object* obj = (const LV2_Atom_Object*)atom;
    if (obj->body.otype == self->uris.UIData) {          
      recv_ui_state(self, obj);
    } 
  }
  if ( format == 0 ) {
    float value =  *(float *)buffer;
    update_ports(self, port_index, value);
  }
  
  return;
}

static void update_ports( SuperWelleGUI* self, int port, float value) {
  double sval = voice_param_normalize(port , value);
  switch ( port ) {
  case V_VOLUME:
    self->widget->volume->value( sval );
    self->widget->volume->redraw();
    break;
  case V_AMT:
    break;
  case V_F_MIX:
    self->widget->filter_mix->value( sval );
    break;
  case V_OSC_MIX:
    self->widget->osc_mix->value( sval );
    break;
  case V_UNI_TYP:       
    switch (f_round(sval)) {
    case 0:
      self->widget->osc1_unison_type->label("Roland");
      break;
    case 1:
      self->widget->osc1_unison_type->label("Exponential");
      break;
    default:
      break;
    }
    break;
  case V_DLY_FBL:
    self->widget->feedback_l->value( sval );
    break;
  case V_DLY_FBR:
    self->widget->feedback_r->value( sval );
    break;
  case V_DLY_TML:
    self->widget->delay_timel->value( sval );
    break;
  case V_DLY_TMR:
    self->widget->delay_timer->value( sval );
    break;
  case V_DLY_BPM:
    if (f_round(sval)) {
      self->widget->delay_sync->label( "On" );
    } else {
      self->widget->delay_sync->label( "Off" );
    }
    break;
  case V_DLY_SPT:
    if (f_round(sval)) {
      self->widget->delay_split->label( "On" );
    } else {
      self->widget->delay_split->label( "Off" );
    }            
    break;
  case V_GLIDE:
    self->widget->glide->value( sval);
    break;
  case V_PLY:
    self->widget->polyphony->value( sval );
    self->widget->polyphony->redraw();
    break;
  case V_PAN:
    self->widget->pan->set_value(0, sval);
    break;
  case V_PAN_M1:
    self->widget->pan->set_type(0, f_round(sval));
    break;
  case V_LOW:
    self->widget->eq_low->value( sval );
    break;
  case V_MID:
    self->widget->eq_mid->value( sval );
    break;
  case V_HGH:
    self->widget->eq_high->value( sval );
    break;
  case V_TMP:
    self->widget->tempo->value( sval );
    break;
  case V_OS1_SHP:
    self->widget->osc1_wave->value( sval );
    break;
  case V_OS1_TUNE:
    self->widget->osc1_tune->value( sval );
    break;
  case V_OS1_WID:
    self->widget->osc1_phase->set_value(0, sval);
    break;
  case V_OS1_WID_MOD:
    self->widget->osc1_phase->set_type(0, f_round(sval));
    break;
  case V_OS1_VOL:
    self->widget->osc1_vol->value( sval );
    break;
  case V_OS1_SUB:
    self->widget->osc1_sub->value( sval );
    break;
  case V_OS1_UNI:
    self->widget->osc1_unison->value( sval );
    break;
  case V_OS1_UNI_DET:
    self->widget->osc1_det->value( sval );
    break;
  case V_OS1_UNI_MIX:
    self->widget->osc1_mix->value( sval );
    break;
  case V_OS1_DST:
    self->widget->osc1_distortion->value( sval );
    break;
  case V_OS1_PIT_M1:
    self->widget->osc1_pitch_mod->mod_button[0]->label(
                                                       self->widget->modulation_menu[f_round(sval)].label());
    break;
  case V_OS1_PIT_M2:
    self->widget->osc1_pitch_mod->mod_button[1]->label(
                                                       self->widget->modulation_menu[f_round(sval)].label());
    break;
  case V_OS1_PIT_M3:
    self->widget->osc1_pitch_mod->mod_button[2]->label(
                                                       self->widget->modulation_menu[f_round(sval)].label());
    break;
  case V_OS1_PIT_M1A:
    self->widget->osc1_pitch_mod->set_value(0, sval);
    break;
  case V_OS1_PIT_M2A:
    self->widget->osc1_pitch_mod->set_value(1, sval);
    break;
  case V_OS1_PIT_M3A:
    self->widget->osc1_pitch_mod->set_value(2, sval);
    break;
  case V_OS2_SHP:
    self->widget->osc2_wave->value( sval );
    break;
  case V_OS2_WID:
    self->widget->osc2_phase->set_value(0, sval);
    break;
  case V_OS2_WID_MOD:
    self->widget->osc2_phase->set_type(0, f_round(sval));
    break;
  case V_OS2_VOL:
    self->widget->osc2_vol->value( sval );
    break;
  case V_OS2_NSE:
    self->widget->osc2_noise->value( sval );
    break;
  case V_OS2_TUNE:
    self->widget->osc2_tune->value( sval );
    break;
  case V_OS2_UNI:
    self->widget->osc2_unison->value( sval );
    break;
  case V_OS2_UNI_DET:
    self->widget->osc2_det->value( sval );
    break;
  case V_OS2_UNI_MIX:
    self->widget->osc2_mix->value( sval );
    break;
  case V_OS2_DST:
    self->widget->osc2_distortion->value( sval );
    break;       
  case V_OS2_PIT_M1:
    self->widget->osc2_pitch_mod->mod_button[0]->label(
                                                       self->widget->modulation_menu[f_round(sval)].label());
    break;
  case V_OS2_PIT_M2:
    self->widget->osc2_pitch_mod->mod_button[1]->label(
                                                       self->widget->modulation_menu[f_round(sval)].label());
    break;
  case V_OS2_PIT_M3:
    self->widget->osc2_pitch_mod->mod_button[2]->label(
                                                       self->widget->modulation_menu[f_round(sval)].label());
    break;
  case V_OS2_PIT_M1A:
    self->widget->osc2_pitch_mod->set_value(0, sval);
    break;
  case V_OS2_PIT_M2A:
    self->widget->osc2_pitch_mod->set_value(1, sval);
    break;
  case V_OS2_PIT_M3A:
    self->widget->osc2_pitch_mod->set_value(2, sval);
    break;
  case V_FL1_CUT:
    self->widget->filter1_cutoff->value( sval );
    self->widget->filter1_cutoff->redraw();
    break;
  case V_FL1_RES:
    self->widget->filter1_resonance->value( sval );  
    break;
  case V_FL1_WID:
    break;
  case V_FL1_TYP:
    self->widget->filter1_type->value( sval ); 
    break;
  case V_FL1_TRG:
    // self->widget->filt1_retrig->value( sval );
    break;
  case V_FL1_TRK:
    self->widget->filter1_keytrack->value( sval);
    break;
  case V_FL1_M1:
    self->widget->filt1_cutoff_mod->set_type(0, f_round(sval));
    break;
  case V_FL1_M2:
    self->widget->filt1_cutoff_mod->set_type(1, f_round(sval));
    break;
  case V_FL1_M3:
    self->widget->filt1_cutoff_mod->set_type(2, f_round(sval));
    break;
  case V_FL1_M1A:
    self->widget->filt1_cutoff_mod->set_value(0, sval);
    break;
  case V_FL1_M2A:
    self->widget->filt1_cutoff_mod->set_value(1, sval);
    break;
  case V_FL1_M3A:
    self->widget->filt1_cutoff_mod->set_value(2, sval);
    break;
  case V_FL2_CUT:
    self->widget->filter2_cutoff->value(sval);
    break;
  case V_FL2_RES:
    self->widget->filter2_resonance->value( sval ); 
    break;
  case V_FL2_WID:
    break;
  case V_FL2_TYP:
    self->widget->filter2_type->value( sval ); 
    break;
  case V_FL2_TRG:
    break;
  case V_FL2_TRK:
    self->widget->filter2_keytrack->value( sval);
    break;
  case V_FL2_M1:
    self->widget->filt2_cutoff_mod->set_type(0, f_round(sval));
    break;
  case V_FL2_M2:
    self->widget->filt2_cutoff_mod->set_type(1, f_round(sval));
    break;
  case V_FL2_M3:
    self->widget->filt2_cutoff_mod->set_type(2, f_round(sval));
    break;
  case V_FL2_M1A:
    self->widget->filt2_cutoff_mod->set_value(0, sval);
    break;
  case V_FL2_M2A:
    self->widget->filt2_cutoff_mod->set_value(1, sval);
    break;
  case V_FL2_M3A:
    self->widget->filt2_cutoff_mod->set_value(2, sval);
    break;
  case V_FL3_CUT:
    self->widget->filter3_cutoff->value(sval); 
    break;
  case V_FL3_RES:
    self->widget->filter3_resonance->value( sval );  
    break;
  case V_FL3_WID:
    break;
  case V_FL3_TYP:
    self->widget->filter3_type->value( sval ); 
    break;
  case V_FL3_IN:
    switch (f_round(sval)) {
    case FI_NONE:
      self->widget->filt3_button->label("Off");
      break;
    case FI_OSC1OSC2:
      self->widget->filt3_button->label("OSC1+2");
      break;
    case FI_OSC1:
      self->widget->filt3_button->label("OSC1");
      break;        
    case FI_OSC2:
      self->widget->filt3_button->label("OSC2");
      break;
    default:
      break;
    }
    break;
  case V_FL3_M1:
    self->widget->filt3_cutoff_mod->set_type(0, f_round(sval));
    break;
  case V_FL3_M2:
    self->widget->filt3_cutoff_mod->set_type(1, f_round(sval));
    break;
  case V_FL3_M3:
    self->widget->filt3_cutoff_mod->set_type(2, f_round(sval));
    break;
  case V_FL3_M1A:
    self->widget->filt3_cutoff_mod->set_value(0, sval);
    break;
  case V_FL3_M2A:
    self->widget->filt3_cutoff_mod->set_value(1, sval);
    break;
  case V_FL3_M3A:
    self->widget->filt3_cutoff_mod->set_value(2, sval);
    break;
  case V_FL4_CUT:
    self->widget->filter4_cutoff->value(sval); 
    break;
  case V_FL4_RES:
    self->widget->filter4_resonance->value( sval );  
    break;
  case V_FL4_WID:
    break;
  case V_FL4_TYP:
    self->widget->filter4_type->value( sval ); 
    break;
  case V_FL4_IN:
    switch (f_round(sval)) {
    case FI_NONE:
      self->widget->filt4_button->label("Off");
      break;
    case FI_OSC1OSC2:
      self->widget->filt4_button->label("OSC1+2");
      break;
    case FI_OSC1:
      self->widget->filt4_button->label("OSC1");
      break;        
    case FI_OSC2:
      self->widget->filt4_button->label("OSC2");
      break;
    default:
      break;
    }
    break;
  case V_FL4_M1:
    self->widget->filt4_cutoff_mod->set_type(0, f_round(sval));
    break;
  case V_FL4_M2:
    self->widget->filt4_cutoff_mod->set_type(1, f_round(sval));
    break;
  case V_FL4_M3:
    self->widget->filt4_cutoff_mod->set_type(2, f_round(sval));
    break;
  case V_FL4_M1A:
    self->widget->filt4_cutoff_mod->set_value(0, sval);
    break;
  case V_FL4_M2A:
    self->widget->filt4_cutoff_mod->set_value(1, sval);
    break;
  case V_FL4_M3A:
    self->widget->filt4_cutoff_mod->set_value(2, sval);
    break;
  case V_LF1_SYN:
    if (f_round(sval)) {
      self->widget->lfo1->sync->label( "On" );
    } else {
      self->widget->lfo1->sync->label( "Off" );
    }
    break;
  case V_LF1_FRQ:
    self->widget->lfo1->rate->value( sval );
    break;
  case V_LF1_AMP:
    self->widget->lfo1->depth->value( sval );
    break;
  case V_LF1_SHP:
    self->widget->lfo1->wave->value( sval );
    break;
  case V_LF1_PHS:
    self->widget->lfo1->phase->value( sval );
    break;
  case V_LF2_SYN:
    if (f_round(sval)) {
      self->widget->lfo2->sync->label( "On" );
    } else {
      self->widget->lfo2->sync->label( "Off" );
    }
    break;
  case V_LF2_FRQ:
    self->widget->lfo2->rate->value( sval );
    break;
  case V_LF2_AMP:
    self->widget->lfo2->depth->value( sval);
    break;
  case V_LF2_SHP:
    self->widget->lfo2->wave->value( sval );
    break;
  case V_LF2_PHS:
    self->widget->lfo2->phase->value( sval );
    break;
  case V_LF3_SYN:
    if (f_round(sval)) {
      self->widget->lfo3->sync->label( "On" );
    } else {
      self->widget->lfo3->sync->label( "Off" );
    }            
    break;
  case V_LF3_FRQ:
    self->widget->lfo3->rate->value( sval );
    break;
  case V_LF3_AMP:
    self->widget->lfo3->depth->value( sval );
    break;
  case V_LF3_SHP:
    self->widget->lfo3->wave->value( sval );
    break;
  case V_LF3_PHS:
    self->widget->lfo3->phase->value( sval );
    break;
  case V_EN1_ATT:
    self->widget->env1->attack->value(sval);
    break;
  case V_EN1_DEC:
    self->widget->env1->decay->value(sval );
    break;
  case V_EN1_SUS:
    self->widget->env1->sustain->value(sval);
    break;
  case V_EN1_REL:
    self->widget->env1->release->value(sval);
    break;
  case V_EN2_ATT:
    self->widget->env2->attack->value(sval);
    break;
  case V_EN2_DEC:
    self->widget->env2->decay->value(sval);
    break;
  case V_EN2_SUS:
    self->widget->env2->sustain->value(sval);
    break;
  case V_EN2_REL:
    self->widget->env2->release->value(sval);
    break;
  case V_EN3_ATT:
    self->widget->env3->attack->value(sval);
    break;
  case V_EN3_DEC:
    self->widget->env3->decay->value(sval);
    break;
  case V_EN3_SUS:
    self->widget->env3->sustain->value(sval);
    break;
  case V_EN3_REL:
    self->widget->env3->release->value(sval);
    break;
  case V_EN3_TRG:       
    switch (f_round(sval)) {
    case 0:
      self->widget->env3->trig->label("Poly");
      break;
    case 1:
      self->widget->env3->trig->label("Loop");
      break;
    default:
      break;
    }
    break;
  case V_EN4_ATT:
    self->widget->env4->attack->value(sval);
    break;
  case V_EN4_DEC:
    self->widget->env4->decay->value(sval);
    break;
  case V_EN4_SUS:
    self->widget->env4->sustain->value(sval);
    break;
  case V_EN4_REL:
    self->widget->env4->release->value(sval);
    break;
  case V_EN4_TRG:       
    switch (f_round(sval)) {
    case 0:
      self->widget->env4->trig->label("Poly");
      break;
    case 1:
      self->widget->env4->trig->label("Loop");
      break;
    default:
      break;
    }
    break;
  case V_MID_KBD_SPT:
    if (f_round(sval)) {
      self->widget->key_split->label("On");
    } else {
      self->widget->key_split->label("Off");
    }
    break;
  case V_MID_CHN:
    char buf[6];
    if (f_round(sval)) {
      self->widget->midi_channel->label(buf);
    } else {
      self->widget->midi_channel->label("All");
    }
    break;
  case V_MID_ON:
    if (f_round(sval)) {
      if (self->widget->midi_learn->color()!=FL_RED)
        self->widget->midi_learn->color(FL_RED);
      self->widget->midi_learn_on = 1;   
    } else {
      if (self->widget->midi_learn->color()!=49)
        self->widget->midi_learn->color(49);
      self->widget->midi_learn_on = 0;
    }
    self->widget->window->redraw();
    break;
  case V_MID_L01:
    self->widget->midi_map[0] = value;
    break;
  case V_MID_L02:
    self->widget->midi_map[1] = value;
    break;
  case V_MID_L03:
    self->widget->midi_map[2] = value;
    break;
  case V_MID_L04:
    self->widget->midi_map[3] = value;
    break;
  case V_MID_L05:
    self->widget->midi_map[4] = value;
    break;
  case V_MID_L06:
    self->widget->midi_map[5] = value;
    break;
  case V_MID_L07:
    self->widget->midi_map[6] = value;
    break;
  case V_MID_L08:
    self->widget->midi_map[7] = value;
    break;
  case V_MID_L09:
    self->widget->midi_map[8] = value;
    break;
  case V_MID_L10:
    self->widget->midi_map[9] = value;
    break;
  case V_MID_L11:
    self->widget->midi_map[10] = value;
    break;
  case V_MID_L12:
    self->widget->midi_map[11] = value;
    break;
  case V_MID_L13:
    self->widget->midi_map[12] = value;
    break;
  case V_MID_L14:
    self->widget->midi_map[13] = value;
    break;
  case V_MID_L15:
    self->widget->midi_map[14] = value;
    break;
  case V_MID_L16:
    self->widget->midi_map[15] = value;
    break;
       
  default:
    break;
            
  }
  ++update_ui;
  if (update_ui % 500 == 0) {
    self->widget->window->redraw();
    update_ui = 0;
  }
}

static int idle(LV2UI_Handle handle) {
  SuperWelleGUI* self = (SuperWelleGUI*)handle;
  
  self->widget->idle();
  
  return 0;
}

static const LV2UI_Idle_Interface idle_iface = { idle };

static const void* extension_data(const char* uri) {
  if (!strcmp(uri, LV2_UI__idleInterface)) {
    return &idle_iface;
  }
  return NULL;
}

static LV2UI_Descriptor descriptors[] = {
  {SW_UI_URI, instantiate, cleanup, port_event, extension_data}
};

const LV2UI_Descriptor * lv2ui_descriptor(uint32_t index) {
  if (index >= sizeof(descriptors) / sizeof(descriptors[0])) {
    return NULL;
  }
  return descriptors + index;
}
