#include <RF22.h>
#include <SPI.h>
#include <RF22Router.h>
#include "DHT.h"

#define DHTPIN 7
#define SOURCE_ADDRESS 2 //Node 2
#define DESTINATION_ADDRESS 10
#define DHTTYPE DHT11


#define USE_ARDUINO_INTERRUPTS true     // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>  

//  Variables and object for Pulse Sensor
const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.

PulseSensorPlayground pulseSensor;  
                            

// Variables initialized for Rain Sensor
const int sensorMin = 0 ;
const int sensorMax = 1024;

// Object for Temperature-Humidity Sensor
DHT dht(DHTPIN, DHTTYPE);

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
  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13); 
  pulseSensor.setThreshold(Threshold);   

  // Check if Pulse Sensor object is created
  if (pulseSensor.begin()) {
     Serial.println("Pulse Sensor Object created");
  }
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
  dht.begin(); //Begin the DHT object
  sensorVal= analogRead(A0);
  randomSeed(sensorVal);
}



void loop() {
  delay(2000); 
  int myBPM = pulseSensor.getBeatsPerMinute();  // Get BPM Measurement

 if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened". 
  Serial.println("♥  A HeartBeat Happened ! "); 
  Serial.print("BPM: ");                       
  Serial.println(myBPM);     
 }
 //4 Measurements to be sent
  int SensorValue1 = myBPM;
  int SensorValue2 = RainMeasure();
  int SensorValue3 = dht.readTemperature();
  int SensorValue4 = dht.readHumidity();
  
  Serial.println(SensorValue3);
  Serial.println(SensorValue4);
  
  char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(data_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  sprintf(data_read, "%d %d %d %d", SensorValue1,SensorValue2,SensorValue3,SensorValue4); //Load the 4 Measurements in one packet
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

//Function to get the Rain Sensor Measurement
int RainMeasure(){
  int sensorReading=analogRead(A1);
  int range=map(sensorReading,sensorMin,sensorMax,0,3);
  switch (range){
  case 0:    // Sensor getting wet
    Serial.println("It's raining");
    break;
  case 1:    // Sensor getting wet
    Serial.println("Rain Warning");
    break;
  case 2:    // Sensor dry - To shut this up delete the " Serial.println("Not Raining"); " below.
    Serial.println("Not Raining");
    break;
  }
  return range;
}


  
