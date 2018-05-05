#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
}

/*The Digital Output pins used to multiplex the one analog input of the wemos,
 by applying LOW/HIGH on the potentiometer to measure.*/
#define x1 D7
#define x2 D5
#define y1 D4
#define y2 D3

/* how often to measure the current joystick position.
   You may increase this value with further testing. 
   Setting it too high might cause weird stepper 
   behavior, since the arduino uno has limited processing
   speed compared to the ESP's.
   Setting it to >1001 will result in maximum transmission rate,
   but stalled my arduion. */
#define packetsPerSecond 10

/* WIFI channel used for communication,
you might change it for better performance/range */
#define WIFI_CHANNEL 1

/* this address must be similar to the one used in the receiver */
byte remoteMac[] = {0xEE, 0xFA, 0xBC, 0x87, 0x72, 0xFE};
const byte dataLength = 4;
const int sendingDelay =1000/ packetsPerSecond;
byte rcvData[dataLength];

void configureOutputPins(){
  pinMode(x1, OUTPUT);
  pinMode(x2, OUTPUT);
  pinMode(y1, OUTPUT);
  pinMode(y2, OUTPUT);
  pinMode(A0, INPUT);
}
/* Configure E*/
void configureESPNow(){
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  Serial.print("Sender Device MAC: ");
  Serial.println(WiFi.macAddress());
    esp_now_init();
  delay(10);
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(remoteMac, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
}
void setup()
{
  Serial.begin(115200);
  configureESPNow();
  confugireOutputPins();
}


/* method to read the specific potentiometer, by raising the potential at the dir pin*/
int readPotentiometer(unsigned int dir) {
  digitalWrite(x1, LOW);
  digitalWrite(x2, LOW);
  digitalWrite(y1, LOW);
  digitalWrite(y2, LOW);
  digitalWrite(dir, HIGH);
  return analogRead(A0);
}
void loop()
{
  /* Read both directions of the potentiometer*/
  int x = readPotentiometer(x1);
  int y = readPotentiometer(y1);
  /*Write the values in a byte array to send them over ESP-now */
  rcvData[0] = x;
  rcvData[1] = x >> 8;
  rcvData[2] = y;
  rcvData[3] = y >> 8;
  esp_now_send(remoteMac, rcvData, dataLength);
  /* print the measured values for easier debugging */
  Serial.printf("X%04dY%04d\n", x, y);
  /* wait some time allowing the other controllers to process the data */
  delay(sendingDelay);
}