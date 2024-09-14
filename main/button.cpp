
#include "button.h"

void Button::init(lv_event_cb_t event_cb, lv_obj_t *parent)
{
    _cb = event_cb;
    btn = lv_button_create(parent); /*Add a button the current screen*/
    callback(_cb);
    _label = lv_label_create(btn);
}

void Button::style(lv_style_t *style)
{
    lv_obj_add_style(btn, style, LV_STATE_DEFAULT);
}

void Button::style(lv_style_t *old_style, lv_style_t *new_style)
{
    lv_obj_replace_style(btn, old_style, new_style, LV_STATE_DEFAULT);
}

void Button::position(int x, int y)
{
    lv_obj_set_pos(btn, x, y);
}

void Button::size(int x, int y)
{
    lv_obj_set_size(btn, x, y);
}

void Button::label(const char *lbl)
{
    lv_label_set_text(_label, lbl);
    lv_obj_center(_label);
}

void Button::callback(lv_event_cb_t cb)
{
    if (cb)
        lv_obj_add_event_cb(btn, cb, LV_EVENT_ALL, this); /*Assign a callback to the button*/
    else
        lv_obj_remove_event_cb(btn, _cb);
}

void Button::gridPos(int col, int row, const char *txt)
{
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, col_span, LV_GRID_ALIGN_STRETCH, row, 1);
    label(txt);
}
