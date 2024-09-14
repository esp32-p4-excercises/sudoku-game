#pragma once

#include "lvgl.h"

class Button
{
protected:
    lv_obj_t *btn = nullptr;
    lv_event_cb_t _cb = nullptr;
    uint8_t buf[64] = {};
    lv_obj_t *grid = nullptr;
    uint8_t id = 0;
    lv_obj_t *_label = nullptr;

public:
    uint8_t _col = 0;
    uint8_t _row = 0;
    uint8_t col_span = 1;

    Button() = default;
    Button(Button &) = delete;
    Button(uint8_t col, uint8_t row) : _col(col), _row(row){ }
    virtual ~Button()
    {
        lv_obj_delete(btn); // FIXME
    }
    void init(lv_event_cb_t event_cb, lv_obj_t *obj);
    void style(lv_style_t *style);
    void style(lv_style_t *old_style, lv_style_t *new_style);
    void position(int x, int y);
    void size(int x, int y);
    void label(const char *lbl);
    void callback(lv_event_cb_t cb);
    void gridPos(int col, int row, const char *txt);

    virtual void onPress() = 0;
    virtual void onLongPress() = 0;
    virtual void onRelease() = 0;
};

class Grid : public Button
{
private:
public:
    using Button::Button;

    virtual void onPress();
    virtual void onLongPress(){};
    virtual void onRelease() {};
};

class MiniGrid : public Button
{
private:
public:
    using Button::Button;

    virtual void onPress();
    virtual void onLongPress(){};
    virtual void onRelease() {};
};

class Start : public Button
{
private:
public:
    using Button::Button;

    virtual void onPress();
    virtual void onLongPress(){};
    virtual void onRelease(){};
};

class Clear : public Button
{
public:
    using Button::Button;

    virtual void onPress();
    virtual void onLongPress(){};
    virtual void onRelease(){};
};
