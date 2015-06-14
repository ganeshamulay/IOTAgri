#include <Manchester.h>


//#include <Manchester.h>

const int ledTx = 8; //green
const int ledRx = 7; //red
const int Sensor1Pin = A2; //sensor data 
const int tx_pin= 12;// tx 
const int rx_pin=11; //rx

int Sensor1Data; // tx 
int alert; // rx
int command=0;

void setup() {
 pinMode(ledTx,OUTPUT);
 pinMode(ledRx,OUTPUT);
 pinMode(Sensor1Pin,INPUT);
 digitalWrite(ledTx, LOW);
 digitalWrite(ledRx, HIGH);
 man.setupReceive(rx_pin, MAN_1200);
 man.beginReceive();
 man.setupTransmit(tx_pin, MAN_1200);

 Serial.begin(9600); 
 
}
void transmitTx() {
   
  delay(50);
 
  // Read and store Sensor 1 data
  Sensor1Data = analogRead(Sensor1Pin);
  Serial.print("Sensor1 Integer: ");
  Serial.println(Sensor1Data);

 digitalWrite(ledTx, HIGH); // Turn on a light to show transmitting
 man.transmit(Sensor1Data);
 digitalWrite(ledTx, LOW); // Turn off a light after transmission
} // END void loop...

int receiveRx(){
     alert=0; 
  if (man.receiveComplete())
    {
        digitalWrite(ledRx, true); 
        alert= man.getMessage();
        Serial.print("alert 1: ");
        Serial.println(alert);
        digitalWrite(ledRx, false);
        man.beginReceive();
       
        
    }
    return alert;
}
 void loop()
{
  pinMode(ledRx, HIGH);
   if(command==0)
  {
   
    //delay(100);
    Serial.println("Reception on");
    command= receiveRx();
    
    alert=0;
  }
  if(command==1)
  {
      Serial.println("Reception stopped");
    transmitTx();
    command = receiveRx();
    while((char)command != '&' )
    {
          transmitTx();
          command= receiveRx();
    }
    man.beginReceive();
    Serial.println("transmitted");
  }
  command= 0;
}

