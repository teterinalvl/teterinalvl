#include <OneWire.h>
#include "secrets.h"

#define Max_Sensors_Count 10                    //Максимальное количество датчиков для попыток опроса 
byte Founded_Sensors_Count = 0;                 //Переменная для количества найденых датчиков начиная с 0     

byte Sensor_Address[Max_Sensors_Count][8];      //Создал массив: строки-кол-во датчиков по 8 байт в каждой для хранения адреса(ROM, ID) датчика
byte data[Max_Sensors_Count][12];               //Создал массив: строки-ROM датчика по 12 байт с температурой
OneWire OW_Line_1(pinONE_WIRE_BUS_1);                    //на этой шине будет несколько датчиков
OneWire OW_Line_2(pinONE_WIRE_BUS_2); 

/* в головном скетче необходимо определить массив например:, в который занести адреса найденых датчиков после функции Find_DS18b20
byte HEX_Sensor_Address[][8]= {                                
                              {0x28, 0xFF, 0xCC, 0xF8, 0x80, 0x14, 0x2, 0x2F},   //улица
                              {0x28, 0xFF, 0x7, 0x56, 0x81, 0x14, 0x2, 0x6A}     //холодильник                  
                              };  
*/

int Get_DS18B20_Temp_by_HEX(OneWire OW_LineObj, byte HEX_SensorAddress[8])      //example: Serial.println(Get_DS18B20_Temp_by_HEX(HEX_Sensor_Address[0]));
{
    OW_LineObj.reset();
    OW_LineObj.select(HEX_SensorAddress);
    OW_LineObj.write(0x44); //, 1); 1-for parasite power
    delay (750);
    OW_LineObj.reset();//работа с датчиком всегда должна начинаться с сигнала ресета
    OW_LineObj.select(HEX_SensorAddress);//выбирается адрес (ROM ) датчика
    OW_LineObj.write(0xBE);
    
    for ( byte i = 0; i < 9; i++)
    {
      data[0][i] = OW_LineObj.read();//сохраняем температуру(9 байт) в массиве соответствующего датчика
    }

    int16_t raw = (data[0][1] << 8) | data[0][0];//Тип 16-разрядных целых
    byte cfg = (data[0][4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7; // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms

    int celsius = (float)raw / 16.0; //16.0 будет 2 знака после запятой. Мне надо целое

    return celsius;
}

void Find_DS18b20(OneWire OW_LineObj)
{
  for (int i = 0; i < Max_Sensors_Count; i++) 
  {
    if (!OW_LineObj.search(Sensor_Address[i]))
    {
      if (Founded_Sensors_Count==0){
        Serial.println("ErrInit1W DS18B20 not Found");//инициализация не выполнена:DS18B20 не найдены
      }
      return;
    }
    else //датчики найдены показать их в порт
    {
      
    //если нужно показать адреса датчиков
    Serial.print("Found ");
    Serial.print(i);
    Serial.print(" sensor: ");
    Serial.print("{");
        for (int j = 0; j < 8; j++)
            {
              if (j!=0 and j!=8)
                {
                  Serial.print(", ");      
                }
              Serial.print("0x");
              Serial.print(Sensor_Address[i][j] , HEX);         
            }
            Serial.print("} = ");
            Serial.print(Get_DS18B20_Temp_by_HEX(OW_LineObj, Sensor_Address[i]));

    Serial.println();
    }//конец else 
    
    if (OneWire::crc8(Sensor_Address[i], 7) != Sensor_Address[i][7]) 
    {
      Serial.println("ErrCRC");//CRC Failed!
      return;
    }
    if (Sensor_Address[i][0] != 0x28)
    {
      Serial.println("notDS18B20");//"OW Device is not DS18B20!"
    }
  Founded_Sensors_Count = i;    //записываем количество найденых
  } 
}



void PrintAllFoundedDS18B20ToSerial(OneWire OW_LineObj)
{         
          for (int i = 0; i <= Founded_Sensors_Count; i++)
          {
            for (int j = 0; j < 8; j++)
              {
                Serial.print(Sensor_Address[i][j],HEX);
              }
          Serial.print("=");
          Serial.println(Get_DS18B20_Temp_by_HEX(OW_LineObj, Sensor_Address[i]));
          }
          //Sent_serial();
}

String SensorAddress_to_str(byte* xSensor_Address)    // Sensor_Address[0...Max_Sensors_Count] format
{
    return String(xSensor_Address[0],HEX) +
           String(xSensor_Address[1],HEX) +
           String(xSensor_Address[2],HEX) +
           String(xSensor_Address[3],HEX) +
           String(xSensor_Address[4],HEX) +
           String(xSensor_Address[5],HEX) +
           String(xSensor_Address[6],HEX) +
           String(xSensor_Address[7],HEX);
}