#ifndef BUTTON_H
#define BUTTON_H

#include"Timers/Timer_SD.h"

class Button
{
    public:

        //Конструктор класса
        Button();
        Button(int pin);

        //Метод, который читает состояние пина
        bool Read();

        bool IsPressed();

        //Метод, который возвращает единицу, 
        //если состояние ноги поменялось с false
        //на true
        bool FrontPositive();

        //Добака к  переменной
        //Если нажата кнопка
        int GetAdditive();

        //Таймер, который используется для
        //быстрого изменения значения при просмотре
        Timer_SD Timer = Timer_SD(300);

    private:
        //Номер пина, к которому подключена кнопка
        int pin;

        //Переменная, для хранения предыдущего значения
        bool oldState;

        //Время в мс, когда состояние кнопки 
        //изменилось с 0 на 1
        long TimeIsFrontPositive;

        void Reset();
};

#endif