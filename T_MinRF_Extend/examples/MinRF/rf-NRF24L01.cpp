#include <Arduino.h>
#include "SPI.h"
#include "RF24.h"
#include "nRF24L01.h"
#include "MinRf.h"

// nRF24 has the following connections:
// CS pin:    34
// IRQ pin:   5
// CE pin:    4
#define INTERVAL_MS_TRANSMISSION 250
#define INTERVAL_MS_SIGNAL_RETRY 250
#define INTERVAL_MS_SIGNAL_LOST 2500
static RF24 radio(NRF24L01_CE, NRF24L01_CS);
static const byte address[6] = "00001";
static unsigned long lastSignalMillis = 0;
static bool nrf24_mode = 0;

static byte send_data = 0;
static byte recv_data = 0;
static bool recv_status = false;
// NRF24L01 buffer limit is 32 bytes (max struct size)
struct payload
{
    byte data1;
    char data2;
};
payload payload;

void lostConnection();

void nrf24_set_mode(bool mode)
{
    nrf24_mode = mode;
    // Act as transmitter
    if (mode == 0)
    {
        radio.openWritingPipe(address);
        radio.stopListening();
    }
    else
    {
        radio.openReadingPipe(0, address);
        radio.startListening();
    }
}

bool nrf24_get_mode(void)
{
    return nrf24_mode;
}

uint8_t nrf24_get_recv_data(void)
{
    return recv_data;
}

bool nrf24_get_recv_status(void)
{
    return recv_status;
}

int nrf24_init(void)
{
    bool ret = false;

    ret = radio.begin();
    // Append ACK packet from the receiving radio back to the transmitting radio
    radio.setAutoAck(false); //(true|false)
    // Set the transmission datarate
    radio.setDataRate(RF24_250KBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
    // Greater level = more consumption = longer distance
    radio.setPALevel(RF24_PA_MAX); //(RF24_PA_MIN|RF24_PA_LOW|RF24_PA_HIGH|RF24_PA_MAX)
    // Default value is the maximum 32 bytes
    radio.setPayloadSize(sizeof(payload));
    // select mode
    nrf24_set_mode(nrf24_get_mode());

    return (ret == true);
}

void nrf24_loop(void)
{
    payload.data1 = send_data++;
    payload.data2 = 'x';
    radio.write(&payload, sizeof(payload));
    Serial.print("Data1:");
    Serial.println(payload.data1);
    Serial.print("Data2:");
    Serial.println(payload.data2);
    Serial.println("Sent");
    // delay(INTERVAL_MS_TRANSMISSION);
}

void nrf24_recv(void)
{
    unsigned long currentMillis = millis();

    if (radio.available() > 0)
    {
        radio.read(&payload, sizeof(payload));
        Serial.println("Received");
        Serial.print("Data1:");
        Serial.println(payload.data1);
        Serial.print("Data2:");
        Serial.println(payload.data2);

        recv_data = payload.data1;

        lastSignalMillis = currentMillis;
    }

    if (currentMillis - lastSignalMillis > INTERVAL_MS_SIGNAL_LOST)
    {
        lostConnection();
        recv_status = false;
    } else {
        recv_status = true;
    }
}

void lostConnection()
{
    Serial.println("We have lost connection, preventing unwanted behavior");
    delay(INTERVAL_MS_SIGNAL_RETRY);
}