#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>  // Include Wire if you're using I2C
#include <SPI.h>  // Include SPI if you're using SPI
#include <SFE_MicroOLED.h>
#include <Adafruit_NeoPixel.h>  // Include the SFE_MicroOLED library
//////////////////////////
// MicroOLED Definition //
//////////////////////////
#define PIN_RESET D4  // Connect RST to pin 9 (SPI & I2C)
#define DC_JUMPER D5  // DC jumper setting(I2C only)

MicroOLED oled(D5, D4);  // I2C Example

#define PIN D8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);


WiFiUDP Udp;
unsigned int localUdpPort = 4210;
#define BUFFER_LEN 1024
char incomingPacket[BUFFER_LEN];
char  replyPacekt[] = "ack jesse";
int packetCount;

void setup(){

  Serial.begin(115200);
    Serial.println();
    strip.begin();
    strip.show();
    strip.setBrightness(60);

    WiFi.begin("Adrenaline", "Hertzcar123");

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());


    Udp.begin(localUdpPort);
    oled.begin();

}

static unsigned int running_max=255;
  static unsigned int running_avg=0;
  static float scalefactor = 1;
  unsigned int instant_avg=0;
  int reading;
  static int lastValue[7]={0,0,0,0,0,0,0};



void loop(){
  //oled.clear(PAGE);
  //oled.clear(All);
  oled.setFontType(0);
  oled.setCursor(0,0);
  //oled.printf("Count: %i\n", packetCount);
  //oled.printf("Time: %i", millis()/1000);
//  oled.display();
  int packetSize = Udp.parsePacket();
if (packetSize)
{
  //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
  int len = Udp.read(incomingPacket, BUFFER_LEN);
  if (len > 0)
  {
    /*oled.clear(PAGE);
    oled.printf("Low: %i\n",(uint8_t)incomingPacket[1]);
    oled.printf("Mid: %i\n",(uint8_t)incomingPacket[5]);
    oled.printf("High: %i\n",(uint8_t)incomingPacket[4]);
    oled.printf("Time %i\n", millis()/1000);
    oled.printf("Count: %i", packetCount);
    oled.display();*/


    //Serial.printf("%i | %i | %i\n",(uint16_t)incomingPacket[0],
    //(uint16_t)incomingPacket[1],(uint16_t)incomingPacket[2]);



    for (int i = 0; i < 7; i++){
    reading=max((uint8_t)incomingPacket[i],1);
    instant_avg+=reading;
    if(reading<32*lastValue[i]){
      incomingPacket[i] = int(reading/scalefactor); //FLOAT division!
    }
    lastValue[i]=reading;
    }

    instant_avg =instant_avg/5;
    const byte ALPHA=32;
    running_avg = (ALPHA * (long)instant_avg + (256 - ALPHA) * (long)running_avg )/ 256;
    if((instant_avg>running_avg+20) && (instant_avg<running_avg*8)){
    running_max=(ALPHA * (long)instant_avg + (256 - ALPHA) * (long)running_max )/ 256;
  }

  if(running_max>255){
    //scalefactor is greater than 1 because the max volume is greater than 255
    scalefactor = running_max/255.0; //FLOAT!!
  }else{
    scalefactor = 1;
    //potentially we could have a <1 value here, to scale quiet sounds up.
    //but in my tests this just brought in useless noise
  }

    for(int n = 0; n<8; n++){
     strip.setPixelColor(n, (uint8_t)incomingPacket[1],
     (uint8_t)incomingPacket[5], (uint8_t)incomingPacket[4]);
    }
    strip.show();

    incomingPacket[len] = 0;
    packetCount++;
  }
  //Serial.printf("UDP packet contents: %s\n", incomingPacket);


}

//Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
//Udp.write(replyPacekt);
//Udp.endPacket();

}
