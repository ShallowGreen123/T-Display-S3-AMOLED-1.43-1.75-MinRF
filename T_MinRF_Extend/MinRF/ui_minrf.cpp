

#include "ui_minrf.h"
#include "ui_minrf_port.h"
#include "ui_scr_mrg.h"
#include "assets/assets.h"
#include "Arduino.h"

// extern
extern String lr1121_rssi;
extern String lr1121_snr;
extern String lr1121_payload;


uint32_t EMBED_COLOR_BG         = 0x161823;  // UI 的背景色
uint32_t EMBED_COLOR_FOCUS_ON   = 0x91B821;  // 组件选中时的颜色
uint32_t EMBED_COLOR_TEXT       = 0xffffff;  // 文本的颜色
uint32_t EMBED_COLOR_BORDER     = 0xBBBBBB;  // 一些组件边框的颜色
uint32_t EMBED_COLOR_PROMPT_BG  = 0xfffee6;  // 提示弹窗的背景色
uint32_t EMBED_COLOR_PROMPT_TXT = 0x1e1e00;  // 提示弹窗的文本色

void scr_back_btn_create(lv_obj_t *parent, lv_event_cb_t cb)
{
    lv_obj_t *back_btn = lv_btn_create(parent);
    lv_obj_set_size(back_btn, 86, 6);
    lv_obj_set_style_border_width(back_btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(back_btn, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(back_btn, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(back_btn, 3, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(back_btn, lv_color_hex(0x49a6fd), LV_STATE_FOCUS_KEY);
    lv_obj_set_style_bg_color(back_btn, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_PART_MAIN);
    lv_obj_set_ext_click_area(back_btn, 20);
    lv_obj_add_event_cb(back_btn, cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(back_btn, cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(back_btn, cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 5, -22);
}

//************************************[ screen 0 ]****************************************** menu
#if 1
static void scr0_btn_event_cb(lv_event_t * e)
{
    scr_mgr_switch((int)e->user_data, false);
}

void Button_create(lv_obj_t *parent, const char *text, int id, bool state)
{
    lv_obj_t * ui_Button1 = lv_btn_create(parent);
    lv_obj_set_width(ui_Button1, 250);
    lv_obj_set_height(ui_Button1, 50);
    lv_obj_set_x(ui_Button1, -2);
    lv_obj_set_y(ui_Button1, -10);
    lv_obj_set_align(ui_Button1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    // lv_obj_set_style_bg_color(ui_Button1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(ui_Button1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Button1, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_Button1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_Button1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_Button1, 10, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(ui_Button1, lv_color_hex(0x00ff00), LV_STATE_FOCUS_KEY);
    lv_obj_add_event_cb(ui_Button1, scr0_btn_event_cb, LV_EVENT_CLICKED, (void *)id);

    if(!state) {
        lv_obj_set_style_bg_color(ui_Button1, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    lv_obj_t *lab = lv_label_create(ui_Button1);
    lv_obj_set_style_text_font(lab, &Font_Mono_Bold_25, 0);
    lv_label_set_text(lab, text);
    lv_obj_center(lab);
}

static void create0(lv_obj_t *parent)
{
    lv_obj_t * ui_Panel1 = lv_obj_create(parent);
    lv_obj_set_width(ui_Panel1, 300);
    lv_obj_set_height(ui_Panel1, 393);
    lv_obj_set_align(ui_Panel1, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_Panel1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_Panel1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_Panel1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_Panel1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_Panel1, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_Panel1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Panel1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    char buf[32];
    lv_snprintf(buf, 32, "CC1101 %s", ui_get_cc1101_ready()? "PASS" : "ERROR");
    Button_create(ui_Panel1, buf, SCREEN1_ID, ui_get_cc1101_ready());

    lv_snprintf(buf, 32, "LR1121 %s", ui_get_lr1121_ready()? "PASS" : "ERROR");
    Button_create(ui_Panel1, buf, SCREEN2_ID, ui_get_lr1121_ready());

    lv_snprintf(buf, 32, "NRF24 %s", ui_get_nrf24_ready()? "PASS" : "ERROR");
    Button_create(ui_Panel1, buf, SCREEN3_ID, ui_get_nrf24_ready());

    lv_snprintf(buf, 32, "ST32R2916 %s", ui_get_st32r2916_ready()? "PASS" : "ERROR");
    Button_create(ui_Panel1, buf, SCREEN4_ID, ui_get_st32r2916_ready());
}

static void entry0(void) {
    // ui_batt_power_off();
}
static void exit0(void) {
}
static void destroy0(void) { 

}

static scr_lifecycle_t screen0 = {
    .create = create0,
    .entry = entry0,
    .exit  = exit0,
    .destroy = destroy0,
};
#endif
//************************************[ screen 1 ]****************************************** CC1101
#if 1
lv_timer_t *scr1_timer = NULL;
lv_obj_t *scr1_lab = NULL;
char *scr1_buf = NULL;
bool cc1101_mode = 1;

static void scr1_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        scr_mgr_switch(SCREEN0_ID, false);
    }
}

static void cc1101_mode_switch(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        cc1101_mode = !cc1101_mode;
    }
}

char * cc1101_send(const char *text)
{
    lv_snprintf(scr1_buf, 1024,
                            "             CC1101 SEND            \n"
                            " ---------------------------------- \n"
                            "   Freq:%0.0fM       BW:%0.0fK      \n"
                            "------------------------------------\n"
                            "   Power:%d        Interval:%02d     \n"
                            "------------------------------------\n"
                            "   Send: %s\n"
                            "------------------------------------\n", 
                            ui_cc1101_get_freq(), ui_cc1101_get_bw(),
                            ui_cc1101_get_power(), ui_cc1101_get_interval(),
                            text);
    // printf("%f  %f  %d  %d\n", ui_cc1101_get_freq(), ui_cc1101_get_bw(),ui_cc1101_get_power(), ui_cc1101_get_interval());
    return scr1_buf;
}

char *cc1101_recv(void)
{
    lv_snprintf(scr1_buf, 1024,
                            "            CC1101 RECV             \n"
                            " ---------------------------------- \n"
                            "   Freq:%0.0fM       BW:%0.0fK      \n"
                            "------------------------------------\n"
                            "   Power:%02d        RSSI:%02d  \n"
                            "------------------------------------\n"
                            "   Recv: \n"
                            "------------------------------------\n", 
                            ui_cc1101_get_freq(), ui_cc1101_get_bw(),
                            ui_cc1101_get_power(), ui_cc1101_get_interval());

    return scr1_buf;
}

void rf_cc1101_timer(lv_timer_t *t)
{
    if(cc1101_mode)
    {
        static int cnt = 0;
        char buf[30];
        lv_snprintf(buf, 30, "[%d] TX num %d", ui_get_tick() / 1000, cnt++);
        lv_label_set_text_fmt(scr1_lab, "%s", cc1101_send(buf));
    } else {
        lv_label_set_text_fmt(scr1_lab, "%s", cc1101_recv());
    }
}

static void create1(lv_obj_t *parent)
{
    scr1_buf = (char *)heap_caps_malloc(1024, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    scr1_lab = lv_label_create(parent);
    lv_obj_set_style_text_font(scr1_lab, &Font_Mono_Bold_20, 0);
    lv_label_set_text_fmt(scr1_lab, "%s", cc1101_send("hello"));
    lv_obj_align(scr1_lab, LV_ALIGN_CENTER, 0, -20);
    lv_obj_center(scr1_lab);

    lv_obj_t *ui_Button1 = lv_btn_create(parent);
    lv_obj_set_size(ui_Button1, 89, 50);
    lv_obj_align(ui_Button1, LV_ALIGN_BOTTOM_MID, -100, -80);
    lv_obj_add_flag(ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_shadow_width(ui_Button1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_Button1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_Button1, cc1101_mode_switch, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lab1 = lv_label_create(ui_Button1);
    lv_obj_set_style_text_font(lab1, &Font_Mono_Bold_20, 0);
    lv_label_set_text(lab1, "SWITCH");
    lv_obj_center(lab1);

    lv_obj_t *ui_Button2 = lv_btn_create(parent);
    lv_obj_set_size(ui_Button2, 89, 50);
    lv_obj_align(ui_Button2, LV_ALIGN_BOTTOM_MID, 100, -80);
    lv_obj_add_flag(ui_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_shadow_width(ui_Button2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_Button2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t *lab2 = lv_label_create(ui_Button2);
    lv_obj_set_style_text_font(lab2, &Font_Mono_Bold_20, 0);
    lv_label_set_text(lab2, "CONFIG");
    lv_obj_center(lab2);

    scr_back_btn_create(parent, scr1_btn_event_cb);
}

static void entry1(void) {
    scr1_timer = lv_timer_create(rf_cc1101_timer, ui_cc1101_get_interval() * 1000, NULL);
}
static void exit1(void) {
    if(scr1_timer) {
        lv_timer_del(scr1_timer);
        scr1_timer = NULL;
    }
}
static void destroy1(void) { 
    if(scr1_buf) {
        heap_caps_free(scr1_buf);
        scr1_buf = NULL;
    }
}

static scr_lifecycle_t screen1 = {
    .create = create1,
    .entry = entry1,
    .exit  = exit1,
    .destroy = destroy1,
};
#endif
//************************************[ screen 2 ]****************************************** LR1121
#if 1
static lv_timer_t *scr2_timer = NULL;
static lv_obj_t *scr2_lab = NULL;
static char *scr2_buf = NULL;

static void scr2_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        scr_mgr_switch(SCREEN0_ID, false);
    }
}

static void lr1121_mode_switch(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        bool mode = !ui_lr1121_get_mode();
        ui_lr1121_set_mode(mode);
    }
}

char * scr2_lr1121_send_info(char *text)
{
    lv_snprintf(scr2_buf, 1024,
                            "             LR1121 SEND            \n"
                            " ---------------------------------- \n"
                            "   Freq:%0.0fM       BW:%0.0fK      \n"
                            "------------------------------------\n"
                            "   Power:%02d        Interval:%02d  \n"
                            "------------------------------------\n"
                            "   Send: %s\n"
                            "------------------------------------\n", 
                            ui_lr1121_get_freq(), ui_lr1121_get_bw(),
                            ui_lr1121_get_power(), ui_lr1121_get_interval(),
                            text);

    
    return scr2_buf;
}

char *scr2_lr1121_recv_info(void)
{
    lv_snprintf(scr2_buf, 1024,
                            "             LR1121 RECV            \n"
                            " ---------------------------------- \n"
                            "   Freq:%0.0fM       BW:%0.0fK      \n"
                            "------------------------------------\n"
                            "   Power:%02d        RSSI:%s  \n"
                            "------------------------------------\n"
                            "   Recv: %s\n"
                            "------------------------------------\n",
                            ui_lr1121_get_freq(), ui_lr1121_get_bw(),
                            ui_lr1121_get_power(), lr1121_rssi.c_str(),
                            lr1121_payload.c_str());
    return scr2_buf;
}

void rf_lr1121_timer(lv_timer_t *t)
{

    if(!ui_lr1121_get_mode())
    {
        static int cnt = 0;
        char buf[30];
        lv_snprintf(buf, 30, "[%d] LR1121 %d", ui_get_tick() / 1000, cnt++);
        ui_lr1121_send(buf);
        lv_label_set_text_fmt(scr2_lab, "%s", scr2_lr1121_send_info(buf));
    } else {
        lv_label_set_text_fmt(scr2_lab, "%s", scr2_lr1121_recv_info());
    }
}

static void create2(lv_obj_t *parent)
{
    scr2_buf = (char *)heap_caps_malloc(1024, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    lv_obj_t *ui_Button1 = lv_btn_create(parent);
    lv_obj_set_size(ui_Button1, 89, 50);
    lv_obj_align(ui_Button1, LV_ALIGN_BOTTOM_MID, -100, -80);
    lv_obj_add_flag(ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_shadow_width(ui_Button1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_Button1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_Button1, lr1121_mode_switch, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lab1 = lv_label_create(ui_Button1);
    lv_obj_set_style_text_font(lab1, &Font_Mono_Bold_20, 0);
    lv_label_set_text(lab1, "SWITCH");
    lv_obj_center(lab1);

    lv_obj_t *ui_Button2 = lv_btn_create(parent);
    lv_obj_set_size(ui_Button2, 89, 50);
    lv_obj_align(ui_Button2, LV_ALIGN_BOTTOM_MID, 100, -80);
    lv_obj_add_flag(ui_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_shadow_width(ui_Button2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_Button2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t *lab2 = lv_label_create(ui_Button2);
    lv_obj_set_style_text_font(lab2, &Font_Mono_Bold_20, 0);
    lv_label_set_text(lab2, "CONFIG");
    lv_obj_center(lab2);

    scr2_lab = lv_label_create(parent);
    lv_obj_set_style_text_font(scr2_lab, &Font_Mono_Bold_20, 0);
    lv_label_set_text_fmt(scr2_lab, "%s", scr2_lr1121_send_info("L1121"));
    lv_obj_align(scr2_lab, LV_ALIGN_CENTER, 0, -20);

    scr_back_btn_create(parent, scr2_btn_event_cb);
}

static void entry2(void) {
    scr2_timer = lv_timer_create(rf_lr1121_timer, ui_nfr24_get_interval() * 1000, NULL);

    ui_lr1121_init();
}
static void exit2(void) {
    if(scr2_timer) {
        lv_timer_del(scr2_timer);
        scr2_timer = NULL;
    }
}
static void destroy2(void) { 
    if(scr2_buf) {
        heap_caps_free(scr2_buf);
        scr2_buf = NULL;
    }
}

static scr_lifecycle_t screen2 = {
    .create = create2,
    .entry = entry2,
    .exit  = exit2,
    .destroy = destroy2,
};
#endif
//************************************[ screen 3 ]****************************************** NRF24
#if 1

static lv_timer_t *scr3_timer = NULL;
static lv_obj_t *scr3_lab = NULL;
static char *scr3_buf = NULL;
static bool nrf24_mode = 1;

static void scr3_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        scr_mgr_switch(SCREEN0_ID, false);
    }
}

static void nrf24_mode_switch(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        nrf24_mode = !nrf24_mode;
    }
}

char * nrf24_send(char *text)
{
    lv_snprintf(scr3_buf, 1024,
                            "             NRF24 SEND             \n"
                            " ---------------------------------- \n"
                            "   Freq:%0.0fM       BW:%0.0fK      \n"
                            "------------------------------------\n"
                            "   Power:%02d        Interval:%02d  \n"
                            "------------------------------------\n"
                            "   Send: %s\n"
                            "------------------------------------\n", 
                            ui_nfr24_get_freq(), ui_nfr24_get_bw(),
                            ui_nfr24_get_power(), ui_nfr24_get_interval(),
                            text);

    
    return scr3_buf;
}

char *nrf24_recv(void)
{
    lv_snprintf(scr3_buf, 1024,
                            "             NRF24 RECV             \n"
                            " ---------------------------------- \n"
                            "   Freq:%0.0fM       BW:%0.0fK      \n"
                            "------------------------------------\n"
                            "   Power:%02d        RSSI:%02d  \n"
                            "------------------------------------\n"
                            "   Recv: \n"
                            "------------------------------------\n", 
                            ui_nfr24_get_freq(), ui_nfr24_get_bw(),
                            ui_nfr24_get_power(), ui_nfr24_get_interval());

    
    return scr3_buf;
}

void rf_nrf24_timer(lv_timer_t *t)
{
    if(nrf24_mode)
    {
        static int cnt = 0;
        char buf[30];
        lv_snprintf(buf, 30, "[%d] TX num %d", ui_get_tick() / 1000, cnt++);
        ui_nfr24_send();
        lv_label_set_text_fmt(scr3_lab, "%s", nrf24_send(buf));
    } else {
        lv_label_set_text_fmt(scr3_lab, "%s", nrf24_recv());
    }
}

static void create3(lv_obj_t *parent)
{
    scr3_buf = (char *)heap_caps_malloc(1024, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    lv_obj_t *ui_Button1 = lv_btn_create(parent);
    lv_obj_set_size(ui_Button1, 89, 50);
    lv_obj_align(ui_Button1, LV_ALIGN_BOTTOM_MID, -100, -80);
    lv_obj_add_flag(ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_shadow_width(ui_Button1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_Button1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_Button1, nrf24_mode_switch, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lab1 = lv_label_create(ui_Button1);
    lv_obj_set_style_text_font(lab1, &Font_Mono_Bold_20, 0);
    lv_label_set_text(lab1, "SWITCH");
    lv_obj_center(lab1);

    lv_obj_t *ui_Button2 = lv_btn_create(parent);
    lv_obj_set_size(ui_Button2, 89, 50);
    lv_obj_align(ui_Button2, LV_ALIGN_BOTTOM_MID, 100, -80);
    lv_obj_add_flag(ui_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_shadow_width(ui_Button2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_Button2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t *lab2 = lv_label_create(ui_Button2);
    lv_obj_set_style_text_font(lab2, &Font_Mono_Bold_20, 0);
    lv_label_set_text(lab2, "CONFIG");
    lv_obj_center(lab2);

    scr3_lab = lv_label_create(parent);
    lv_obj_set_style_text_font(scr3_lab, &Font_Mono_Bold_20, 0);
    lv_label_set_text_fmt(scr3_lab, "%s", nrf24_send("NRF24"));
    lv_obj_align(scr3_lab, LV_ALIGN_CENTER, 0, -20);

    scr_back_btn_create(parent, scr3_btn_event_cb);
}

static void entry3(void) {
    scr3_timer = lv_timer_create(rf_nrf24_timer, ui_nfr24_get_interval() * 1000, NULL);

    ui_nfr24_init();
}
static void exit3(void) {
    if(scr3_timer) {
        lv_timer_del(scr3_timer);
        scr3_timer = NULL;
    }
}
static void destroy3(void) { 

}

static scr_lifecycle_t screen3 = {
    .create = create3,
    .entry = entry3,
    .exit  = exit3,
    .destroy = destroy3,
};
#endif
//************************************[ screen 4 ]****************************************** ST32R2916
#if 1

lv_timer_t *scr4_timer = NULL;

static void scr4_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        scr_mgr_switch(SCREEN0_ID, false);
    }
}

static void create4(lv_obj_t *parent)
{
    scr_back_btn_create(parent, scr4_btn_event_cb);
}

static void entry4(void) {
    // scr4_timer = lv_timer_create(rf_timer_event_cb, 3000, NULL);
}
static void exit4(void) {
    if(scr4_timer) {
        lv_timer_del(scr4_timer);
        scr4_timer = NULL;
    }
}
static void destroy4(void) { 

}

static scr_lifecycle_t screen4 = {
    .create = create4,
    .entry = entry4,
    .exit  = exit4,
    .destroy = destroy4,
};
#endif

//************************************[ UI ENTRY ]******************************************
void ui_minrf_entry(void)
{
    // lv_disp_t *disp = lv_disp_get_default();
    // disp->theme = lv_theme_mono_init(disp, false, LV_FONT_DEFAULT);

    scr_mgr_init();
    // scr_mgr_set_bg_color(EPD_COLOR_BG);
    scr_mgr_register(SCREEN0_ID,   &screen0);    // menu
    scr_mgr_register(SCREEN1_ID,   &screen1);    // menu
    scr_mgr_register(SCREEN2_ID,   &screen2);    // menu
    scr_mgr_register(SCREEN3_ID,   &screen3);    // menu
    scr_mgr_register(SCREEN4_ID,   &screen4);    // menu

    scr_mgr_switch(SCREEN0_ID, false); // set root screen
    scr_mgr_set_anim(LV_SCR_LOAD_ANIM_OVER_LEFT, LV_SCR_LOAD_ANIM_OVER_LEFT, LV_SCR_LOAD_ANIM_OVER_LEFT);
}
