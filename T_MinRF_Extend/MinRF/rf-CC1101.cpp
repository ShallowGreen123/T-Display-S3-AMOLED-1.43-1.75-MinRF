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

static void setFlag(void)
{
    // we sent a packet, set the flag
    transmittedFlag = true;
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
    } else
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

    // set the function that will be called
    // when packet transmission is finished
    radio.setPacketSentAction(setFlag);

    // start transmitting the first packet
    Serial.print(F("[CC1101] Sending first packet ... "));

    // you can transmit C-string or Arduino string up to
    // 64 characters long
    transmissionState = radio.startTransmit("Hello World!");

    return RADIOLIB_ERR_NONE;
}

// counter to keep track of transmitted packets
int count = 0;

void cc1101_loop(void)
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
        String str = "Hello World! #" + String(count++);
        transmissionState = radio.startTransmit(str);

        Serial.println(str.c_str());

        // you can also transmit byte array up to 256 bytes long
        /*
        byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
                            0x89, 0xAB, 0xCD, 0xEF};
        int state = radio.startTransmit(byteArr, 8);
        */
    }
    delay(1);
}