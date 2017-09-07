/*
  sidechain_gate_ui.cxx
  A SCGATE Gate with envelope and range.
  
  
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

#include <string>
#include <iostream>

#include <FL/x.H>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"
#include "sidechain_gate_widget.h"

using namespace std;

typedef struct {
  SidechainGateUI* widget;
  
  LV2UI_Write_Function write_function;
  LV2UI_Controller controller;
} SidechainGateGUI;

static LV2UI_Handle instantiate(const struct _LV2UI_Descriptor * descriptor,
                                const char * plugin_uri,
                                const char * bundle_path,
                                LV2UI_Write_Function write_function,
                                LV2UI_Controller controller,
                                LV2UI_Widget * widget,
                                const LV2_Feature * const * features) {
  if (strcmp(plugin_uri, SCGATE_URI) != 0) {
    fprintf(stderr, "SCGATE_URI error: invalid URI %s\n", plugin_uri);
    return NULL;
  }
  
  SidechainGateGUI* self = (SidechainGateGUI*)malloc(sizeof(SidechainGateGUI));
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
  
  self->widget = new SidechainGateUI();
  self->widget->window->border(0);
  self->widget->controller = controller;
  self->widget->write_function = write_function;
  
  if (resize) {
    resize->ui_resize(resize->handle, self->widget->getWidth(), self->widget->getHeight());
  } else {
    cout << "SidechainGateUI: TODO: Resize disabled."<< endl;
  }
  fl_embed( self->widget->window, (Window)parentXwindow );
  
  return (LV2UI_Handle)self;
}



static void cleanup(LV2UI_Handle ui) {
  SidechainGateGUI *pluginGui = (SidechainGateGUI *) ui;
  delete pluginGui->widget;
  free( pluginGui);
}

static void port_event(LV2UI_Handle ui,
                       uint32_t port_index,
                       uint32_t buffer_size,
                       uint32_t format,
                       const void * buffer) {
  SidechainGateGUI *self = (SidechainGateGUI *) ui;
  
  if ( format == 0 ) {
    float value =  *(float *)buffer;
    switch ( port_index ) {
      
    case SCGATE_ATTACK: {
      self->widget->attack->value( value );
      self->widget->attack_out->value( value );
    } break;
    case SCGATE_DECAY: {
      self->widget->release->value( value );
      self->widget->release_out->value( value );
    } break;
    case SCGATE_HOLD: {
      self->widget->hold->value( value );
      self->widget->hold_out->value( value );
    } break;
    case SCGATE_RANGE: {
      self->widget->range->value( value );
      self->widget->range_out->value( value );
    } break;
    case SCGATE_LEVEL_LEFT: {
      self->widget->level_left->value( value );
      self->widget->level_left_out->value( value );
    } break;
    case SCGATE_LEVEL_RIGHT: {
      self->widget->level_right->value( value );
      self->widget->level_right_out->value( value );
    } break;
    case SCGATE_OUT_LEVEL_LEFT: {
      self->widget->out_level_left->value( value );
      self->widget->out_level_left_out->value( value );
    } break;
    case SCGATE_OUT_LEVEL_RIGHT: {
      self->widget->out_level_right->value( value );
      self->widget->out_level_right_out->value( value );
    } break;
    case SCGATE_GATE_STATE: {
      self->widget->gate_state->value( value );
    } break;
    case SCGATE_SELECT: {
      switch ((int) value) {
      case -1: {
        self->widget->select_l->value(1);
        self->widget->select_g->value(0);
        self->widget->select_b->value(0);
      }
        break;
      case 0:  {
        self->widget->select_l->value(0);
        self->widget->select_g->value(1);
        self->widget->select_b->value(0);
      }
        break;
      default:  {
        self->widget->select_l->value(0);
        self->widget->select_g->value(0);
        self->widget->select_b->value(1);
      }
        break;
      }
    } break;
    case SCGATE_CHAIN_BAL: {
      self->widget->chain_bal_in->value( !value ? 1 : 0);
      self->widget->chain_bal_side->value( value ? 1 : 0);
    } break;
            
    }
  }
  
  return;
}


static int idle(LV2UI_Handle handle) {
  SidechainGateGUI* self = (SidechainGateGUI*)handle;
  
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
  {SCGATE_UI_URI, instantiate, cleanup, port_event, extension_data}
};

const LV2UI_Descriptor * lv2ui_descriptor(uint32_t index) {
  if (index >= sizeof(descriptors) / sizeof(descriptors[0])) {
    return NULL;
  }
  return descriptors + index;
}
