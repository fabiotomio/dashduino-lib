/*
 Dashduino Client
 
 Site: http://dashduino.com
 Author: fabiotomio@gmail.com


 The MIT License (MIT)
 Copyright (c) 2014 Dashduino.com
 */
#include <DashduinoClient.h>

// Start flow of connection with Dashduino platform
bool DashduinoClient::connect(char host[], int port) {
    _hostname = host;
    _port = port;

    _client.stop();
    _client.connect(_hostname, _port);

    getWebSocketKey();
    readWebSocketKey();
    delay(1000);
    sendHandshake();
    return readHandshake();
}

// Get websocket Key to HandShake
// from Dashduino platform
void DashduinoClient::getWebSocketKey() {
    _client.println(F("GET /socket.io/1/ HTTP/1.1"));
    _client.print(F("Host: "));
    _client.println(_hostname);
    _client.println(F("Origin: Arduino\r\n"));
}

bool DashduinoClient::readWebSocketKey() {
    if (!waitData())
        return false;

    readLine();
    if (atoi(&_buffer[9]) != 200) {        
        Serial.println(F("Houston we have a ploblem! Dashduino platform is down! =("));
        Serial.println(F("Please contact mailto:fabiotomio@gmail.com"));
        _client.stop();
        return false;
    }
    readHeader();
    readLine(); // read first line of response
    readLine(); // read sid : transport : timeout

    char *iptr = _buffer;
    char *optr = _socket_key;
    while (*iptr && (*iptr != ':') && (optr < &_socket_key[WEBSOCKET_KEY_LENGTH-2])) 
        *optr++ = *iptr++;
    *optr = 0;

#ifdef DEBUG_CLIENT
    Serial.print(F("Socket Key: "));
    Serial.println(_socket_key); 
#endif

    while (_client.available())
        readLine();

    return true;
}

// Send Handshake to Dashduino platform
// Reference: http://tools.ietf.org/html/rfc6455#page-6
void DashduinoClient::sendHandshake() {
#ifdef DEBUG_CLIENT
    Serial.println(F("Send Handshake."));
#endif
    _client.print(F("GET /socket.io/1/websocket/"));
    _client.print(_socket_key);
    _client.println(F(" HTTP/1.1"));
    _client.print(F("Host: "));
    _client.println(_hostname);
    _client.println(F("Origin: Dashduino"));
    _client.println(F("Upgrade: WebSocket"));
    _client.println(F("Connection: Upgrade\r\n"));
}

bool DashduinoClient::readHandshake() {

    if (!waitData()) 
        return false;

    readLine();
    if (atoi(&_buffer[9]) != 101) {
        while (_client.available())
            readLine();
        _client.stop();
        return false;
    }
    readHeader();
    readLine();
#ifdef DEBUG_CLIENT
    Serial.println(F("Connected."));
#endif
    return true;
}

void DashduinoClient::send(char *eventCode, char *value) {
    if (_client.connected()) {
#ifdef DEBUG_CLIENT
        Serial.print("Send event: ");
        Serial.print(eventCode);
        Serial.print(" value: ");
        Serial.println(value);
#endif
        _client.print((char)0);
        _client.print("5:::{\"name\":\"");
        _client.print(eventCode);
        _client.print("\",\"args\":[\"");
        _client.print(value);
        _client.print("\"]}");
        _client.print((char)255);
        delay(1000);
    }
}


// Event listener
void DashduinoClient::setEventListener(EventListener eventListener) {
      _eventListener = eventListener;
}

void DashduinoClient::monitor() {

    *_buffer = 0;

    if (!_client.connected()) {
        while (!connect(_hostname, _port)) {
#ifdef DEBUG_CLIENT            
            Serial.println(F("Try reconnect to Platform."));
#endif
            delay(10000);
        }
    }

    if (!_client.available()) return;

    char which;
    while (_client.available()) {
        readLine();
        _data = _buffer;
        switch (_buffer[0]) {    
            // CONNECT
            case '1':
                which = 6;
                break;
            // HEARTBEAT
            case '2':
                _client.print((char)0);
                _client.print("2::");
                _client.print((char)255);
                continue;
             // EVENT
            case '5':
                which = 4;
                break;

            default: 
#ifdef DEBUG_CLIENT            
                Serial.print("Drop: ");
                Serial.println(_data);
#endif
                continue;
        }

        findColon(which);
        _data += 2;

        // handle backslash-delimited escapes
        char *optr = _buffer;
        while (*_data && (*_data != '"')) {
            if (*_data == '\\') {
                ++_data;      // todo: this just handles "; handle \r, \n, \t, \xdd
            }
            *optr++ = *_data++;
        }
        *optr = 0;

        if (_eventListener != NULL) _eventListener(_buffer);
    }
}

bool DashduinoClient::connected() {
    return _client.connected();
}

void DashduinoClient::disconnect() {
    _client.stop();
}

// Utils ----------------------------------------------------------------------
void DashduinoClient::findColon(char which) {    
    while (*_data) {
        if (*_data == ':') {
            if (--which <= 0) return;
        }
        ++_data;
    }
}

void DashduinoClient::terminateCommand(void) {
    _data[strlen(_data)-3] = 0;
}

void DashduinoClient::readHeader(void) {
    while (_client.available()) {
        readLine();
        if (strlen(_buffer) == 0) break;
    }
}

bool DashduinoClient::waitData(void) {
    unsigned long now = millis();
    while (!_client.available() && ((millis() - now) < 30000UL)) {;}
    return _client.available();
}

void DashduinoClient::readLine() {
    _data = _buffer;
    while (_client.available() && (_data < &_buffer[BUFFER_SIZE-2])) {
        char c = _client.read();
        if (c == 0) {;}
        else if (c == 255) {;}
        else if (c == '\r') {;}
        else if (c == '\n') break;
        else *_data++ = c;
    }
    *_data = 0;
}