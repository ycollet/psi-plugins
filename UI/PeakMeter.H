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

#include <stdio.h>

#include <FL/Fl_Box.H>

namespace psi
{

class PeakMeter : public Fl_Slider
{
  public:
      PeakMeter(int _x, int _y, int _w, int _h, const char *_label = 0):
        Fl_Slider(_x, _y, _w, _h, _label)
    {
        x = _x; y = _y; w = _w; h = _h; label = _label;
    }
    int x, y, w, h;
    const char *label;
    
  public:
    
    void draw()
    {
        const int iec_db = iec_scale(value());
        const float iec_dbp = iec_db / 100.0f;
        float red, green, blue, c;
        draw_box();
        draw_label();
        
        cairo_t *cc = Fl::cairo_cc();
        cairo_save(cc);
        cairo_set_line_width( cc, 1.0);
        cairo_move_to(cc, x + 2, y + 2);
        for (int j = h-4; j > 0; j-=3) {
            c = (float)j/(float)h;
            if (iec_dbp > c) {
                blue = 0;
                red = c;
                green = 1-c;
                cairo_set_source_rgb(cc, red,green,blue); 
                cairo_rectangle(cc, x+3, y+h-2-j, w-6, 2); 
                cairo_stroke(cc);
            } 
        }
        cairo_stroke(cc);
        cairo_restore( cc );
        
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
