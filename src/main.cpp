#include <Modbus/Modbus.h>
#include <Valve/Valve.h>
//#include <Ticker.h>
#include <Menu/Menu.h>
#include "Menu/Items.h"

#pragma region ОБЪЯВЛЕНИЕ ОБЪЕКТОВ

  //ЭКЗЕМПЛЯР MODBUS
  MODBUS_TCP_SLAVE* MODBUS;

  //ЭКЗЕМПЛЯРЫ КЛАПАНОВ
  Valve*  Valve_1;
  Valve*  Valve_2;
  Valve*  Valve_3;
  Valve*  Valve_4;

  /*
  * УКАЗАТЕЛЬ ЭКЗЕМПЛЯР КЛАССА Menu
  */
  Menu* MainMenu;

#pragma endregion

#pragma region ОПРЕДЕЛЕНИЯ РЕГИСТРОВ

  #define CURRUNT_TIME        MODBUS.MB_HOLDING_REGISTERS[5] //Текущее время в минутах
  #define SECONDS             MODBUS.MB_HOLDING_REGISTERS[6] //Количество секунд
 
  #define TEMPERATURE_AIR     MODBUS.MB_HOLDING_REGISTERS[0] //Температура воздуха
  #define TEMPERATURE_WAITER  MODBUS.MB_HOLDING_REGISTERS[1] //Температура воды в баке
  #define TEMPERATURE_T1      MODBUS.MB_HOLDING_REGISTERS[2] //Температура воздуха
  #define TEMPERATURE_T2      MODBUS.MB_HOLDING_REGISTERS[3] //Температура воздуха
  #define LEVEL_WAITER        MODBUS.MB_HOLDING_REGISTERS[4] //Уровень воды в баке

#pragma endregion


#pragma region ИНИЦИАЛИЗАЦИЯ ЭКЗКМПЛЯРОВ ОБЪЕКТОВ И ПЕРЕМЕННЫХ ПРОГРАММЫ

//Таймер - счетчик
//Ticker timer;

//СИСТЕМНЫЕ ПЕРЕМЕННЫЕ
bool WiFi_ST;

#pragma endregion

#pragma region ИНИЦИАЛИЗАЦИЯ МЕНЮ



/**
* Метод для инициализации меню
**/
bool temp = false;
int t = 56;

void MenuInitialize()
{
  //Стартовый экран
  MainMenu->AddScreen(new StartScreenItem("SMART", "WATERING"));

  //СИСТЕМА
  //Время
  MainMenu->CreateTimeScreen("C\4CTEMHOE BPEM\1",  &t);


  //Статус WiFi
  MainMenu->CreateBoolScreen("WI-FI           ",  &MODBUS->WiFi_State);

  //Настройки автоматическое вкл реле
  MainMenu->CreateBoolScreen("PE\2E 1 FORCE",  &temp);
  MainMenu->CreateTimeScreen("PE\2E 1 BP. BK\2  ",  &t);
  MainMenu->CreateTimeScreen("PE\2E 1 BP. B\3K\2 ", &t);


  MainMenu->CreateBoolScreen("PE\2E 2 FORCE",  &temp);
  MainMenu->CreateTimeScreen("PE\2E 2 BP. BK\2  ",  &t);
  MainMenu->CreateTimeScreen("PE\2E 2 BP. B\3K\2 ", &t);

  MainMenu->CreateBoolScreen("PE\2E 3 FORCE",  &temp);
  MainMenu->CreateTimeScreen("PE\2E 3 BP. BK\2  ",  &t);
  MainMenu->CreateTimeScreen("PE\2E 3 BP. B\3K\2 ", &t);
 /**
  MainMenu->CreateBoolScreen("PE\2E 4 FORCE",  &temp);
  MainMenu->CreateTimeScreen("PE\2E 4 BP. BK\2  ",  &t);
  MainMenu->CreateTimeScreen("PE\2E 4 BP. B\3K\2 ", &t);
  **/

  //Запускаем меню
  MainMenu->Begin();

}

#pragma endregion


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

void setup() 
{ 
  //Инициализируем последовательный порт
  //для вывода информации диагностики
  Serial.begin(115200);

  MainMenu = new Menu(0x27, 4, 2, 15);

  //Инициализация меню
  MenuInitialize();

  //Инициализация EEPROM
  EEPROM.begin(512);

  Valve_1 = new Valve(13, 14, MODBUS->MB_HOLDING_REGISTERS);
  Valve_2 = new Valve(12, 20, MODBUS->MB_HOLDING_REGISTERS);
  Valve_3 = new Valve(14, 26, MODBUS->MB_HOLDING_REGISTERS);
  Valve_4 = new Valve(4,  32, MODBUS->MB_HOLDING_REGISTERS);

  //Подпись на событие тика
  //timer.attach(1, timer_tick);

}


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
  MODBUS->MODBUS_UPDATE();
  MainMenu->Update();

}