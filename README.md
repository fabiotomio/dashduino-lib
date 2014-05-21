# Dashduino
Site: [Dashduino Platform](http://dashduino.com)

Dashduino é uma plataforma para monitoração e compartilhamento de eventos para projetos construídos com o Arduino ou qualquer outra plataforma Open Source Hardware.

O objetivo é melhorar a qualidade da nossa interação com os projetos, de uma forma simples, intuitiva e colaborativa! 

Já imaginou, não precisar adicionar um sensor de temperatura ao seu projeto e mesmo assim ter essa informação em tempo real, porque o seu visinho compartilhou essa informação?
Ou ser avisado quando sua loja está com um movimento menor que o normal, e você conseguir publicar automaticamente um post previamente cadastrado, no Facebook, com uma promoção relâmpago para os seus clientes!?
Ou simplesmente sincronizar eventos reais com o seu Google Analytics!?

Dashduino tem como objetivo ser uma plataforma para a solução de problemas reais de forma interativa, colaborativa e open-source.

## Instalação

Você precisa clonar esse repositório no diretórico com as bibliotecas do seu Arduino Sketchbook e reiniciar a sua Arduino IDE ou baixar o zip e importar para as suas bibliotecas clicando em Scketch>Import Library>Add Library.

## Como utilizar

```
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
```

Caso queira acompanhar o funcionamento da biblioteca descomente a linha abaixo do arquivo: DashduinoClient.h
// #define DEBUG_CLIENT