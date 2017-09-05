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

#pragma once

#include <FL/Fl_Dial_Base.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Window.H>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

extern bool default_draw_value_box;

class psiDialX : public Fl_Dial_Base
{
    static int _default_style;
    static Fl_Image *_default_image;
    static int _def_box_w;
    static int _def_box_h;
    
    int _scaleticks;
    double xmax;
    double xmin;
    double _aux_value;
    char _suffix[12];
    char _custom_format[12];
    Fl_Image *_pixmap;
    int _last_pixmap_index;
    char _display_values[32][32];
    Fl_Color cursor_color;
    int _box_w;
    int _box_h;
    void draw_knob ( int type );
    void draw_scale ( int ox,  int oy,  int side );
    void draw_cursor ( int ox,  int oy,  int sidei );
    void draw_value_box( int X, int Y, int S );
    void get_knob_dimensions ( int *X, int *Y, int *S );
    
    void get_knob_dimensionsf ( double *X, double *Y, double *S );

protected:

    virtual int handle ( int );
    virtual void draw ( void );
    virtual void draw_box ( void );
    
public:

    bool mouseover_display;
    static bool _draw_value_box;
    bool draw_val_box;
    bool _display_vals;
    void scaleticks ( int tck );

    int
    type ( void ) const
        { 
        if ( Fl_Dial_Base::type() == DEFAULT )
            return psiDialX::_default_style;
        else
            return Fl_Dial_Base::type();
        }

    void type ( int n )
        { 
            Fl_Dial_Base::type( n );
        }

    virtual void value_damage ( void ) 
    { 
         if ( window() ) 
             window()->damage( FL_DAMAGE_ALL, x(), y(), w(), h() );
    } 

    static void default_style ( int n ) { psiDialX::_default_style = n; }
    
    static void default_image ( Fl_Image *i ) { psiDialX::_default_image = i; }
    
    void set_suffix (const char* s) { strncpy(_suffix,s,strlen(s)+1); }
    
    void set_format (const char* s) { strncpy(_custom_format,s,strlen(s)+1); }
    
    void set_cursor_color(Fl_Color c) 
    {
        cursor_color = c;
    }
    
    void display_values_set (const char** s, int n ) 
    {
        _display_vals=1;
        for (int y= 0; y < n; ++y)
        {
            strncpy(&_display_values[y][0],s[y],strlen(s[y])+1);           
        }
    }
    void pixmap ( Fl_Image *i ) { _pixmap = i; }

    Fl_Image* pixmap ( void ) { return _pixmap; }

    enum 
    {
        DEFAULT,
        BURNISHED_DIAL,
        ARC_DIAL,
        PLASTIC_DIAL,
        PIXMAP_DIAL
    };


    psiDialX ( int X, int Y, int W, int H, const char *L = 0 ) :
        Fl_Dial_Base( X, Y, W, H, L )
        {
            _suffix[0] = '\0';
            _custom_format[0] = '\0';
            _scaleticks = 10;
            _display_vals = 0;
            _pixmap = 0;
            _last_pixmap_index = -1;
            _box_w = W;
            _box_h = 12;
            draw_val_box = default_draw_value_box;
            xmin = 0;
            xmax = 0;
            _aux_value = 0.0f;
            angle1(30);angle2(330);
            
            for (int i=0; i< 32; i++) _display_values[i][0] = 0;
            mouseover_display = false;
            box( FL_NO_BOX );
            type( PIXMAP_DIAL );
            set_cursor_color(112);
        }

};
