#include "ui_minrf_port.h"
#include "MinRf.h"

bool ui_get_cc1101_ready(void)      { return (cc1101_init_flag == RADIOLIB_ERR_NONE); }
bool ui_get_lr1121_ready(void)      { return (lr1121_init_flag == RADIOLIB_ERR_NONE); }
bool ui_get_nrf24_ready(void)       { return (nrf24_init_flag == RADIOLIB_ERR_NONE); }
bool ui_get_st32r2916_ready(void)   { return st32r2916_init_flag; }

uint32_t ui_get_tick(void)
{
    return millis();
}

// CC1101
float ui_cc1101_get_freq(void)   { return 868.0; }
float ui_cc1101_get_bw(void)     { return 85.0; }
int ui_cc1101_get_power(void)    { return 22; }
int ui_cc1101_get_interval(void) { return 1; }
void ui_cc1101_init(void) { cc1101_init(); }
void ui_cc1101_send(void) 
{ 
    if(xSemaphoreTake(radioLock, portMAX_DELAY) == pdTRUE) {
        cc1101_loop();
        xSemaphoreGive(radioLock);
    }
}

// LR1121
float ui_lr1121_get_freq(void)   { return 868.0; }
float ui_lr1121_get_bw(void)     { return 85.0; }
int ui_lr1121_get_power(void)    { return 22; }
int ui_lr1121_get_interval(void) { return 1; }
void ui_lr1121_init(void) { lr1121_init(); }
void ui_lr1121_send(char *text) 
{  
    if(xSemaphoreTake(radioLock, portMAX_DELAY) == pdTRUE) {
        lr1121_send(text);
        xSemaphoreGive(radioLock);
    }
}
bool ui_lr1121_get_mode(void) { return lr1121_get_mode(); }     // 0: send ; 1: recv
void ui_lr1121_set_mode(bool mode) { lr1121_set_mode(mode); }

// NRF24
float ui_nfr24_get_freq(void)   { return 868.0; }
float ui_nfr24_get_bw(void)     { return 85.0; }
int ui_nfr24_get_power(void)    { return 22; }
int ui_nfr24_get_interval(void) { return 1; }
void ui_nfr24_init(void) { nfr24_init(); }
void ui_nfr24_send(void) 
{ 
    if(xSemaphoreTake(radioLock, portMAX_DELAY) == pdTRUE) {
        nrf24_loop();
        xSemaphoreGive(radioLock);
    }
}

// ST32R2916