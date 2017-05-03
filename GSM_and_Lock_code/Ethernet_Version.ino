#include "Adafruit_FONA.h"
#include <SPI.h>
#include <Ethernet.h>

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
IPAddress ip(191,11,1,1); //<<< enter ip here

boolean incoming = 0;

EthernetServer server(80); // Default for HTTP is 80

// Fona protocol 
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// Magnetic sensor pins (each one can be based on location)
const int switchPin1 = 6;
const int switchPin2 = 7;
// additional lock sensors would be const int switchPin3 = ...;

bool cd = false;

bool sent = false; 
int textCounter = 0;

char phoneNum[] = "8155756150";
char message[] = "A break-in has been detected at the solar farm!";

void setup() {
  
  // Setup ethernet shield
  Ethernet.begin(mac, ip);
  server.begin();
  
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
  
  EthernetClient client = server.available();
  
  if (client) {
    
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    
    while (client.connected()) {
      
      if (client.available()) {
        
        char c = client.read();
        
        //reads URL string from $ to first blank space
        if(incoming && c == ' '){ 
          incoming = 0;
        }
        if(c == '$'){ 
          incoming = 1; 
        }
        
        //Checks for the URL string $1 or $2
        if(incoming == 1){
          Serial.println(c);
          
          if(c == '1'){
            Serial.println("ON");
            cd = true;
          }
          if(c == '2'){
            Serial.println("OFF");
            cd = false;
          }
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
  
  if(cd){
    
    if(digitalRead(switchPin2) == HIGH) { // if lock with pin 2 is broken
    
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
  Serial.println(F("All clear"));
  
  }
  } 
}
