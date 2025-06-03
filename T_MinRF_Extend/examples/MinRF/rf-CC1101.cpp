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
static CC1101 radio = new Module(CC1101_CS, CC1101_GDO0, -1, CC1101_GDO2, rfspi, RADIOLIB_DEFAULT_SPI_SETTINGS);
static int transmissionState = RADIOLIB_ERR_NONE;
static volatile bool transmittedFlag = false;

// flag to indicate that a packet was recv
static volatile bool receivedFlag = false;
static bool cc1101_mode = 0; // 0: send ; 1: recv
String cc1101_rssi = "0dBm";
String cc1101_lqi = "0dB";
String cc1101_payload = "0";

static void setSendFlag(void)
{
    // we sent a packet, set the flag
    transmittedFlag = true;
}

static void setRecvFlag(void)
{
    // we sent a packet, set the flag
    receivedFlag = true;
}

bool cc1101_get_mode(void)
{
    return cc1101_mode;
}

void cc1101_set_mode(bool mode)
{
    cc1101_mode = mode; // 0: send ; 1: recv

    if (cc1101_mode)
    {
        radio.clearPacketSentAction();
        radio.setPacketReceivedAction(setRecvFlag);

        // start listening for packets
        Serial.print(F("[CC1101] Starting to listen ... "));
        int state = radio.startReceive();
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
    else
    {
        // set the function that will be called
        // when new packet is received
        radio.clearPacketReceivedAction();
        radio.setPacketSentAction(setSendFlag);

        // start transmitting the first packet
        Serial.print(F("[CC1101] Sending first packet ... "));

        // you can transmit C-string or Arduino string up to
        // 64 characters long
        transmissionState = radio.startTransmit("Hello World!");
    }
}

int cc1101_init(void)
{
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
        return state;
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

    // set mode
    cc1101_set_mode(cc1101_get_mode());

    return RADIOLIB_ERR_NONE;
}

// counter to keep track of transmitted packets
int count = 0;

void cc1101_send(char *text)
{
    // check if the previous transmission finished
    if (transmittedFlag)
    {
        // reset flag
        transmittedFlag = false;

        if (transmissionState == RADIOLIB_ERR_NONE)
        {
            // packet was successfully sent
            // Serial.println(F("transmission finished!"));

            // NOTE: when using interrupt-driven transmit method,
            //       it is not possible to automatically measure
            //       transmission data rate using getDataRate()
        }
        else
        {
            Serial.print(F("failed, code "));
            Serial.println(transmissionState);
        }

        // clean up after transmission is finished
        // this will ensure transmitter is disabled,
        // RF switch is powered down etc.
        radio.finishTransmit();

        // wait a second before transmitting again
        delay(50);

        // send another one
        Serial.print(F("[CC1101] Sending another packet ... "));

        // you can transmit C-string or Arduino string up to
        // 256 characters long
        // String str = "Hello World! #" + String(count++);
        transmissionState = radio.startTransmit(text);

        Serial.println(text);

        // you can also transmit byte array up to 256 bytes long
        /*
        byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                            0x89, 0xAB, 0xCD, 0xEF};
        int state = radio.startTransmit(byteArr, 8);
        */
    }
    delay(1);
}

void cc1101_recv(void)
{
    // check if the previous transmission finished
    if (receivedFlag)
    {
        // reset flag
        receivedFlag = false;

        // you can read received data as an Arduino String
        int state = radio.readData(cc1101_payload);

        // you can also read received data as byte array
        /*
          byte byteArr[8];
          int numBytes = radio.getPacketLength();
          int state = radio.readData(byteArr, numBytes);
        */

        if (state == RADIOLIB_ERR_NONE)
        {
            cc1101_rssi = String(radio.getRSSI()) + "dBm";
            cc1101_lqi = String(radio.getLQI()) + "dB";
            
            // packet was successfully received
            Serial.println(F("[CC1101] Received packet!"));

            // print data of the packet
            Serial.print(F("[CC1101] Data:\t\t"));
            Serial.println(cc1101_payload);

            // print RSSI (Received Signal Strength Indicator)
            // of the last received packet
            Serial.print(F("[CC1101] RSSI:\t\t"));
            Serial.print(radio.getRSSI());
            Serial.println(F(" dBm"));

            // print LQI (Link Quality Indicator)
            // of the last received packet, lower is better
            Serial.print(F("[CC1101] LQI:\t\t"));
            Serial.println(cc1101_lqi);
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
