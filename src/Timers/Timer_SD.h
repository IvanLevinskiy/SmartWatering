//

#ifndef TIMER_SD_H
#define TIMER_SD_H

//#include <ESP8266WiFi.h>
#include <Arduino.h>


class Timer_SD
{
    private:

        /**
	    * Флаг, разрешающий работу таймера
	    */
        bool isrunning;

        /**
	    * Время, заданное для сроботки таймера
	    */
        unsigned long time;

        /**
	    * Время, мс прошлого срабатывания таймера
	    */
        unsigned long oldMillis;

        /**
	    * Текущее время
	    */
        unsigned long currentTime;

        /**
	    * Статус флага
	    */
        bool Tick_State;

        /**
	    * Статус флага Blink
	    */
        bool Blink_State;

        //Состояние выходного сигнала
        bool Output_State;

        /**
	    * Время до сработки таймера
	    */
        long value;

        public:
        /**
	    * Конструктор класса
	    */
        Timer_SD(long millisecconds)
        {
            time = millisecconds;
        }

        /**
	    * Метод для запуска таймера
	    */
        void Start()
        {
            isrunning = true;
            Reset();
        }

        /**
	    * Метод для запуска таймера
	    */
        void Stop()
        {
            isrunning = false;
            Output_State = false;   
        }

        /**
	    * Метод для обновления состояния таймера
        * (должен вызываться в каждом цикле иначе работать не будет)
	    */
        void Update()
        {
            //Если работа таймера запрещена, 
            //сбрасываем все флаги и выходим из функции
            if(isrunning == false)
            {
                value = time;
                Blink_State = false;
                Tick_State = false;
                Output_State = false;
                return;
            }

            //Вычисляем время, оставшееся до тика
            value = oldMillis + time - millis();

            //Если таймер долщен тикнуть
            if( _isTimer(time))
            {
                //Взводим флаг, что таймер тикает
                Tick_State = true;

                //Инвертируем состояние блинкер-бита
                Blink_State = !Blink_State;

                //Запоминаем время в которое он сработал
                oldMillis = millis();

                //Выход
                Output_State = true;   
                
                //Выходим из функции
                return;
            }

            //Сбрасываем флаг, что таймер тикает
            Tick_State = false;
        }

        //Метод для определения события тика
        bool _isTimer(unsigned long period)
        {
            currentTime = millis();

            if (currentTime >= oldMillis) 
            {   
                return (currentTime >= (oldMillis + period));
            } 
            else  
            {   
                return(currentTime >=(4294967295 - oldMillis + period));
            }
        }

        //Проверяет, тикнул ли таймер
        bool IsTick()
        {
            return Tick_State;
        }

        //Возвращает состояние BLINK
        bool GetBlinkerState()
        {
            return Blink_State;
        }

        bool GetOutputState()
        {
            return Output_State;
        }

        long GetTime()
        {
            return value;
        }

        //Сбрасывает состояние таймера
        void Reset()
        {
            oldMillis = millis();
            Output_State = false;   
        }
};

#endif