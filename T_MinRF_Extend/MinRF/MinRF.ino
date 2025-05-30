/*
 * @Description: lvgl benchmark
 * @Author: LILYGO_L
 * @Date: 2024-09-05 08:59:53
 * @LastEditTime: 2025-03-05 18:06:08
 * @License: GPL 3.0
 */
#include "lvgl.h"
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include "TouchDrvCST92xx.h"
#include "SensorWireHelper.h"
#include "MinRf.h"
#include "ui_minrf.h"
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "app_nfc.h"
#include <OneButton.h>
#include "ui_scr_mrg.h"
#include "ui_minrf_port.h"

// extern void ble_init();

int rf_select = 0;
void rf_task_cb(void *parm);
void rf_timer_event_cb(lv_timer_t *t);
SPIClass rfspi(HSPI);
// SPIClass *spi = new SPIClass(FSPI);

RfalRfST25R3916Class nfc_hw(&rfspi, ST25R3916_BSS, ST25R3916_IQR);
RfalNfcClass NFCReader(&nfc_hw);

int cc1101_init_flag = -1;
int lr1121_init_flag = -1;
int nrf24_init_flag = -1;
int st32r2916_init_flag = false;

TaskHandle_t rf_handle;
SemaphoreHandle_t radioLock;

static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;

volatile int8_t IIC_Interrupt_Flag;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
    LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

#if defined DO0143FAT01
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST /* RST */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);
#elif defined H0175Y003AM || defined DO0143FMST10
Arduino_GFX *gfx = new Arduino_CO5300(bus, LCD_RST /* RST */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT,
                                      6 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);
#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif

class Button{
private:
  OneButton button;
  int value;
  int double_value;
public:
  explicit Button(uint8_t pin):button(pin) {
    button.attachClick([](void *scope) { ((Button *) scope)->Clicked();}, this);
    button.attachDoubleClick([](void *scope) { ((Button *) scope)->DoubleClicked();}, this);
    button.attachLongPressStart([](void *scope) { ((Button *) scope)->LongPressed();}, this);
  }

  void Clicked(){
    Serial.println("Click then value++");
    value++;
  }

  void DoubleClicked(){

    Serial.println("DoubleClick");
    
  }

  void LongPressed(){
    Serial.println("LongPress and the value is");
    Serial.println(value);
    double_value++;
  }

  int get_click(){
    return value;
  }

  int get_double_key(void){
    return double_value;
  }

  void clear_click() {
    value = 0;
    double_value = 0;
  }

  void handle(){
    button.tick();
  }
};
Button button(0);

TouchDrvCST92xx CST9217;

void notify_callback()
{
    Serial.println("NDEF Detected.");
}

