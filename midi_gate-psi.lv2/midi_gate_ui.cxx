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

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"
#include "midi_gate_widget.h"

using namespace std;

typedef struct {
  MidiGateUI* widget;
  
  LV2UI_Write_Function write_function;
  LV2UI_Controller controller;
} MidiGateGUI;

static LV2UI_Handle instantiate(const struct _LV2UI_Descriptor * descriptor,
                                const char * plugin_uri,
                                const char * bundle_path,
                                LV2UI_Write_Function write_function,
                                LV2UI_Controller controller,
                                LV2UI_Widget * widget,
                                const LV2_Feature * const * features) {
  if (strcmp(plugin_uri, MIDIGATE_URI) != 0) {
    fprintf(stderr, "MIDIGATE_URI error: invalid URI %s\n", plugin_uri);
    return NULL;
  }
  
  MidiGateGUI* self = (MidiGateGUI*)malloc(sizeof(MidiGateGUI));
  if (self == NULL) return NULL;
  
  self->controller     = controller;
  self->write_function = write_function;
  
  void* parentXwindow = 0;
  LV2UI_Resize* resize = NULL;
  
  for (int i = 0; features[i]; ++i) {
    if (!strcmp(features[i]->URI, LV2_UI__parent)) {
      parentXwindow = features[i]->data;
    } else if (!strcmp(features[i]->URI, LV2_UI__resize)) {
      resize = (LV2UI_Resize*)features[i]->data;
    }
  }
  fl_open_display();
  
  self->widget = new MidiGateUI();
  self->widget->window->border(0);
  self->widget->controller = controller;
  self->widget->write_function = write_function;
  
  if (resize) {
    resize->ui_resize(resize->handle, self->widget->getWidth(), self->widget->getHeight());
  } else {
    cout << "MidiGateUI: TODO: Resize disabled."<< endl;
  }
  fl_embed( self->widget->window, (Window)parentXwindow );
  
  return (LV2UI_Handle)self;
}

static void cleanup(LV2UI_Handle ui) {
  MidiGateGUI *pluginGui = (MidiGateGUI *) ui;
  delete pluginGui->widget;
  free( pluginGui);
}

static void port_event(LV2UI_Handle ui,
                       uint32_t port_index,
                       uint32_t buffer_size,
                       uint32_t format,
                       const void * buffer) {
  MidiGateGUI *self = (MidiGateGUI *) ui;
  
  if ( format == 0 ) {
    float value =  *(float *)buffer;
    switch ( port_index ) {
    case MIDIGATE_ATTACK: {
      self->widget->attack->value( value );
      self->widget->attack_out->value( value );
    } break;
    case MIDIGATE_DECAY: {
      self->widget->release->value( value );
      self->widget->release_out->value( value );
    } break;
    case MIDIGATE_HOLD: {
      self->widget->hold->value( value );
      self->widget->hold_out->value( value );
    } break;
    case MIDIGATE_RANGE: {
      self->widget->range->value( value );
      self->widget->range_out->value( value );
    } break;
    case MIDIGATE_LEVEL: {
      self->widget->level->value( value );
      self->widget->level_out->value( value );
    } break;
    case MIDIGATE_GATE_STATE: {
      self->widget->gate_state->value( value );
    } break;
    case MIDIGATE_SELECT: {
      self->widget->select->value( value );
    } break;
    case MIDIGATE_PROGRAM: {
      self->widget->noteon->value( !value ? 1 : 0);
      self->widget->noteoff->value( value ? 1 : 0);
    } break;
      
    }
  }
  
  return;
}

static int idle(LV2UI_Handle handle) {
  MidiGateGUI* self = (MidiGateGUI*)handle;
  
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
  {MIDIGATE_UI_URI, instantiate, cleanup, port_event, extension_data}
};

const LV2UI_Descriptor * lv2ui_descriptor(uint32_t index) {
  if (index >= sizeof(descriptors) / sizeof(descriptors[0])) {
    return NULL;
  }
  return descriptors + index;
}
