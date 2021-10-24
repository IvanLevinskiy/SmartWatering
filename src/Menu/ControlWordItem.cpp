#include "MENU\ControlWordItem.h"

/**
 *Конструктор
 *header - заголовок экрана
 *object - объект, связанный с текущим экраном
**/
ControlWordItem::ControlWordItem(char* header, void * object, int bit)
{
    //Передаем в экземпляр класса заголовок,
    //который отображаться вверху дисплея
    Header = header;

    //Передаем указатель на элемент массива
    //в свойства класса
    Pointer = object;

    //Передача бита в слове
    Bit = bit;

}

//Приведение булевой переменной
//к типу стринг
String ControlWordItem::BoolToStr(uint16_t value)
{
    uint16_t new_value = value & (uint16_t)(1 << Bit);

    if(new_value > 0)
    {
        return TEXT_TRUE;
    }

    return TEXT_FALSE;

}

//Приведение булевой переменной
//к типу стринг
String ControlWordItem::BoolToStr(bool value)
{
    if(value == true)
    {
        return TEXT_TRUE;
    }

    return TEXT_FALSE;

}

//Метод для получения бита в слове управления
bool ControlWordItem::GetBit()
{
    return GetBit(Bit, Registr_Value);
}

//Метод для получения бита в слове управления
bool ControlWordItem::GetBit(int _bit, uint16_t _value)
{
    uint16_t res = _value;
    res &= (uint16_t)(1<<_bit);

    //возвращаем результат
    if(res == 0) return false;
    return true;
}

// Запись бита в слово управления
void ControlWordItem::WriteBit()
{
      //Получаем указатель на переменную
      uint16_t* pnt = (uint16_t*)Pointer;

      //получаем значение из указателя
      Registr_Value = *pnt;

      if(TempBitState == true)
      {
          Registr_Value |= (uint16_t)(1<<Bit);
      }
      else
      {
          Registr_Value &= ~((uint16_t)(1<<Bit));
      }

      //Записываем новое значение в память
      *pnt = Registr_Value;

      //Разрешение для видимости следующего окна
      VisibilityNextItem = TempBitState;
}

//Метод для отображения
void ControlWordItem::Display()
{
    //Обновляем состояние таймера
    Timer_2Hz->Update();

    //Признак, что параметр только для чтения
    if(ReadOnly == true)
    {
        LCD->setCursor(1, 1);
        LCD->print("R");
    }

    //Печатаем заголовок
    printHeader(Header);

    //Печатаем в дисплей то значение, которое планируем изменить
    LCD->setCursor(12, 1);

    //Режим просмотра переменной
    if(mode == ReadMode)
    {
        //Получаем указатель на переменную
        uint16_t* pointer = (uint16_t*)Pointer;
        uint16_t value = *pointer;
        bool bs = GetBit(Bit, value);

        //Переносим состояние бита
        //в разрешение видимости экрана
        VisibilityNextItem = bs;

        bottomRow = BoolToStr(bs);
        printBottomRow();

        //Затираем карандаш
        AnimationStop();
        return;
    }

    bottomRow = BoolToStr(TempBitState);
    printBottomRow();

    //Мигаем
    if(Timer_2Hz->IsTick())
    {
        AnimationEdit();
        return;
    }
}

//Добавляет к значению 1
void ControlWordItem::UpValue()
{
    //Инвертируем сигнал
    if(TempBitState)
    {
        TempBitState = false;
    }
    else
    {
        TempBitState = true;
    }
}

//Вычитает из значения 1
void ControlWordItem::DownValue()
{
    //Инвертируем сигнал
    if(TempBitState)
    {
        TempBitState = false;
    }
    else
    {
        TempBitState = true;
    }
}

//Начало редактирования
void ControlWordItem::StartEdit()
{
    //Получаем указатель на переменную
    uint16_t* pnt = (uint16_t*)Pointer;

    //получаем значение из указателя
    uint16_t value =  *pnt;

    //Получение бита
    TempBitState = GetBit(Bit, value);
}

//Конец редактирования
void ControlWordItem::EndEdit()
{
    //Установка бита
    WriteBit();
}

//Переключение шага для редактирования
//(меняем разряды)
bool ControlWordItem::IsEdition()
{
    return false;
}

void ControlWordItem::BtnUpPressed()
{
    if(mode == WriteMode)
    {
        UpValue();
    }
}

void ControlWordItem::BtnDownPressed()
{
    if(mode == WriteMode)
    {
        DownValue();
    }
}

void ControlWordItem::BtnSelectPressed()
{
      //Если разрешение на редактирование нет
      //тогда пропускаем ход
      if(ReadOnly == true)
      {
          return;
      }


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
