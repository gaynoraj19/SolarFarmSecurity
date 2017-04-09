#include "Adafruit_FONA.h"
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

// Fona protocol 
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// Magnetic sensor pins
const int switchPin1 = 6;
const int switchPin2 = 7;
// additional lock sensors would be const int switchPin3 = ...;

bool sent = false; 
int textCounter = 0;

char phoneNum[] = "8155756150";
char message[] = "A break-in has been detected at the solar farm!";

void setup() {
  
  // Setup magnetic lock sensor pins
  pinMode(switchPin1, INPUT);
  pinMode(switchPin2, INPUT);
  digitalWrite(switchPin1, HIGH);
  digitalWrite(switchPin2, HIGH);

  //Finding FONA sequence  
  while (!Serial);

  Serial.begin(115200);
  Serial.println(F("FONA test"));
  Serial.println(F("Initializing...(May take time)"));

  fonaSerial->begin(4800);
  
  if (! fona.begin(*fonaSerial)) {
    
    Serial.println(F("Couldn't find FONA"));
    while (1);
    
  }
  
  Serial.println(F("FONA is OK"));
  Serial.print(F("Found "));
  
  //Identify FONA version
  switch (fona.type()) {
    
    case FONA800H:
      Serial.println(F("FONA 800H")); break;
    default: 
      Serial.println(F("???")); break;
      
  }
}

void loop() {
  
  if(digitalRead(switchPin2) == HIGH) { // if lock is brocken
    
    if(!sent) {  // Checking sent condition then sending text
      
      Serial.flush();
      Serial.println(phoneNum);
      Serial.println(message);
    
    if (!fona.sendSMS(phoneNum, message)) { // send confirmation protocol
      
      Serial.println(F("Failed to send text message"));
    
    } else {
      
      Serial.println(F("Text message sent"));
      textCounter++; // Add to text count
      sent = true; // Text message condition 
      delay(600000); // Delay (10 minutes) in milliseconds
      
      }
    }
  
  } else {
 
  sent = false;
  Serial.println(F("check"));
  
  } 
}
