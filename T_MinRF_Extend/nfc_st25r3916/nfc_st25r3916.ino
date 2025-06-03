#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include "nfc_include.h"
#include "app_nfc.h"
#include "MinRf.h"

SPIClass rfspi(HSPI);

RfalRfST25R3916Class nfc_hw(&rfspi, ST25R3916_BSS, ST25R3916_IQR);
RfalNfcClass NFCReader(&nfc_hw);

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

void setup()
{
    Serial.begin(115200);
    // while (!Serial);

    Serial.println("Initializing Sensors...");


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


    beginNFC(notify_callback, ndef_event_callback);

    // bool res = false;
    // Serial.print("Initializing NFC: ");
    // res = NFCReader.rfalNfcInitialize() == ST_ERR_NONE;
    // if (!res) {
    //     Serial.println("Failed to find NFC Reader - check your wiring!");
    // } else {
    //     Serial.println("Initializing NFC Reader succeeded");
    // }
}

void loop()
{
    loopNFCReader();
    delay(10);
}
