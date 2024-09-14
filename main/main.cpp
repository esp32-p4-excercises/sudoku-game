#include <stdio.h>
#include <vector>

#include "bsp/esp32_p4_function_ev_board.h"

#include "esp_lvgl_port.h"

#include "esp_sudoku_port.h"

#include "lvgl.h"
#include "button.h"

using namespace std;

static lv_style_t style_btn1;
static lv_style_t style_btn2;
static lv_style_t style_btn3;
static lv_style_t style_btn4;
static lv_style_t style_cont;
static void init_styles();
static void grid_btn_event_cb(lv_event_t *e);
static void init_grid();
static void helper_grid();

static EspSudoku *board = nullptr;
static Start start;
static Clear *clr;
static Grid *selected = nullptr;
static volatile int state = 0;
static bool check_value = true;
static lv_obj_t *grid = nullptr;
static lv_obj_t *gen_label = nullptr;
static lv_obj_t *mini_grid = nullptr;
static vector<Grid *> big_grid_vector;
static vector<MiniGrid *> small_grid_vector;

#define IDLE_STATE 0
#define GENERATE_GRID 1
#define SUDOKU_FINISHED 2

static void delete_grid(int n)
{
    bsp_display_lock(0);
    if (n == 1)
    {
        for (auto btn : big_grid_vector)
        {
            delete (btn);
        }
        big_grid_vector.resize(0);
        if (grid)
            lv_obj_delete(grid);

        grid = nullptr;
    }
    if (n == 2)
    {
        if (clr)
        {
            delete (clr);
            clr = nullptr;
        }
        for (auto btn : small_grid_vector)
        {
            delete (btn);
        }
        small_grid_vector.resize(0);
        if (mini_grid)
            lv_obj_delete(mini_grid);

        mini_grid = nullptr;
    }
    bsp_display_unlock();
}

extern "C" void app_main(void)
{
    const bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
        .flags = {
            .buff_dma = true,
            .buff_spiram = true,
            .sw_rotate = true,
        }};

    bsp_display_start_with_config(&cfg);

    bsp_display_lock(10);
    bsp_display_brightness_set(10);

    bsp_display_unlock();

    init_styles();
    lv_obj_set_style_bg_color(lv_scr_act(), LV_COLOR_MAKE(0, 0, 100), LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lv_scr_act(), LV_COLOR_MAKE(200, 200, 200), LV_STATE_DEFAULT);

    start.init(grid_btn_event_cb, lv_screen_active());
    start.style(&style_btn2);
    start.position(900, 20);
    start.size(100, 60);
    start.label("Start");

    while (1)
    {
        switch (state)
        {
        case IDLE_STATE:
        {
            vTaskDelay(100);
            break;
        }
        case GENERATE_GRID:
        {
            bsp_display_lock(10);
            lv_label_set_text(gen_label, "generating sudoku grid");
            lv_obj_set_size(gen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            bsp_display_unlock();

            delete_grid(1);
            delete_grid(2);
            if (board)
                delete board;
            board = new EspSudoku();
            board->createSeed();
            // board->genPuzzle();
            board->genDifficulty(50);
            board->calculateDifficulty();

            board->printGrid();
            printf("\t grid: \t %s\n", board->getGrid().c_str());
            init_grid();
            lv_label_set_text(gen_label, "");

            state = IDLE_STATE;
            break;
        }
        case SUDOKU_FINISHED:
        {
            delete_grid(1);
            delete_grid(2);
            lv_label_set_text(gen_label, "SUDOKU SOLVED");
            state = IDLE_STATE;
            break;
        }
        default:
            break;
        }
    }
}

static void grid_btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    auto btnClass = (Button *)lv_event_get_user_data(e);
    if (!btnClass)
        return;
    switch (code)
    {
    case LV_EVENT_PRESSED:
    {
        btnClass->onPress();
        break;
    }
    case LV_EVENT_RELEASED:
    {
        btnClass->onRelease();
        break;
    }
    case LV_EVENT_LONG_PRESSED:
    {
        btnClass->onLongPress();
        break;
    }
    default:
        break;
    }
}

