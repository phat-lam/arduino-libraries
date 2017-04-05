
// sample application which uses weighless GPS lib
//
// weightLess GPS lib for Arduino
// Rev.0 Reha Yurdakul, 01/04/2013 , rehay@tr.ibm.com
//       initial version of gps lib 
//
// purpose of weighless GPS Lib
// this is very simple library which extracts GPS info from 
// standard NMEA GPS data. 
//
// functions of GSP Lib
//   gps_enable()    // call this function before asking GPS info
//   gps_disable()   // disable GPS when you dont need it any more
//   readGPSInfo(readMode,maxreTry)  // major function to read GPS info
//   readGPSline(char* aGpsData)    // internal function for Lib

////////////////////////////////////////////////////////////////////
//////              GPS Data Section            ////////////////////
////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>
#define INSIST                            1        // insist to get GPS info
#define QUERY                             2        // just process NMEA word
#define GPSDataPin                        1        //change accourding to your board
char gpsTime[15], gpsPos[30], gpsSpeed[7], gpsCourse[7],gpsDate[8];
boolean gpsValid;
SoftwareSerial gpsSerial(GPSDataPin, GPSDataPin);  // SoftSerial declaration
#define    GPS_ENABLE_PIN   0                      // GPS Module pin
/////////////////////  End of GPS Data Section /////////////////////



#define    LED1_PIN         13        // Boot up complete led
#define    LED2_PIN         22        // GSM Network found led
#define    LED3_PIN         23        // SMS sent led


void setup() {

  Serial.begin(9600);
  Serial.println("Hello from simple application which uses weighlessGPS library");

  init_pins(); 
  getGPSPosition();    // sample function to get GPS info
}

void loop() {

}


void init_pins() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);  

  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
}

//  sample function to read GPSinformation 
//  sample function to read GPSinformation 
//  sample function to read GPSinformation 

void  getGPSPosition() {
  gps_enable();
  Serial.println("\n\n\ntry to get GPS Position ( INSIST Mode , 200 try !)");
  for (int i=0; i < 5 ;i++) {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    delay(200);
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, LOW);
    delay(200);
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    delay(200);
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    delay(200);
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, HIGH);
    delay(200);
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    delay(400);
  }

  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  if ( gpsValid == true ) digitalWrite(LED3_PIN, HIGH);    // set the LED off

  digitalWrite(LED1_PIN, HIGH);   // set the LED on
  Serial.println("\n\nReq: Query for GPS info");
  if (readGPSInfo(QUERY,0) == false) Serial.println("  ...GPS position is not valid .. ");
  if ( gpsValid == false ) digitalWrite(LED1_PIN, LOW);    // set the LED off

  digitalWrite(LED2_PIN, HIGH);   // set the LED on
  Serial.println("\n\nReq: Wait for GPS info");
  if (readGPSInfo(INSIST, 200) == false) Serial.println("  ...GPS position is not valid .. ");
  if ( gpsValid == false ) digitalWrite(LED2_PIN, LOW);    // set the LED off

  if ( gpsValid == true ) {
    Serial.print("\nGPS Lock: ");
    Serial.print(gpsValid);
    Serial.print("  GPS Time: ");
    Serial.print(gpsTime);
    Serial.print(" GPS Position: ");
    Serial.print(gpsPos);
    Serial.print(" GPS Speed: ");
    Serial.print(gpsSpeed);
    Serial.print(" GPS Course: ");
    Serial.print(gpsCourse);
    Serial.print(" GPS Date: ");
    Serial.println(gpsDate);
    gps_disable();
    return;
  }
  delay(1000);
}

////////////////////////////////////////////////////////////////////////////////////
/////////////////////// weightlessGPS library code  ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

