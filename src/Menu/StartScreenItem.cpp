#include "MENU/StartScreenItem.h"

StartScreenItem::StartScreenItem(char* header, char* row_2)
{
    //Передаем в экземпляр класса заголовок,
    //который отображаться вверху дисплея
    Header = header;
    Row_2 = row_2;
}

//Метод для отображения
void StartScreenItem::Display()
{
    printHeader(Header);
    LCD->setCursor(0,1);
    LCD->print(Row_2);
}

//Переключение шага для редактирования
//(меняем разряды)
bool StartScreenItem::IsEdition()
{
    return false;
}
