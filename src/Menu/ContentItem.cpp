#include "Menu/ContentItem.h"

/**
 *Конструктор
 *header - заголовок экрана
**/
ContentItem::ContentItem(char* header)
{
    Header = header;
}

//Метод для отображения
void ContentItem::Display()
{
    //Устанавливаем курсор во 2 колонку
    //и выводим заголовок
    LCD->setCursor(2, Row);
    LCD->print(Header);
}

/*
    Метод для отображения содержимого
*/
void ContentItem::DisplayContent()
{
    //Если перешли к параметрам в дереве
    if(Array[ActiveScreen] != NULL)
    {
      Array[ActiveScreen]->Display();
      return;
    }
}

/**
 *  Метод для добавления нового экрана
 */
void ContentItem::AddScreen(BaseItem *screen)
{
    //Если превышено максимальное количество
    //пунктов меню, тогда пропускаем дальнейшие шаги
    if(ScreenCount == MAX_SCEEN)
    {
        return;
    }

    //Передаем указатель на таймер
    //для эффектов
    screen->BindingTimer(Timer_2Hz);

    //Присваиваем индикатор строки в дисплее
    screen->Row = ScreenCount;

    //Добавляем указатель на экран
    Array[ScreenCount] = screen;

    //Инкрементируем счетчик экранов
    ScreenCount++;
}

/*
    Нажатие на кнопку ВВЕРХ
*/
void ContentItem::BtnUpPressed()
{
    //Если в режиме просмотра содержимого
    if(Array[ActiveScreen]->mode == ReadMode)
    {
        if(ActiveScreen < ScreenCount - 1)
        {
            //Очищаем дисплей
            LCD->clear();

            ActiveScreen ++;
        }
    }
    //Иначе передаем сигнал встроенному экрану
    else
    {
      Array[ActiveScreen]->BtnUpPressed();
    }
}

/*
    Нажатие на кнопку ВНИЗ
*/
void ContentItem::BtnDownPressed()
{
  //Если в режиме просмотра содержимого
  if(Array[ActiveScreen]->mode == ReadMode)
  {
    if(ActiveScreen > 0)
    {
        //Очищаем дисплей
        LCD->clear();

        ActiveScreen --;
    }
  }
  //Иначе передаем сигнал переменной
  else
  {
    Array[ActiveScreen]->BtnDownPressed();
  }
}

/*
    Нажатие на кнопку ВЫБОР
*/
void ContentItem::BtnSelectPressed()
{
    //Если содержимое было выключено
    //переходим к отображению содержимого
    if(ContentView == false)
    {
        //Очищаем дисплей
        LCD->clear();

        //Переходим в режим просмотра
        //содержимого
        ContentView = true;
        return;
    }

    if(Array[ActiveScreen] != 0)
    {
        Array[ActiveScreen]->BtnSelectPressed();
    }
}

void  ContentItem::BindingLCD(LiquidCrystal_I2C* lcd)
{
    //Передача дисплея
    LCD = lcd;

    //Передаем указатель на дисплей
    //каждому item
    for(int i = 0; i < ScreenCount; i ++)
    {
        Array[i]->BindingLCD(lcd);
        Array[i]->BindingTimer(Timer_2Hz);
    }
}
