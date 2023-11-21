#include "secrets.h"
#include "esp8266_network.h"
#include "ds18b20.h"

#include "DHT.h"                      // Подключаем библиотеку для работы с датчиками DHT
//DHT dht1(3, DHT11);                   // Создаем объект dht1 для датчика DHT11 на 3 пине
DHT DHTSENSOR1(D5, DHT22);                   // Создаем объект dht2 для датчика DHT22 на 2 пине

//#include <BlynkSimpleEthernet.h>
#include <BlynkSimpleEsp8266.h>


#define BLYNK_PRINT Serial
#define hp_pinD4_relay D4           //for freezer relay
#define hp_pinD7_relay D7           //for radiator relay
int hp_pinD4_state = 0;             //off for this relay
int hp_pinD7_state = 0;             //off for this relay
BlynkTimer blynk_timer;
WidgetTerminal terminal(V0);
int targetTemp = -10;             //for freezer relay control
int gisterezis = 6;               //for freezer relay control
int radiator_targetTemp = 26;     //for radiator relay control
int radiator_gisterezis = 2;      //for radiator relay control

byte HEX_Sensor_Address[][8]= {         
                              {0x28, 0xFF, 0xE9, 0xE6, 0x24, 0x17, 0x3, 0x4B},   //холодильник     28FF E9 E6 24 17 34 B
                              {0x28, 0xFF, 0xCC, 0xF8, 0x80, 0x14, 0x2, 0x2F},   //кухня
                              {0x28, 0xFF, 0x40, 0x51, 0x4B, 0x4, 0x0, 0x3D},    //улица
                              {0x28, 0xFF, 0x46, 0x5F, 0x4D, 0x4, 0x0, 0x65}     //баня                     
                              };  

void setup()
{
  Serial.begin(9600);

  wifi_ota_init(wifi_ssid,wifi_password);

  Blynk.begin(blynk_auth_token, wifi_ssid, wifi_password, blynk_local_server_ip, blynk_local_server_port);
  blynk_timer.setInterval(3000L, timer_code);            //sending to Blynk every 3 sec ...
  
  pinMode(hp_pinD4_relay, OUTPUT);                            //OUTPUT SETUP for freezer relay
  pinMode(hp_pinD7_relay, OUTPUT);                            //OUTPUT SETUP for radiator relay
  toggleRelay(1,hp_pinD4_relay,vp_BLYNK_1,hp_pinD4_state);    //RELAY_OFF for freezer  //(bool state, hw_relay_pin, vp_blynk_button, int state_variable)
  toggleRelay(1,hp_pinD7_relay,vp_BLYNK_2,hp_pinD7_state);    //RELAY_OFF for radiator //(bool state, hw_relay_pin, vp_blynk_button, int state_variable)

  DHTSENSOR1.begin();                       // Инициализируем датчик 5
  timer_code();    //send first data
}


void loop()
{
  ArduinoOTA.handle();
  blynk_timer.run();
  Blynk.run();
}

void PrintToSerialAndTerminal(String send_text)
{
  Serial.println(send_text);
  terminal.println(send_text);
  terminal.flush();
}

void timer_code()
{
  float h2 = DHTSENSOR1.readHumidity();     // Получить значение влажности с датчика 5
  float t2 = DHTSENSOR1.readTemperature();  // Получить значение температуры с датчика 5

  Blynk.virtualWrite(V10, Get_DS18B20_Temp_by_HEX(OW_Line_1, HEX_Sensor_Address[0]));                      //sending to Blynk холодильниик
  Blynk.virtualWrite(V11, Get_DS18B20_Temp_by_HEX(OW_Line_1, HEX_Sensor_Address[1]));                      //sending to Blynk кухня
  Blynk.virtualWrite(V16, Get_DS18B20_Temp_by_HEX(OW_Line_1, HEX_Sensor_Address[2]));                      //sending to Blynk улица
  Blynk.virtualWrite(V13, Get_DS18B20_Temp_by_HEX(OW_Line_2, HEX_Sensor_Address[3]));                      //sending to Blynk баня
  Blynk.virtualWrite(V14, String(t2));                      //sending to Blynk temp 2nd floor
  Blynk.virtualWrite(V15, String(h2));                      //sending to Blynk hum  2nd floor

  freezer_control ((Get_DS18B20_Temp_by_HEX(OW_Line_1, HEX_Sensor_Address[0])), targetTemp, (targetTemp+gisterezis), hp_pinD4_relay);
  radiator_control (t2, radiator_targetTemp, (radiator_targetTemp-radiator_gisterezis), hp_pinD7_relay);



  //Find_DS18b20(OW_Line_2);
  PrintToSerialAndTerminal (String("[0] freezer temp = " + String(Get_DS18B20_Temp_by_HEX(OW_Line_1, HEX_Sensor_Address[0]))));
  PrintToSerialAndTerminal (String("[0] freezer target temp = " + String(targetTemp)));
  PrintToSerialAndTerminal (String("[0] freezer gisterezis = " + String(gisterezis)));
  PrintToSerialAndTerminal (String("[1] home temp = " + String(Get_DS18B20_Temp_by_HEX(OW_Line_1, HEX_Sensor_Address[1]))));
  PrintToSerialAndTerminal (String("[2] out temp = " + String(Get_DS18B20_Temp_by_HEX(OW_Line_1, HEX_Sensor_Address[2]))));
  PrintToSerialAndTerminal (String("[3] bath temp = " + String(Get_DS18B20_Temp_by_HEX(OW_Line_2, HEX_Sensor_Address[3]))));
  PrintToSerialAndTerminal (String("[4] radiator target temp = " + String(radiator_targetTemp)));
  PrintToSerialAndTerminal (String("[4] radiator gisterezis = " + String(radiator_gisterezis)));
  PrintToSerialAndTerminal (String("2nd floor hum: " + String(h2)));
  PrintToSerialAndTerminal (String("2nd floor temp:" + String(t2)));
}

