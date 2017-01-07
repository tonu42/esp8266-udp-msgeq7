#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp;
unsigned int localUdpPort = 4210;
char incomingPacket[255];
char  replyPacekt[] = "ack jesse";

int strobePin = 12; // strobe is attached to digital pin 2
int resetPin = 2; // reset is attached to digital pin 3
int spectrumValue[7]; // to hold a2d values
uint8_t spectrumResult[7];
int FilterValue[7] = {355 , 250 , 300 , 300 , 250 , 225 , 250 };

static unsigned int running_max=255;
  static unsigned int running_avg=0;
  static float scalefactor = 1;
  unsigned int instant_avg=0;
  int reading;
  static int lastValue[7]={0,0,0,0,0,0,0};
//int FilterValue[7] = {0 , 0 , 0 , 0 , 0 , 0 , 0 };


void setup(){

  Serial.begin(9600);
    Serial.println();

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

    pinMode(strobePin, OUTPUT);
    pinMode(resetPin, OUTPUT);


    digitalWrite(resetPin, LOW);
    digitalWrite(strobePin, HIGH);


}

uint16_t counter = 1;
void loop(){
digitalWrite(resetPin, HIGH);
digitalWrite(resetPin, LOW);

for (int i = 0; i < 7; i++)
{
digitalWrite(strobePin, LOW);
delayMicroseconds(30); // to allow the output to settle
spectrumValue[i] = analogRead(A0);
spectrumValue[i] = constrain(spectrumValue[i], FilterValue[i], 1023);
spectrumResult[i] = map(spectrumValue[i], FilterValue[i], 1023, 0, 255);
digitalWrite(strobePin, HIGH);
}







//delay(100);
//Serial.println("sending");
//uint8_t colors[3];
//colors[0] = counter;
//colors[1] = counter;
//colors[2] = counter;

Udp.beginPacket("192.168.0.30", 4210);
//Udp.write(colors, 3);
Udp.write(spectrumResult, 7);
Serial.printf("%i | %i | %i | %i | %i | %i | %i \n",
spectrumResult[0],spectrumResult[1],spectrumResult[2],spectrumResult[3],spectrumResult[4]
,spectrumResult[5],spectrumResult[6]);
Udp.endPacket();
counter++;




}
