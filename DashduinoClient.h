/*
 Dashduino Client
 
 Site: http://dashduino.com
 Author: fabiotomio@gmail.com


 The MIT License (MIT)
 Copyright (c) 2014 Dashduino.com
 */
#include "Arduino.h"
#include <UIPEthernet.h>
#include "SPI.h"

#define BUFFER_SIZE 120
#define WEBSOCKET_KEY_LENGTH 24

// #define DEBUG_CLIENT

class DashduinoClient {
    public:
        typedef void (*EventListener)(char *data);
        void setEventListener(EventListener eventListener);
        
        bool connect(char hostname[], int port = 80);
        void disconnect();

        void send(char *eventCode, char *value);
        bool connected();
        void monitor();
        
    private:
        EthernetClient _client;
        EventListener _eventListener;
    
        char *_hostname;
        int _port;
        char _socket_key[WEBSOCKET_KEY_LENGTH];

        char *_data;
        char _buffer[BUFFER_SIZE];
        
        void getWebSocketKey();
        bool readWebSocketKey();
        void sendHandshake();
        bool readHandshake();
        void terminateCommand(void);
        
        void readHeader(void);
        bool waitData(void);
        void readLine();
        void findColon(char which);
};
