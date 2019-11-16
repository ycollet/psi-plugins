#ifndef PSILFO_H
#define PSILFO_H

#include "UI/ui_util.h"
#include "UI/psiLabelBox.H"
#include "UI/psiDialX.H"

#include "types.h"

class psiLFO : public Fl_Group {
private:
    int     idx;
    double  val;
    double  rate_val    = 0;
    double  depth_val   = 0;
    double  phase_val   = 0;
    double  wave_val    = 0;
    int     sync_val    = 0;
    
    Fl_Menu_Item on_off_menu[3] = 
    {
      { "Off" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { "On" ,0, 0, 0, 0, 0, FL_HELVETICA , 10,  0 },
      { 0 } 
    };
     
    void cb_psiLFO(int _idx, double _val)
    {
        value(_val);
        port_index(_idx);
        this->do_callback();
    }
    
    static void cb_wave(psiDialX* w, void* f)
    {
        if (w->value() != ((psiLFO*)f)->wave_val)
        {
            ((psiLFO*)f)->cb_psiLFO( 2, w->value());
            ((psiLFO*)f)->wave_val = w->value();
        }
    }
    
    static void cb_rate(psiDialX* w, void* f)
    {
        if (w->value() != ((psiLFO*)f)->rate_val)
        {
            ((psiLFO*)f)->cb_psiLFO( 0, w->value());
            ((psiLFO*)f)->rate_val = w->value();
        }
    }

    static void cb_depth(psiDialX* w, void* f)
    {
        if (w->value() != ((psiLFO*)f)->depth_val) 
        {
            ((psiLFO*)f)->cb_psiLFO( 1, w->value());
            ((psiLFO*)f)->depth_val = w->value();
        }
    }

    static void cb_phase(psiDialX* w, void* f)
    {
        if (w->value() != ((psiLFO*)f)->phase_val) 
        {
            ((psiLFO*)f)->cb_psiLFO( 3, w->value());
            ((psiLFO*)f)->phase_val = w->value();
        }
    }

    static void cb_sync(Fl_Button* w, void* f)
    {
        double _val;
        if (strcmp(w->label(),"On")==0) {
            _val = 0;
            w->label("Off");
        } else {
           _val = 1;
            w->label("On"); 
        }
        if (_val != ((psiLFO*)f)->sync_val) 
        {
            ((psiLFO*)f)->cb_psiLFO( 4, _val);
            ((psiLFO*)f)->sync_val= _val;
        }
    }

public:
    psiDialX    *wave;
    psiDialX    *rate;
    psiDialX    *depth;
    psiDialX    *phase;
    Fl_Button   *sync;

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
    
    int synced() 
    { 
        if (strcmp(sync->label(),"On")==0) return 1;
        return 0; 
    }
    
    psiLFO(int X, int Y, int W, int H, const char *L)
  : Fl_Group(X, Y, W, H, L) 
{ 
    psiLabelBox* box = new psiLabelBox(X, Y, 190, 90, label());
    
    box->box(FL_BORDER_BOX);
    box->color((Fl_Color)48);
    box->selection_color(FL_BACKGROUND_COLOR);
    box->labeltype(FL_NORMAL_LABEL);
    box->labelfont(0);
    box->labelsize(14);
    box->labelcolor(FL_LIGHT2);
    box->align(Fl_Align(FL_ALIGN_CENTER));
    box->when(FL_WHEN_RELEASE);
    
    wave = new psiDialX(X+10, Y+10, 40, 60, "Shape");
    wave->tooltip("LFO wave shape");
    wave->box(FL_ROUNDED_BOX);
    wave->color(FL_DARK_RED);
    wave->selection_color(FL_INACTIVE_COLOR);
    wave->labeltype(FL_NORMAL_LABEL);
    wave->labelfont(0);
    wave->labelsize(10);
    wave->labelcolor((Fl_Color)221);
    wave->maximum(5);
    wave->step(1);
    wave->callback((Fl_Callback*)cb_wave,(void*) this);
    wave->align(Fl_Align(FL_ALIGN_BOTTOM));
    wave->when(FL_WHEN_CHANGED);
    
    const char *a[6];
                a[0] = "Sine";
                a[1] = "Triangle";
                a[2] = "Saw";
                a[3] = "Rev Saw";
                a[4] = "Square";
                a[5] = "Stair";
    wave->display_values_set(a, 6);
    wave->draw_val_box = 1;
    
    rate = new psiDialX(X+55, Y+15, 40, 40, "Rate");
    rate->tooltip("LFO rate");
    rate->box(FL_ROUNDED_BOX);
    rate->color(FL_DARK_RED);
    rate->selection_color(FL_INACTIVE_COLOR);
    rate->labeltype(FL_NORMAL_LABEL);
    rate->labelfont(0);
    rate->labelsize(10);
    rate->labelcolor((Fl_Color)221);
    rate->maximum(20);
    rate->step(0.01);
    rate->callback((Fl_Callback*)cb_rate,(void*) this);
    rate->align(Fl_Align(FL_ALIGN_BOTTOM));
    rate->when(FL_WHEN_CHANGED);
    rate->visible_focus(0);
    
    depth = new psiDialX(X+100, Y+15, 40, 40, "Depth");
    depth->tooltip("LFO depth");
    depth->box(FL_ROUNDED_BOX);
    depth->color(FL_DARK_RED);
    depth->selection_color(FL_INACTIVE_COLOR);
    depth->labeltype(FL_NORMAL_LABEL);
    depth->labelfont(0);
    depth->labelsize(10);
    depth->labelcolor((Fl_Color)221);
    depth->maximum(100);
    depth->step(0.01);
    depth->value(7);
    depth->callback((Fl_Callback*)cb_depth,(void*) this);
    depth->align(Fl_Align(FL_ALIGN_BOTTOM));
    depth->when(FL_WHEN_CHANGED);
    
    phase = new psiDialX(X+145, Y+15, 40, 40, "Phase");
    phase->tooltip("Adjust LFO phase (voice only)");
    phase->box(FL_ROUNDED_BOX);
    phase->color(FL_DARK_RED);
    phase->selection_color(FL_INACTIVE_COLOR);
    phase->labeltype(FL_NORMAL_LABEL);
    phase->labelfont(0);
    phase->labelsize(10);
    phase->labelcolor((Fl_Color)221);
    phase->maximum(100);
    phase->step(1);
    phase->value(7);
    phase->callback((Fl_Callback*)cb_phase,(void*) this);
    phase->align(Fl_Align(FL_ALIGN_BOTTOM));
    phase->when(FL_WHEN_CHANGED);
    
    Fl_Box* o = new Fl_Box(X+110, Y+68, 30, 18, "Sync");
    o->labelsize(9);
    o->labelcolor((Fl_Color)221);
    
    sync = new Fl_Button(X+145, Y+70, 40, 15, "Off");
    sync->tooltip("Sync LFO to BPM");
    sync->down_box(FL_BORDER_BOX);
    sync->labelsize(10);
    sync->callback((Fl_Callback*)cb_sync,(void*) this);
    sync->visible_focus(0);
    end();
}
};
#endif
