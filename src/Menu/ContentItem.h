#ifndef CONTENT_ITEM_H
#define CONTENT_ITEM_H

#include <LiquidCrystal_I2C.h>
#include "Timers/Timer_SD.h"
#include "Menu/BaseItem.h"
#include "Menu/BoolItem.h"
#include "Menu/ReturnItem.h"
#include "Menu/ControlWordItem.h"
#include "Menu/StartScreenItem.h"
#include "Menu/SystemTimeItem.h"
#include "Menu/TimeItem.h"

#define MAX_SCEEN 30

//Класс для редактирования времени
//время задается в минутах
class ContentItem
{
    private:

        //Экземпляр дисплея
        LiquidCrystal_I2C* LCD;

        //Количество экранов
        int ScreenCount = 0;

    public:

        //Заголовк (надпись вверху)
        char* Header;

        //Объявляем массив указателей на класс BaseItem()
        BaseItem *Array[MAX_SCEEN];

        //Показывает уровень меню (нужна для навигации)
        //по меню
        int ActiveScreen = 0;

        //Указатель на локальный таймер
        //для анимации
        Timer_SD* Timer_2Hz;


        //Бит, показывающий, что необходимо
        //отображать содержимое
        bool ContentView = false;

        //Позиция строки на дисплее
        int Row = 0;

        //метод для добавления нового экрана
        void AddScreen(BaseItem *screen);

        //Метод, который возвращает выбрано ли текущий
        //пункт меню
        bool IsSelect();

        /**
         *Конструктор
         *header - заголовок экрана
         *object - указатель на предыдущий экран
        **/
        ContentItem(char* header);

        //Метод для отображения
        void Display();

        //Метод для отображения содержимого
        void DisplayContent();

        //Разрешение на переключение к слежующей
        //модели представления
        bool EnableSwitching();

        //Сообщаем модели, что нажата кнопка
        //вверх
        void BtnUpPressed();

        //Сообщаем модели, что нажата кнопка
        //вниз
        void BtnDownPressed();

        //Сообщаем модели, что нажата кнопка
        //выбора
        void BtnSelectPressed();

        //Передача указателя на дисплей
        void  BindingLCD(LiquidCrystal_I2C* lcd);

};

#endif
