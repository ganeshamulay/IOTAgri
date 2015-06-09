//#include <SoftwareSerial.h>
#include <VirtualWire.h>

String msg = String("");
String NoStore= String("");
String FarmData= String("");

int  com, quant;
float field1, field2, field3, field4, percentage;
String overall= String("");
// EN: Set to 1 when the next GPRS shield message will contains the SMS message
const int ledTx = 8; //green
const int ledRx = 7; //red
 
const int tx_pin= 12;// tx 
const int rx_pin= 11; //rx

int Command; // tx 
char CommandMsg[5]; 
int WaterData; // rx
char WaterDataMsg[5];


byte flag= 1;
int SmsContentFlag = 0;
byte sms= 0;
int hang=0;
int relay_a=4;
void setup()
{
 pinMode(ledTx,OUTPUT);
 pinMode(ledRx,OUTPUT);
 //pinMode(Sensor1Pin,INPUT);
 digitalWrite(ledTx, HIGH);
 digitalWrite(13, LOW);
 digitalWrite(ledRx, LOW);
 
 vw_setup(2000);
 
  Serial.begin(9600);   // the GPRS baud rate
  pinMode( 4, OUTPUT );
  digitalWrite( 4, LOW );
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
      Serial.print(s + " To on the Pump Send ON, To switch off the motor send OFF!!");
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
  
      if( (msg.indexOf( "+CLIP:" ) >= 0)  && (flag==1) )
    {
    // EN: Next message will contains the BODY of SMS
    
    int Pos = msg.indexOf( "+91" );
    NoStore = msg.substring( Pos, Pos+13 );
    Serial.print( "Mobile no: " );
    Serial.println( NoStore );
    
    hang=1;
    
    
    flag=0;
    
    // EN: Following lines are essentiel to not clear the flag!
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
    Serial.println(FarmData);
    //send(NoStore, FarmData);
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
}
void ProcessSms( String sms ){
  
  if( sms.indexOf("ON") >= 0 ){
    digitalWrite( relay_a, HIGH );
  }
  if( sms.indexOf("OFF") >= 0 ){
    digitalWrite(  relay_a, LOW );
  }
}

void onRec() {
    // Initialise the IO and ISR
    // Required for DR3100
    vw_set_ptt_inverted(true); 
    // Start the receiver PLL running
    vw_set_rx_pin(rx_pin);
    vw_rx_start();       
}

void offRec()
{
  vw_rx_stop();
}

void transmit(int Command) {
  vw_set_tx_pin(tx_pin);
   // Convert integer data to Char array directly 
  itoa(Command,CommandMsg,10);
  
  
  Serial.print("Command for ATtiny ");
  Serial.print(Command);
  Serial.print(" Command msg for ATTiny: ");
  Serial.print(CommandMsg);
  Serial.println(" ");
 
 digitalWrite(ledTx, LOW); // Turn on a light to show transmitting
 vw_send((uint8_t *)CommandMsg, strlen(CommandMsg));

 vw_wait_tx(); // Wait until the whole message is gone
 //digitalWrite(ledTx, false); // Turn off a light after transmission
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
          WaterDataMsg[i] = char(buf[i]);
	}
        WaterDataMsg[buflen] = '\0';
        
        // Convert CommandMsg Char array to integer
        WaterData = atoi(WaterDataMsg);
        Serial.print("Sensor 1: ");
        Serial.println(WaterData);
        return WaterData;

                
        // Turn off light to and await next message 
        //digitalWrite(ledRx, false);
        
    }
    
}

String getData()
{
  Serial.println("Getting data from field 1");
  field1= water(1);
  Serial.println("Field 1 has " + (String)field1 + "% Water");
  
  /*Serial.println("Getting data from field 2");
  field2= water(2);
  Serial.println("Field 2 has " + (String)field2 + "% Water");
  
  Serial.println("Getting data from field 3");
  field3= water(3);
  Serial.println("Field 3 has " + (String)field3 + "% Water");
  
  Serial.println("Getting data from field 4");
  field1= water(4);
  Serial.println("Field 4 has " + (String)field4 + "% Water");*/
  
  overall = ("Field 1: "+ (String)field1 /*+ "% Field 2: "+ (String)field2 + "% Field 3: "+ (String)field3 + "% Field 4: "+ (String)field4 + "%"*/  );
  return overall;
  
}

float water(int com)
{
  offRec();
  Serial.println("Transmitting command for field " + (String)com );
  transmit(com);
  Serial.println("Receiving data from field" + (String)com );
  onRec();
  quant= receive();
  /*while(quant==0)
  {
    offRec();
    transmit(com);
    onRec();
    quant= receive();
    Serial.println(quant);
   }*/
  percentage = 100*(1023- quant)/1023;
  Serial.println(quant);
  return percentage;
}

