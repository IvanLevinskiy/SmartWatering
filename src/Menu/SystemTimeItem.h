#ifndef STIME_ITEM_H
#define STIME_ITEM_H

#include <LiquidCrystal_I2C.h>
#include "Timers/Timer_SD.h"
#include "RTClib.h"
#include "Menu/BaseItem.h"


//Класс стартового экрана
class SystemTimeItem : public BaseItem
{
    public:

        //Указатель на часы реального времени
        RTC_DS3231* RTC;

        //Конструктор класса
        SystemTimeItem(char* header, RTC_DS3231* rtc);

        //Метод для отображения
        void Display() override;

        //Добавляет к значению 1
        void UpValue();

        //Вычитает из значения 1
        void DownValue();

        //Начало редактирования
        void StartEdit();

        //Конец редактирования
        void EndEdit();

        //Переключение шага для редактирования
        //(меняем разряды)
        bool IsEdition() override;

        //Сообщаем модели, что нажата кнопка
        //вверх
        void BtnUpPressed() override;

        //Сообщаем модели, что нажата кнопка
        //вниз
        void BtnDownPressed() override;

        //Сообщаем модели, что нажата кнопка
        //выбора
        void BtnSelectPressed() override;

        void SetTime();

        private:

        char* printNum(int num);

        void DisplayOfReadMode(Timer_SD* Timer_2Hz);

        void DisplayOfWriteMode(Timer_SD* Timer_2Hz);

        int hours = 0;
        int minutetes = 0;
        int seconds = 0;

};

#endif
