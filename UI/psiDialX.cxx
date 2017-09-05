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

#include <psiDialX.H>

#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <FL/Fl_Shared_Image.H>
#include "ui_util.h"

class image_node {
    
public:
    
    Fl_Image *original;                                     /* originl image */
    
    Fl_Image *scaled;                                        /* downscaled image */
    
    class image_node *next;
};

static image_node *_first = 0;

bool default_draw_value_box;

int psiDialX::_default_style = psiDialX::PIXMAP_DIAL;

Fl_Image *psiDialX::_default_image = 0;

void
psiDialX::draw_box ( void )
{
}

static Fl_Widget *_mouse_inside = NULL;

int
psiDialX::handle ( int m )
{
    switch ( m )
    {
        case FL_ENTER:
            _mouse_inside = this;
            redraw();
            return Fl_Dial_Base::handle(m) || 1;
        case FL_LEAVE:
            _mouse_inside = NULL;
            redraw();
            return Fl_Dial_Base::handle(m) || 1;
        case FL_MOUSEWHEEL:
        {
            if ( this != Fl::belowmouse() )
                return 0;
            if (Fl::e_dy==0)
                return 0;
            
            const int steps = Fl::event_ctrl() ? 128 : 16;
            
            const float step = fabs( maximum() - minimum() ) / (float)steps;
            
            int dy = Fl::e_dy;
            
            if ( maximum() > minimum() )
                dy = 0 - dy;
            
            handle_drag(clamp(value() + step * dy));
            return 1;
        }
    }

    int X, Y, S;

    get_knob_dimensions ( &X, &Y, &S );

    return Fl_Dial_Base::handle( m, X, Y, S, S );
}

void
psiDialX::draw ( void )
{
    int X, Y, S;
    get_knob_dimensions ( &X, &Y, &S);

    draw_box();
    draw_label();

    int t = type();

    if ( t == PIXMAP_DIAL )
    {
        Fl_Image *im = pixmap();
        
        if ( !im )
            im = psiDialX::_default_image;
               
        if ( im )
        {
            fl_push_clip( x(), y(), w(), h() );
      
            int knob_width = im->h();
            
            const int frames = im->w() / im->h();

            const int index = (int)( ( frames - 1 ) * ( psiDialX::value() - minimum()) / ( maximum() - minimum() ));

            if ( ( damage() == FL_DAMAGE_ALL ) || 
                  ( ( damage() & FL_DAMAGE_EXPOSE ) && 
                    index != _last_pixmap_index ) ) 
            {

                if ( w() >= knob_width )
                {
                    im->draw( x() + ( w() / 2 ) - ( knob_width / 2 ),
                              y() + ( h() / 2 ) - ( knob_width / 2 ),
                              knob_width,
                              knob_width,
                              knob_width * index,
                              0 );
                }
                else
                {

                    knob_width = w();

                    image_node *i;

                    Fl_Image *scaled = 0;

                    for ( i = _first; i; i = i->next )
                    {
                        if ( i->original == im &&
                             i->scaled && i->scaled->h() == knob_width )
                        {
                            scaled = i->scaled;
                            break;
                        }
                    }

                    if ( ! scaled )
                    {
                        scaled = im->copy( knob_width * frames, knob_width );

                        i = new image_node();

                        i->original = im;
                        i->scaled = scaled;
                        i->next = _first;
                        _first = i;
                    }
                  
                    scaled->draw( x() + ( w() / 2 ) - ( knob_width / 2 ),
                                  y() + ( h() / 2 ) - ( knob_width / 2 ),
                                  knob_width,
                                  knob_width,
                                  knob_width * index,
                                  0 );
                }
                
                _last_pixmap_index = index;
            }
            /* TODO: Make this optional */
            if (draw_val_box) draw_value_box(X, Y, S);
                
            
            fl_color(fl_color_average(FL_BACKGROUND_COLOR, FL_WHITE, .7f));
            draw_scale(X, Y, S);
            fl_pop_clip();
            draw_cursor( X, Y, S);
    
        }
    }

    if (( _mouse_inside == this ) && ( mouseover_display ))
    {
        /* TODO: Make this optional */
        char s[128];
    
        fl_font( FL_HELVETICA, 10 );
    
        char buf[128];
        format(buf);
        
        snprintf( s, sizeof( s ), buf, value()  );
        
        fl_color( labelcolor() );
        fl_draw( s, X, Y, S, S, FL_ALIGN_CENTER );
    }
    draw_cursor( X, Y, S);
}

