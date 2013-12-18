// vim: syntax=cpp
/**
 *
 * Pins:
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13
 *
 * Configurable:
 * CE -> 8
 * CSN -> 7
 *
 * Note: To see best case latency comment out all Serial.println
 * statements not displaying the result and load 
 * 'ping_server_interupt' on the server.
 */

#include <nRF24L01.h>
#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>

void setup() {
    Serial.begin(115200);

    Mirf.cePin = 8;
    Mirf.csnPin = 7;
    Mirf.spi = &MirfHardwareSpi;
    Mirf.init();

    uint8_t tx_addr[] = {0x34,0x43,0x10,0x10,0x00};
    uint8_t rx_addr[] = {0x34,0x00,0x10,0x10,0x00};

    Mirf.payload = 32;
    Mirf.channel = 40;

    Mirf.config();

    Mirf.setRADDR((byte *)&rx_addr);
    Mirf.setTADDR((byte *)&tx_addr);

    uint8_t val;
    for (int i=0; i<=0x1d; i++) {
        Mirf.readRegister(i,&val,1);
        Serial.print("0x");
        Serial.print(i,HEX);
        Serial.print("\t");
        Serial.print("0x");
        Serial.println(val,HEX);
    }

    //Mirf.powerUpRx();
    Mirf.powerUpTx();

    delay(3000);
}

void loop() {
    while(Serial.available() == 0);
 
    short byteCount = 0;
    byteCount += Serial.read() << 8;
    byteCount += Serial.read();
 
    char buffer [byteCount];
    Serial.readBytes(buffer,byteCount);
 
    char ledStart = buffer[0];
    short dataIndex = ((short)ledStart)*3;

    int i = 1;
    int tx_index = 2; //ignore the first 2 bytes, those are control data
    char data[32];
    int dataLength = 0;
    int dataStartLed = ledStart;
    while (i<byteCount) {
        data[tx_index++] = buffer[i+1]; //reverse r/g to normalize ordering
        data[tx_index++] = buffer[i];
        data[tx_index++] = buffer[i+2];
        dataLength += 3;

        i+=3;

        if (tx_index >= 32 || i>=byteCount) {
            //send packet
            Mirf.resetStatus();
            data[0] = dataStartLed;
            data[1] = dataLength;
            Mirf.populateTx((byte *)data);
            int time = millis();
            while(!Mirf.isDataSent() && !Mirf.isMaxRetransmit());
            if (Mirf.isMaxRetransmit()) {
                Serial.print("Miss!");
            }

            //prepare next data packet
            dataStartLed+= 10; //we can store 10 LEDs in each packet
            tx_index = 2;
            dataLength = 0;
        }
    }

    Serial.write((uint8_t)0); //write null character to escape
}

/* rec block (works)
void loop(){
    uint8_t data [] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
    uint8_t rec [] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

    //delay(5);

    //Mirf.resetStatus();
    
    if (Mirf.dataReady()) {
        Serial.println("data ready!");
        Mirf.getData((byte *)rec);

        for (int i=0; i<32; i++) {
            Serial.print(rec[i],HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    delay(100);
}
*/

/* send block (works)
void loop(){
    uint8_t data [] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
    uint8_t rec [] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

    Mirf.powerUpTx();
    delay(5);

    Mirf.resetStatus();
    
    Mirf.populateTx((byte *)data);

    delay(100);
} 
*/
