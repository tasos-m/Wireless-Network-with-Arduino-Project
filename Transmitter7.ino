#include <RF22.h>
#include <RF22Router.h>
#define SOURCE_ADDRESS 7 //Node 7
#define DESTINATION_ADDRESS 10

//Variables and object for ALOHA Protocol and RF22
long randNumber;
int sensorVal= 23;
int max_delay=3000;                               
boolean successful_packet=false;
RF22Router rf22(SOURCE_ADDRESS);

// Variables initialized for success rate
int retransmissions=0;
int successfuTransmissions=0;
float percentage;

// Variables initalized for throughput
int final_time;
int initial_time;
int delta_time;
int number_of_bytes=0;
float throughput=0;

void setup() {   

  Serial.begin(9600);  
  // RF22 initialization         
  if (!rf22.init()){
     Serial.println("RF22 init failed");}
  // Set frequency        
  if (!rf22.setFrequency(431.0)){
     Serial.println("setFrequency Fail");}
  // Set power and modulation       
  rf22.setTxPower(RF22_TXPOW_20DBM);
  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);
  rf22.addRouteTo(DESTINATION_ADDRESS, DESTINATION_ADDRESS);
  sensorVal = analogRead(A0);
  randomSeed(sensorVal);// (μία μόνο φορά μέσα στην setup)
}



void loop() {

  delay(2000);
  int SensorValue1= random(50,200);
  int SensorValue2= random(0,2); 
  int SensorValue3=random(0,50); 
  int SensorValue4=random(20,80); 
  
  
  char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(data_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  sprintf(data_read, "%d %d %d %d", SensorValue1,SensorValue2,SensorValue3,SensorValue4);
  data_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
  memcpy(data_send, data_read, RF22_ROUTER_MAX_MESSAGE_LEN);
  successful_packet = false;
  initial_time=millis();  

  while (!successful_packet)
  {
    if (rf22.sendtoWait(data_send, sizeof(data_send), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
    {
      Serial.println("sendtoWait failed");
      retransmissions++;
      randNumber=random(200,max_delay);
      Serial.println(randNumber);
      delay(randNumber);
    }
    else
    {
      successful_packet = true;
      Serial.println("sendtoWait Succesful");
      successfuTransmissions ++;  
      number_of_bytes+=sizeof(data_send);
      final_time=millis();
      delta_time+=final_time-initial_time;      
    }

  }
// Caluclation of success rate    
  percentage = (float) (successfuTransmissions)/(successfuTransmissions+retransmissions);
  Serial.print("Success Rate: ");
  Serial.print(percentage*100);
  Serial.println("%");
// Caluclation of throughput    
  throughput= (float) number_of_bytes/delta_time; 
  Serial.print("Troughput: ");
  Serial.println(throughput);
}




  
