#include "SPI.h"
#include "RF24.h"
#include "nRF24L01.h"
#include "MinRf.h"

#define CE_PIN NRF24L01_CE
#define CSN_PIN NRF24L01_CS
#define INTERVAL_MS_TRANSMISSION 250
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";
// NRF24L01 buffer limit is 32 bytes (max struct size)
struct payload
{
    byte data1;
    char data2;
};
payload payload;

void setup()
{
    Serial.begin(115200);

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

    // SPI.setFrequency(100000);
    SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI, NRF24L01_CS);

    radio.begin();
    // Append ACK packet from the receiving radio back to the transmitting radio
    radio.setAutoAck(false); //(true|false)
    // Set the transmission datarate
    radio.setDataRate(RF24_250KBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
    // Greater level = more consumption = longer distance
    radio.setPALevel(RF24_PA_MAX); //(RF24_PA_MIN|RF24_PA_LOW|RF24_PA_HIGH|RF24_PA_MAX)
    // Default value is the maximum 32 bytes
    radio.setPayloadSize(sizeof(payload));
    // Act as transmitter
    radio.openWritingPipe(address);
    radio.stopListening();
}
void loop()
{
    payload.data1 = 123;
    payload.data2 = 'x';
    radio.write(&payload, sizeof(payload));
    Serial.print("Data1:");
    Serial.println(payload.data1);
    Serial.print("Data2:");
    Serial.println(payload.data2);
    Serial.println("Sent");
    delay(INTERVAL_MS_TRANSMISSION);
}
