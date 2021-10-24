#include "MENU/TextViewItem.h"

TextViewItem::TextViewItem(char* header, String* row_2)
{
    //Передаем в экземпляр класса заголовок,
    //который отображаться вверху дисплея
    Header = header;
    Row_2 = row_2;
}

//Метод для отображения
void TextViewItem::Display()
{
    //Вывод текста в верхней строке
    printHeader(Header);

    //Вывод текста в нижней строке
    bottomRow = *Row_2;
    printBottomRow();
}

//Переключение шага для редактирования
//(меняем разряды)
bool TextViewItem::IsEdition()
{
    return false;
}

//Сообщаем модели, что нажата кнопка
//вверх
void TextViewItem::BtnUpPressed()
{

}

//Сообщаем модели, что нажата кнопка
//вниз
void TextViewItem::BtnDownPressed()
{

}

//Сообщаем модели, что нажата кнопка
//выбора
void TextViewItem::BtnSelectPressed()
{

}
