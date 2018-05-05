#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
#include "user_interface.h"
}

/*
    the following values configure the joystick's behaviour,
    depending on the power supply of your Wemos, the Joystick,
    and the diodes you've chosen, you need to adjust the values.
*/

/* below this, the input is considered as backward */
#define joystickLowerThreshold 300
/* above this, the input is considered as forward */
#define joystickUpperThreshold 450
/* value which will be considered as fully forward*/
#define joystickMaxInput 750
/* value which will be considered as fully backward */
#define joystickMinInput 0


/* multiplier applied to the joystick input (-100 to 100),
   a higher multiplier results in a higher motor speed,
   setting it too high will result in not moving motors.
   This value depends on your steppers, motor power supply,
   driver shield and the required torque to move the suitcase. 
   Maximum value: 9*/
#define motorSpeedMultiplicator 3

/* this address must be similar to the one configured in the sender*/
uint8_t mac[] = {0xEE, 0xFA, 0xBC, 0x87, 0x72, 0xFE};
void initVariant() {
  wifi_set_macaddr(STATION_IF, &mac[0]);
}

void setup()
{
  Serial.begin(115200);

  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len)
  { /* execute this when a package is received*/
    if (len == 4) {
      int x = data[0] + (data[1] << 8);
      int y = data[2] + ( data[3] << 8);
      int scaledX = mapAxis(x);
      int scaledY = mapAxis(y);
      mapInput(scaledY, scaledX);
    }
  });
}
/* converts the raw joystick input to a range of -100 to +100 */
int mapAxis(int input) {
  if (input < joystickLowerThreshold) {
    return map(input, joystickMinInput, joystickLowerThreshold, -100, 0);
  }
  if (input > joystickUpperThreshold) {
    return map(input, joystickUpperThreshold, joystickMaxInput, 0, 100);
  }
  return 0;
}
/* calculates the right/left motor speed from the input (-100 to +100) for X/Y */
void mapInput(int x, int y)
{
  if (x == 0 && y == 0) {
    /* send to Arduino.*/
    Serial.println("X0000Y0000");
    return;
  }

/* motor speeds calculated from the left/right axis*/
  int leftM=y;
  int rightM=-y;
  if(y>0){
    rightM =constrain(map(y,0,100,100,-100),-100,0);
  }
  else{
    leftM = constrain(map(y,-100,0,-100,100),-100,0);
  }

  /* adding forward/backward motion*/
  int leftMotor = leftM+x;
  int rightMotor = rightM+x;
  /* send to Arduino.*/
  Serial.printf("X%04dY%04d\n", motorSpeedMultiplicator*leftMotor, motorSpeedMultiplicator*rightMotor);
}
void loop()
{

}