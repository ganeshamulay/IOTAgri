int timeTosend=1;
int count=0;
String phone_no=String("+919972933658");
void setup()
{
  Serial.begin(9600);
  delay(3000);
  Serial.println("AT+CMGF=1");//set GSM to text mode
  delay(200);
  send();
}
void send()
{
  while(count<timeTosend){
    delay(1500);
    Serial.print("AT+CMGS=\"");
    Serial.print(phone_no);
    Serial.println("\"");
    while(Serial.read()!='>');
    {
      Serial.print("Test Message from Arduino GSM...HELLO..!! Ganesh");
      delay(500);
      Serial.println(26, DEC); // sends ctrl+z end of message
      //Serial.write(0x0D); // Carriage Return in Hex
      //Serial.write(0x0A); // Line feed in Hex
      
      delay(5000);
    }
    count++;
  }
}
void loop() {
}
