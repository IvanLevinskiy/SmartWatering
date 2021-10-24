#ifndef MENU_H
#define MENU_H

#include <Wire.h>
#include "RTClib.h"
#include "Menu/BaseItem.h"
#include "Menu/BoolItem.h"
#include "Menu/TimeItem.h"
#include <RotaryEncoder.h>
#include <Menu/MenuButton.h>
#include <LiquidCrystal_I2C.h>
#include "Menu/ReturnItem.h"
#include "Menu/ContentItem.h"
#include "Menu/TextViewItem.h"
#include "Menu/ControlWordItem.h"
#include "Menu/SystemTimeItem.h"
#include "Menu/NumericViewItem.h"

#define MAX_SCEEN 30

//Класс, осуществляющий реализацию движка
//меню
class Menu
{
    private:

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
    Menu(uint8_t LCD_ADDR);


    Menu(LiquidCrystal_I2C* lcd);

    //Объявляем массив указателей на класс BaseItem()
    ContentItem *Array[MAX_SCEEN];

    //Количество экранов
    int ScreenCount = 0;

    //Указатель на экземпляр дисплея
    LiquidCrystal_I2C* LCD;

    //Показывает уровень меню (нужна для навигации)
    //по меню
    int ActiveScreen;

    //Таймер, который будет отслеживаать бездействие
    Timer_SD Timer = Timer_SD(60000);

    //Таймер, который передается в каждый экран
    //и необходиый в основном для анимации
    Timer_SD Timer_2Hz = Timer_SD(500);

    //Старый режим меню
    MenuMode OldMode = ReadMode;

    //Метод для сбросы режима сна
    void SleepCancel();

    //Временная переменная для хранения
    //состояния энкодера
    EncoderState State;

    //Указатель на экземпляр класса часов реального времени
    RTC_DS3231* RTC;

    //метод для добавления нового экрана
    void AddScreen(ContentItem *screen);

    //Метод, осуществляющий запуск инициализации
    void Begin();

    //Загрузка пользовательского символа
    void createSymbol(int pos, byte symbol []);

    //Метод, осуществляющий обновление данных
    void Update();

    //Построение меню
    void Build();

    //Метод для привязки часов реального времени к меню
    void BindingRTC(RTC_DS3231* rtc);

     //Метод, который генерируется когда
    //нажимается кнопка вверх
    void UpCMD();

    //Метод, который генерируется когда
    //нажимается кнопка вниз
    void DownCMD();

    //Метод, который генерируется когда
    //нажимается кнопка выбора
    void SelectCMD();

};

#endif
