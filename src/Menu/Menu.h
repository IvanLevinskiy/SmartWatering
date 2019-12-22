#ifndef MENU_H
#define MENU_H

#include <Wire.h>
#include <ESP8266WiFi.h>
//#include <Arduino.h>
#include "Menu/Enumeration.h"
#include "Menu/MenuButton.h"
#include "Menu/Items.h"
#include "Menu/LCD_Display.h"


#define MAX_SCEEN 20

//Класс, осуществляющий реализацию движка
//меню
class Menu
{
    private:

    //Объявляем массив указателей на класс BaseItem()
    BaseItem *Array[MAX_SCEEN];

    //Объявляем указатель на класс BaseItem
    BaseItem *ptr;  

    //Количество экранов
    int ScreenCount;

    //Указатель на экземпляр дисплея
    LCD_Display* LCD;

    //Показывает уровень меню (нужна для навигации)
    //по меню
    int ActiveScreen;

    //Таймер, который будет отслеживаать бездействие
    Timer_SD Timer = Timer_SD(60000);

    //Таймер, который передается в каждый экран
    //и необходиый в основном для анимации
    Timer_SD Timer_2Hz = Timer_SD(500);

    //Определения для кнопок, по которым осуществляется 
    //навигация по меню
    Button* BtnUp;
    Button* BtnDown;
    Button* BtnSelect;

    //Режим меню
    MenuMode Mode = ReadMode;

    void NavigationOfReadMode();
    void NavigationOfWriteMode();

    //Метод для сбросы режима сна
    void SleepCancel();

public:

    //Конструктор класса
    Menu();

    /**
	 * Перегруженный конструктор класса
	 *
	 * @param LCD_ADDR	I2C сетевой адрес устройства
	 * @param pinUp 	Пин, к которму подключена кнопка вверх
	 * @param pinDown	Пин, к которму подключена кнопка вниз
	 * @param pinSelect	Пин, к которму подключена кнопка выбора
	 */
    Menu(uint8_t LCD_ADDR, int pinUp, int pinDown, int pinSelect);

    //метод для добавления нового экрана
    void AddScreen(BaseItem *screen);

    //Метод, осуществляющий запуск инициализации
    void Begin();

    //Метод, осуществляющий обновление данных
    void Update();

    //Метод для навигации по меню
    void Navigation();
};

#endif







