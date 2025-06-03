#include <Arduino.h>
#include <RadioLib.h>
#include "MinRf.h"

// The maximum power of LR1121 2.4G band can only be set to 13 dBm
#define CONFIG_RADIO_FREQ 868.0       // Allowed values are in range from 150.0 to 960.0 MHz, 1900 - 2200 MHz and 2400 - 2500 MHz. Will also perform calibrations.
#define CONFIG_RADIO_OUTPUT_POWER 22
#define CONFIG_RADIO_BW 125.0

// LR1121 has the following connections:
// NSS pin:   10
// IRQ pin:   2
// NRST pin:  3
// BUSY pin:  9
LR1121 radio = new Module(LR1121_CS, LR1121_DIO9, LR1121_RST, LR1121_BUSY);

// flag to indicate that a packet was sent
static volatile bool receivedFlag = false;
static String rssi = "0dBm";
static String snr = "0dB";
static String payload = "0";

static void setFlag(void)
{
    // we sent a packet, set the flag
    receivedFlag = true;
}

void setup(void)
{
    Serial.begin(115200);
    
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

    SPI.begin(LR1121_SCK, LR1121_MISO, LR1121_MOSI, LR1121_CS);

    Serial.print(F("[Radio] Initializing ... "));
    int state = radio.begin();
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.println(F("failed!"));
        Serial.println(state);
    }

    /*
     *   Sets carrier frequency.
     *   SX1278/SX1276 : Allowed values range from 137.0 MHz to 525.0 MHz.
     *   SX1268/SX1262 : Allowed values are in range from 150.0 to 960.0 MHz.
     *   SX1280        : Allowed values are in range from 2400.0 to 2500.0 MHz.
     *   LR1121        : Allowed values are in range from 150.0 to 960.0 MHz, 1900 - 2200 MHz and 2400 - 2500 MHz. Will also perform calibrations.
     * * * */

    if (radio.setFrequency(CONFIG_RADIO_FREQ) == RADIOLIB_ERR_INVALID_FREQUENCY)
    {
        Serial.println(F("Selected frequency is invalid for this module!"));
        while (true)
            ;
    }

    /*
     *   Sets LoRa link bandwidth.
     *   SX1278/SX1276 : Allowed values are 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125, 250 and 500 kHz. Only available in %LoRa mode.
     *   SX1268/SX1262 : Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0 and 500.0 kHz.
     *   SX1280        : Allowed values are 203.125, 406.25, 812.5 and 1625.0 kHz.
     *   LR1121        : Allowed values are 62.5, 125.0, 250.0 and 500.0 kHz.
     * * * */
    if (radio.setBandwidth(CONFIG_RADIO_BW) == RADIOLIB_ERR_INVALID_BANDWIDTH)
    {
        Serial.println(F("Selected bandwidth is invalid for this module!"));
        while (true)
            ;
    }

    /*
     * Sets LoRa link spreading factor.
     * SX1278/SX1276 :  Allowed values range from 6 to 12. Only available in LoRa mode.
     * SX1262        :  Allowed values range from 5 to 12.
     * SX1280        :  Allowed values range from 5 to 12.
     * LR1121        :  Allowed values range from 5 to 12.
     * * * */
    if (radio.setSpreadingFactor(10) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR)
    {
        Serial.println(F("Selected spreading factor is invalid for this module!"));
        while (true)
            ;
    }

    /*
     * Sets LoRa coding rate denominator.
     * SX1278/SX1276/SX1268/SX1262 : Allowed values range from 5 to 8. Only available in LoRa mode.
     * SX1280        :  Allowed values range from 5 to 8.
     * LR1121        :  Allowed values range from 5 to 8.
     * * * */
    if (radio.setCodingRate(6) == RADIOLIB_ERR_INVALID_CODING_RATE)
    {
        Serial.println(F("Selected coding rate is invalid for this module!"));
        while (true)
            ;
    }

    /*
     * Sets LoRa sync word.
     * SX1278/SX1276/SX1268/SX1262/SX1280 : Sets LoRa sync word. Only available in LoRa mode.
     * * */
    if (radio.setSyncWord(0xAB) != RADIOLIB_ERR_NONE)
    {
        Serial.println(F("Unable to set sync word!"));
        while (true)
            ;
    }

    /*
     * Sets transmission output power.
     * SX1278/SX1276 :  Allowed values range from -3 to 15 dBm (RFO pin) or +2 to +17 dBm (PA_BOOST pin). High power +20 dBm operation is also supported, on the PA_BOOST pin. Defaults to PA_BOOST.
     * SX1262        :  Allowed values are in range from -9 to 22 dBm. This method is virtual to allow override from the SX1261 class.
     * SX1268        :  Allowed values are in range from -9 to 22 dBm.
     * SX1280        :  Allowed values are in range from -18 to 13 dBm. PA Version range : -18 ~ 3dBm
     * LR1121        :  Allowed values are in range from -17 to 22 dBm (high-power PA) or -18 to 13 dBm (High-frequency PA)
     * * * */
    if (radio.setOutputPower(CONFIG_RADIO_OUTPUT_POWER) == RADIOLIB_ERR_INVALID_OUTPUT_POWER)
    {
        Serial.println(F("Selected output power is invalid for this module!"));
        while (true)
            ;
    }

    /*
     * Sets preamble length for LoRa or FSK modem.
     * SX1278/SX1276 : Allowed values range from 6 to 65535 in %LoRa mode or 0 to 65535 in FSK mode.
     * SX1262/SX1268 : Allowed values range from 1 to 65535.
     * SX1280        : Allowed values range from 1 to 65535.
     * LR1121        : Allowed values range from 1 to 65535.
     * * */
    if (radio.setPreambleLength(12) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH)
    {
        Serial.println(F("Selected preamble length is invalid for this module!"));
        while (true)
            ;
    }

    // Enables or disables CRC check of received packets.
    if (radio.setCRC(false) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION)
    {
        Serial.println(F("Selected CRC is invalid for this module!"));
        while (true)
            ;
    }

    // LR1121
    // set RF switch configuration for Wio WM1110
    // Wio WM1110 uses DIO5 and DIO6 for RF switching
    static const uint32_t rfswitch_dio_pins[] = {
        RADIOLIB_LR11X0_DIO5, RADIOLIB_LR11X0_DIO6,
        RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC};

    static const Module::RfSwitchMode_t rfswitch_table[] = {
        // mode                  DIO5  DIO6
        {LR11x0::MODE_STBY, {LOW, LOW}},
        {LR11x0::MODE_RX, {HIGH, LOW}},
        {LR11x0::MODE_TX, {HIGH, HIGH}},
        {LR11x0::MODE_TX_HP, {LOW, HIGH}},
        {LR11x0::MODE_TX_HF, {LOW, LOW}},
        {LR11x0::MODE_GNSS, {LOW, LOW}},
        {LR11x0::MODE_WIFI, {LOW, LOW}},
        END_OF_MODE_TABLE,
    };
    radio.setRfSwitchTable(rfswitch_dio_pins, rfswitch_table);

    // LR1121 TCXO Voltage 2.85~3.15V
    radio.setTCXO(3.0);

    // set the function that will be called
    // when new packet is received
    radio.setPacketReceivedAction(setFlag);

    // start listening for LoRa packets
    Serial.print(F("Radio Starting to listen ... "));
    state = radio.startReceive();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
    }

}

void loop(void)
{
    // check if the flag is set
    if (receivedFlag) {

        // reset flag
        receivedFlag = false;

        // you can read received data as an Arduino String
        int state = radio.readData(payload);

        // you can also read received data as byte array
        /*
          byte byteArr[8];
          int state = radio.readData(byteArr, 8);
        */

        if (state == RADIOLIB_ERR_NONE) {

            rssi = String(radio.getRSSI()) + "dBm";
            snr = String(radio.getSNR()) + "dB";

            // packet was successfully received
            Serial.println(F("Radio Received packet!"));

            // print data of the packet
            Serial.print(F("Radio Data:\t\t"));
            Serial.println(payload);

            // print RSSI (Received Signal Strength Indicator)
            Serial.print(F("Radio RSSI:\t\t"));
            Serial.println(rssi);

            // print SNR (Signal-to-Noise Ratio)
            Serial.print(F("Radio SNR:\t\t"));
            Serial.println(snr);

        } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
            // packet was received, but is malformed
            Serial.println(F("CRC error!"));
        } else {
            // some other error occurred
            Serial.print(F("failed, code "));
            Serial.println(state);
        }

        // put module back to listen mode
        radio.startReceive();
    }
}