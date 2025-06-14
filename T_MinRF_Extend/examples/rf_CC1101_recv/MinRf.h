#pragma once

#include "Arduino.h"
#include <SPI.h>

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
