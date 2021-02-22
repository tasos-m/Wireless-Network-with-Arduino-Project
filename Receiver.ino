#include <RF22.h>
#include <RF22Router.h>

#define SOURCE_ADDRESS1 2
#define SOURCE_ADDRESS2 5
#define SOURCE_ADDRESS3 7
#define DESTINATION_ADDRESS 10
RF22Router rf22(DESTINATION_ADDRESS);

void setup() {
  Serial.begin(9600);
// RF22 initialization 
  if (!rf22.init())
    Serial.println("RF22 init failed");
// Set frequency   
  if (!rf22.setFrequency(431.0))
    Serial.println("setFrequency Fail");
// Set power and modulation       
  rf22.setTxPower(RF22_TXPOW_20DBM);
  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);
// Manually define the routes for this network
  rf22.addRouteTo(SOURCE_ADDRESS1, SOURCE_ADDRESS1);
  rf22.addRouteTo(SOURCE_ADDRESS2, SOURCE_ADDRESS2);
  rf22.addRouteTo(SOURCE_ADDRESS3, SOURCE_ADDRESS3);
}

void loop() {
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  uint8_t len = sizeof(buf);
  uint8_t from;
  int received_value = 0;
  if (rf22.recvfromAck(buf, &len, &from))
  {
    buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
    Serial.print("INFORMATION RECEIVED BY THE ATHLETE: ");
    Serial.println(from, DEC);    
    const char s[3] = " " ;
    char *token ;
    token = strtok(incoming,s);
    int BPM = atoi(token);
    token = strtok(NULL,s);
    int Rain = atoi(token);
    token=strtok(NULL,s);
    int Temp = atoi(token);
    token = strtok(NULL,s);
    int Hum = atoi(token);
    Serial.print("  ♡ The heart rate is: ");
    Serial.print(BPM);
    Serial.println(" ♡");  
    Serial.println("Weather conditions near the athlete:");
    switch(Rain){
     case 0:
      Serial.println("  ϟ It's raining ");
      break;
     case 1:
      Serial.println("  ☁ Rain warning");
      break;
    case 2:
      Serial.println("  ☀ It's not raining");
      break;
      }
    Serial.print("  🌡 Temperature: ");       
    Serial.print(Temp);
    Serial.println("°C");    
    Serial.print("  🌢 Humidity: ");
    Serial.print(Hum);  
    Serial.println("%");
    Serial.println("");
    Serial.println("");

  }

}
