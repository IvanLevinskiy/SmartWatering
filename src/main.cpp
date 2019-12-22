#include <Modbus/Modbus.h>
//#include <WiFi.h>
//#include <Valve/Valve.h>
//#include <Ticker.h>
//#include <Menu/Menu.h>
//#include "Menu/Items.h"

//Объявление экземпляра класса MODBUS_TCP_SLAVE
//MODBUS_TCP_SLAVE MODBUS;
/**
#pragma region ОПРЕДЕЛЕНИЯ

#define CURRUNT_TIME        MODBUS.MB_HOLDING_REGISTERS[5] //Текущее время в минутах
#define SECONDS             MODBUS.MB_HOLDING_REGISTERS[6] //Количество секунд
 
#define TEMPERATURE_AIR     MODBUS.MB_HOLDING_REGISTERS[0] //Температура воздуха
#define TEMPERATURE_WAITER  MODBUS.MB_HOLDING_REGISTERS[1] //Температура воды в баке
#define TEMPERATURE_T1      MODBUS.MB_HOLDING_REGISTERS[2] //Температура воздуха
#define TEMPERATURE_T2      MODBUS.MB_HOLDING_REGISTERS[3] //Температура воздуха
#define LEVEL_WAITER        MODBUS.MB_HOLDING_REGISTERS[4] //Уровень воды в баке

#pragma endregion

**/

#pragma region ИНИЦИАЛИЗАЦИЯ ЭКЗКМПЛЯРОВ ОБЪЕКТОВ И ПЕРЕМЕННЫХ ПРОГРАММЫ

//Таймер - счетчик
//Ticker timer;

//Инициализация клапанов
//Valve  Valve_1;
//Valve  Valve_2;
//Valve  Valve_3;
//Valve  Valve_4;

#pragma endregion

#pragma region ИНИЦИАЛИЗАЦИЯ МЕНЮ

/*
* Экземпляркласса Menu
*/
//Menu MainMenu = Menu(0x20, 2, 3, 4);

/*
* Метод для инициализации меню
*/
void MenuInitialize()
{
  
  //Создание экранов меню
  //MainMenu.AddScreen(new StartScreenItem("SMART", "WATERING"));
  //MainMenu.AddScreen(new BoolItem("WIFI STATE      ", &(WiFi_ST)));
  //MainMenu.AddScreen(new BoolItem("COIL 1 FORCE    ", &(C1_Force)));
  //MainMenu.AddScreen(new BoolItem("COIL 2 FORCE    ", &(C2_Force)));
  //MainMenu.AddScreen(new BoolItem("COIL 3 FORCE    ", &(C3_Force)));
  //MainMenu.AddScreen(new BoolItem("COIL 4 FORCE    ", &(C4_Force)));
  //MainMenu.AddScreen(new TimeItem("COIL 1 TIME STRT", &(Values[0])));

  /**
  MainMenu.AddScreen(new TimeItem("CURRENT TIME    ", &(CURRUNT_TIME)));

  MainMenu.AddScreen(new TimeItem("COIL 1 TIME STRT", &(MODBUS.MB_HOLDING_REGISTERS[15])));
  MainMenu.AddScreen(new TimeItem("COIL 1 TIME END ", &(MODBUS.MB_HOLDING_REGISTERS[16])));

  MainMenu.AddScreen(new TimeItem("COIL 2 TIME STRT", &(MODBUS.MB_HOLDING_REGISTERS[21])));
  MainMenu.AddScreen(new TimeItem("COIL 2 TIME END ", &(MODBUS.MB_HOLDING_REGISTERS[22])));

  MainMenu.AddScreen(new TimeItem("COIL 3 TIME STRT", &(MODBUS.MB_HOLDING_REGISTERS[27])));
  MainMenu.AddScreen(new TimeItem("COIL 3 TIME END ", &(MODBUS.MB_HOLDING_REGISTERS[28])));

  MainMenu.AddScreen(new TimeItem("COIL 4 TIME STRT", &(MODBUS.MB_HOLDING_REGISTERS[33])));
  MainMenu.AddScreen(new TimeItem("COIL 4 TIME END ", &(MODBUS.MB_HOLDING_REGISTERS[34])));
**/
  //Запускаем меню
  //MainMenu.Begin();
}

#pragma endregion


//ПОКА ЧТО ЗНАЧЕНИЯ ЗАДАНЫ СТАТИЧНО
void DataProcess_Update()
{
    //TEMPERATURE_AIR = 23;
    //TEMPERATURE_WAITER = 13; 
    //TEMPERATURE_T1 = 22;     
    //TEMPERATURE_T2 = 21;    
    //LEVEL_WAITER = analogRead(A0);
}

//Обработка тиков
void timer_tick() 
{
  //Каждую секунду инкрементируем секунды
  //SECONDS ++;

  //Если секунд больше 60,
  //тогда инкрементируем минуты,
  //секунды сбрасываем
  //if(SECONDS >= 60)
  //{
    //Инкрементируем минуты
  //  CURRUNT_TIME ++;

    //Обнуляем счетчик секунд
  //  SECONDS = 0;
  //}

  //Если часы досчитали до 1440 минут, тогда их обнуляем
  //Это значит, что наступило 00:00 часов
  //if(CURRUNT_TIME >= 1440)
  //{
  //  CURRUNT_TIME = 0;  
  //}

  //Обновляем состояние клапанов
  //Valve_1.Update();
  //Valve_2.Update();
  //Valve_3.Update();
  //Valve_4.Update();
}

void MB_NEW_DATA_RECIVED()
{
  //Обновление данных в клапанах
  // Valve_1.UpdateRegister();
  // Valve_2.UpdateRegister();
  // Valve_3.UpdateRegister();
   //Valve_4.UpdateRegister();
}


void setup() 
{ 
  //Инициализируем последовательный порт
  //для вывода информации диагностики
  //Serial.begin(115200);

  //Инициализация EEPROM
  //EEPROM.begin(512);

  //Valve_1 = Valve(13, 14, MODBUS.MB_HOLDING_REGISTERS);
  //Valve_2 = Valve(12, 20, MODBUS.MB_HOLDING_REGISTERS);
 // Valve_3 = Valve(14, 26, MODBUS.MB_HOLDING_REGISTERS);
  //Valve_4 = Valve(4,  32, MODBUS.MB_HOLDING_REGISTERS);

  //Создаем сеть с  MODBUS
 // MODBUS.WIFI_AP_INIT();

  //Подпись на событие тика
 // timer.attach(1, timer_tick);

}

//Переменная для вызова методов
//кадждую секунду
long millis_1Hz;

//Бесконечный цикл
void loop() 
{
  
  //Вызываем методы каждую секунду
 // if(millis() - millis_1Hz >= 1000)
  //{
      //Чтение данных от датчиков 
      //и передача их в регистры
     // DataProcess_Update();
     // millis_1Hz = millis();
  //}

      
  //Обновление значений регистров MODBUS 
  //(если пришел запроса на их изменение) 
  //MODBUS.MODBUS_UPDATE();

  //MainMenu.Update();

}