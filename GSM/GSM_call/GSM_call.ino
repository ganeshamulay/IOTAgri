char phone_no[]="09538069129";
void setup()
{
  Serial.begin(9600);
  Serial.println("AT");
  delay(3000);
  Serial.print("ATD");//Dial the phone number using ATD command
  Serial.print(phone_no);
  Serial.println(";");//Semicolon is compulsory at the end of the AT command
  delay(20000);
  Serial.println("ATH");//Ater a delay of 5s,hang the call
}

void loop()
{
}
