/*
 -------------------------------------------------------------------------------
 NMEAGPS.cpp
 NMEAGPS Project
 
 Initially created by Rowland O'Flaherty (___WEBSITE___) on 5/28/12.
 
 Version 1.0
 -------------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "NMEAGPS.h"

//------------------------------------------------------------------------------
// Friends
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Friend Overloaded Operators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Lifecycle
//------------------------------------------------------------------------------
// Constructors
NMEAGPS::NMEAGPS()
:
_rxPin(2),
_txPin(3) {
    NMEAGPS(_rxPin, _txPin);
}

NMEAGPS::NMEAGPS(int rxPin, int txPin) {
    initialize();
}

//NMEAGPS::NMEAGPS(SoftwareSerial &gpsSerial) {
//    _gpsSerial = gpsSerial;
//}


// Destructor
NMEAGPS::~NMEAGPS() {
    
}

// Copy constructor
NMEAGPS::NMEAGPS(const NMEAGPS& srcObj) {
    copyHelper(srcObj);
}

// Assignment operator
const NMEAGPS& NMEAGPS::operator=(const NMEAGPS& rhsObj) {
	// Self-assignment check
    if (this == &rhsObj) {
        return (*this);
    }
    
    // Free old memory
    
    // Copy new memory
    copyHelper(rhsObj);
    
    return (*this);
}

// Copy helper function
void NMEAGPS::copyHelper(const NMEAGPS& srcObj) {
    
}

//------------------------------------------------------------------------------
// Overloaded Operators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Public Member Functions
//------------------------------------------------------------------------------
void NMEAGPS::update() {
    readGPSStream();
    parseBuffer();
}

//------------------------------------------------------------------------------
// Protected Member Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Private Member Functions
//------------------------------------------------------------------------------
void NMEAGPS::initialize() {
    _gpsSerial = SoftwareSerial(2, 3);
    _gpsSerial.begin(_baud);
    _gpsSerial.listen();
}

void  NMEAGPS::readGPSStream() {
    if (_gpsSerial.overflow()) {
        _buffer = "";
    }
    
	while (_gpsSerial.available()) {
        _buffer += (char)_gpsSerial.read();
    }
}

void NMEAGPS::parseBuffer() {
    int msgStartInd, msgEndInd, dollarInd;
    String msgID;
    String msg;
    
    while (_buffer.length() > 1) {
        msgStartInd = _buffer.indexOf('$');
        if (msgStartInd == -1) {
            _buffer = "";
            break;
        }        
        
        msgEndInd = _buffer.indexOf('\n',msgStartInd);
        if (msgEndInd == -1) {
            break;
        }
        
        dollarInd = _buffer.indexOf('$',msgStartInd);
        if (dollarInd != -1 && dollarInd < msgEndInd) {
            _buffer = _buffer.substring(dollarInd);
            msgStartInd = 0;
            msgEndInd = msgEndInd - dollarInd;
        }
        
        msgID = _buffer.substring(msgStartInd+1, msgStartInd+6);
        msg = _buffer.substring(msgStartInd+7,msgEndInd);
        
        if (msgID == String("GPGGA")) {
            parseGPGGA(msg);
        } else if (msgID == String("GPGSA")) {
            Serial.println("GPGSA");
        } else if (msgID == String("GPRMC")) {
            Serial.println("GPRMC");
        } else if (msgID == String("GPGSV")) {
            Serial.println("GPGSV");
        }
        
        // Trim off found message
        _buffer = _buffer.substring(msgEndInd+1);
    }
}

void NMEAGPS::parseMsg(String &msg, String msgParts[], int nParts) {
    int commaInd = 0;
    for (int i = 0; i < nParts; i++) {
        commaInd = msg.indexOf(',');
        msgParts[i] = msg.substring(0,commaInd);
        msg = msg.substring(commaInd+1);
    }
}

int NMEAGPS::stoi(const String &str) {
    char charBuffer[str.length()];
    str.toCharArray(charBuffer, str.length());
    return (atoi(charBuffer));
}

float NMEAGPS::stof(const String &str) {
    char charBuffer[str.length()];
    str.toCharArray(charBuffer, str.length());
    return (atof(charBuffer));
}

void NMEAGPS::parseGPGGA(String &msg) {
    String msgParts[] = {
        String("hhmmss.sss"), 
        String("ddmm.mmmm"), 
        String("N"),
        String("dddmm.mmmm"),
        String("E"),
        String("0"),
        String("00"),
        String("0000.0"),
        String("000.0"),
        String("M"),
        String(""),
        String("0000"),
        String("*00")};
    
    parseMsg(msg, msgParts, 13);
    
    _time = stof(msgParts[0]);
    _lat = stof(msgParts[1]);
    
    Serial.println("Time");
    Serial.println(msgParts[0]);
    Serial.println(_time,3);
    Serial.println("Lat");
    Serial.println(msgParts[1]);
    Serial.println(_lat,4);
}


