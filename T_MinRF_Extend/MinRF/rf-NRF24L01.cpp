#include <Arduino.h>
#include <RadioLib.h>
#include "MinRf.h"

// nRF24 has the following connections:
// CS pin:    34
// IRQ pin:   5
// CE pin:    4
static nRF24 radio = new Module(NRF24L01_CS, NRF24L01_IQR, NRF24L01_CE, RADIOLIB_NC, rfspi, RADIOLIB_DEFAULT_SPI_SETTINGS);

byte addr[] = {0x00, 0x23, 0x45, 0x67, 0x89};

// save transmission state between loops
static int transmissionState = RADIOLIB_ERR_NONE;

// flag to indicate that a packet was sent
static volatile bool transmittedFlag = false;

void nrf24_transmit(void);

static void nrf24_setFlag(void)
{
    // we sent a packet, set the flag
    transmittedFlag = true;
    Serial.println(F("nrf24_setFlag!"));
}

int nfr24_init(void)
{
    // initialize nRF24 with default settings
    Serial.print(F("[nRF24] Initializing ... "));
    int state = radio.begin();
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

    // set transmit address
    // NOTE: address width in bytes MUST be equal to the
    //       width set in begin() or setAddressWidth()
    //       methods (5 by default)
    // byte addr[] = {0x00, 0x23, 0x45, 0x67, 0x89};
    Serial.print(F("[nRF24] Setting transmit pipe ... "));
    state = radio.setTransmitPipe(addr);
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

    radio.setOutputPower(0);

    // set the function that will be called
    // when packet transmission is finished
    // radio.setPacketSentAction(nrf24_setFlag);

    // start transmitting the first packet
    Serial.print(F("[nRF24] Sending first packet ... "));

    // radio.startReceive();

    // you can transmit C-string or Arduino string up to
    // 256 characters long
    // transmissionState = radio.startTransmit("Hello World!");

    const uint8_t transmitBytes = 3;

    uint8_t pipeAddress = 0;

    uint8_t buffer[transmitBytes] = {addr[0], 1, 2};
    radio.startTransmit(buffer, transmitBytes, pipeAddress);
    
    return RADIOLIB_ERR_NONE;
}

static int count = 0;

void nrf24_transmit(uint8_t item, uint8_t on)
{
    const uint8_t transmitBytes = 3;

    uint8_t pipeAddress = 0;

    uint8_t buffer[transmitBytes] = {addr[0], item, on};

    int state = radio.startTransmit(buffer, transmitBytes, pipeAddress);
    if (state == RADIOLIB_ERR_NONE)
    {
        // the packet was successfully transmitted
        Serial.println(F("success!"));
    }
    else if (state == RADIOLIB_ERR_PACKET_TOO_LONG)
    {
        // the supplied packet was longer than 32 bytes
        Serial.println(F("too long!"));
    }
    else if (state == RADIOLIB_ERR_ACK_NOT_RECEIVED)
    {
        // acknowledge from destination module
        // was not received within 15 retries
        Serial.println(F("ACK not received!"));
    }
    else if (state == RADIOLIB_ERR_TX_TIMEOUT)
    {
        // timed out while transmitting
        Serial.println(F("timeout!"));
    }
    else
    {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
    }
}

void nrf24_loop(void)
{
    // if (transmittedFlag)
    // {
    //     transmittedFlag = false;

        nrf24_transmit(1, 2);
    // }

    delay(200);

    // // check if the previous transmission finished
    // if (transmittedFlag)
    // {
    //     // reset flag
    //     transmittedFlag = false;

    //     if (transmissionState == RADIOLIB_ERR_NONE)
    //     {
    //         // packet was successfully sent
    //         Serial.println(F("transmission finished!"));

    //         // NOTE: when using interrupt-driven transmit method,
    //         //       it is not possible to automatically measure
    //         //       transmission data rate using getDataRate()
    //     }
    //     else
    //     {
    //         Serial.print(F("failed, code "));
    //         Serial.println(transmissionState);
    //     }

    //     // clean up after transmission is finished
    //     // this will ensure transmitter is disabled,
    //     // RF switch is powered down etc.
    //     radio.finishTransmit();

    //     // wait a second before transmitting again
    //     delay(200);

    //     // send another one
    //     Serial.print(F("[nRF24] Sending another packet ... "));

    //     // you can transmit C-string or Arduino string up to
    //     // 32 characters long
    //     String str = "Hello World! #" + String(count++);
    //     transmissionState = radio.startTransmit(str);

    //     // you can also transmit byte array up to 256 bytes long
    //     /*
    //       byte byteArr[] = {0x01, 0x23, 0x45, 0x67,
    //                         0x89, 0xAB, 0xCD, 0xEF};
    //       int state = radio.startTransmit(byteArr, 8);
    //     */
    // }
}