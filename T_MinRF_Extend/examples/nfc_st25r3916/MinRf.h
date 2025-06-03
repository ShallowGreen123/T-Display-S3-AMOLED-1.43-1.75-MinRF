#pragma once

#include "Arduino.h"
#include <SPI.h>
#include "RadioLib.h"

extern SPIClass rfspi;

// #define LR1121_TEST
// #define CC1101_TEST
#define NRF24_TEST

extern int cc1101_init_flag;
extern int lr1121_init_flag;
extern int nrf24_init_flag;
extern int st32r2916_init_flag;

extern SemaphoreHandle_t radioLock;

// LR1121
extern bool lr1121_get_mode(void);
extern void lr1121_set_mode(bool mode);
extern int lr1121_init(void);
extern void lr1121_send(char *text);
extern void lr1121_recv(void);

extern int cc1101_init(void);
extern void cc1101_loop(void);

// NRF24
extern bool nrf24_get_mode(void);
extern void nrf24_set_mode(bool mode);
extern int nrf24_init(void);
extern void nrf24_loop(void);
extern void nrf24_recv(void);
extern uint8_t nrf24_get_recv_data(void);
extern bool nrf24_get_recv_status(void);

// INIT
int cc1101_spi_init(void);
int lr1121_spi_init(void);
int nrf24_spi_init(void);
bool st32r2916_spi_init(void);

#if 1
// Frequency define 
#define LR1121_FREQ 433.0       // Allowed values are in range from 150.0 to 960.0 MHz, 1900 - 2200 MHz and 2400 - 2500 MHz. Will also perform calibrations.
#define CC1101_FREQ 868.0       // Allowed values are in bands 300.0 to 348.0 MHz, 387.0 to 464.0 MHz and 779.0 to 928.0 MHz.
#define NRF24L01_FREQ 2400.0     // Allowed values range from 2400 MHz to 2525 MHz.

// SPI
#define BOARD_SPI_SCK   18
#define BOARD_SPI_MOSI  7
#define BOARD_SPI_MISO  6

// LR1121
#define LR1121_CS       42
#define LR1121_DIO9     44
#define LR1121_RST      43
#define LR1121_BUSY     41
#define LR1121_SCK      BOARD_SPI_SCK
#define LR1121_MOSI     BOARD_SPI_MOSI
#define LR1121_MISO     BOARD_SPI_MISO

// CC1101
#define CC1101_CS       5
#define CC1101_GDO0     3
#define CC1101_GDO2     1
#define CC1101_SW0      38
#define CC1101_SW1      39
#define CC1101_SCK      BOARD_SPI_SCK
#define CC1101_MOSI     BOARD_SPI_MOSI
#define CC1101_MISO     BOARD_SPI_MISO

// NRF24L01
#define NRF24L01_CS     21  // SPI Chip Select
#define NRF24L01_CE     48   // Chip Enable Activates RX or TX(High) mode
#define NRF24L01_IQR    47   // Maskable interrupt pin. Active low
#define NRF24L01_SCK    BOARD_SPI_SCK
#define NRF24L01_MOSI   BOARD_SPI_MOSI
#define NRF24L01_MISO   BOARD_SPI_MISO

// ST25R3916
#define ST25R3916_BSS       2
// #define ST25R3916_I2C_EN    35  // Connect to GND (SPI) or VDD_D (I2C)
#define ST25R3916_IQR       46
#define ST25R3916_SCK       BOARD_SPI_SCK
#define ST25R3916_MOSI      BOARD_SPI_MOSI
#define ST25R3916_MISO      BOARD_SPI_MISO

#else

// Frequency define 
#define LR1121_FREQ 433.0       // Allowed values are in range from 150.0 to 960.0 MHz, 1900 - 2200 MHz and 2400 - 2500 MHz. Will also perform calibrations.
#define CC1101_FREQ 433.0       //
#define NRF24L01_FREQ 2400.0     // Allowed values range from 2400 MHz to 2525 MHz.

// SPI
#define BOARD_SPI_SCK   2
#define BOARD_SPI_MOSI  1
#define BOARD_SPI_MISO  3

// LR1121
#define LR1121_CS       47
#define LR1121_DIO9     48
#define LR1121_RST      21
#define LR1121_BUSY     46
#define LR1121_SCK      BOARD_SPI_SCK
#define LR1121_MOSI     BOARD_SPI_MOSI
#define LR1121_MISO     BOARD_SPI_MISO

// CC1101
#define CC1101_CS       6
#define CC1101_GDO0     8
#define CC1101_GDO2     1
#define CC1101_SW0      38
#define CC1101_SW1      39
#define CC1101_SCK      BOARD_SPI_SCK
#define CC1101_MOSI     BOARD_SPI_MOSI
#define CC1101_MISO     BOARD_SPI_MISO

// NRF24L01
#define NRF24L01_CS     21  // SPI Chip Select
#define NRF24L01_CE     48   // Chip Enable Activates RX or TX(High) mode
#define NRF24L01_IQR    47   // Maskable interrupt pin. Active low
#define NRF24L01_SCK    BOARD_SPI_SCK
#define NRF24L01_MOSI   BOARD_SPI_MOSI
#define NRF24L01_MISO   BOARD_SPI_MISO

// ST25R3916
#define ST25R3916_BSS       5
// #define ST25R3916_I2C_EN    35  // Connect to GND (SPI) or VDD_D (I2C)
#define ST25R3916_IQR       46
#define ST25R3916_SCK       BOARD_SPI_SCK
#define ST25R3916_MOSI      BOARD_SPI_MOSI
#define ST25R3916_MISO      BOARD_SPI_MISO
#endif