void
psiDialX::draw_value_box( int X, int Y, int S )
{
    double v = value();
    
    fl_font( FL_HELVETICA, 9 );
    
    char buf[128];
    char s[128];
    bool done = false;
    format(buf);
    int rint = (int)round(value());
    if (_display_vals) {
        if ((rint<32) && (_display_values[rint]!=0)) {
            done = true;    
            strncpy( s, _display_values[rint], strlen(_display_values[rint])+1);
        }
    } 
    
    if (_custom_format[0] != '\0') 
        strncpy( buf, _custom_format, strlen(_custom_format));
    
    if (_suffix[0] != '\0') strcat(buf,"%s");
    
    if (!done) {
        snprintf( s, 128, buf, v,_suffix);
    }
    
    fl_draw_box( box(), X+S*0.05, Y+(S *0.97 ), S*.96, S*0.25, FL_BLACK);
    fl_font( FL_HELVETICA, 9);
    fl_color( labelcolor());
    fl_draw( s, X, Y, S, S*1.2, FL_ALIGN_BOTTOM );
}

void
psiDialX::get_knob_dimensions ( int *X, int *Y, int *S )
{
    int ox, oy, ww, hh, side;
    ox = x();
    oy = y();
    ww = w();
    hh = h();
    
    if (ww > hh)
    {
        side = hh;
        ox = ox + (ww - side) / 2;
    }
    else
    {
        side = ww;
        oy = oy + (hh - side) / 2;
    }
    side = w() > h() ? hh : ww;

    *X = ox;
    *Y = oy;
    *S = side;
}

void
psiDialX::get_knob_dimensionsf ( double *X, double *Y, double *S )
{
    double ox, oy, ww, hh, side;
    ox = (double)x();
    oy = (double)y();
    ww = (double)w();
    hh = (double)h();
    
    if (ww > hh)
    {
        side = hh;
        ox = ox + (ww - side) / 2.0;
    }
    else
    {
        side = ww;
        oy = oy + (hh - side) / 2.0;
    }
    side = w() > h() ? hh : ww;

    *X = ox;
    *Y = oy;
    *S = side;
}

void 
psiDialX::draw_cursor ( int ox, int oy, int side )
{
 
    double angle;
    double fox, foy, fs;
    get_knob_dimensionsf(&fox, &foy, &fs);
    angle = ( angle2() - angle1() ) * ( value() - minimum()) / ( maximum() - minimum() ) + angle1();

    fl_color(fl_color_average(FL_BACKGROUND_COLOR, FL_BLACK, .7f));
    fl_line_style( FL_SOLID, side / 10);
    
    const int d = 4;
    
    /* account for edge conditions */
    angle = angle < angle1() + d ? angle1() + d : angle;
    angle = angle > angle2() - d ? angle2() - d : angle;
    fox += fs / 2.0;
    foy += fs / 2.0;
    fl_color( 50 );
    fl_line_style( FL_SOLID, 2);
    if (abs(minimum()) == abs(maximum())) {
        angle = ( angle2() - angle1() ) / 2 * ( value()) / abs( maximum()) + 1;
        fl_arc( round(fox - fs * 0.25),round(foy- fs * 0.25), round(fs* 0.5),round(fs* 0.5),90,90 - angle);
    } else {
        fl_arc(round(fox - fs * 0.25),round(foy- fs * 0.25),round(fs* 0.5),round(fs * 0.5), 270 - angle1(), 270 - angle);
    }
    fl_line_style( FL_SOLID, 0 );
    
}

void
psiDialX::draw_scale ( int ox, int oy, int side )
{
    float x2, y2, rds, cx, cy, ca, sa;
    
    rds = side / 2;
    cx = ox + side / 2;
    cy = oy + side / 2;
    if (_scaleticks == 0)
        return;
    double a_step = (10.0 * 3.14159 / 6.0) / _scaleticks;
    a_step = 0.454;
    double a_orig = -(0.7);
    a_orig = (double) angle1() * 3.14159 / 180.0 - 2 * 3.14159 / 4.0 ;
    a_step = (((double) angle2() -(double) angle1())  * 3.14159 / 180.0)
            / 10;
    for (int a = 0; a <= 10; a++)
    {
        double na = a_orig + a * a_step;
        ca = cos(na);
        sa = sin(na);
        x2 = cx + (rds - 3.0) * ca;
        y2 = cy - (rds - 3.0) * sa;
        
        fl_color(labelcolor());
        fl_circle(x2, y2, 0.5);
    }
}

void 
psiDialX::scaleticks ( int tck )
{
    _scaleticks = tck;
    if (_scaleticks < 0)
        _scaleticks = 0;
    if (_scaleticks > 31)
        _scaleticks = 31;
    if (visible())
        damage(FL_DAMAGE_ALL);
}
