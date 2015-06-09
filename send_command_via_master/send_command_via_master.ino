/* 

Sensor Transmitter 
By Markus Ulfberg 2012-07-06

Takes a sensor reading 0-1023
converts it to a char array and sends 
to RF receiver unit via VirtualWire  

*/

#include <VirtualWire.h>

// set up a new serial port
//SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);

// LED's
const int ledPin = 13;
const int tx_pin= 40;// tx

// Sensors 
//const int Sensor1Pin = A2;
// const int Sensor2Pin = 3; 

int command=1;
// char Sensor1CharMsg[4]; 
// Trying to use 5 instead to fit trailing null char 
// go back to 4 if this does not work. 
char commandMsg[5]; 


void setup() {

 // PinModes 
 // LED 
 pinMode(ledPin,OUTPUT);
 //pinMode(rxPin, INPUT);
  //pinMode(txPin, OUTPUT);
 // Sensor(s)
 //pinMode(Sensor1Pin,INPUT);
 vw_set_tx_pin(tx_pin);
 
 // for debugging
 Serial.begin(9600); 
 
 // VirtualWire setup
 vw_setup(2000);	 // Bits per sec


}

void loop() {
  
  // Read and store Sensor 1 data
  //Sensor1Data = analogRead(Sensor1Pin);
  
  // Convert integer data to Char array directly 
  itoa(command,commandMsg,10);
  
  
  // DEBUG
  Serial.print("Command Integer: ");
  Serial.print(command);
  Serial.print(" Command CharMsg: ");
  Serial.print(commandMsg);
  Serial.println(" ");

  // END DEBUG
  
 
 digitalWrite(13, true); // Turn on a light to show transmitting
 vw_send((uint8_t *)commandMsg, strlen(commandMsg));

 vw_wait_tx(); // Wait until the whole message is gone
 digitalWrite(13, false); // Turn off a light after transmission
 delay(200); 
 
} // END void loop...
