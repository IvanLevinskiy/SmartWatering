#ifndef BASE_ITEM_H
#define BASE_ITEM_H

#include <LiquidCrystal_I2C.h>
#include "Timers/Timer_SD.h"
#include "MENU/Enumeration.h"
#include "RTClib.h"

//Базовый (абстрактный класс)
class BaseItem
{
    protected:

        //Переменная для анимации
        int AnimationStep = 0;

        //Шаг редактирования
        int StepEditor = 0;

        //Указатель на массив символов
        byte *SymbolsList[8];

        //Счетчик пользовательских символов
        //(приемущественно кирилица)
        int SymbolCounter = 0;

        //Экземпляр дисплея
        LiquidCrystal_I2C* LCD;

    public:

        //Заголовк (надпись вверху)
        char* Header;

        //Указатель на элемент, который требуется
        //редактировать или изменять
        void* Pointer;

        //Режим меню (чтение параметра или запись)
        MenuMode mode = ReadMode;

        //Разрешение на редактирование
        //значения
        bool IsEditEnable = true;

        //Указатель на локальный таймер
        //для анимации
        Timer_SD* Timer_2Hz;

        //Строка
        int Row = 0;

        //Разрешение на видимость других Item
        bool VisibleNextItem = true;

        //Видимость даного Item
        bool* IsVisible;

        //Метод для отображения связанной переменной
        virtual void Display() = 0;

        //Сообщаем модели, что нажата кнопка
        //вверх
        virtual void BtnUpPressed() = 0;

        //Сообщаем модели, что нажата кнопка
        //вниз
        virtual void BtnDownPressed() = 0;

        //Сообщаем модели, что нажата кнопка
        //выбора
        virtual void BtnSelectPressed() = 0;

        /*
            МЕТОД ДЛЯ ПРИВЯЗКИ ДИСПЛЕЯ К МЕНЮ
        */
        void BindingLCD(LiquidCrystal_I2C* lcd);

        /*
            МЕТОД ДЛЯ ПРИВЯЗКИ ТАЙМЕРА
        */
        void BindingTimer(Timer_SD* timer);

        //Метод, который переключает
        //следующий разряд для редактирования
        //и возвращает false, когда редактирование
        //завершено
        virtual bool IsEdition() = 0;

        //Метод для печати заголовка
        void printHeader(char * header);

        //Метод для печати заголовка
        void printIntAsTime(int time);

        //Печать времени
        void printIntAsTime(int value, Timer_SD* Timer_2Hz);

        //Передвижение символа карандаша
        //(анимацияф редактирования)
        void AnimationEdit();

        //стоп анимации
        void AnimationStop();

        //Добавление символа
        void AddSymbol(byte* symbol);

        //Метод для загрузки символов
        //в память дисплея
        void LoadSymbols(int offset);

};

#endif
