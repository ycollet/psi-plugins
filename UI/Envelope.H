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

class Envelope : public Fl_Slider
{
  public:
      Envelope(int _x, int _y, int _w, int _h, const char *_label = 0):
        Fl_Slider(_x, _y, _w, _h, _label)
    {
        x = _x; y = _y; w = _w; h = _h; label = _label;
        a = 20; d = 0; s = 100; r = 50; range =   20.0f / - 70.0f;
        
    }
    int x, y, w, h;
    const char *label;
            
    void set_values(float _a, float _d, float _s, float _r, float _range_length = 7000.0f )
    {
        a = _a; d = _d; s = _s; r = _r; 
        range_length = _range_length;    
        redraw();
    }
    void attack( float _a) { 
        a = _a; 
        redraw();
    }
    void sustain( float _s) { 
        s = _s; 
        redraw(); 
    }
    void decay( float _d) { 
        d = _d; 
        redraw(); 
    }
    void release( float _r) { 
        r = _r; 
        redraw(); 
    }
    void set_range( float _min, float _max, float _len, float _value) { 
        range_max = _max; 
        range_min = _min; 
        range_length = _len;
        range = _value;
        redraw(); 
    }
    void set_range( float _value) { 
        range = _value;
        redraw(); 
    }
    /*void invert( bool _i) { 
        inverse = _i;
        redraw(); 
    }*/
  protected:
    float a, d, s, r, range;
    float range_length;
    float range_max;
    float range_min;
    //bool inverse = false;
    
  public:
    
    void draw()
    {
        float start = 0;
        float peak = 0;
        float range_diff = range_max - range_min;
        float center_line = (range_max / range_diff) * h;
        
        const double dashes[2] = {1.0,0};
        
        draw_box();
        draw_label();
        
        cairo_t *cc = Fl::cairo_cc();
        cairo_save(cc);
        cairo_set_line_width(cc, 1.0);
        cairo_set_source_rgb(cc,.5f,  .5f, .5f);
        //cairo_rectangle(cc, x, y, w, h );
        cairo_stroke(cc);
        
        cairo_set_source_rgb(cc, 0.0,0.4,0.0);
        //cairo_set_dash ( cc, dashes, 1, 0.0);
        cairo_set_line_width( cc, .2);
        for (int j = 5; j < h -2; j+= 10 ) {
            cairo_move_to(cc, x + 2, y + j);
            cairo_line_to(cc, x + w -5, y + j );
        }
        for (int j = 5; j < w -1; j+= 10) {
            cairo_move_to(cc, x + j, y + 2);
            cairo_line_to(cc, x + j, y + h - 3);
        }
        cairo_stroke(cc);
   
        cairo_set_dash (cc, dashes, 0, 0.0);
        
        if  (range < 0) {
            start = 5 +(-range / range_diff) * (h - 8) +center_line;
            peak = ( center_line ) - start;
        }
        else {
            start = center_line;
            peak = - (range / range_diff) * (h - 8);
        }
        cairo_set_line_width(cc, 1.5);
        cairo_set_line_join(cc, CAIRO_LINE_JOIN_ROUND);
        cairo_set_line_cap (cc, CAIRO_LINE_CAP_ROUND);
        cairo_set_source_rgb(cc, 39.0 / 255, 193.0 /255, 193.0 /255);
        cairo_move_to(cc, x +3, y + start);
        cairo_rel_line_to(cc, w * 0.1, 0);
        cairo_rel_line_to(cc, w * 0.8 * (a / range_length),peak);
        cairo_rel_line_to(cc, w * 0.8 * (s / range_length), 0);
        cairo_rel_line_to(cc, w * 0.8 * (r / range_length), -peak);
        cairo_line_to(cc, x + w - 3,y + start);
        cairo_stroke(cc);
        cairo_restore( cc );
        
    }
};
}
