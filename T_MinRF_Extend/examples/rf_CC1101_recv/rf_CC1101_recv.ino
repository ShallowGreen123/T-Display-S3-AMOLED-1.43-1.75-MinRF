

#include <Arduino.h>
#include <RadioLib.h>
#include "MinRf.h"

#define ASSERT_FUN(name, fun)                                                       \
    if (fun == RADIOLIB_ERR_INVALID_BIT_RATE)                                       \
    {                                                                               \
        Serial.printf("[%s] Selected bit rate is invalid for this module\n", name); \
        while (true)                                                                \
            ;                                                                       \
    }

static float CC1101_freq = CC1101_FREQ;
static CC1101 radio = new Module(CC1101_CS, CC1101_GDO0, -1, CC1101_GDO2);
static int transmissionState = RADIOLIB_ERR_NONE;
static volatile bool receivedFlag = false;

static void setFlag(void)
{
    // we sent a packet, set the flag
    receivedFlag = true;
}

void setup(void)
{
    Serial.begin(115200);

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

    SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI, CC1101_CS);

    // Set antenna frequency settings
    pinMode(CC1101_SW1, OUTPUT);
    pinMode(CC1101_SW0, OUTPUT);

    // SW1:1  SW0:0 --- 315MHz
    // SW1:0  SW0:1 --- 868/915MHz
    // SW1:1  SW0:1 --- 434MHz
    if (CC1101_freq - 315 < 0.1)
    {
        digitalWrite(CC1101_SW1, HIGH);
        digitalWrite(CC1101_SW0, LOW);
    }
    else if (CC1101_freq - 434 < 0.1)
    {
        digitalWrite(CC1101_SW1, HIGH);
        digitalWrite(CC1101_SW0, HIGH);
    }
    else if (CC1101_freq - 868 < 0.1)
    {
        digitalWrite(CC1101_SW1, LOW);
        digitalWrite(CC1101_SW0, HIGH);
    }
    else if (CC1101_freq - 915 < 0.1)
    {
        digitalWrite(CC1101_SW1, LOW);
        digitalWrite(CC1101_SW0, HIGH);
    }
    else
    {
        digitalWrite(CC1101_SW1, LOW);
        digitalWrite(CC1101_SW0, HIGH);
    }

    // initialize CC1101
    Serial.print(F("[CC1101] Initializing ... "));
    Serial.print(CC1101_freq);
    Serial.println(" MHz ");

    int state = radio.begin(CC1101_freq);
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        // while (true)
        //     ;
    }

    // you can also change the settings at runtime
    // and check if the configuration was changed successfully

    // set carrier frequency
    ASSERT_FUN("setFrequency", radio.setFrequency(CC1101_freq));

    // Enables/disables OOK modulation instead of FSK.
    ASSERT_FUN("setOOK", radio.setOOK(true));

    // Set bit rate to 5 kbps
    ASSERT_FUN("setBitRate", radio.setBitRate(1.2));

    // set receiver bandwidth to 135.0 kHz
    ASSERT_FUN("setRxBandwidth", radio.setRxBandwidth(58.0));

    // set allowed frequency deviation to 10.0 kHz
    ASSERT_FUN("setFrequencyDeviation", radio.setFrequencyDeviation(5.2));

    // set output power to 10 dBm
    ASSERT_FUN("setOutputPower", radio.setOutputPower(10));

    // 2 bytes can be set as sync word
    ASSERT_FUN("setSyncWord", radio.setSyncWord(0x01, 0x23));

    // set the function that will be called
    // when new packet is received
    radio.setPacketReceivedAction(setFlag);

    // start listening for packets
    Serial.print(F("[CC1101] Starting to listen ... "));
    state = radio.startReceive();
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true)
        {
            delay(10);
        }
    }
}

void loop(void)
{
    // check if the previous transmission finished
    if (receivedFlag)
    {
        // reset flag
        receivedFlag = false;

        // you can read received data as an Arduino String
        String str;
        int state = radio.readData(str);

        // you can also read received data as byte array
        /*
          byte byteArr[8];
          int numBytes = radio.getPacketLength();
          int state = radio.readData(byteArr, numBytes);
        */

        if (state == RADIOLIB_ERR_NONE)
        {
            // packet was successfully received
            Serial.println(F("[CC1101] Received packet!"));

            // print data of the packet
            Serial.print(F("[CC1101] Data:\t\t"));
            Serial.println(str);

            // print RSSI (Received Signal Strength Indicator)
            // of the last received packet
            Serial.print(F("[CC1101] RSSI:\t\t"));
            Serial.print(radio.getRSSI());
            Serial.println(F(" dBm"));

            // print LQI (Link Quality Indicator)
            // of the last received packet, lower is better
            Serial.print(F("[CC1101] LQI:\t\t"));
            Serial.println(radio.getLQI());
        }
        else if (state == RADIOLIB_ERR_CRC_MISMATCH)
        {
            // packet was received, but is malformed
            Serial.println(F("CRC error!"));
        }
        else
        {
            // some other error occurred
            Serial.print(F("failed, code "));
            Serial.println(state);
        }

        // put module back to listen mode
        radio.startReceive();
    }
}
