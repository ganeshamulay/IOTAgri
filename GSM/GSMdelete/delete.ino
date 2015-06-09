void setup() {
 Serial.begin(9600);                 // the GPRS baud rate
 
 Serial.print( "AT+CMGDA=" );
 Serial.print("\"");
 Serial.print("DEL ALL");
 Serial.println("\"");

 
 delay(200);
}

void loop() {
  // put your main code here, to run repeatedly:

}
