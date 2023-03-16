//library 

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <ADS1115_WE.h> 
#include <Wire.h>

// define arus, tegangan, 
#define I2C_ADDRESS 0x48
#define ONE_WIRE_BUS 2 //one wire buat pin 2 arduino

OneWire oneWire(ONE_WIRE_BUS); // mengaktifkan fungsi one wire
DallasTemperature sensors(&oneWire); // mengambil library sensor suhu

RF24 radio(7, 8); // CE, CSN nrf24


const byte address[6] = "00001"; // kode antar nrf
const float zeroCurrentVoltage_mV = 19.37;  // nilai awal untuk kalibrasi voltage
String coba = ""; // string perantara 

unsigned long timer[2];
byte last_channel[1];
int input[1];

ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS); // mengaktifkan I2C 

 float readChannel(ADS1115_MUX channel) {
  float voltage = 0.0;
  adc.setCompareChannels(channel);
  adc.startSingleMeasurement();
  while(adc.isBusy()){}
  voltage = adc.getResult_V(); // alternative: getResult_mV for Millivolt
  return voltage;}

void setup() {
  Serial.begin(9600);
  
  //Mengaktifkan daerah mana yang menjadi interrupt
  PCICR |= (1 << PCIE2);    //Enable Interrupt di pin pcint 16 sampai 23

  //Memilih pin mana saja yang digunakan dari daerah (PCICR) yang dipilih
  PCMSK2 |= (1 << PCINT21); //Memilih pin pcint21 yaitu D5 sebagai interrupt
  
  Wire.begin();
  adc.setVoltageRange_mV(ADS1115_RANGE_0256); 
  adc.setCompareChannels(ADS1115_COMP_0_1); 
  sensors.begin();
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  
  //sensor suhu
  sensors.requestTemperatures(); // mengirim data request temperature
  float suhu      = sensors.getTempCByIndex(0);

  //sensor arus

  float voltage = 0.0;
  adc.setCompareChannels(ADS1115_COMP_0_1); // Channel 0 vs. 1
  adc.setVoltageRange_mV(ADS1115_RANGE_6144); // Limit: 512 mV
  
  for(int i=0; i<10; i++){
    adc.startSingleMeasurement();
    while(adc.isBusy()){}
    voltage += adc.getResult_mV();
  }
  voltage /= 10;
  voltage += zeroCurrentVoltage_mV;

  Serial.println(voltage);
  float current_mA = voltage * 0.0152 - 0.0133; // y = -0.0099x - 0.0743 experimentell ermittelt

  //sensor tegangan

  voltage = readChannel(ADS1115_COMP_2_GND);
  Serial.println(voltage);
  float Voltage_BT = voltage * 2.4869 - 0.0594; //y = 4.0135x + 0.069 PV

  if (Voltage_BT <= 0){
    Voltage_BT = 0;
  }
  
  //if (current_mA <= 0){
    //current_mA  = 0;
  //}
 
  
  float arus      = current_mA;
  float tegangan  = Voltage_BT;

  //sensor power
  float power     = arus * tegangan;

  coba += suhu;
  coba += "!";

  coba += arus;
  coba += "@";

  coba += tegangan;
  coba += "#";
  
  coba += power;
  coba += "$";

  coba += input[0];
  coba += "%";
  
  Serial.println(coba);
  Serial.println("========================");
  

  char text[coba.length() + 1];
  coba.toCharArray(text, sizeof(text));
  radio.write(&text, sizeof(text));

  //delay(15500);
  coba = "";
}

ISR(PCINT2_vect)
{
  timer[0] = micros();
  // channel 1 ---------------
  if(last_channel[0] == 0 && PIND & B00100000) {
    last_channel[0] = 1;
    timer[1] = timer[0];
  }
  else if(last_channel[0] == 1 && !(PIND & B00100000)) {
    last_channel[0] = 0;
    input[0] = timer[0] - timer[1];
  }
}
