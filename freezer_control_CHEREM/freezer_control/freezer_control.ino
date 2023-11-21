 
/*_________________ DS18B20 PART _________________*/
  #include <OneWire.h>
  #define pinONE_WIRE_BUS 3                      //Pin для OneWire
  #define Max_Sensors_Count 10                    //Максимальное количество датчиков для попыток опроса 
  byte Founded_Sensors_Count = 0;                 //Переменная для количества найденых датчиков начиная с 0     

  byte Sensor_Address[Max_Sensors_Count][8];      //Создал массив: строки-кол-во датчиков по 8 байт в каждой для хранения адреса(ROM, ID) датчика
  byte data[Max_Sensors_Count][12];               //Создал массив: строки-ROM датчика по 12 байт с температурой
  OneWire ds(pinONE_WIRE_BUS);                    //на этой шине будет несколько датчиков
/*_________________ DS18B20 PART _________________*/

#define pinRELAY 2
#define maxTemp -10
#define minTemp -15


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);               // Debug console
  Serial.println("");
  //Serial.println("find_ds18b20_v2.ino for wemos D1, OWpin = D4");
  pinMode(pinRELAY, OUTPUT);   //OUTPUT SETUP 
  digitalWrite(pinRELAY, 1);
  Serial.println("call Find_DS18b20");
  Find_DS18b20();       /*_________________ DS18B20 PART _________________*/ //РАСКОММЕНТИРОВАТЬ ДЛЯ ПОИСКА АДРЕСОВ ДАТЧИКОВ
}

void loop() {
  // put your main code here, to run repeatedly:

  //PrintAllFoundedDS18B20ToSerial(Founded_Sensors_Count, Sensor_Address);
  long CurrentTemp;

  delay(1500);
  CurrentTemp = Get_DS18B20_Temp_by_HEX(Sensor_Address[0]);

  Serial.println(CurrentTemp);

    if (CurrentTemp >= maxTemp) {
    digitalWrite(pinRELAY, 0);
  } else if (CurrentTemp <= minTemp) {
    digitalWrite(pinRELAY, 1);
  }


}


void Find_DS18b20()
{
  for (int i = 0; i < Max_Sensors_Count; i++) 
  {
    if (!ds.search(Sensor_Address[i]))
    {
      Serial.println("ErrInit1W DS18B20 not Found");//инициализация не выполнена:DS18B20 не найдены
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
            Serial.print("}");   
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

//**********1-Wire CODE Возврат значения температуры датчика по HEX адресу датчика
long Get_DS18B20_Temp_by_HEX(byte HEX_SensorAddress[8])
{
    ds.reset();
    ds.select(HEX_SensorAddress);
    ds.write(0x44, 1);

    ds.reset();//работа с датчиком всегда должна начинаться с сигнала ресета
    ds.select(HEX_SensorAddress);//выбирается адрес (ROM ) датчика
    ds.write(0xBE);
    
    for ( byte i = 0; i < 9; i++)
    {
      data[0][i] = ds.read();//сохраняем температуру(9 байт) в массиве соответствующего датчика
    }

    int16_t raw = (data[0][1] << 8) | data[0][0];//Тип 16-разрядных целых
    byte cfg = (data[0][4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7; // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms

    int celsius = (float)raw / 16.0; //16.0 будет 2 знака после запятой. Мне надо целое

    return celsius;
}

void PrintAllFoundedDS18B20ToSerial(byte xFounded_Sensors_Count, byte xSensor_Address[][8])
{         
          for (int i = 0; i <= xFounded_Sensors_Count; i++)
          {
            for (int j = 0; j < 8; j++)
              {
                Serial.print(xSensor_Address[i][j],HEX);
              }
          Serial.print("=");
          Serial.println(Get_DS18B20_Temp_by_HEX(xSensor_Address[i]));
          }
          //Sent_serial();
}
