#ifndef CW_ITEM_H
#define CW_ITEM_H

#include <LiquidCrystal_I2C.h>
#include "Timers/Timer_SD.h"
#include "Menu/BaseItem.h"

//Класс для редактирования булевых переменных
class ControlWordItem : public BaseItem
{

    private:

        uint16_t Registr_Value = 0;

        //Бит который необходимо менять
        int Bit;

        //Временная переменная для хранения
        //состояния бита  в слове
        bool TempBitState = false;

        //Шаг редактирования
        int StepEditor = -1;

    public:

        /**
         *Конструктор
         *header - заголовок экрана
         *object - объект, связанный с текущим экраном
        **/
        ControlWordItem(char* header, void * object, int bit)
        {
            //Передаем в экземпляр класса заголовок,
            //который отображаться вверху дисплея
            Header = header;

            //Передаем указатель на элемент массива
            //в свойства класса
            Pointer = object;

            Bit = bit;

        }

        //Приведение булевой переменной
        //к типу стринг
        char* BoolToStr(uint16_t value)
        {
            uint16_t new_value = value & (uint16_t)(1 << Bit);

            if(new_value > 0)
            {
                return "  ON";
            }

            return " OFF";

        }

        //Приведение булевой переменной
        //к типу стринг
        char* BoolToStr(bool value)
        {
            if(value == true)
            {
                return "  ON";
            }

            return " OFF";

        }

        //Метод для получения бита в слове управления
        bool GetBit()
        {
            return GetBit(Bit, Registr_Value);
        }

        //Метод для получения бита в слове управления
        bool GetBit(int _bit, uint16_t _value)
        {
            uint16_t res = _value;
            res &= (uint16_t)(1<<_bit);

            //возвращаем результат
            if(res == 0) return false;
            return true;
        }

        // Запись бита в слово управления
        void WriteBit()
        {
              //Получаем указатель на переменную
              uint16_t* pnt = (uint16_t*)Pointer;

              //получаем значение из указателя
              Registr_Value = *pnt;

              if(TempBitState == true)
              {
                  Registr_Value |= (uint16_t)(1<<Bit);
              }
              else
              {
                  Registr_Value &= ~((uint16_t)(1<<Bit));
              }

              //Записываем новое значение в память
              *pnt = Registr_Value;
        }

       //Метод для отображения
        void Display() override
        {
            //Обновляем состояние таймера
            Timer_2Hz->Update();

            //Печатаем заголовок
            printHeader(Header);

            //Печатаем в дисплей то значение, которое планируем изменить
            LCD->setCursor(12, 1);

            //Режим просмотра переменной
            if(mode == ReadMode)
            {
                //Получаем указатель на переменную
                uint16_t* pointer = (uint16_t*)Pointer;
                uint16_t value = *pointer;
                bool bs = GetBit(Bit, value);

                LCD->print(BoolToStr(bs));

                //Затираем карандаш
                AnimationStop();
                return;
            }

            LCD->print(BoolToStr(TempBitState));

            //Мигаем
            if(Timer_2Hz->IsTick())
            {
                AnimationEdit();
                return;
            }
        }

        //Добавляет к значению 1
        void UpValue()
        {
            //Инвертируем сигнал
            if(TempBitState)
            {
                TempBitState = false;
            }
            else
            {
                TempBitState = true;
            }
        }

        //Вычитает из значения 1
        void DownValue()
        {
            //Инвертируем сигнал
            if(TempBitState)
            {
                TempBitState = false;
            }
            else
            {
                TempBitState = true;
            }
        }

        //Начало редактирования
        void StartEdit()
        {
            //Получаем указатель на переменную
            uint16_t* pnt = (uint16_t*)Pointer;

            //получаем значение из указателя
            uint16_t value =  *pnt;

            //Получение бита
            TempBitState = GetBit(Bit, value);
        }

        //Конец редактирования
        void EndEdit()
        {
            //Установка бита
            WriteBit();
        }

        //Переключение шага для редактирования
        //(меняем разряды)
        bool IsEdition() override
        {
            return false;
        }

        void BtnUpPressed()
        {
            if(mode == WriteMode)
            {
                UpValue();
            }
        }

        void BtnDownPressed()
        {
            if(mode == WriteMode)
            {
                DownValue();
            }
        }

        void BtnSelectPressed()
        {
              //Если разрешение на редактирование нет
              //тогда пропускаем ход
              if(IsEditEnable == false)
              {
                  //Признак, что параметр только для чтения
                  LCD->setCursor(0, 1);
                  LCD->print("R");
                  return;
              }


              if(mode == ReadMode)
              {
                  //Подготовка к редактированию
                  //параметров
                  StartEdit();

                  //Сохранение режима меню
                  mode = WriteMode;
                  return;
              }

              if(IsEdition() == false)
              {
                  //Процедура завершения
                  //редактирования переменной
                  EndEdit();

                  //Сохранение режима меню
                  mode = ReadMode;
              }
        }

};

#endif
