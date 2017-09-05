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

#include <FL/Fl_Box.H>
#include <stdio.h>
#include <FL/fl_draw.H>

namespace psi
{

class Fader : public Fl_Slider
{
  public:
      Fader(int _x, int _y, int _w, int _h, const char *_label = 0):
        Fl_Slider(_x, _y, _w, _h, _label)
    {
        x = _x; y = _y; w = _w; h = _h; label = _label;
    }
    int x, y, w, h;
    const char *label;
    
  public:
    
void draw() {

  
  int X = x, Y = y, W = w, H = h;
    
  double slider_size = .1 ;
  int xsl, ysl, wsl, hsl;
  for (float val = 0.0; val <= 1.1; val+= 0.1) {
    int ww = (horizontal() ? W : H);
    int xx, S;
    if (type()==FL_HOR_FILL_SLIDER || type() == FL_VERT_FILL_SLIDER) {
        S = int(val*ww+.5);
        if (minimum()>maximum()) {S = ww-S; xx = ww-S;}
        else xx = 0;
    } else {
        S = int(slider_size*ww+.5);
        int T = (horizontal() ? H : W)/2+1;
        if (type()==FL_VERT_NICE_SLIDER || type()==FL_HOR_NICE_SLIDER) T += 4;
        if (S < T) S = T;
        xx = int(val*(ww-S)+.5);
    }
    
    if (horizontal()) {
        xsl = X+xx;
        wsl = S;
        ysl = Y;
        hsl = H;
    } else {
        ysl = Y+xx;
        hsl = S;
        xsl = X;
        wsl = W;
    }

    Fl_Boxtype box1 = slider();
    if (!box1) {box1 = (Fl_Boxtype)(box()&-2); if (!box1) box1 = FL_UP_BOX;}
    if (type() == FL_VERT_NICE_SLIDER) {
       
        int d = (hsl-4)/2 + 1;
        //draw_box(FL_THIN_DOWN_BOX, xsl+2, ysl+d, wsl-4, hsl-2*d,selection_color());
        fl_color(fl_lighter(selection_color()));
        if (val==0.5) {
            fl_line( xsl-6, ysl+d, xsl+2, ysl+d);
            fl_line( xsl+wsl-2, ysl+d, xsl+wsl+6, ysl+d);
        }
        else {
            fl_line( xsl-4, ysl+d, xsl+2, ysl+d);
            fl_line( xsl+wsl-2, ysl+d, xsl+wsl+4, ysl+d);
        }
        
    } 
  }
  Fl_Boxtype box1 = slider();
  draw_box(box1, xsl, ysl, wsl, hsl, FL_GRAY);
  Fl_Slider::draw(x,y,w,h);

}

private:
    float iec_scale(float db) {
         float def = 0.0f; /* Meter deflection %age */
 
         if (db < -70.0f) {
                 def = 0.0f;
         } else if (db < -60.0f) {
                 def = (db + 70.0f) * 0.25f;
         } else if (db < -50.0f) {
                 def = (db + 60.0f) * 0.5f + 5.0f;
         } else if (db < -40.0f) {
                 def = (db + 50.0f) * 0.75f + 7.5;
         } else if (db < -30.0f) {
                 def = (db + 40.0f) * 1.5f + 15.0f;
         } else if (db < -20.0f) {
                 def = (db + 30.0f) * 2.0f + 30.0f;
         } else if (db < 0.0f) {
                 def = (db + 20.0f) * 2.5f + 50.0f;
         } else {
                 def = 100.0f;
         }
         return (int)(def);
    }
};
}