void ndef_event_callback(ndefTypeId id, void*data)
{
    static ndefTypeRtdDeviceInfo   devInfoData;
    static ndefConstBuffer         bufAarString;
    static ndefTypeWifi            wifiConfig;
    static ndefRtdUri              url;
    static ndefRtdText             text;

    switch (id) {
    case NDEF_TYPE_EMPTY:
        break;
    case NDEF_TYPE_RTD_DEVICE_INFO:
        memcpy(&devInfoData, data, sizeof(ndefTypeRtdDeviceInfo));
        break;
    case NDEF_TYPE_RTD_TEXT:
        memcpy(&text, data, sizeof(ndefRtdText));
        Serial.printf("LanguageCode:%s Sentence:%s\n", reinterpret_cast<const char *>(text.bufLanguageCode.buffer), reinterpret_cast<const char *>(text.bufSentence.buffer));
        break;
    case NDEF_TYPE_RTD_URI:
        memcpy(&url, data, sizeof(ndefRtdUri));
        Serial.printf("PROTOCOL:%s URL:%s\n", reinterpret_cast<const char *>(url.bufProtocol.buffer), reinterpret_cast<const char *>(url.bufUriString.buffer));
        break;
    case NDEF_TYPE_RTD_AAR:
        memcpy(&bufAarString, data, sizeof(ndefConstBuffer));
        Serial.printf("NDEF_TYPE_RTD_AAR :%s\n", (char*)bufAarString.buffer);
        break;
    case NDEF_TYPE_MEDIA:
        break;
    case NDEF_TYPE_MEDIA_VCARD:
        break;
    case NDEF_TYPE_MEDIA_WIFI: {

#if 0
        memcpy(&wifiConfig, data, sizeof(ndefTypeWifi));
        // const char *ssid = reinterpret_cast<const char *>(wifiConfig.bufNetworkSSID.buffer);
        // const char *password = reinterpret_cast<const char *>(wifiConfig.bufNetworkKey.buffer);
        char ssid[128] = {0};
        char password[128] = {0};
        memcpy(ssid, wifiConfig.bufNetworkSSID.buffer, wifiConfig.bufNetworkSSID.length);
        memcpy(password, wifiConfig.bufNetworkKey.buffer, wifiConfig.bufNetworkKey.length);

        Serial.printf(">> ssid     :<%s> len:%d\n", reinterpret_cast<const char *>(wifiConfig.bufNetworkSSID.buffer), wifiConfig.bufNetworkSSID.length);
        Serial.printf(">> password :<%s> len:%d\n", reinterpret_cast<const char *>(wifiConfig.bufNetworkKey.buffer), wifiConfig.bufNetworkKey.length);
        Serial.printf("<< ssid     :<%s> len:%u\n", ssid, strlen(ssid));
        Serial.printf("<< password :<%s> len:%u\n", password, strlen(password));
#endif
        ndefTypeWifi * wifi = (ndefTypeWifi*)data;
        std::string ssid(reinterpret_cast<const char *>(wifi->bufNetworkSSID.buffer), wifi->bufNetworkSSID.length);
        std::string password(reinterpret_cast<const char *>(wifi->bufNetworkKey.buffer), wifi->bufNetworkKey.length);
        Serial.printf("ssid:<%s> password:<%s>\n", ssid.c_str(), password.c_str());

        // uint8_t authentication = wifiConfig.authentication;
        // uint8_t encryption = wifiConfig.encryption;
        // Serial.printf("SSID:<%s> PASSWORD:<%s> authentication:<%u> encryption:<%d>\n", ssid, password, authentication, encryption);
    }
    break;
    default:
        break;
    }
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    if(xSemaphoreTake(radioLock, portMAX_DELAY) == pdTRUE){

    #if (LV_COLOR_16_SWAP != 0)
        gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
    #else
        gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
    #endif
        xSemaphoreGive(radioLock);
    }

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

static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;
    extern uint8_t BLE_REMOTE_VAL;

    // uint32_t act_key = BLE_REMOTE_VAL;
    // if(act_key != 0)
    // {
    //     Serial.printf("act_key=%x\n", act_key);
    //     data->state = LV_INDEV_STATE_PR;
    //     switch(act_key) {
    //         case 0x41:
    //             act_key = LV_KEY_NEXT;
    //             break;
    //         case 0x40:
    //             act_key = LV_KEY_PREV;
    //             break;
    //         case 0x7:
    //             act_key = LV_KEY_LEFT;
    //             break;
    //         case 0x6:
    //             act_key = LV_KEY_RIGHT;
    //             break;
    //         case 0x44:
    //             act_key = LV_KEY_ENTER;
    //             break;
    //     }
    //     last_key = act_key;
    //     data->key = last_key;
    //     BLE_REMOTE_VAL = 0;
    // }else {
    //     data->state = LV_INDEV_STATE_REL;
    // }

    int click_key = button.get_click();
    int double_key = button.get_double_key();

    if((click_key != 0) || (double_key != 0))
    {
        if(click_key != 0) 
        {
            data->key = LV_KEY_NEXT;
        } 
        else if(double_key != 0) 
        {
            data->key = LV_KEY_ENTER;
        }

        data->state = LV_INDEV_STATE_PR;
        button.clear_click();
    }else {
        data->state = LV_INDEV_STATE_REL;
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
    // static lv_indev_drv_t indev_drv;
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.type = LV_INDEV_TYPE_POINTER;
    // indev_drv.read_cb = my_touchpad_read;
    // lv_indev_drv_register(&indev_drv);

    static lv_indev_drv_t indev_drv_keypad;
    static lv_indev_t * indev_keypad;
    
    lv_indev_drv_init(&indev_drv_keypad);
    indev_drv_keypad.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv_keypad.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&indev_drv_keypad);
    lv_group_t *keypad_group = lv_group_create();
    lv_indev_set_group(indev_keypad, keypad_group);
    lv_group_set_default(keypad_group);

}

int cc1101_spi_init(void)
{
    pinMode(LR1121_CS, OUTPUT);
    pinMode(LR1121_RST, OUTPUT);
    // pinMode(CC1101_CS, OUTPUT);
    pinMode(NRF24L01_CE, OUTPUT);
    pinMode(NRF24L01_CS, OUTPUT);
    pinMode(ST25R3916_BSS, OUTPUT);
    // pinMode(ST25R3916_EN, OUTPUT);

    digitalWrite(LR1121_CS, HIGH);
    digitalWrite(LR1121_RST, LOW);
    // digitalWrite(CC1101_CS, HIGH);
    digitalWrite(NRF24L01_CS, HIGH);
    digitalWrite(NRF24L01_CE, HIGH);
    digitalWrite(ST25R3916_BSS, HIGH);
    // digitalWrite(ST25R3916_EN, LOW);

    // pinMode(CC1101_CS, OUTPUT);
    // digitalWrite(CC1101_CS, LOW);

    rfspi.end();
    rfspi.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI, CC1101_CS);

    return cc1101_init();
}

