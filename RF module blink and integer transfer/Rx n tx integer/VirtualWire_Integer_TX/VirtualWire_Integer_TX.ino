/* 

Sensor Transmitter 
By Markus Ulfberg 2012-07-06

Takes a sensor reading 0-1023
converts it to a char array and sends 
to RF receiver unit via VirtualWire  

*/

#include <VirtualWire.h>

#include <SoftwareSerial.h>

const int rxPin = 2;
const int txPin = 3;

// set up a new serial port
//SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);

// LED's
const int ledPin = 8;
const int tx_pin= 12;// tx

// Sensors 
const int Sensor1Pin = A2;
// const int Sensor2Pin = 3; 

int Sensor1Data;
// char Sensor1CharMsg[4]; 
// Trying to use 5 instead to fit trailing null char 
// go back to 4 if this does not work. 
char Sensor1CharMsg[5]; 


void setup() {

 // PinModes 
 // LED 
 pinMode(ledPin,OUTPUT);
 pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
 // Sensor(s)
 pinMode(Sensor1Pin,INPUT);
 vw_set_tx_pin(tx_pin);
 
 // for debugging
 Serial.begin(9600); 
 
 // VirtualWire setup
 vw_setup(300);	 // Bits per sec


}

void loop() {
  
  // Read and store Sensor 1 data
  Sensor1Data = analogRead(Sensor1Pin);
  
  // Convert integer data to Char array directly 
  itoa(Sensor1Data,Sensor1CharMsg,10);
  
  
  // DEBUG
  Serial.print("Sensor1 Integer: ");
  Serial.print(Sensor1Data);
  Serial.print(" Sensor1 CharMsg: ");
  Serial.print(Sensor1CharMsg);
  Serial.println(" ");

  // END DEBUG
  
 
 digitalWrite(8, true); // Turn on a light to show transmitting
 vw_send((uint8_t *)Sensor1CharMsg, strlen(Sensor1CharMsg));

 vw_wait_tx(); // Wait until the whole message is gone
 digitalWrite(8, false); // Turn off a light after transmission
 delay(200); 
 
} // END void loop...
