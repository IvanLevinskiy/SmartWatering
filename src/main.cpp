#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <Valve/Valve.h>
#include <Modbus/Modbus.h>

MODBUS_TCP_SLAVE MODBUS;

/**
 * ОПРЕДЕЛЕНИЕ ДЛЯ ЧАСОВ
 **/
#define CURRUNT_TIME  MODBUS.MB_HOLDING_REGISTERS[5] //Текущее время в минутах
#define SECONDS       MODBUS.MB_HOLDING_REGISTERS[6] //Количество секунд
 

//Температура воздуха
#define TEMPERATURE_AIR     MODBUS.MB_HOLDING_REGISTERS[0]

//Температура воды в баке
#define TEMPERATURE_WAITER  MODBUS.MB_HOLDING_REGISTERS[1]

//Температура воздуха
#define TEMPERATURE_T1      MODBUS.MB_HOLDING_REGISTERS[2]

//Температура воздуха
#define TEMPERATURE_T2      MODBUS.MB_HOLDING_REGISTERS[3]

//Уровень воды в баке
#define LEVEL_WAITER       MODBUS.MB_HOLDING_REGISTERS[4]


/**
    ##########  ИНИЦИАЛИЗАЦИЯ ЭКЗКМПЛЯРОВ ОБЪЕКТОВ И ПЕРЕМЕННЫХ ПРОГРАММЫ  #################
**/
//Таймер - счетчик
Ticker timer;

//Определения для адресов регистров с данными
#define MB_CONTROL_WORD  MODBUS.MB_HOLDING_REGISTERS[firstRegister]     //Слово управления
#define MB_STRT_TIME     MODBUS.MB_HOLDING_REGISTERS[firstRegister + 1] //Время процесса начала
#define MB_STP_TIME      MODBUS.MB_HOLDING_REGISTERS[firstRegister + 2] //Время окончания процесса


//Инициализация клапанов
Valve  Valve_1;
Valve  Valve_2;
Valve  Valve_3;
Valve  Valve_4;

//Буфер байтов
byte ByteArray[260];

//Обработка тиков
void timer_tick() 
{
  //Каждую секунду инкрементируем секунды
  SECONDS ++;

  //Если секунд больше 60,
  //тогда инкрементируем минуты,
  //секунды сбрасываем
  if(SECONDS >= 60)
  {
    //Инкрементируем минуты
    CURRUNT_TIME ++;

    //Обнуляем счетчик секунд
    SECONDS = 0;
  }

  //Если часы досчитали до 1440 минут, тогда их обнуляем
  //Это значит, что наступило 00:00 часов
  if(CURRUNT_TIME >= 1440)
  {
    CURRUNT_TIME = 0;  
  }

  //Обновляем состояние клапанов
  Valve_1.Update();
  Valve_2.Update();
  Valve_3.Update();
  Valve_4.Update();
}

void MB_NEW_DATA_RECIVED()
{
  //Обновление данных в клапанах
   Valve_1.UpdateRegister();
   Valve_2.UpdateRegister();
   Valve_3.UpdateRegister();
   Valve_4.UpdateRegister();
}

//ПОКА ЧТО ЗНАЧЕНИЯ ЗАДАНЫ СТАТИЧНО
void DataProcess_Update()
{
    TEMPERATURE_AIR = 23;
    TEMPERATURE_WAITER = 13; 
    TEMPERATURE_T1 = 22;     
    TEMPERATURE_T2 = 21;    
    LEVEL_WAITER = analogRead(A0);
}

void setup() 
{ 
  //Инициализируем последовательный порт
  //для вывода информации диагностики
  Serial.begin(115200);

  //Инициализация EEPROM
  EEPROM.begin(512);

  Valve_1 = Valve(13, 14, MODBUS.MB_HOLDING_REGISTERS);
  Valve_2 = Valve(12, 20, MODBUS.MB_HOLDING_REGISTERS);
  Valve_3 = Valve(14, 26, MODBUS.MB_HOLDING_REGISTERS);
  Valve_4 = Valve(4,  32, MODBUS.MB_HOLDING_REGISTERS);


  //Создаем сеть
  MODBUS.WIFI_AP_INIT();
  //MODBUS.WIFI_STA_INIT();
  //MODBUS.AP_CONNECT();

  //Подпись на событие тика
  timer.attach(1, timer_tick);

  //Выводим диагностическую информацию,
  //что сервер устройство запущено
  Serial.println("\nDevice is starting");
}

void loop() 
{
  //Чтение данных от датчиков 
  //и передача их в регистры
  //DataProcess_Update();
      
   //Обновление значений регистров MODBUS 
   //(если пришел запроса на их изменение) 
   MODBUS.MODBUS_UPDATE();

}