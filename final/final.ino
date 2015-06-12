#include <Manchester.h>
String msg = String("");
String NoStore= String("");
String FarmData= String("");
String overall= String("");

int quant;
int WaterData;
float field[11];
float percentage;

// EN: Set to 1 when the next GPRS shield message will contains the SMS message
const int ledTx = 8; //green
const int ledRx = 7; //red
 
const int tx_pin= 12;// tx 
const int rx_pin= 9; //rx

byte flag= 1;
int SmsContentFlag = 0;
byte sms= 0;
int hang=0;
int relay_a=4;
void setup()
{
 pinMode(ledTx,OUTPUT);  // declare pins
 pinMode(ledRx,OUTPUT);
 pinMode( relay_a, OUTPUT );

 digitalWrite(ledTx, HIGH);
 digitalWrite(ledRx, LOW);
 digitalWrite( relay_a, LOW );
 
 man.setupReceive(rx_pin, MAN_1200);  // setup rf module tx and rx
 man.beginReceive();
 man.setupTransmit(tx_pin, MAN_1200);
 

 
 Serial.begin(9600);   // the GPRS baud rate
 
  
 Serial.println("AT");
 delay(2000);
 Serial.println("AT+CLIP=1");
 Serial.println( "AT+CMGF=1" ); 
 delay(200);
}

void loop()
{
    char SerialInByte;
    if(Serial.available())
    {       
        SerialInByte = (unsigned char)Serial.read();
       delay(5);
        
        // -------------------------------------------------------------------
        // EN: Program also listen to the GPRS shield message.
        // -------------------------------------------------------------------
       // EN: If the message ends with <CR> then process the message
        if( SerialInByte == 13 ){
          // EN: Store the char into the message buffer
          ProcessGprsMsg();
         }
         if( SerialInByte == 10 ){
            // EN: Skip Line feed
         }
         else {
           if(msg.length()<50)
           {
           // EN: store the current character in the message string buffer
           msg += String(SerialInByte);
           }
           else {
           }
         }
     }   
}
// EN: Request Text Mode for SMS messaging
void GprsTextModeSMS(){
  Serial.println( "AT+CMGF=1" );
}

void GprsReadSmsStore( String SmsStorePos ){
  Serial.print( "AT+CMGR=" );
  Serial.println( SmsStorePos );
}

void send(String no, String s){
   
    Serial.println("AT+CMGF=1");
    delay(3000);
    Serial.print("AT+CMGS=\"");
    Serial.print(no);
    Serial.println("\"");
    delay(100);
    while(!(msg.substring(msg.indexOf('>'))));
    {
      Serial.print("Data: "+ s + "Send ON or OFF");
      delay(500);
      Serial.write(0x1A); // sends ctrl+z end of message
      Serial.write(0x0D); // Carriage Return in Hex
      Serial.write(0x0A); // Line feed in Hex
      
      delay(5000);
    }
  }


// EN: Clear the GPRS shield message buffer
void ClearGprsMsg(){
  msg = "";
}

// EN: interpret the GPRS shield message and act appropiately
void ProcessGprsMsg() {
  
  if( msg.indexOf( "Call Ready" ) >= 0 ){
     Serial.println( "*** GPRS Shield registered on Mobile Network ***" );
     GprsTextModeSMS();
  }
  
   if( (msg.indexOf( "+CLIP:" ) >= 0)  && (flag==1) )
    {
    // EN: Next message will contains the BODY of SMS
    
    int Pos = msg.indexOf( "+91" );
    NoStore = msg.substring( Pos, Pos+13 );
    Serial.print( "Mobile no: " );
    Serial.println( NoStore );
    
    hang=1;
    flag=0;
    delay(500);
    ClearGprsMsg();
    return;
    }
    
    if(hang==1)
    {
    delay(2000);
    Serial.println("ATH");
    delay(5000);
    hang=0;
    FarmData = getData();
    //Serial.println(FarmData);
    //send(NoStore, FarmData);
    }
  
  // EN: unsolicited message received when getting a SMS message
  if( msg.indexOf( "+CMTI" ) >= 0 ){
     Serial.println( "*** SMS Received ***" );
     // EN: Look for the coma in the full message (+CMTI: "SM",6)
     //     In the sample, the SMS is stored at position 6
     int iPos = msg.indexOf( "," );
     String SmsStorePos = msg.substring( iPos+1 );
     Serial.print( "SMS stored at " );
     Serial.println( SmsStorePos );     
     // EN: Ask to read the SMS store
     GprsReadSmsStore( SmsStorePos );
  }
  
  // EN: SMS store readed through UART (result of GprsReadSmsStore request)  
  if( msg.indexOf( "+CMGR:" ) >= 0 ){
    // EN: Next message will contains the BODY of SMS
    SmsContentFlag = 1;
    // EN: Following lines are essentiel to not clear the flag!
    ClearGprsMsg();
    return;
  }

  // EN: +CMGR message just before indicate that the following GRPS Shield message 
  //     (this message) will contains the SMS body 
  if( SmsContentFlag == 1 ){
    Serial.println( "*** SMS MESSAGE CONTENT ***" );
    Serial.println(msg);
    Serial.println( "*** END OF SMS MESSAGE ***" );
    ProcessSms( msg );
  }
  
  ClearGprsMsg();
  // EN: Always clear the flag
  SmsContentFlag = 0; 
  flag=1;
}
void ProcessSms( String sms ){
  
  if( sms.indexOf("ON") >= 0 ){
    digitalWrite( relay_a, HIGH );
  }
  if( sms.indexOf("OFF") >= 0 ){
    digitalWrite(  relay_a, LOW );
  }
}


void transmitTx(int command) {
 
  delay(50);
  Serial.print("Command for ATtiny: ");
  Serial.println(command);
  digitalWrite(ledTx, HIGH); // Turn on a light to show transmitting
  man.transmit(command);
  digitalWrite(ledTx, LOW); // Turn off a light after transmission
  delay(200);
} // END void loop...

/*rf receiver*/

int receiveRx(){
  Serial.println("running");
   if (man.receiveComplete())
    {
        digitalWrite(ledRx, HIGH);
        WaterData= man.getMessage(); 
        Serial.print("Sensor 1: ");
        Serial.println(WaterData);
        digitalWrite(ledRx, LOW); 
        //man.beginReceive();   
    }
    return WaterData;   
}

String getData()
{
  Serial.println("Getting data from field");
  for(int j=1;j<=1;j++)
  {
    field[j]= water(j);
    overall= overall + "Field " + (String)j + ":" + (String)field[j] + "%";
  }
  Serial.println(overall);
  return overall;  
}

float water(int com)
{
  int x= com;
  int i=0;
  pinMode(ledTx, HIGH);
  
  Serial.println("Sending command");
  transmitTx(x);
  man.beginReceive();
  quant= receiveRx();
  while(quant==0)
  {  
    Serial.println("not yet " + (String)quant);
    quant= receiveRx(); 
  }
  Serial.println(quant);
  percentage= 100*(1-(((float)quant)/1023));
  return percentage;
}

