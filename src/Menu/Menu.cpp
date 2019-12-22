#include "Menu/Menu.h"

/**
 * Перегруженный конструктор класса
 *
 * @param LCD_ADDR	I2C сетевой адрес устройства
 * @param pinUp 	Пин, к которму подключена кнопка вверх
 * @param pinDown	Пин, к которму подключена кнопка вниз
 * @param pinSelect	Пин, к которму подключена кнопка выбора
 */
 Menu::Menu(uint8_t LCD_ADDR, int pinUp, int pinDown, int pinSelect)
{
    //Инициализируем дисплей
    LCD = new LCD_Display(LCD_ADDR, 16,2);

    //Инициализируем кнопки для навигации по меню
    BtnUp =     new  Button(pinUp);
    BtnDown =   new  Button(pinDown);
    BtnSelect = new  Button(pinSelect);
}

/**
 * Метод для настройки меню для работы
 * (должен вызываться в setup)
 */
void Menu::Begin()
{
    //Символ карандаша
    byte Edit[8] = 
    {
	    0b00011,
	    0b00000,
	    0b00110,
	    0b00110,
	    0b01100,
	    0b01100,
	    0b11000,
	    0b10000
    };

   
    //Инициализируем дисплей
    LCD->init();
    LCD->backlight();

    //Загружаем символ в lcd
    LCD->createChar(0, Edit);

    //Запускаем вспомогательный таймер, который
    //впоследствии передаем в каждый Item для анимации
    Timer_2Hz.Start();

    //Запускаем таймер, отслеживающий бездействие
    Timer.Start();
}

/**
 * Метод для добавления нового экрана
 */
void Menu::AddScreen(BaseItem *screen)
{
    //Если превышено максимальное количество 
    //пунктов меню, тогда пропускаем дальнейшие шаги
    if(ScreenCount == MAX_SCEEN)
    {
        return;
    }

    //Добавляем указатель на экран
    Array[ScreenCount] = screen;

    //Инкрементируем счетчик экранов
    ScreenCount++;
}

void Menu::SleepCancel()
{
        //Сбрасываем таймер, который отслеживает
        //бездействие
        Timer.Reset();

        //Включаем подсветку
        LCD->backlight();
}

/**
 * Метод для навигации по меню в режиме просмотра
 */
void Menu::NavigationOfReadMode()
{
    //Eсли нажата кнопка вверх в режиме просмотра
    //переходим к следующему параметру
    if(BtnUp->FrontPositive() == true)
    {
        
        //Отменяем режим сна
        SleepCancel();

        //Очищаем экран
        LCD->clear();

        //Изменяем активный экран
        ActiveScreen ++;

        //Зацикливаем меню
        if(ActiveScreen > ScreenCount - 1)
        {
            ActiveScreen = 1;
        }

        //Выходим из функции
        return;
    }

    //Eсли нажата кнопка вниз в режиме просмотра
    //переходим к предыдущему параметру
    if(BtnDown->FrontPositive() == true)
    {
        //Отменяем режим сна
        SleepCancel();

         //Очищаем экран
        LCD->clear();

         //Изменяем активный экран
        ActiveScreen --;

        //Зацикливаем меню
        if(ActiveScreen < 1 )
        {
            ActiveScreen = ScreenCount - 1;
        }
        
        //Выходим из функции
        return;
    }

    //Eсли нажата кнопка выбора в режиме просмотра
    //переходим в режим редактирования значений
    if(BtnSelect->FrontPositive() == true)
    {
        //Отменяем режим сна
        SleepCancel();

        //Если в качестве текущего экрана
        //запущен стартовый экран, тогда выходим
        //из функции
        if(ActiveScreen == 0)
        {
            return;
        }

        //Изменяем режим на режим записи
        Mode = WriteMode;

        //Выходим из функции
        return;
    }
}

/*
 * Метод для навигации по меню в режиме редактора
 * параметров
 */
void Menu::NavigationOfWriteMode()
{
    //Eсли нажата кнопка вверх в режиме просмотра
        if(BtnUp->FrontPositive() == true)
        {
            //Отменяем режим сна
            SleepCancel();

           //Увеличиваем значение
            Array[ActiveScreen]->UpValue();

            //Сброс таймера, отслеживающего 
            //бездействие
            Timer.Reset();

            //Выходим из функции
            return;
        }

        //Если кнопка вверх удерживается
        //тогда по хитроумной формуле изменяем 
        //значение нашей переменной
        if(BtnUp->Read() == false)
        {
            int additive = BtnUp->GetAdditive();
            Array[ActiveScreen]->AddValue(additive);
            return;
        }
        

        //Eсли нажата кнопка вверх в режиме редактора
        //тогда добавляем единицу
        if(BtnDown->FrontPositive() == true)
        {
            //Отменяем режим сна
            SleepCancel();

            Array[ActiveScreen]->DownValue();
            return;
        }

        //Если кнопка нажата, пытаемся имзменить
        //значение, по хитрой формуле
        if(BtnDown->Read() == false)
        {
            int additive = BtnDown->GetAdditive();
            Array[ActiveScreen]->SubValue(additive);
            return;
        }

        //Если в режиме редактора параметров нажата
        //кнопка выбора, переключаем на режим просмотра
        if(BtnSelect->FrontPositive() == true)
        {
            //Отменяем режим сна
            SleepCancel();

            Mode = ReadMode;
            return;
        }
}

/*
 * Метод для навигации по меню
 */
void Menu::Navigation()
{
    //Навигация в зависимости от режима меню
    switch (Mode)
    {
        case ReadMode:  NavigationOfReadMode();  break;
        case WriteMode: NavigationOfWriteMode(); break;
    }
}

/*
 * Метод для обновления состояния экрана
 * (должен вызываться каждый цикл в loop)
 */
void Menu::Update()
{
    //Обновляем состояния таймера
    Timer.Update();

    //Включаем навигацию
    Navigation();

    //Отображаем экран, который надо показать
    Array[ActiveScreen]->Display(LCD, Mode, &Timer_2Hz);   

    

    //Если сработал таймер, тогда возвращаемся на экран 0
    if(Timer.IsTick())
    {
        //Выключаем подсветку
        LCD->noBacklight();

        //Если текущий экран это стартровый экран,
        //тогда выходим из функции
        if( ActiveScreen == 0)
        {
            return;
        }

        //Возвращаемся в режим редактирования
        Mode = ReadMode;

        //Очищаем дисплей
        LCD->clear();

        //Устанавливаем в качестве начального 
        //экрана экран с индексом 0
        ActiveScreen = 0;

        
    }
}
