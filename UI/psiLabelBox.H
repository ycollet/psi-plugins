#ifndef PSILABELBOX_H
#define PSILABELBOX_H

#include <valarray>
#include <string>

#include <FL/Fl_Widget.H>

#include "ui_util.h"

enum orientation {
    LBL_LEFT = 0
  , LBL_CENTER
  , LBL_RIGHT
};

enum enum_boxtype  {
    BOX_NORMAL = 0
  , BOX_ROUND
  , BOX_NORMAL_NO_FILL
  , BOX_ROUND_NO_FILL
};

class psiLabelBox : public Fl_Widget
{
    
  public:
      
    psiLabelBox(int _x, int _y, int _w, int _h, const char *_label
                , int _radius = 5):
        Fl_Widget(_x, _y, _w, _h, _label)
    {
      x = _x;
      y = _y;
      w = _w;
      h = _h;
      lx = 5;
      ly = 10;
      radius = _radius;
      label = _label;
      lbl_width = round(w * .20);
      lbl_height = 15;
      lbl_align = LBL_LEFT;
      boxtype = BOX_ROUND;
      highlight = false;
    }
    
    bool highlight;
    int x, y, w, h, lx ,ly, radius;
    const char* label;
    int lbl_width;
    int lbl_height;
    int lbl_align;
    int boxtype;
    
    void label_align( int _w, int _h, int _a, int _lx, int _ly, int _btype)
    {
        lbl_width = _w;
        lbl_height = _h;
        lbl_align = _a;
        lx = _lx; 
        ly = _ly;
        boxtype = _btype;
    }
    
    void draw()
    {
        unsigned char r, g, b = 0;
        
        if (damage() & FL_DAMAGE_ALL)
        {
            cairo_t *cr = Fl::cairo_cc();
            
            cairo_save( cr );
            color_to_rgb(Fl::get_color(labelcolor()),&r, &g, &b);
            cairo_set_source_rgba( cr, r / 255.f, g / 255.f , b / 255.f , 1 );
                        
            if (boxtype == BOX_NORMAL) cairo_rectangle( cr, x, y, w, h);
            else round_rectangle( cr, x, y, w, h, 1.0, radius);
            cairo_set_line_width(cr, 1.0);
            color_to_rgb(Fl::get_color(color()),&r, &g, &b);
            cairo_set_source_rgba( cr, r / 255.f,  g / 255.f ,  b / 255.f , 1 );
            if ((boxtype != BOX_NORMAL_NO_FILL) &&
                (boxtype != BOX_ROUND_NO_FILL) ) cairo_fill( cr );
            cairo_set_source_rgba( cr,  28 / 255.f,  28 / 255.f 
                                ,  28 / 255.f , 0.5 );
            cairo_stroke(cr);

            if (lbl_width > 0) {
                switch (lbl_align)
                {
                    case (LBL_LEFT):
                        color_to_rgb(Fl::get_color(labelcolor()),&r, &g, &b);
                        cairo_set_source_rgba( cr, r / 255.f, g / 255.f
                                            , b / 255.f , 1 );
                        cairo_move_to( cr, x, y);
                        cairo_line_to( cr, x, y + lbl_height);
                        cairo_line_to( cr, x + lbl_width, y + lbl_height);
                        cairo_line_to( cr, x + lbl_width + lbl_height, y);
                        cairo_line_to( cr, x,y);
                        cairo_fill( cr );

                        cairo_stroke(cr);
                    
                        color_to_rgb(Fl::get_color(color()),&r, &g, &b);
                        cairo_set_source_rgba( cr, r / 255.f,  g / 255.f 
                                            ,  b / 255.f , 1 );
                        cairo_select_font_face( cr, "", CAIRO_FONT_SLANT_NORMAL
                                            , CAIRO_FONT_WEIGHT_BOLD);
                        cairo_set_font_size( cr, 10 );
                    
                        cairo_move_to( cr, x + lx, y + ly);
                        cairo_show_text( cr, label );
                        cairo_stroke( cr );
                        break;
                    case (LBL_CENTER):
                        color_to_rgb(Fl::get_color(labelcolor()),&r, &g, &b);
                        cairo_set_source_rgba( cr, r / 255.f, g / 255.f 
                                            , b / 255.f , 1 );
                        cairo_move_to( cr
                                    , x + w / 2 - lbl_width * 0.5 - lbl_height 
                                    , y);
                        cairo_line_to( cr
                                    , x + w / 2 + lbl_width * 0.5 + lbl_height
                                    , y);
                        cairo_line_to( cr, x + w / 2 + lbl_width * 0.5
                                    , y + lbl_height);
                        cairo_line_to( cr, x + w / 2 - lbl_width * 0.5
                                    , y + lbl_height);
                        cairo_line_to( cr
                                    , x + w / 2 - lbl_width * 0.5 - lbl_height
                                    , y);
                        cairo_fill( cr );

                        cairo_stroke(cr);
                    
                        color_to_rgb(Fl::get_color(color()),&r, &g, &b);
                        cairo_set_source_rgba( cr, r / 255.f,  g / 255.f
                                            ,  b / 255.f , 1 );
                        cairo_select_font_face( cr, ""
                                            , CAIRO_FONT_SLANT_NORMAL
                                            , CAIRO_FONT_WEIGHT_BOLD);
                        cairo_set_font_size( cr, 10 );
                    
                        cairo_move_to( cr, x + lx, y + ly );
                        cairo_show_text( cr, label );
                        cairo_stroke( cr );
                    default:
                        break;
                }  

            }
            
            color_to_rgb(Fl::get_color(labelcolor()),&r, &g, &b);
            cairo_set_source_rgba( cr, r / 255.f, g / 255.f , b / 255.f , 1 );
            if (!image()) 
            {
                if (boxtype == BOX_NORMAL || boxtype == BOX_NORMAL_NO_FILL)
                    cairo_rectangle( cr, x, y, w, h);
                else round_rectangle( cr, x, y, w, h, 1.0, radius);
            }
            cairo_set_source_rgba( cr, r / 255.f,g/ 255.f , b/ 255.f , 1 );
            cairo_stroke( cr );  
            cairo_restore( cr );
        }
    }
};

#endif // PSILABELBOX_H
