#include "MENU/BoolItem.h"

BoolItem::BoolItem(char* header, void * object)
{
    //Передаем в экземпляр класса заголовок,
    //который отображаться вверху дисплея
    Header = header;

    //Передаем указатель на элемент массива
    //в свойства класса
    Pointer = object;

    //Сохраняем дисплей
    this->LCD = LCD;
}

/**
 *Конструктор
 *header - заголовок экрана
 *object - объект, связанный с текущим экраном
**/
char* BoolItem::BoolToStr(bool val)
{
    //Вычитаем позицию курсора
    LCD->setCursor(16, 1);

    if(val < 1)
    {
        return TEXT_FALSE;
    }

    return TEXT_TRUE;
}

char* BoolItem::BoolToStr()
{
    return "    ";
}

//Метод для отображения
void BoolItem::Display()
{
    //Обновляем состояние таймера
    Timer_2Hz->Update();

    //Выводим заголовок
    printHeader(Header);

    //Печатаем в дисплей то значение, которое планируем изменить
    if(mode == ReadMode)
    {
        //Получаем указатель на переменную
        bool* pointer = (bool*)Pointer;
        bool value = *pointer;

        LCD->rightToLeft();
        LCD->print(BoolToStr(value));
        LCD->leftToRight();
        return;
    }

    //Если блинкер == истине
    if(Timer_2Hz->GetBlinkerState())
    {
        LCD->rightToLeft();
        LCD->print(BoolToStr(temp_value));
        LCD->leftToRight();
    }
    else
    {
       LCD->rightToLeft();
       LCD->print(BoolToStr());
       LCD->leftToRight();
    }

    //Мигаем
    if(Timer_2Hz->IsTick())
    {
        //LCD->AnimationEdit();
        return;
    }
}

//Добавляет к значению 1
void BoolItem::UpValue()
{
    //Инвертируем сигнал
    if(temp_value)
    {
        temp_value = false;
    }
    else
    {
        temp_value = true;
    }
}

//Вычитает из значения 1
void BoolItem::DownValue()
{
    //Инвертируем сигнал
    if(temp_value)
    {
        temp_value = false;
    }
    else
    {
        temp_value = true;
    }
}

//Начало редактирования
void BoolItem::StartEdit()
{
     //Получаем указатель на переменную
    bool* pnt = (bool*)Pointer;

    //получаем значение из указателя
    temp_value =  *pnt;
}

//Конец редактирования
void BoolItem::EndEdit()
{
    //Получаем указатель на переменную
    bool* pnt = (bool*)Pointer;

    //Записываем новое значение в память
    *pnt = temp_value;
}

//Переключение шага для редактирования
//(меняем разряды)
bool BoolItem::IsEdition()
{
    return false;
}

void BoolItem::BtnUpPressed()
{
    if(mode == WriteMode)
    {
        UpValue();
    }
}

void BoolItem::BtnDownPressed()
{
    if(mode == WriteMode)
    {
        DownValue();
    }
}

void BoolItem::BtnSelectPressed()
{
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
