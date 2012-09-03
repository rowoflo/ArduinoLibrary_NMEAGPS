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
NMEAGPS::NMEAGPS() {
    NMEAGPS(2, 3);
}

NMEAGPS::NMEAGPS(int rxPin, int txPin) {
    _rxPin = rxPin;
    _txPin = txPin;
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
    _txPin = srcObj._txPin;
    _rxPin = srcObj._rxPin;
    _gpsSerial = srcObj._gpsSerial;
    _buffer = srcObj._buffer;
    _valid = srcObj._valid;
    _hour = srcObj._hour;
    _min = srcObj._min;
    _sec = srcObj._sec;
    _lat = srcObj._lat;
    _lon = srcObj._lon;
    _alt = srcObj._alt;
    _altUnits = srcObj._altUnits;
    _geoid = srcObj._geoid;
    _geoidUnits = srcObj._geoidUnits;
    _nSats = srcObj._nSats;
    _PDOP = srcObj._PDOP;
    _HDOP = srcObj._HDOP;
    _VDOP = srcObj._VDOP;
    _status = srcObj._status;
    _mode = srcObj._mode;
}

//------------------------------------------------------------------------------
// Overloaded Operators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Public Member Functions
//------------------------------------------------------------------------------
void NMEAGPS::initialize() {
    _gpsSerial = SoftwareSerial(_rxPin, _txPin);
    _gpsSerial.begin(_baud);
    _gpsSerial.listen();
    
    // Initialize private variables
    _valid = false;
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

bool NMEAGPS::update() {
    bool updated;
        
    if (readGPSStream()) {       
        parseBuffer();
        updated = true;
    } else {        
        updated = false;
    }
    return updated;    
}

bool NMEAGPS::isListening() {
    return _gpsSerial.isListening();
}

void NMEAGPS::listen() {
    _gpsSerial.listen();
}

int NMEAGPS::available() {
    return _gpsSerial.available();
}

bool NMEAGPS::overflow() {
    return _gpsSerial.overflow();
}

//------------------------------------------------------------------------------
// Protected Member Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Private Member Functions
//------------------------------------------------------------------------------
bool NMEAGPS::readGPSStream() {    
    if (!_gpsSerial.available()) {
        return false;
    }
	while (_gpsSerial.available()) {
        _buffer += (char)_gpsSerial.read();
    }
    return true;
}

void NMEAGPS::parseBuffer() {    
    int msgStartInd, msgEndInd, dollarInd;
    String msgID;
    String msg;
    
    while (_buffer.length() > 1) {
        
        msgStartInd = _buffer.indexOf('$'); // Check if start of message is there
        if (msgStartInd == -1) {
            _buffer = "";
            return;
        }        
        
        msgEndInd = _buffer.indexOf('\n',msgStartInd+1); // Check if end of message is there
        if (msgEndInd == -1) {
            break;
        }
        
        dollarInd = _buffer.indexOf('$',msgStartInd+1);
        if (dollarInd != -1 && dollarInd < msgEndInd) { // Check that message has no other $ chars
            _buffer = _buffer.substring(dollarInd);
            msgStartInd = 0;
            msgEndInd = msgEndInd - dollarInd;
        }
        
        msgID = _buffer.substring(msgStartInd+1, msgStartInd+6);
        msg = _buffer.substring(msgStartInd+1,msgEndInd);
        
        if (msgID == String("GPGGA")) {
            parseGPGGA(msg);
        } else if (msgID == String("GPRMC")) {
            parseGPRMC(msg);
        } else {
            _valid = false;
        }
        
        // Trim off found message
        if (_buffer.length() > msgEndInd+1) {
            dollarInd = _buffer.indexOf('$',msgEndInd+1);
            if (dollarInd != -1) {
                _buffer = _buffer.substring(dollarInd);
            } else {
                _buffer = "";
            }
        } else {
            _buffer = "";
        }
        _buffer = _buffer.substring(msgEndInd+1);
    }
}

bool NMEAGPS::parseMsg(String &msg, String msgParts[], int nParts) {
    
    // Get checksum value by doing Exclusive OR 
    int starInd = msg.indexOf('*');
    
    char checksumMsg = 0;
    if (starInd != -1) {
        for (int i = 0; i < starInd; i++) {
            checksumMsg ^= msg.charAt(i);
        }
    } else {
        return (false);
    }
    
    // Get checksum characters from message
    char checksumTruth;
    char checksumStr[3];
    if (msg.length() > starInd + 2) {
        checksumStr[0] = msg.charAt(starInd+1);
        checksumStr[1] = msg.charAt(starInd+2);
        checksumStr[2] = '\0';
    } else {
        return (false);
    }
    sscanf(checksumStr,"%x",&checksumTruth); // Convert ascii characters to hex value
    
    // Compare checksum values
    if (checksumTruth != checksumMsg) {
        return (false);
    }
    
    // Parse message
    int commaInd = 0;
    for (int i = 0; i < nParts; i++) {
        commaInd = msg.indexOf(',');
        if (commaInd == -1 && i != nParts-1) {
            return (false);
        }
        msgParts[i] = msg.substring(0,commaInd);
        msg = msg.substring(commaInd+1);
    }
    
    // Make sure message has correct amount of fields
    if (msg.indexOf(',') != -1) {
        return (false);
    } else {
        return (true);
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
        String("GPGGA"),
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
    
    // Parse msg
    if (!parseMsg(msg, msgParts, 15)) { // Exit if not a valid message
        _valid = false;
        return;
    } else {
        _valid = true;
    }
    
    // Time
    _hour = stoi(msgParts[1].substring(0,2));
    _min = stoi(msgParts[1].substring(2,4));
    _sec = stof(msgParts[1].substring(4));
    
    // Lat
    int latD = stoi(msgParts[2].substring(0,2));
    float latM = stof(msgParts[2].substring(2));
    int latS = msgParts[3] == "N" ? 1 : -1;
    _lat = latS * (latD + latM/60);
    
    // Lon
    int lonD = stoi(msgParts[4].substring(0,3));
    float lonM = stof(msgParts[4].substring(3));
    int lonS = msgParts[5] == "E" ? 1 : -1;
    _lon = lonS * (lonD + lonM/60);
    
    // # Sats
    _nSats = stoi(msgParts[7]);
    
    // HDOP
    _HDOP = stof(msgParts[8]);
    
    // Alt
    _alt = stof(msgParts[9]);
    _altUnits = msgParts[10].charAt(0);
    
    // Geoid
    _geoid = stof(msgParts[11]);
    _geoidUnits = msgParts[12].charAt(0);
}

void NMEAGPS::parseGPRMC(String &msg) {
    String msgParts[] = {
        String("GPRMC"),
        String("hhmmss.sss"),
        String("V"),
        String("ddmm.mmmm"),
        String("N"),
        String("dddmm.mmmm"),
        String("E"),
        String("000.00"),
        String("000.00"),
        String("ddmmyy"),
        String("00"),
        String("E"),
        String("A*00"),
    };
    
    // Parse msg
    if (!parseMsg(msg, msgParts, 13)) { // Exit if not a valid message
        _valid = false;
        return;
    } else {
        _valid = true;
    }
    
    // Time
    _hour = stoi(msgParts[1].substring(0,2));
    _min = stoi(msgParts[1].substring(2,4));
    _sec = stof(msgParts[1].substring(4));
    
    // Status
    _status = msgParts[2] == "A";
    
    // Lat
    int latD = stoi(msgParts[3].substring(0,2));
    float latM = stof(msgParts[3].substring(2));
    int latS = msgParts[4] == "N" ? 1 : -1;
    _lat = latS * (latD + latM/60);
    
    // Lon
    int lonD = stoi(msgParts[5].substring(0,3));
    float lonM = stof(msgParts[5].substring(3));
    int lonS = (msgParts[6] == "E" ? 1 : -1);
    _lon = lonS * (lonD + lonM/60);
    
    // Speed
    _speed = stof(msgParts[7]);
    
    // Course
    _course = stof(msgParts[8]);
    
    // Date
    _day = stoi(msgParts[9].substring(0,2));
    _month = stoi(msgParts[9].substring(2,4));
    _year = stoi(msgParts[9].substring(4,6));
    
    // Magnetic variation
    _magVar = msgParts[10].charAt(0);
    
    // Mode
    _mode = msgParts[12].charAt(0);
}

