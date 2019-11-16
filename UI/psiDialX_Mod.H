#ifndef PSIDIALXMOD_H
#define PSIDIALXMOD_H

#include <Fl/Fl_Group.H>
#include <Fl/Fl_Button.H>
#include <Fl/Fl_Menu_Item.H>

#include "UI/ui_util.h"
#include "UI/psiLabelBox.H"
#include "UI/psiDialX.H"

#include "types.h"

class psiDialX_Mod : public Fl_Group {

public:
    psiDialX        *moddial;
    Fl_Button *mod_button[3];
    Fl_Menu_Item    *value_menu = NULL;
    double  mod_type[3];        // button modulator
    double  mod_value[3];       // dial modulator amount
    
private:
    int     idx         = 0;    // the button index
    int     menu_length = 0;    // number of mod tyes in the menu
    int     mod_cnt     = 3;    // number of modulators
                                // last changed
    int     changed_idx = 0;    // 0 - 2 - mod type, 3 - 5 mod value
    double  changed_val = 0; 
    
    void cb_dial(double _val)
    {
        if (mod_value[idx] != _val )
        {
            mod_value[idx] = _val;
            changed_idx = idx + mod_cnt;
            changed_val = (double) _val;
            this->do_callback();
        }
    }
    
    void cb_type(int i)
    {
        
        idx = i;
        
        if (moddial->value() != mod_value[i])
            moddial->value(mod_value[i]);
        
        int x = mod_button[i]->x();
        int y = mod_button[i]->y() + mod_button[i]->h();
        
        const Fl_Menu_Item *m = value_menu->popup(x, y, 0, 0, 0);
        
        if ( !m ) {
            return;
        } 
    
        for (int j=0; j < menu_length ; j++) {
            if ( strcmp(value_menu[j].label(), m->label()) == 0 ) 
            {

                if (mod_type[i] != j) 
                {
                    mod_button[i]->label(m->label()); 
                    mod_type[i] =j;
                    changed_idx = i;
                    changed_val = (double) j;
                    this->do_callback();
                }
            }
        }
    }
    
    static void cb_moddial(psiDialX* w, void* f)
    {
        const double current = ((psiDialX_Mod*)f)->mod_value[((psiDialX_Mod*)f)->idx];
        
        if (w->value() != current) 
        {
            ((psiDialX_Mod*)f)->cb_dial(w->value());
        }
    }

    static void cb_mod_button(Fl_Button* w, void* f)
    {
        int i = 0, sel = 0;
        while (i < ((psiDialX_Mod*)f)->mod_cnt) {
            Fl_Button* button = ((psiDialX_Mod*)f)->mod_button[i];
            if (button != w)
            {
                button->value(0);
                button->color(FL_BLACK);
            }
            else 
            {
                button->value(1);
                button->color(FL_WHITE);
                sel = i;
            }
            i++;
        }

        ((psiDialX_Mod*)f)->cb_type( sel );
        return;  
    }

public:    
    psiDialX_Mod(int X, int Y, int W, int H, const char *L)
        : Fl_Group(X, Y, W, H, L) 
    { 
        Fl_Group::copy_label("");
        moddial = new psiDialX(X, Y, 40, 40, "Mod");
        moddial->box(FL_ROUNDED_BOX);
        moddial->color(FL_DARK_RED);
        moddial->selection_color(FL_INACTIVE_COLOR);
        moddial->labeltype(FL_NORMAL_LABEL);
        moddial->labelfont(0);
        moddial->labelsize(10);
        moddial->labelcolor((Fl_Color)221);
        moddial->maximum(64);
        moddial->step(0.01);
        moddial->callback((Fl_Callback*)cb_moddial, (void*) this);
        moddial->align(Fl_Align(FL_ALIGN_BOTTOM));
        moddial->when(FL_WHEN_CHANGED);
        copy_label(L);
        
        mod_button[0] = new Fl_Button(X + 43, Y + 0, 40, 14, "OFF");
        mod_button[0]->down_box(FL_DOWN_BOX);
        mod_button[0]->value(1);
        mod_button[0]->color(FL_WHITE);
        mod_button[0]->labelsize(8);
        mod_button[0]->callback((Fl_Callback*)cb_mod_button, (void*) this);
        mod_button[0]->visible_focus(0);
        
        mod_button[1] = new Fl_Button(X + 43, Y + 15, 40, 14, "OFF");
        mod_button[1]->down_box(FL_DOWN_BOX);
        mod_button[1]->color(FL_BLACK);
        mod_button[1]->labelsize(8);
        mod_button[1]->callback((Fl_Callback*)cb_mod_button, (void*) this);
        mod_button[1]->visible_focus(0);
        
        mod_button[2] = new Fl_Button(X + 43, Y + 30, 40, 14, "OFF");
        mod_button[2]->down_box(FL_DOWN_BOX);
        mod_button[2]->color(FL_BLACK);
        mod_button[2]->labelsize(8);
        mod_button[2]->callback((Fl_Callback*)cb_mod_button, (void*) this);
        mod_button[2]->visible_focus(0);
        end();
    }

    void    mod_menu(Fl_Menu_Item *_m, int _len) 
    {
        value_menu = _m;
        menu_length = _len;
    }
    // last changed value ( should only be used in the callback of the group )
    int     changed_index() { return changed_idx; }
    double  changed_value()   { return changed_val; }    
    
    void maximum(double _val)
    {
         moddial->maximum(_val);
    }
    
    void minimum(double _val)
    {
         moddial->minimum(_val);
    }
    
    void step(double _val)
    {
         moddial->step(_val);
    }    
    
    void set_value(int _idx, float _val)
    {
        mod_value[_idx] = _val;
        if (idx == _idx) {
            moddial->value((double)_val);
        }
    }
    
    void set_type(int _idx, int _type)
    {
        mod_button[idx]->label(value_menu[_type].label());
    }
    
    /*
       Single modulator under the dial
    */
    void single() 
    {
        mod_cnt = 1;
        idx     = 0;
        mod_button[1]->hide();
        mod_button[2]->hide();
        mod_button[0]->position(moddial->x(),moddial->y() + moddial->h() + 12);
    }
    
    void copy_label(const char* lbl) 
    {
        moddial->copy_label(lbl);
    }
};
#endif
