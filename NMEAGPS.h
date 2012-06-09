/*
 -------------------------------------------------------------------------------
 NMEAGPS.h
 NMEAGPS Project
 
 CLASS NAME:
    NMEAGPS
 
 DESCRIPTION:
    description...
 
 FILES:
    NMEAGPS.h
    NMEAGPS.cpp

 DEPENDENCIES:
    
 
 CONSTRUCTORS:
    NMEAGPS();    
 
 PUBLIC PROPERTIES:
    prop1 - description... .

    prop2 - description... .
 
 PUBLIC METHODS:
    type method1(type arg1);
        Description... .
            arg1 - description... .
 
 NOTES:
 
 
 EXAMPLES:
    Example 1: description
    ----------------------------------------------------------------------------
    code...
    ----------------------------------------------------------------------------
 
 
 VERSIONS:
    1.0 - 5/28/12 - Rowland O'Flaherty (___WEBSITE___)
 
 -------------------------------------------------------------------------------
 */

#ifndef _NMEAGPS_h_
#define _NMEAGPS_h_

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <Arduino.h>
#include <SoftwareSerial.h>


class NMEAGPS {
    //--------------------------------------------------------------------------
    // Friends
    //--------------------------------------------------------------------------
    
    
    //--------------------------------------------------------------------------
    // Friend Overloaded Operators
    //--------------------------------------------------------------------------
    
    
public:
    //--------------------------------------------------------------------------
    // Constants, Enums, and Types
    //--------------------------------------------------------------------------
	
    
    //--------------------------------------------------------------------------
    // Lifecycle
    //--------------------------------------------------------------------------
    // Constructors
    NMEAGPS();
    NMEAGPS(int rxPin, int txPin);
    NMEAGPS(const SoftwareSerial &gpsSerial);
    
    // Destructor
    virtual ~NMEAGPS();
    
    // Copy constructor
    NMEAGPS(const NMEAGPS& srcObj);
    
    // Assignment operator
    const NMEAGPS& operator=(const NMEAGPS& rhsObj);
    
    
    //--------------------------------------------------------------------------
    // Overloaded Operators
    //--------------------------------------------------------------------------
    
    
    //--------------------------------------------------------------------------
    // Public Member Functions
    //--------------------------------------------------------------------------
    void initialize();
    void update();
    bool isListening();
    void listen();
    
    // Getters
    bool overflow() {return _overflow;};
    int hour() {return _hour;}
    int minute() {return _min;}
    float second() {return _sec;}
    float latitude() {return _lat;}
    float longitude() {return _lon;}
    float altitude() {return _alt;}
    char altUnits() {return _altUnits;}
    float speed() {return _speed;}
    float course() {return _course;}
    float geoid() {return _geoid;}
    char geoidUnits() {return _geoidUnits;}
    int numberOfSats() {return _nSats;}
    float PDOP() {return _PDOP;}
    float HDOP() {return _HDOP;}
    float VDOP() {return _VDOP;}
    
    
    
private:
    //--------------------------------------------------------------------------
    // Constants, Enums, and Types
    //--------------------------------------------------------------------------
    
    
    //--------------------------------------------------------------------------
    // Lifecycle
    //--------------------------------------------------------------------------
    void copyHelper(const NMEAGPS& srcObj);
    
    //--------------------------------------------------------------------------
    // Private Member Functions
    //--------------------------------------------------------------------------
    void readGPSStream();
    void parseBuffer();
    void parseMsg(String &msg, String msgParts[], int nParts);
    int stoi(const String &str);
    float stof(const String &str);
    void parseGPGGA(String &msg);
    void parseGPGSA(String &msg);
    void parseGPGSV(String &msg);
    void parseGPRMC(String &msg);
    
    //--------------------------------------------------------------------------
    // Private Member Variables
    //--------------------------------------------------------------------------
    int _rxPin; // Receive pin
    int _txPin; // Transmit pin
    static const int _baud = 4800; // BAUD rate
    
    SoftwareSerial _gpsSerial; // Softserial object for GPS 
    String _buffer; // Receive data buffer
    
    bool _overflow; // Overflow flag
    
    // GPS Data
    int _year;
    int _month;
    int _day;
    int _hour; // UTC time hour (0 to 23)
    int _min; // UTC time min (0 to 59)
    float _sec; // UTC time sec (0 to 59)
    
    float _lat; // Latitude in deg (-90 to 90)
    float _lon; // Latitude in deg (-180 to 180)
    
    float _alt; // MSL altitute
    char _altUnits; // Units for altitute measurement
    float _geoid; // Geoid separation
    char _geoidUnits; // Units for geoid
    
    float _speed; // Ground speed in knots
    float _course; // Course over ground in degree from true north
    
    char _magVar; // Magnetic variation E or W
    
    int _nSats; // Number of satellites used (0 to 12)
    
    float _PDOP; // Position dilution of precision
    float _HDOP; // Horizontal dilution of precision 
    float _VDOP; // Horizontal dilution of precision
    
    bool _status; // Valid data flag
    char _mode; // Mode of operation
    
    
};

#endif