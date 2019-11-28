
#include <ESP8266WiFi.h>
#include "EEPROM.h"

#define CURRUNT_TIME  MB_HOLDING_REGISTERS[5] //Текущее время в минутах
#define SECONDS       MB_HOLDING_REGISTERS[6] //Количество секунд

//Класс клапана
class Valve
{
  public:

    //Активация ручного режима
    bool AUTO_MODE = true;

    //Разрешение, пока не применено
    bool Enable;

    //Форcирование катшки реле
    bool Force;

    //Время начала полива, мин
    uint16_t  StartTime;

    //Время окончания полива, мин
    uint16_t  EndTime;

    //Конструктор класса
    Valve();

    //Конструктор класса
    //_pin - пин, к которому подключен клапан
    Valve(int _pin, int _firstRegister, uint16_t MB_HOLDING_REGISTERS[]);

    //Обновление данных. Метод вызывается в loop
    //Управляет логикой
    void Update();
   
    //Получаем регистр, СЛОВО УПРАВЛЕНИЯ и другие
    void UpdateRegister();
   
  
   private:  
          
          //Пин, к которому подключен
          //клапан
          int pin;

          //Состояние HRegister
          uint16_t CONTROL_WORD;

          //Стартовый регистр
          int firstRegister;

          //Указатель на элемент массива с регистрами
          uint16_t* MB_HOLDING_REGISTERS [];

          //Получаем бит из слова
          bool GetBit(uint16_t _value, int _bit);
};
