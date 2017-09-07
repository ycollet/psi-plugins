#ifndef PSIMIDIMAPPING_H
#define PSIMIDIMAPPING_H

#include <FL/Fl_Window.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Theme.H>
#include <FL/Fl_Menu.H>
#include <string>
#include <vector>
#include <array>
#include <stdlib.h>
#include <math.h>
#include <lo/lo_osc_types.h>
#include "voice_params.h"
#include "midi_map.h"
#include "ui_util.h"

static Fl_Menu_Item cc_menu[129];
static Fl_Menu_Item port_menu[V_PARAMS+1];

static Fl_Menu_Item channel_menu[18] = {
      { "All" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "1" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "2" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "3" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "4" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "5" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "6" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "7" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "8" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "9" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "10" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "11" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "12" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "13" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "14" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "15" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "16" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { 0 } 
};


typedef struct {
    uint8_t pad;
    uint8_t channel;
    uint8_t cc;
    uint8_t port;
} MidiMapping;

class psiIdxButton : public Fl_Button
{
public:
    int index;
    int intval;
    psiIdxButton(int _x, int _y, int _w, int _h, const char* _label)
        : Fl_Button(_x, _y, _w, _h, _label)
    {
    }
    psiIdxButton(int _x, int _y, int _w, int _h, const char* _label, int idx)
        : Fl_Button(_x, _y, _w, _h, _label)
    {
        index = idx;
        intval = -1;
    }
};

class psiMidiMapping : public Fl_Window
{
  std::vector<psiIdxButton*> slot_labels;
  std::vector<psiIdxButton*> chn_buttons;
  std::vector<psiIdxButton*> cc_buttons;
  std::vector<psiIdxButton*> port_buttons;
  std::vector<psiIdxButton*> del_buttons;
  std::vector<Fl_Output*> cc_desc_buttons;
  
  psiLabelBox* labelBox;
  
  public:
    void* ui;
    std::array<MidiMapping, V_MIDI_MAPS> data;
    psiMidiMapping(int _x, int _y, int _w, int _h, const char* _label)
        : Fl_Window(_x, _y, _w, _h, _label)
    {
        begin();
        Fl_Theme::set( "Vector");
        
        labelBox = new psiLabelBox(_x + 5, _y + 5, _w - 10, _h - 10
                                    , "MIDI Mappings");
        labelBox->label_align( 100, 15, LBL_CENTER, x()+w()/2-45, 10, BOX_ROUND);
        add(labelBox);
        
        Fl_Box* l1 = new Fl_Box(x()+15, y()+30, 25, 18, "Slot");
        l1->labelsize(10);
        l1->labelcolor((Fl_Color)221);
        Fl_Box* l2 = new Fl_Box(x()+50, y()+30, 25, 18, "Chan");
        l2->labelsize(10);
        l2->labelcolor((Fl_Color)221);
        Fl_Box* l3 = new Fl_Box(x()+85, y()+30, 25, 18, "CC");
        l3->labelsize(10);
        l3->labelcolor((Fl_Color)221);
        
        Fl_Box* l4 = new Fl_Box(x()+260, y()+30, 25, 18, "Port");
        l4->labelsize(10);
        l4->labelcolor((Fl_Color)221);
        
        add(l1);
        add(l2);
        add(l3);
        add(l4);
        std::vector<float>::size_type idx = 0;
        while(idx < V_MIDI_MAPS) {
            psiIdxButton*  lbl = new psiIdxButton(x()+15,y()+(idx*20)+50,25,15,"", (int) idx);
            lbl->visible_focus(0);
            lbl->labelsize(10);
            slot_labels.push_back(lbl);
            
            psiIdxButton* chbtn = new psiIdxButton(x()+50,y()+(idx*20)+50,25,15,"", (int) idx);
            chbtn->visible_focus(0);
            chbtn->labelsize(10);
            chbtn->callback((Fl_Callback*)cb_chn_button, (void*) this);
            chn_buttons.push_back(chbtn);
            
            psiIdxButton* btn = new psiIdxButton(x()+85,y()+(idx*20)+50,25,15,"", (int) idx);
            btn->visible_focus(0);
            btn->labelsize(10);
            btn->callback((Fl_Callback*)cb_cc_button, (void*) this);
            cc_buttons.push_back(btn);
            
            Fl_Output* lbl2 = new Fl_Output(x()+120,y()+(idx*20)+50,130,15,"");
            lbl2->visible_focus(0);
            lbl2->textsize(10);
            cc_desc_buttons.push_back(lbl2);
            
            btn = new psiIdxButton(x()+260,y()+(idx*20)+50,155,15,"", (int)idx);
            btn->visible_focus(0);
            btn->labelsize(10);
            btn->callback((Fl_Callback*)cb_port_button, (void*) this);
            port_buttons.push_back(btn);
            
            btn = new psiIdxButton(x()+425,y()+(idx*20)+50,20,15,"X", (int) idx);
            btn->visible_focus(0);
            btn->labelsize(10);
            btn->callback((Fl_Callback*)cb_del_button, (void*) this);
            del_buttons.push_back(btn);
            
            add(slot_labels[idx]);
            add(cc_buttons[idx]);
            add(port_buttons[idx]);
            add(del_buttons[idx]);
            idx++;
        }

        for (int x = 0; x < 128; x++) {
            char buf[64];
            snprintf(buf, sizeof(buf), "%2d: %s",x, midi_cc_display[x]);
            cc_menu[x].label(midi_cc_display[x]);
            cc_menu[x].labelsize(10);
        }

        int lidx=0;
        for (int x = 0; x < V_PARAMS; x++) {
            //port_menu[x] = new Fl_Menu_Item();
            if (strcmp(parameters[x].description,"")) {
                port_menu[lidx].label(parameters[x].description);
                port_menu[lidx].labelsize(10);
                lidx++;
            }
        }
        end();
    }
    
    int handle(int e) 
    {
        return Fl_Window::handle(e);
    }
        
    static void cb_port_button(psiIdxButton* o, void* f) 
    {
        ((psiMidiMapping *)f)->cb_port_button_i(o);
    }
    
    void cb_port_button_i(psiIdxButton* o) {
        int done =0;
        int j = 0;
        const Fl_Menu_Item* m = port_menu->popup(o->x(),o->y()+20, 0, 0, 0);
        if (!m) return;
        else {
            while ((j < 128)&&(!done)) {
                if (!strcmp(m->label(),parameters[j].description)) {
                    this->port_buttons.at(o->index)->copy_label(parameters[j].description);   
                    data[o->index].port = j;
                    
                    o->intval = j;
                    done = 1;
                }
                j++;
            }
        }
    }
    static void cb_chn_button(psiIdxButton* o, void* f) {
        ((psiMidiMapping *)f)->cb_chn_button_i(o); 
    }
    
    void cb_chn_button_i(psiIdxButton* o) {
        char chl[5];
        int done = 0;
        int j = 0;
        
        const Fl_Menu_Item* m = channel_menu->popup(o->x(),o->y()+20, 0, 0, 0);
        if (!m) return;
        else {
            
            while ((j < 17)&&(!done)) {
                if (!strncmp(m->label(), channel_menu[j].label(),sizeof(m->label()))) {
                    snprintf(chl, sizeof(chl),"%d",j);
                    this->chn_buttons.at(o->index)->copy_label(chl);
                    o->intval = j;
                    data[o->index].channel = j;
                    done = 1;
                }
                j++;
            }
        }
    }
    static void cb_cc_button(psiIdxButton* o, void* f) {
        ((psiMidiMapping *)f)->cb_cc_button_i(o); 
    }
    
    void cb_cc_button_i(psiIdxButton* o) {
        char ccl[5];
        int done = 0;
        int j = 0;
        
        const Fl_Menu_Item* m = cc_menu->popup(o->x(),o->y()+20, 0, 0, 0);
        if (!m) return;
        else {
            while ((j < 128)&&(!done)) {
                if (!strncmp(m->label(), midi_cc_display[j],sizeof(m->label()))) {
                    snprintf(ccl, sizeof(ccl),"%d",j);
                    this->cc_buttons.at(o->index)->copy_label(ccl);
                    this->cc_desc_buttons.at(o->index)->value(midi_cc_values[j]);   
                    o->intval = j;
                    data[o->index].cc = j;
                    done = 1;
                }
                j++;
            }
        }
    }
    
    static void cb_del_button(psiIdxButton* o, void* f) {
        ((psiMidiMapping *)f)->cb_del_button_i(o);
    }
    
    void cb_del_button_i(psiIdxButton* o) {
        this->port_buttons.at(o->index)->copy_label(" ");
        this->cc_buttons.at(o->index)->copy_label("0"); 
        this->cc_desc_buttons.at(o->index)->value(" "); 
        data[o->index].cc = 0;
        data[o->index].port = 0;
        data[o->index].channel = 0;
    }
    
    void populate(float from[], int length)
    {
        std::vector<float>::size_type idx = 0;
        char ccl[5];
        while(idx < V_MIDI_MAPS) {
            const int32_t fint = lrintf(from[idx]);
            
            data[idx].channel  = (fint >> 16 & 0x7f) ;
            data[idx].cc       = fint >> 8 & 0x7f;
            data[idx].port     = fint & 0x7f;
            
            snprintf(ccl, sizeof(ccl),"%2d",(int)idx);
            slot_labels.at(idx)->copy_label(ccl);
            
            snprintf(ccl, sizeof(ccl),"%d",data[idx].cc);
            cc_buttons.at(idx)->copy_label(ccl);
            
            snprintf(ccl, sizeof(ccl),"%d",data[idx].channel);
            chn_buttons.at(idx)->copy_label(ccl);
            
            if (data[idx].port>0) {
                cc_desc_buttons.at(idx)->value(midi_cc_values[data[idx].cc]);
                cc_buttons.at(idx)->intval = data[idx].cc;
            }
            else {
                cc_desc_buttons.at(idx)->value("");
                cc_buttons.at(idx)->intval = -1;
            }
            
            if (data[idx].port>0) port_buttons.at(idx)->copy_label(parameters[data[idx].port].description);
            else port_buttons.at(idx)->copy_label("");
            
            idx++;
        }
    }
};
#endif // PSIMIDIMAPPING_H
