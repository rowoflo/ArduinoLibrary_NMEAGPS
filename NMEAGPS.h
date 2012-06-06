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
    void update();
    
    //--------------------------------------------------------------------------
    // Public Member Variables
    //--------------------------------------------------------------------------
    
    
    
protected:
    //--------------------------------------------------------------------------
    // Protected Member Functions
    //--------------------------------------------------------------------------
    
    
    //--------------------------------------------------------------------------
    // Protected Member Variables
    //--------------------------------------------------------------------------
    
    
    
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
    void initialize();
    void readGPSStream();
    void parseBuffer();
    void parseMsg(String &msg, String msgParts[], int nParts);
    int stoi(const String &str);
    float stof(const String &str);
    void parseGPGGA(String &msg);
    
    //--------------------------------------------------------------------------
    // Private Member Variables
    //--------------------------------------------------------------------------
    int _rxPin; // Receive pin
    int _txPin; // Transmit pin
    static const int _baud = 4800; // BAUD rate
    
    SoftwareSerial _gpsSerial; // Softserial object for GPS 
    String _buffer; // Receive data buffer
    
    // GPS Data
    float _time; // UTC Time
    float _lat; // Latitude in deg min sec
    float _lon; // Latitude in deg min sec
};

#endif