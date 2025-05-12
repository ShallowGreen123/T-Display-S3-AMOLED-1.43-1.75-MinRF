/*
 * @Description: GFX屏幕显示+触摸切换图片
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-09-06 10:58:19
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2025-05-08 11:22:09
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "TouchDrvCST92xx.h"
#include "lvgl.h"

static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;

volatile int8_t IIC_Interrupt_Flag;
TouchDrvCST92xx CST9217;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
    LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

// DO0143FAT01
// Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST /* RST */,
//                                       0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);

// H0175Y003AM
Arduino_GFX *gfx = new Arduino_CO5300(bus, LCD_RST /* RST */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT,
                                      6 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

#define SCREEN_SIZE 466
LV_IMG_DECLARE(_biaopan_466x466);
LV_IMG_DECLARE(_scan2_alpha_116x200);
LV_IMG_DECLARE(_wujiaoxin2_alpha_20x20);
lv_obj_t *scr;
lv_obj_t *wujiaoxin[7];
void dragon_ball();
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

    lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    if (IIC_Interrupt_Flag == true)
    {
        IIC_Interrupt_Flag = false;

        int16_t touch_x[5];
        int16_t touch_y[5];
        uint8_t fingers_number = 0;

        if (CST9217.getPoint(touch_x, touch_y, 2) == true)
        {
            touch_x[0] = LCD_WIDTH - touch_x[0];
            touch_y[0] = LCD_HEIGHT - touch_y[0];

            data->state = LV_INDEV_STATE_PR;

            /*Set the coordinates*/
            data->point.x = touch_x[0];
            data->point.y = touch_y[0];
        }
        else
        {
            data->state = LV_INDEV_STATE_REL;
        }
    }
}

void my_rounder_cb(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    if (area->x1 % 2 != 0)
        area->x1 += 1;
    if (area->y1 % 2 != 0)
        area->y1 += 1;

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    if (w % 2 != 0)
        area->x2 -= 1;
    if (h % 2 != 0)
        area->y2 -= 1;
}

void lvgl_initialization(void)
{
    lv_init();

    lv_color_t *buf_1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LCD_WIDTH * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    lv_color_t *buf_2 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LCD_WIDTH * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    while ((!buf_1) || (!buf_2))
    {
        Serial.println("LVGL disp_draw_buf allocate failed!");
        delay(1000);
    }

    lv_disp_draw_buf_init(&draw_buf, buf_1, buf_2, LCD_WIDTH * 40);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.rounder_cb = my_rounder_cb;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 1; // 双缓冲全像素刷新
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(LCD_EN, OUTPUT);
    digitalWrite(LCD_EN, HIGH);
    attachInterrupt(TP_INT, []()
                    { IIC_Interrupt_Flag = true; }, FALLING);
    CST9217.jumpCheck();

    CST9217.setPins(-1, TP_INT);
    if (CST9217.begin(Wire, 0x5A, IIC_SDA, IIC_SCL) == false)
    {
        Serial.println("CST9217 initialization failed");
    }
    else
    {
        Serial.print("Model :");
        Serial.println(CST9217.getModelName());
    }

    gfx->begin(120000000);
    gfx->fillScreen(WHITE);

    lvgl_initialization();

    for (int i = 0; i <= 255; i++)
    {
        gfx->Display_Brightness(i);
        delay(3);
    }

    delay(800);
    dragon_ball();
}

void loop()
{
    lv_task_handler();
    delay(5);
}

void rotate_scan_line(void *obj, int32_t angle)
{
    lv_img_set_pivot((lv_obj_t *)obj, 58, 0);
    lv_img_set_angle((lv_obj_t *)obj, angle);

    uint8_t r = 200;
    angle = (angle / 10);
    int32_t x = r * cos(angle * (M_PI / 180));
    int32_t y = r * sin(angle * (M_PI / 180));

    static long time = millis();
    static uint8_t wujiaoxin_num = 0;
    if (millis() - time > 5000)
    {
        time = millis();
        wujiaoxin_num++;
        Serial.printf("x:%d,y:%d\n", x, y);

        if (wujiaoxin_num >= 7)
        {
            wujiaoxin_num = 0;
        }
        if (x > 0 && y > 0)
        {
            lv_obj_set_pos(wujiaoxin[wujiaoxin_num], random(50, 200), random(240, 400));
        }
        else if (x < 0 && y > 0)
        {
            lv_obj_set_pos(wujiaoxin[wujiaoxin_num], random(50, 200), random(50, 200));
        }
        else if (x < 0 && y < 0)
        {
            lv_obj_set_pos(wujiaoxin[wujiaoxin_num], random(200, 400), random(50, 200));
        }
        else if (x > 0 && y < 0)
        {
            lv_obj_set_pos(wujiaoxin[wujiaoxin_num], random(200 , 400), random(200,400));
        }
    }
}

void dragon_ball()
{
    scr = lv_scr_act();
    // lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_img_src(scr, &_biaopan_466x466, 0);

    for (int i = 0; i < 7; i++)
    {
        wujiaoxin[i] = lv_img_create(scr);
        lv_obj_set_size(wujiaoxin[i], 20, 20);
        lv_obj_set_pos(wujiaoxin[i], 0,0);
        lv_img_set_src(wujiaoxin[i], &_wujiaoxin2_alpha_20x20);
    }

    lv_obj_t *scan_lien = lv_img_create(scr);
    lv_obj_set_size(scan_lien, 116, 200);
    lv_obj_set_pos(scan_lien, (SCREEN_SIZE -116)/2, SCREEN_SIZE/2);
    lv_img_set_src(scan_lien, &_scan2_alpha_116x200);
    lv_img_set_pivot(scan_lien, 0, 0);
    lv_img_set_angle(scan_lien, 0);

    lv_anim_t rotation_scan_lien;
    lv_anim_init(&rotation_scan_lien);
    lv_anim_set_var(&rotation_scan_lien, scan_lien);
    lv_anim_set_values(&rotation_scan_lien, 0, 3600);
    lv_anim_set_time(&rotation_scan_lien, 12000);
    lv_anim_set_exec_cb(&rotation_scan_lien, rotate_scan_line);
    lv_anim_set_repeat_count(&rotation_scan_lien, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&rotation_scan_lien);

    lv_obj_update_layout(scr);
}