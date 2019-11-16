#ifndef PSIENV_H
#define PSIENV_H

#include "UI/ui_util.h"
#include "UI/psiLabelBox.H"
#include "UI/psiDialX.H"

#include "types.h"

class psiENV : public Fl_Group {
private:
    psiLabelBox* lbox;
    int     idx;
    double  val;
    double  attack_val    = 0;
    double  decay_val   = 0;
    double  release_val   = 0;
    double  sustain_val    = 0;
    int     trig_val    = 0;
    
    Fl_Menu_Item on_off_menu[3] = 
    {
      { "Off" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "On" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { 0 } 
    };
     
    void cb_psiENV(int _idx, double _val)
    {
        value(_val);
        port_index(_idx);
        this->do_callback();
    }
    
    static void cb_attack(psiDialX* w, void* f)
    {
        if (w->value() != ((psiENV*)f)->attack_val)
        {
            ((psiENV*)f)->cb_psiENV( 0, w->value());
            ((psiENV*)f)->attack_val = w->value();
        }
    }

    static void cb_decay(psiDialX* w, void* f)
    {
        if (w->value() != ((psiENV*)f)->decay_val) 
        {
            ((psiENV*)f)->cb_psiENV( 1, w->value());
            ((psiENV*)f)->decay_val = w->value();
        }
    }

    static void cb_sustain(psiDialX* w, void* f)
    {
        if (w->value() != ((psiENV*)f)->sustain_val)
        {
            ((psiENV*)f)->cb_psiENV( 2, w->value());
            ((psiENV*)f)->sustain_val = w->value();
        }
    }
    
    static void cb_release(psiDialX* w, void* f)
    {
        if (w->value() != ((psiENV*)f)->release_val) 
        {
            ((psiENV*)f)->cb_psiENV( 3, w->value());
            ((psiENV*)f)->release_val = w->value();
        }
    }

    static void cb_trig(Fl_Button* w, void* f)
    {
        double _val;
        if (strcmp(w->label(),"Loop")==0) {
            _val = 0;
            w->label("Poly");
        } else {
           _val = 1;
            w->label("Loop"); 
        }
        if (_val != ((psiENV*)f)->trig_val) 
        {
            ((psiENV*)f)->cb_psiENV( 4, _val);
            ((psiENV*)f)->trig_val= _val;
        }
    }

public:
    psiDialX    *sustain;
    psiDialX    *attack;
    psiDialX    *decay;
    psiDialX    *release;
    Fl_Button   *trig;

    // value( should only be used in the callback of the group
    double  value() { return val; }
    void    value(double _val) { val = _val; }
    /*
     index order (for example)
       , V_LF1_FRQ
       , V_LF1_AMP
       , V_LF1_SHP
       , V_LF1_PHS
       , V_LF1_SYN
    */

    double  port_index() { return idx; }
    void    port_index(double _idx) { idx = _idx; }
    
    int triged() 
    { 
        if (strcmp(trig->label(),"On")==0) return 1;
        return 0; 
    }
    
    void labelcolor(Fl_Color c) 
    { 
        lbox->labelcolor(c);
    }
    
    psiENV(int X, int Y, int W, int H, const char *L)
  : Fl_Group(X, Y, W, H, L) 
{ 
    lbox = new psiLabelBox(X, Y, 190, 90, label());
    lbox->box(FL_BORDER_BOX);
    lbox->color((Fl_Color)48);
    lbox->selection_color(FL_BACKGROUND_COLOR);
    lbox->labeltype(FL_NORMAL_LABEL);
    lbox->labelfont(0);
    lbox->labelsize(14);
    lbox->labelcolor(FL_LIGHT2);
    lbox->align(Fl_Align(FL_ALIGN_CENTER));
    lbox->label_align( 65, 15, LBL_LEFT, 5, 10, BOX_ROUND);
    lbox->when(FL_WHEN_RELEASE);
    
    attack = new psiDialX(X+5, Y+20, 40, 40, "Attack");
    
    attack->box(FL_ROUNDED_BOX);
    attack->color(FL_DARK_RED);
    attack->selection_color(FL_INACTIVE_COLOR);
    attack->labeltype(FL_NORMAL_LABEL);
    attack->labelfont(0);
    attack->labelsize(10);
    attack->labelcolor((Fl_Color)221);
    attack->maximum(10);
    attack->step(0.01);
    attack->callback((Fl_Callback*)cb_attack,(void*) this);
    attack->align(Fl_Align(FL_ALIGN_BOTTOM));
    attack->when(FL_WHEN_CHANGED);
    attack->visible_focus(0);    
    
    decay = new psiDialX(X+50, Y+20, 40, 40, "Decay");
    decay->box(FL_ROUNDED_BOX);
    decay->color(FL_DARK_RED);
    decay->selection_color(FL_INACTIVE_COLOR);
    decay->labeltype(FL_NORMAL_LABEL);
    decay->labelfont(0);
    decay->labelsize(10);
    decay->labelcolor((Fl_Color)221);
    decay->maximum(10);
    decay->step(0.001);
    decay->value(7);
    decay->callback((Fl_Callback*)cb_decay,(void*) this);
    decay->align(Fl_Align(FL_ALIGN_BOTTOM));
    decay->when(FL_WHEN_CHANGED);
    
    sustain = new psiDialX(X+95, Y+20, 40, 40, "Sustain");
    sustain->box(FL_ROUNDED_BOX);
    sustain->color(FL_DARK_RED);
    sustain->selection_color(FL_INACTIVE_COLOR);
    sustain->labeltype(FL_NORMAL_LABEL);
    sustain->labelfont(0);
    sustain->labelsize(10);
    sustain->labelcolor((Fl_Color)221);
    sustain->maximum(100);
    sustain->step(0.01);
    sustain->callback((Fl_Callback*)cb_sustain,(void*) this);
    sustain->align(Fl_Align(FL_ALIGN_BOTTOM));
    sustain->when(FL_WHEN_CHANGED);
    sustain->visible_focus(0);
    
    release = new psiDialX(X+140, Y+20, 40, 40, "Release");
    release->box(FL_ROUNDED_BOX);
    release->color(FL_DARK_RED);
    release->selection_color(FL_INACTIVE_COLOR);
    release->labeltype(FL_NORMAL_LABEL);
    release->labelfont(0);
    release->labelsize(10);
    release->labelcolor((Fl_Color)221);
    release->maximum(10);
    release->step(0.001);
    release->value(7);
    release->callback((Fl_Callback*)cb_release,(void*) this);
    release->align(Fl_Align(FL_ALIGN_BOTTOM));
    release->when(FL_WHEN_CHANGED);
    
    Fl_Box* o = new Fl_Box(X+105, Y+2, 30, 18, "Trigger");
    o->labelsize(9);
    o->labelcolor((Fl_Color)221);
    
    trig = new Fl_Button(X+145, Y+5, 40, 15, "Poly");
    trig->tooltip("Global envelope mode:\n\tPoly - envelope stays open until all notes off\n\tLoop - evelope loops between attack and decay phases");
    trig->down_box(FL_BORDER_BOX);
    trig->labelsize(9);
    trig->callback((Fl_Callback*)cb_trig,(void*) this);
    trig->visible_focus(0);
    end();
}
};
#endif
