#include "Valve\Valve.h"

//Определения для адресов регистров с данными
#define MB_CONTROL_WORD  *(MB_HOLDING_REGISTERS + firstRegister * 2)     //Слово управления
#define MB_STRT_TIME     *(MB_HOLDING_REGISTERS + firstRegister * 2 + 2) //Время процесса начала
#define MB_STP_TIME      *(MB_HOLDING_REGISTERS + firstRegister * 2 + 4) //Время окончания процесса

//Конструктор класса Valve
Valve::Valve()
{             

}

//Конструктор класса Valve
Valve::Valve(int _pin, int _firstRegister, uint16_t *PNT_MB_HOLDING_REGISTERS)
{             
        pin = _pin;
        firstRegister = _firstRegister;

        //Передаем указатель на первый элемент массива HOLDING регистров модбас
        MB_HOLDING_REGISTERS = PNT_MB_HOLDING_REGISTERS;
        
        //Чтение настроек из EEPROM при запуске
        MB_STRT_TIME = StartTime = word(EEPROM.read(_firstRegister), EEPROM.read(_firstRegister + 1));
        MB_STP_TIME = EndTime =    word(EEPROM.read(_firstRegister + 2), EEPROM.read(_firstRegister + 3));
        MB_CONTROL_WORD = CONTROL_WORD = word(EEPROM.read(_firstRegister + 4), EEPROM.read(_firstRegister + 5));

        //Настройка выхода реле
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
}


//Обновление данных. Метод вызывается в loop
//Управляет логикой
void Valve::Update()
{ 
      //Переменная для временного хранения результата состояния пина,
      //к которому полключается клапан
      bool tempStateValve = false;

      //Если слово управления пустое, пропускаем цикл
      if(MB_CONTROL_WORD == 0)
      {
          goto M1;
      }

      //Если в ручную подана команда включить, тогда
      //независимо ни от чего включаем порт
      if(Force == true)
      {
          tempStateValve = true;
          goto M1;
      }

      //Если режим работы ручной
      if(AUTO_MODE == true)
      {              
          //Проверяем есть попадает ли текущее время в диапазон
          //1. Если время начала меньше времени окончания полива
          if( StartTime < EndTime)
          {                    
            tempStateValve = StartTime <= CURRUNT_TIME && CURRUNT_TIME < EndTime;
            goto M1;
          }

          //2. Если время начала больше времени окончания полива, т.е полив продолжается и ночью
          if( StartTime > EndTime)
          {
              bool temp_1 = StartTime <= CURRUNT_TIME && CURRUNT_TIME < 1441;
              bool temp_2 = 0 <= CURRUNT_TIME && CURRUNT_TIME < EndTime;
              tempStateValve = temp_1 || temp_2;
              goto M1;
          }
      }

      //Если не включена FORCE и программа автоматики
      //тогда выключаем переменную
      if(Force == false && AUTO_MODE == false)
      {
          tempStateValve = false;
      }

      //Передаем состояние пина реле с инверсией,
      //так как логика у реле обратная
      M1: digitalWrite(pin, ! tempStateValve);
      
      //Записываем сотояние реле в регистр,
      //для того чтоб было видно его состояние
      bitWrite(MB_HOLDING_REGISTERS[firstRegister], 4, tempStateValve);
}
    

    
//Получаем регистр, СЛОВО УПРАВЛЕНИЯ и другие
void Valve::UpdateRegister()
{     
        //Проверяем, если регистр (слово управления) изменился,
        //тогда пытаемся его парсим
        if( MB_CONTROL_WORD != CONTROL_WORD) 
        {
          CONTROL_WORD = MB_CONTROL_WORD;
          Force = GetBit(CONTROL_WORD, 0);        
          AUTO_MODE = GetBit(CONTROL_WORD, 1);

          //Сохраняем состояние регистра в EEPROM
          EEPROM.write(firstRegister + 4, highByte(CONTROL_WORD));      
          EEPROM.write(firstRegister + 5, lowByte (CONTROL_WORD)); 
          EEPROM.commit();  

          Serial.print("CW Reccord: ");
          Serial.print("cell: ");
          Serial.print(firstRegister + 4);
          Serial.print(" value: ");
          Serial.println(CONTROL_WORD);
        }

        //Проверяем, если регистр (время включения) изменился, 
        //тогда его сохраняем в EEPROM
        if(MB_STRT_TIME != StartTime) 
        {
          StartTime =  MB_STRT_TIME;
          EEPROM.write(firstRegister + 0, highByte(StartTime));      
          EEPROM.write(firstRegister + 1, lowByte (StartTime)); 
          EEPROM.commit();
        }

        //Проверяем, если регистр (время выключения) изменился, 
        //тогда его сохраняем в EEPROM
        if(MB_STP_TIME != EndTime) 
        {
          EndTime = MB_STP_TIME;
          EEPROM.write(firstRegister + 2, highByte(EndTime));      
          EEPROM.write(firstRegister + 3, lowByte(EndTime));  
          EEPROM.commit(); 
        }
}