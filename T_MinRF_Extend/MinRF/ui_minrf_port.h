/************************************************************************
 * FilePath     : ui_base.h
 * Author       : GX.Duan
 * LastEditors  : ShallowGreen123 2608653986@qq.com
 * Copyright (c): 2022 by GX.Duan, All Rights Reserved.
 * Github       : https://github.com/ShallowGreen123/lvgl_examples.git
 ************************************************************************/
#ifndef __UI_PORT_MINFR_H__
#define __UI_PORT_MINFR_H__

/*********************************************************************************
 *                                  INCLUDES
 * *******************************************************************************/
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif
/*********************************************************************************
 *                                   DEFINES
 * *******************************************************************************/

/*********************************************************************************
 *                                   MACROS
 * *******************************************************************************/
enum {
    SCREEN0_ID = 0,
    SCREEN1_ID,
    SCREEN1_1_ID,
    SCREEN1_2_ID,
    SCREEN2_ID,
    SCREEN2_1_ID,
    SCREEN3_ID,
    SCREEN4_ID,
    SCREEN4_1_ID,
    SCREEN4_2_ID,
    SCREEN5_ID,
    SCREEN6_ID,
    SCREEN6_1_ID,
    SCREEN6_2_ID,
    SCREEN7_ID,
    SCREEN8_ID,
    SCREEN8_1_ID,
    SCREEN8_2_ID,
    SCREEN9_ID,
    SCREEN10_ID,
};
/*********************************************************************************
 *                                  TYPEDEFS
 * *******************************************************************************/

/*********************************************************************************
 *                              GLOBAL PROTOTYPES
 * *******************************************************************************/
bool ui_get_cc1101_ready(void);
bool ui_get_lr1121_ready(void);
bool ui_get_nrf24_ready(void); 
bool ui_get_st32r2916_ready(void);

uint32_t ui_get_tick(void);

// CC1101
float ui_cc1101_get_freq(void);
float ui_cc1101_get_bw(void);
int ui_cc1101_get_power(void);
int ui_cc1101_get_interval(void);
void ui_cc1101_init(void);
void ui_cc1101_send(void);

// LR1121
float ui_lr1121_get_freq(void);
float ui_lr1121_get_bw(void);
int ui_lr1121_get_power(void);
int ui_lr1121_get_interval(void);
void ui_lr1121_init(void);
void ui_lr1121_send(char *text);
bool ui_lr1121_get_mode(void);
void ui_lr1121_set_mode(bool mode);

// NRF24
float ui_nfr24_get_freq(void);
float ui_nfr24_get_bw(void);
int ui_nfr24_get_power(void);
int ui_nfr24_get_interval(void);
void ui_nfr24_init(void);
void ui_nfr24_send(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif
#endif /* __UI_PORT_MINFR_H__ */
