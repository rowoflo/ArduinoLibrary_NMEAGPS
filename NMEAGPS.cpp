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

NMEAGPS::NMEAGPS(const SoftwareSerial &gpsSerial) {
    _gpsSerial = gpsSerial;
    _gpsSerial.listen();
}

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
    if (~_gpsSerial.isListening()) {
        _gpsSerial.listen();
    }
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
    
    // Initialize private variables with getters
    _overflow = false;
    _hour = 0;
    _min = 0;
    _sec = 0;
    _lat = 0;
    _lon = 0;
    _alt = 0;
    _altUnits = '0';
    _geoid = 0;
    _geoidUnits = '0';
    _nSats = 0;
    _PDOP = 0;
    _HDOP = 0;
    _VDOP = 0; 
}

void  NMEAGPS::readGPSStream() {
    if (_gpsSerial.overflow()) {
        _overflow = true;
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
    
    while (_buffer.length() > 1) { // Check if start of message is there
        msgStartInd = _buffer.indexOf('$');
        if (msgStartInd == -1) {
            _buffer = "";
            break;
        }        
        
        msgEndInd = _buffer.indexOf('\n',msgStartInd); // Check if end of message is there
        if (msgEndInd == -1) {
            break;
        }
        
        dollarInd = _buffer.indexOf('$',msgStartInd);
        if (dollarInd != -1 && dollarInd < msgEndInd) { // Check message has no other $ signs
            _buffer = _buffer.substring(dollarInd);
            msgStartInd = 0;
            msgEndInd = msgEndInd - dollarInd;
        }
        
        msgID = _buffer.substring(msgStartInd+1, msgStartInd+6);
        msg = _buffer.substring(msgStartInd+7,msgEndInd);
        
        if (msgID == String("GPGGA")) {
            parseGPGGA(msg);
        } else if (msgID == String("GPGSA")) {
            parseGPGSA(msg);
        } else if (msgID == String("GPGSV")) {
            parseGPGSV(msg);
        } else if (msgID == String("GPRMC")) {
            parseGPRMC(msg);
        }
        
        // Trim off found message
        _buffer = _buffer.substring(msgEndInd+1);
        
        _overflow = false;
    }
    if (_buffer.indexOf('$') == -1) {
        _buffer = "";
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
    char charBuffer[str.length()+1];
    str.toCharArray(charBuffer, str.length()+1);
    return (atoi(charBuffer));
}

float NMEAGPS::stof(const String &str) {
    char charBuffer[str.length()+1];
    str.toCharArray(charBuffer, str.length()+1);
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
        String("-000.0"),
        String("M"),
        String("-000.0"),
        String("M"),
        String(""),
        String("0000*00")
    };
    
    parseMsg(msg, msgParts, 14);
    
    // Time
    _hour = stoi(msgParts[0].substring(0,2));
    _min = stoi(msgParts[0].substring(2,4));
    _sec = stof(msgParts[0].substring(4));
    
    // Lat
    int latD = stoi(msgParts[1].substring(0,2));
    float latM = stof(msgParts[1].substring(2));
    int latS = msgParts[2] == "N" ? 1 : -1;
    _lat = latS * (latD + latM/60);
    
    // Lon
    int lonD = stoi(msgParts[3].substring(0,3));
    float lonM = stof(msgParts[3].substring(3));
    int lonS = msgParts[4] == "E" ? 1 : -1;
    _lon = lonS * (lonD + lonM/60);
    
    // # Sats
    _nSats = stoi(msgParts[6]);
    
    // HDOP
    _HDOP = stof(msgParts[7]);
    
    // Alt
    _alt = stof(msgParts[8]);
    _altUnits = msgParts[9].charAt(0);
    
    // Geoid
    _geoid = stof(msgParts[10]);
    _geoidUnits = msgParts[11].charAt(0);
    
    // Check Sum
    char checkSum1 = msgParts[13].charAt(5);
    char checkSum2 = msgParts[13].charAt(6);   
}

void NMEAGPS::parseGPGSA(String &msg) {
    // Still need to implement
}

void NMEAGPS::parseGPGSV(String &msg) {
    // Still need to implement
}

void NMEAGPS::parseGPRMC(String &msg) {
    String msgParts[] = {
        String("hhmmss.sss"),
        String("V"),
        String("ddmm.mmmm"),
        String("N"),
        String("dddmm.mmmm"),
        String("E"),
        String("000.00"),
        String("000.00"),
        String("ddmmyy"),
        String("E"),
        String("A*00"),
    };
    
    parseMsg(msg, msgParts, 11);
    
    // Time
    _hour = stoi(msgParts[0].substring(0,2));
    _min = stoi(msgParts[0].substring(2,4));
    _sec = stof(msgParts[0].substring(4));
    
    // Status
    _status = msgParts[1] == "A";
    
    // Lat
    int latD = stoi(msgParts[2].substring(0,2));
    float latM = stof(msgParts[2].substring(2));
    int latS = msgParts[3] == "N" ? 1 : -1;
    _lat = latS * (latD + latM/60);
    
    // Lon
    int lonD = stoi(msgParts[4].substring(0,3));
    float lonM = stof(msgParts[4].substring(3));
    int lonS = (msgParts[5] == "E" ? 1 : -1);
    _lon = lonS * (lonD + lonM/60);
    
    // Speed
    _speed = stof(msgParts[6]);
    
    // Course
    _course = stof(msgParts[7]);
    
    // Date
    _day = stoi(msgParts[8].substring(0,2));
    _month = stoi(msgParts[8].substring(2,4));
    _year = stoi(msgParts[8].substring(4,6));
    
    // Magnetic variation
    _magVar = msgParts[9].charAt(0);
    
    // Mode
    _mode = msgParts[10].charAt(0);
    
    // Check Sum
    char checkSum1 = msgParts[10].charAt(1);
    char checkSum2 = msgParts[10].charAt(2); 
}

