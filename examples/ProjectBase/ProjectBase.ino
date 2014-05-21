#include "SPI.h"
#include "UIPEthernet.h"
#include "DashduinoClient.h"

byte mac[] = { 0x01, 0x02, 0x03, 0x04, 0xFA, 0x10 };
DashduinoClient dashduino;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac);

  // listener function
  dashduino.setEventListener(eventListener);
  // connect to dashduino platform
  dashduino.connect("ws.dashduino.com", 3000);
  // send event do platform
  dashduino.send("1","Arduino connected.");
}

void loop() {
  dashduino.monitor();
}

// receive events from dashduino platform
void eventListener(char *data) {
  Serial.println(data);
}