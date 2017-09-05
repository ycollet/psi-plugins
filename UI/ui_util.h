#ifndef UI_UTIL_H
#define UI_UTIL_H

#include <cairo/cairo.h>
#include <Fl/Fl_Menu_Button.h>
#include <Fl/Fl_Button.h>
#include <Fl/Fl_Menu_Item.H>
#include <stdlib.h>

// converts a 32 bit color to RGB (Fl_Color is the topmost 3 bytes)
__attribute__((unused)) 
static void color_to_rgb(unsigned int color, unsigned char* r, unsigned char* g
                   , unsigned char* b)
{
    *r = color >> 24 & 0xFF;
    *g = color >> 16 & 0xFF;
    *b = color >> 8 & 0xFF;   
}

/**
 * Given a menu, call write_func on selection
 * 
 */
__attribute__((unused)) 
static void ui_util_menu(const Fl_Menu_Item* items, const Fl_Menu_Item* m, Fl_Button* button
          , int port, void (*write_func)(int,float)) {
    if ( !m ) {
       return;
    } 
    
    for (int i=0; &items[i] ; i++) {
        if ( strcmp(items[i].label(), m->label()) == 0 ) {
           write_func(port,(float) i);
           button->label(m->label());    
           return;
        }
    }
    return;
}

__attribute__((unused)) 
static void round_rectangle(cairo_t* cr, double x ,double y
        , double width, double height, double aspect
        , double corner_radius)
{
    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;
    x = x + 1;
    y = y + 1; 
    height = height - 1;
    width = width -1;
    cairo_new_sub_path (cr);
    cairo_arc (cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc (cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc (cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc (cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path (cr);

    /*cairo_set_source_rgb (cr, 0.5, 0.5, 1);
    cairo_fill_preserve (cr);
    cairo_set_source_rgba (cr, 0.5, 0, 0, 0.5);
    cairo_set_line_width (cr, 1.0);*/
    
}
#endif