int lr1121_spi_init(void)
{
    // pinMode(LR1121_CS, OUTPUT);
    // pinMode(LR1121_RST, OUTPUT);
    pinMode(CC1101_CS, OUTPUT);
    pinMode(NRF24L01_CE, OUTPUT);
    // pinMode(NRF24L01_CS, OUTPUT);
    pinMode(ST25R3916_BSS, OUTPUT);
    // pinMode(ST25R3916_EN, OUTPUT);

    // digitalWrite(LR1121_CS, HIGH);
    // digitalWrite(LR1121_RST, LOW);
    digitalWrite(CC1101_CS, HIGH);
    digitalWrite(NRF24L01_CS, HIGH);
    // digitalWrite(NRF24L01_CE, HIGH);
    digitalWrite(ST25R3916_BSS, HIGH);
    // digitalWrite(ST25R3916_EN, LOW);

    // SPI.setFrequency(100000);
   
    // pinMode(LR1121_SW2, OUTPUT);
    // digitalWrite(LR1121_SW2, HIGH);

    rfspi.end();
    rfspi.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI, LR1121_CS);

    return lr1121_init();
}

int nrf24_spi_init(void)
{
    pinMode(LR1121_CS, OUTPUT);
    pinMode(LR1121_RST, OUTPUT);
    pinMode(CC1101_CS, OUTPUT);
    // pinMode(NRF24L01_CE, OUTPUT);
    // pinMode(NRF24L01_CS, OUTPUT);
    pinMode(ST25R3916_BSS, OUTPUT);
    // pinMode(ST25R3916_EN, OUTPUT);

    digitalWrite(LR1121_CS, HIGH);
    digitalWrite(LR1121_RST, LOW);
    digitalWrite(CC1101_CS, HIGH);
    // digitalWrite(NRF24L01_CS, HIGH);
    // digitalWrite(NRF24L01_CE, HIGH);
    digitalWrite(ST25R3916_BSS, HIGH);
    // digitalWrite(ST25R3916_EN, LOW);

    rfspi.end();
    rfspi.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI, NRF24L01_CS);

    return nrf24_init();
}

bool st32r2916_spi_init(void)
{
    pinMode(LR1121_CS, OUTPUT);
    pinMode(LR1121_RST, OUTPUT);
    pinMode(CC1101_CS, OUTPUT);
    pinMode(NRF24L01_CE, OUTPUT);
    pinMode(NRF24L01_CS, OUTPUT);
    // pinMode(ST25R3916_BSS, OUTPUT);
    // pinMode(ST25R3916_I2C_EN, OUTPUT);

    digitalWrite(LR1121_CS, HIGH);
    digitalWrite(LR1121_RST, LOW);
    digitalWrite(CC1101_CS, HIGH);
    digitalWrite(NRF24L01_CS, HIGH);
    digitalWrite(NRF24L01_CE, HIGH);
    // digitalWrite(ST25R3916_BSS, HIGH);
    // digitalWrite(ST25R3916_I2C_EN, LOW); // Connect to GND (SPI) or VDD_D (I2C)

    pinMode(ST25R3916_IQR, OUTPUT);
    digitalWrite(ST25R3916_IQR, HIGH);

    pinMode(ST25R3916_IQR, INPUT);

    rfspi.end();
    rfspi.begin(ST25R3916_SCK, ST25R3916_MISO, ST25R3916_MOSI, ST25R3916_BSS);

    return beginNFC(notify_callback, ndef_event_callback);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    // ble_init();

    radioLock = xSemaphoreCreateBinary();
    assert(radioLock);
    xSemaphoreGive(radioLock);

    pinMode(LCD_EN, OUTPUT);
    digitalWrite(LCD_EN, HIGH);

    gfx->begin(80000000);
    gfx->fillScreen(BLACK);
    gfx->setRotation(1);

    for (int i = 0; i <= 128; i++)
    {
        gfx->Display_Brightness(i);
        delay(3);
    }

    // xTaskCreate(rf_task_cb, "rf_task_cb", 1024 * 4, NULL, 24, &rf_handle);
    // vTaskSuspend(rf_handle);
    
    lvgl_initialization();

    for(int i = 0; i < 3; i++)
    {
        cc1101_init_flag = cc1101_spi_init();
        if(cc1101_init_flag == RADIOLIB_ERR_NONE) {
            break;
        }
        delay(500);
    }
    for(int i = 0; i < 3; i++)
    {
        lr1121_init_flag = lr1121_spi_init();
        if(lr1121_init_flag == RADIOLIB_ERR_NONE) {
            break;
        }
        delay(500);
    }
    for(int i = 0; i < 3; i++)
    {
        nrf24_init_flag = nrf24_spi_init();
        if(nrf24_init_flag == true) {
            break;
        }
        delay(500);
    }
    for(int i = 0; i < 3; i++)
    {
        st32r2916_init_flag = st32r2916_spi_init();
        if(st32r2916_init_flag == true) {
            break;
        }
        delay(500);
    }
    ui_minrf_entry();

    
}

uint32_t tick = millis();

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(1);
    button.handle();

    if(scr_mgr_get_top_id() == SCREEN2_ID){     // LR1121
        if(xSemaphoreTake(radioLock, portMAX_DELAY) == pdTRUE) {
            lr1121_recv();
            xSemaphoreGive(radioLock);
        }
    }

    // if(scr_mgr_get_top_id() == SCREEN3_ID){     // NRF24
    //     if(xSemaphoreTake(radioLock, portMAX_DELAY) == pdTRUE) {
    //         nrf24_recv();
    //         xSemaphoreGive(radioLock);
    //     }
    // }

    loopNFCReader();
}