BLYNK_WRITE(V20)   //freezer gisterezis slider
{
  gisterezis = param.asInt(); // assigning incoming value from pin V20 to a variable
}
BLYNK_WRITE(V21)   //freezer target temp slider
{
  targetTemp = param.asInt(); // assigning incoming value from pin V21 to a variable
}

BLYNK_WRITE(V23)   //radiator gisterezis slider
{
  radiator_gisterezis = param.asInt(); // assigning incoming value from pin V20 to a variable
}
BLYNK_WRITE(V22)   //radiator target temp slider
{
  radiator_targetTemp = param.asInt(); // assigning incoming value from pin V21 to a variable
}

BLYNK_WRITE(V10)
{
  terminal.flush();
}

void freezer_control(int tempfromsensor, int targetTemp, int turnONtemp, uint8_t pinRelay)
{
  if (tempfromsensor >= turnONtemp) {
    //digitalWrite(pinRelay, 0);
      toggleRelay(0,hp_pinD4_relay,vp_BLYNK_1,hp_pinD4_state);
      PrintToSerialAndTerminal ("freezer ON " + String(tempfromsensor) + "C " + "target:" + String(targetTemp) + " turnOntemp:" + String(turnONtemp));
  } else if (tempfromsensor <= targetTemp) {
    //digitalWrite(pinRelay, 1);
      toggleRelay(1,hp_pinD4_relay,vp_BLYNK_1,hp_pinD4_state);
      PrintToSerialAndTerminal ("freezer OFF " + String(tempfromsensor) + "C " + "target:" + String(targetTemp) + " turnOntemp:" + String(turnONtemp));
  }
}

void radiator_control(int tempfromsensor, int targetTemp, int turnONtemp, uint8_t pinRelay)
{
  if (tempfromsensor <= turnONtemp) {
    //digitalWrite(pinRelay, 0);
      toggleRelay(0,pinRelay,vp_BLYNK_2,hp_pinD7_state);
      PrintToSerialAndTerminal ("radiator ON " + String(tempfromsensor) + "C " + "target:" + String(targetTemp) + " turnOntemp:" + String(turnONtemp));
  } else if (tempfromsensor >= targetTemp) {
    //digitalWrite(pinRelay, 1);
      toggleRelay(1,pinRelay,vp_BLYNK_2,hp_pinD7_state);
      PrintToSerialAndTerminal ("radiator OFF " + String(tempfromsensor) + "C " + "target:" + String(targetTemp) + " turnOntemp:" + String(turnONtemp));
  }
}

void toggleRelay(bool state, uint8_t hw_relay_pin, const char vp_blynk_button, int state_variable) 
{
  if (state == 1) {
    digitalWrite(hw_relay_pin, 1);
    state_variable = 1;
  } else if (state == 0) {
    digitalWrite(hw_relay_pin, 0);
    state_variable = 0;
  }
  Blynk.virtualWrite(vp_blynk_button, state_variable); // this will reflect the relay state to the button so you know if the lamp is on from the app
  //PrintToSerialAndTerminal(hw_relay_pin + " State: " + String(state_variable));
}

BLYNK_WRITE(vp_BLYNK_1) 
{    //(V1)
  // set up a SWITCH button on any available Virtual Pin
  // you will be setting a global var that will be checked in the loop by physicalButton()
  toggleRelay(param.asInt(),hp_pinD4_relay,vp_BLYNK_1,hp_pinD4_state); 
}

BLYNK_WRITE(vp_BLYNK_2) 
{    //(V1)
  // set up a SWITCH button on any available Virtual Pin
  // you will be setting a global var that will be checked in the loop by physicalButton()
  toggleRelay(param.asInt(),hp_pinD7_relay,vp_BLYNK_2,hp_pinD7_state); 
}

// BLYNK_READ(V1) //Blynk app has something on V1
// {
//   Serial.println("APP CALL READ V1: ");
//   //Blynk.virtualWrite(V1, String(propan,2)); //sending to Blynk  
// }