boolean readGPSInfo (int reqType, int maxINSISTRetry) {
  char gpsData[1000];
  int scanNdx, dataNdx;
  int reTryNdx=0;

  gpsValid=false;
  while (gpsValid == false && reTryNdx < maxINSISTRetry) {

    if (readGPSline(&gpsData[0]) == false) return false;  //GPS Data Read Problem !
    if (strncmp(gpsData, "$GPRMC",6) == 0) {
      digitalWrite(LED3_PIN, HIGH);
      delay(100);
      //      Serial.println(gpsData);
      // GPS TIME
      for ( scanNdx = 7; scanNdx < 13; scanNdx++ ) gpsTime[scanNdx-7]= gpsData[scanNdx];
      while (gpsData[scanNdx] != ',') scanNdx++;   //skip rest
      // GPS Valid?
      if ( gpsData[scanNdx+1] == 'A' ) gpsValid = true; 
      else gpsValid= false;
      if (gpsValid == false && reqType == QUERY) return false;
      scanNdx=scanNdx+3; //skip ',' char
      // GPS Position 
      dataNdx=0;
      while (gpsData[scanNdx] != ',') { 
        gpsPos[dataNdx]= gpsData[scanNdx];
        scanNdx++;
        dataNdx++;
      }
      scanNdx++;  // skip ',' char
      gpsPos[dataNdx++]='*';     
      while (gpsData[scanNdx] != ',') { 
        gpsPos[dataNdx]= gpsData[scanNdx];
        scanNdx++;
        dataNdx++;
      }
      scanNdx++;  // skip ',' char
      gpsPos[dataNdx++]='*';     
      while (gpsData[scanNdx] != ',') { 
        gpsPos[dataNdx]= gpsData[scanNdx];
        scanNdx++;
        dataNdx++;
      }
      scanNdx++;  // skip ',' char
      gpsPos[dataNdx++]='*';     
      while (gpsData[scanNdx] != ',') { 
        gpsPos[dataNdx]= gpsData[scanNdx];
        scanNdx++;
        dataNdx++;
      }
      gpsPos[dataNdx++]='\0';

      // GPS Speed 
      scanNdx++;  // skip ',' char
      dataNdx=0;
      while (gpsData[scanNdx] != ',') { 
        gpsSpeed[dataNdx]= gpsData[scanNdx];
        scanNdx++;
        dataNdx++;
      }
      gpsSpeed[dataNdx++]='\0';

      // GPS Course
      scanNdx++;  // skip ',' char
      dataNdx=0;
      while (gpsData[scanNdx] != ',') { 
        gpsCourse[dataNdx]= gpsData[scanNdx];
        scanNdx++;
        dataNdx++;
      }
      gpsCourse[dataNdx++]='\0';

      // GPS Date
      scanNdx++;  // skip ',' char
      dataNdx=0;
      while (gpsData[scanNdx] != ',') { 
        gpsDate[dataNdx]= gpsData[scanNdx];
        scanNdx++;
        dataNdx++;
      }
      gpsDate[dataNdx++]='\0';

    }
    if ( reTryNdx < maxINSISTRetry ) { 
      digitalWrite(LED3_PIN, LOW);
      //      Serial.print(".");
      reTryNdx++;
    }
  }

  if ( reTryNdx == maxINSISTRetry )  return false;
  return true;
}

boolean readGPSline(char* aGpsData) {
  int ndx=0;
  int maxLineLen=80;
  long readGPSTry=0 , readGPSTryLimit=100000;

  while(readGPSTry < readGPSTryLimit) {
    if (gpsSerial.available()) {
      aGpsData[ndx]= gpsSerial.read();
      if (aGpsData[ndx] == '\n')
        continue;
      if (aGpsData[ndx]== '\r')   {
        aGpsData[ndx+1] = '\0';
        return true; 
      }
      ndx++;
      if (ndx > 100) {
        Serial.print("GPS data exceeding 100 char ! buffer overflow! .." ); 
        Serial.println(ndx);
        return false; 
      }
    }
    readGPSTry++;
  }
  return false;  //max read try exceed !
}

void gps_enable() {
  pinMode(GPS_ENABLE_PIN, OUTPUT);
  Serial.println("Init GPS Unit");
  digitalWrite(GPS_ENABLE_PIN, HIGH); 
  gpsSerial.begin(9600);
}


void gps_disable() {
  Serial.println("Disable GPS Unit");
  digitalWrite(GPS_ENABLE_PIN, LOW); 
}



