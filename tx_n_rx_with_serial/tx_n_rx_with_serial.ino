
//Sensor Transmitter

#include <VirtualWire.h>
//#include <SoftwareSerial.h>

//Serial pins

//const int rxSerial = 2;
//const int txSerial = 3;

// set up a new serial port
//SoftwareSerial Serial = SoftwareSerial(rxSerial, txSerial);

const int ledTx = 8; //green
const int ledRx = 7; //red
const int Sensor1Pin = A2; //sensor data 
const int tx_pin= 12;// tx 
const int rx_pin=11; //rx

int Sensor1Data; // tx 
char Sensor1CharMsg[5]; 
int alert; // rx
char alertMsg[5];
int command=0;
 


void setup() {
 pinMode(ledTx,OUTPUT);
 pinMode(ledRx,OUTPUT);
 pinMode(Sensor1Pin,INPUT);
 digitalWrite(ledTx, LOW);
 digitalWrite(ledRx, HIGH);
 
 Serial.begin(9600); 
 
 // VirtualWire setup
 vw_setup(2000);	 // Bits per sec
 vw_set_tx_pin(tx_pin);
 vw_set_ptt_inverted(true); 
    // Start the receiver PLL running
    vw_set_rx_pin(rx_pin);
}


void onRec() {
    // Initialise the IO and ISR
    // Required for DR3100
    
    vw_rx_start();       
}

void offRec()
{
  vw_rx_stop();
}

void transmit() {
  
  
  // Read and store Sensor 1 data
  Sensor1Data = analogRead(Sensor1Pin);
  
  // Convert integer data to Char array directly 
  itoa(Sensor1Data,Sensor1CharMsg,10);
  
  
  Serial.print("Sensor1 Integer: ");
  Serial.print(Sensor1Data);
  Serial.print(" Sensor1 CharMsg: ");
  Serial.print(Sensor1CharMsg);
  Serial.println(" ");
 
 digitalWrite(ledTx, true); // Turn on a light to show transmitting
 vw_send((uint8_t *)Sensor1CharMsg, strlen(Sensor1CharMsg));

 vw_wait_tx(); // Wait until the whole message is gone
 digitalWrite(ledTx, false); // Turn off a light after transmission
 delay(200); 

} // END void loop...

/*rf receiver*/




int receive(){
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    
    
    // Non-blocking
    if (vw_get_message(buf, &buflen)) 
    {
	int i;
        // Turn on a light to show received good message 
        digitalWrite(ledRx, true); 
	
        // Message with a good checksum received, dump it. 
        for (i = 0; i < buflen; i++)
	{               
          alertMsg[i] = char(buf[i]);
	}
        alertMsg[buflen] = '\0';
        
        // Convert Sensor1CharMsg Char array to integer
        alert = atoi(alertMsg);
        Serial.print("alert 1: ");
        Serial.println(alert);
        Serial.println(alertMsg);

                
        // Turn off light to and await next message 
        digitalWrite(ledRx, false);
        return alert;
    }
}
void loop()
{
  if(command==0)
  {
    onRec();
    Serial.println("Reception on");
    command= receive();
  }
  if(command==1)
  {
    offRec();
    Serial.println("Reception stopped");
    delay(1000);
    transmit();
    
    delay(1000);
    onRec();
    Serial.println("on again");
  }
  command= 0;
  //delay(2000);
  
}