static void init_styles()
{
    lv_style_init(&style_btn1);
    lv_style_init(&style_btn2);
    lv_style_init(&style_btn3);
    lv_style_init(&style_btn4);
    lv_style_set_bg_color(&style_btn1, lv_color_make(128, 0, 75));
    lv_style_set_bg_color(&style_btn2, lv_color_make(0, 128, 75));
    lv_style_set_bg_color(&style_btn3, lv_color_make(0, 0, 188));
    lv_style_set_bg_color(&style_btn4, lv_color_make(5, 108, 5));

    lv_style_init(&style_cont);
    lv_style_set_pad_row(&style_cont, 2);
    lv_style_set_pad_column(&style_cont, 2);
    lv_style_set_size(&style_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    gen_label = lv_label_create(lv_screen_active());

    lv_label_set_text(gen_label, "Welcome to SUDOKU");
    lv_obj_set_size(gen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(gen_label);
}

static void init_grid()
{
    bsp_display_lock(10);
    static int32_t col_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    grid = lv_obj_create(lv_screen_active());
    lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);
    lv_obj_set_size(grid, 600, 600);
    vTaskDelay(10);
    lv_obj_add_style(grid, &style_cont, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(grid, LV_COLOR_MAKE(0, 0, 100), LV_STATE_DEFAULT);

    uint32_t i;
    for (i = 0; i < 81; i++)
    {
        uint8_t col = i % 9;
        uint8_t row = i / 9;

        auto btn = new Grid(col, row);
        btn->init(grid_btn_event_cb, grid);
        auto val = board->getValue(row, col);
        // printf("\t x:%d, \ty:%d, \tval:%d\n", row, col, val);
        if (val)
        {
            btn->style(&style_btn4);
            btn->callback(NULL);
        }
        else
        {
            btn->style(&style_btn3);
        }
        auto str = std::to_string(val).c_str();

        btn->gridPos(col, row, str);
        big_grid_vector.push_back(btn);
    }

    bsp_display_unlock();
}

static void helper_grid()
{
    bsp_display_lock(10);
    static int32_t col_dsc[] = {60, 60, 60, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    mini_grid = lv_obj_create(lv_screen_active());
    lv_obj_set_grid_dsc_array(mini_grid, col_dsc, row_dsc);
    lv_obj_set_layout(mini_grid, LV_LAYOUT_GRID);

    lv_obj_add_style(gen_label, &style_cont, LV_STATE_DEFAULT);
    lv_obj_add_style(mini_grid, &style_cont, LV_STATE_DEFAULT);
    lv_obj_set_align(mini_grid, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_size(mini_grid, 250, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(mini_grid, LV_COLOR_MAKE(0, 0, 100), LV_STATE_DEFAULT);

    uint32_t i;
    for (i = 0; i < 9; i++)
    {
        uint8_t col = i % 3;
        uint8_t row = i / 3;
        if (board->countNumber(i + 1) == 9)
        {
            continue;
        }

        auto btn = new MiniGrid(col, row);
        btn->init(grid_btn_event_cb, mini_grid);
        btn->style(&style_btn2);
        auto str = std::to_string((int)(i + 1)).c_str();

        btn->gridPos(col, row, str);
        small_grid_vector.push_back(btn);
    }
    clr = new Clear(0, 3);
    clr->init(grid_btn_event_cb, mini_grid);
    clr->style(&style_btn2);
    clr->col_span = 3;
    clr->gridPos(0, 3, "Clear");

    bsp_display_unlock();
}

void Grid::onPress()
{
    if (state == IDLE_STATE)
    {
        delete_grid(2);
        helper_grid();
        selected = this;
    }
}

void MiniGrid::onPress()
{
    int num = (_row + 0) * 3 + _col + 1;
    auto val = board->check(selected->_row, selected->_col, num);
    selected->label(std::to_string(num).c_str());
    board->set(selected->_row, selected->_col, num);

    if (val || !check_value)
    {
        selected->style(&style_btn2);
    }
    else
    {
        selected->style(&style_btn1);
    }

    if (board->finished())
        state = SUDOKU_FINISHED;

    delete_grid(2);
    helper_grid();
}

void Start::onPress()
{
    state = GENERATE_GRID;
}

void Clear::onPress()
{
    selected->label("0");
    board->set(selected->_row, selected->_col, 0);
    selected->style(&style_btn3);

    delete_grid(2);
    helper_grid();
}
