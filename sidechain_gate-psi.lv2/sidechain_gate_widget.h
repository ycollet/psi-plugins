// generated by Fast Light User Interface Designer (fluid) version 1.0300

#ifndef sidechain_gate_widget_h
#define sidechain_gate_widget_h
#include <FL/Fl.H>
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"
#include <FL/Fl.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Dial.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Theme.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_XPM_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Fill_Slider.H>
#include "UI/Envelope.H"
#include "UI/PeakMeter.H"
#include "UI/psiDialX.H"
#include "UI/psiLabelBox.H"
#include "sidechain_gate.h"
#ifndef LV2_DIR 
#define LV2_DIR "/usr/lib64/lv2" 
#endif
#include <FL/Fl_Double_Window.H>
using namespace std;
#include <FL/Fl_Group.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Box.H>

class SidechainGateUI {
public:
  void update_button(int button);
  SidechainGateUI();
  Fl_Double_Window *window;
  psiLabelBox *lbox;
  psi::Envelope *adsr_box;
  psiDialX *attack;
private:
  void cb_attack_i(psiDialX*, void*);
  static void cb_attack(psiDialX*, void*);
public:
  psiDialX *hold;
private:
  void cb_hold_i(psiDialX*, void*);
  static void cb_hold(psiDialX*, void*);
public:
  psiDialX *release;
private:
  void cb_release_i(psiDialX*, void*);
  static void cb_release(psiDialX*, void*);
public:
  Fl_Value_Input *attack_out;
private:
  void cb_attack_out_i(Fl_Value_Input*, void*);
  static void cb_attack_out(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *hold_out;
private:
  void cb_hold_out_i(Fl_Value_Input*, void*);
  static void cb_hold_out(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *release_out;
private:
  void cb_release_out_i(Fl_Value_Input*, void*);
  static void cb_release_out(Fl_Value_Input*, void*);
public:
  Fl_Slider *threshold;
private:
  void cb_threshold_i(Fl_Slider*, void*);
  static void cb_threshold(Fl_Slider*, void*);
public:
  Fl_Slider *hysterisis;
private:
  void cb_hysterisis_i(Fl_Slider*, void*);
  static void cb_hysterisis(Fl_Slider*, void*);
public:
  Fl_Slider *range;
private:
  void cb_range_i(Fl_Slider*, void*);
  static void cb_range(Fl_Slider*, void*);
public:
  Fl_Value_Input *threshold_out;
private:
  void cb_threshold_out_i(Fl_Value_Input*, void*);
  static void cb_threshold_out(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *hysterisis_out;
private:
  void cb_hysterisis_out_i(Fl_Value_Input*, void*);
  static void cb_hysterisis_out(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *range_out;
private:
  void cb_range_out_i(Fl_Value_Input*, void*);
  static void cb_range_out(Fl_Value_Input*, void*);
public:
  Fl_Light_Button *gate_state;
  Fl_Light_Button *select_l;
private:
  void cb_select_l_i(Fl_Light_Button*, void*);
  static void cb_select_l(Fl_Light_Button*, void*);
public:
  Fl_Light_Button *select_g;
private:
  void cb_select_g_i(Fl_Light_Button*, void*);
  static void cb_select_g(Fl_Light_Button*, void*);
public:
  Fl_Light_Button *select_b;
private:
  void cb_select_b_i(Fl_Light_Button*, void*);
  static void cb_select_b(Fl_Light_Button*, void*);
public:
  Fl_Light_Button *chain_bal_side;
private:
  void cb_chain_bal_side_i(Fl_Light_Button*, void*);
  static void cb_chain_bal_side(Fl_Light_Button*, void*);
public:
  Fl_Light_Button *chain_bal_in;
private:
  void cb_chain_bal_in_i(Fl_Light_Button*, void*);
  static void cb_chain_bal_in(Fl_Light_Button*, void*);
public:
  psiDialX *hf_fc;
private:
  void cb_hf_fc_i(psiDialX*, void*);
  static void cb_hf_fc(psiDialX*, void*);
public:
  psi::PeakMeter *level_left;
  psi::PeakMeter *level_right;
  Fl_Value_Input *hf_fc_out;
private:
  void cb_hf_fc_out_i(Fl_Value_Input*, void*);
  static void cb_hf_fc_out(Fl_Value_Input*, void*);
public:
  psiDialX *lf_fc;
private:
  void cb_lf_fc_i(psiDialX*, void*);
  static void cb_lf_fc(psiDialX*, void*);
public:
  Fl_Value_Input *lf_fc_out;
private:
  void cb_lf_fc_out_i(Fl_Value_Input*, void*);
  static void cb_lf_fc_out(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *level_left_out;
  Fl_Value_Input *level_right_out;
  psi::PeakMeter *out_level_left;
  psi::PeakMeter *out_level_right;
  Fl_Value_Input *out_level_left_out;
  Fl_Value_Input *out_level_right_out;
  LV2UI_Write_Function write_function; 
  LV2UI_Controller controller; 
  void idle();
  int getWidth();
  int getHeight();
private:
  float gain; 
  float cutoff; 
  float Q; 
public:
  void writePort(int port, float value);
#include <FL/Fl_Slider.H>
#include <FL/Fl_Theme.H>
};
#endif
