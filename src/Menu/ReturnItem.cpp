#include "MENU/ReturnItem.h"

ReturnItem::ReturnItem(void * object)
{
    //Передаем в экземпляр класса заголовок,
    //который отображаться вверху дисплея
    //Header = header;
    Pointer = object;

}

//Метод для отображения
void ReturnItem::Display()
{
    printHeader("BO3BPAT   ");

    LCD->rightToLeft();
    LCD->setCursor(15, 1);
    LCD->print("?KO");
    LCD->leftToRight();

    //LCD->setCursor(0, 1);
    //LCD->print("                ");
}

void ReturnItem::BtnUpPressed()
{


}

void ReturnItem::BtnDownPressed()
{
}

void ReturnItem::BtnSelectPressed()
{
  //Запись необходимых значений
  ContentItem* ParentItem = (ContentItem*)Pointer;
  ParentItem->ContentView = false;
  ParentItem->ActiveScreen = 0;

  //Очистка дисплея
  LCD->clear();
}

//Переключение шага для редактирования
//(меняем разряды)
bool ReturnItem::IsEdition()
{
    return false;
}
