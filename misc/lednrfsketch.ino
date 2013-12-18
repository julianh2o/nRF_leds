// vim: syntax=cpp
#include <FastSPI_LED.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>

#define NUM_LEDS 125

// Sometimes chipsets wire in a backwards sort of way
struct CRGB { 
    unsigned char g; 
    unsigned char r; 
    unsigned char b; 
};
// struct CRGB { unsigned char r; unsigned char g; unsigned char b; };
struct CRGB *leds;

#define PIN 3

void setup() {
    FastSPI_LED.setLeds(NUM_LEDS);
    FastSPI_LED.setChipset(CFastSPI_LED::SPI_WS2811);

    FastSPI_LED.setPin(PIN);

    FastSPI_LED.init();
    FastSPI_LED.start();

    leds = (struct CRGB*)FastSPI_LED.getRGBData(); 
    memset(leds, 0, NUM_LEDS * 3); //clear LEDs
    FastSPI_LED.show();

    Serial.begin(115200);

    //init nrf
    Mirf.cePin = 8;
    Mirf.csnPin = 7;
    Mirf.spi = &MirfHardwareSpi;
    Mirf.init();

    uint8_t rx_addr[] = {0x34,0x43,0x10,0x10,0x00};
    uint8_t tx_addr[] = {0x34,0x00,0x10,0x10,0x00};

    Mirf.setRADDR((byte *)&rx_addr);
    Mirf.setTADDR((byte *)&tx_addr);

    Mirf.payload = 32;
    Mirf.channel = 40;

    Mirf.config();
}

void loop() { 
    while(Serial.available() == 0);

    short byteCount = 0;
    byteCount += Serial.read() << 8;
    byteCount += Serial.read();

    char buffer [byteCount];
    Serial.readBytes(buffer,byteCount);

    char ledStart = buffer[0];
    char * startIndex = (char *)(leds) + ledStart*3;
    int i=1;
    while (i<byteCount) {
        startIndex[0] = buffer[i+1];
        startIndex[1] = buffer[i];
        startIndex[2] = buffer[i+2];
        i+=3;
        startIndex+=3;
    }

    if(!Mirf.isSending() && Mirf.dataReady()) {
    }

    //memset(leds, 0, NUM_LEDS * 3);
    FastSPI_LED.show();

    Serial.write(1);
}


