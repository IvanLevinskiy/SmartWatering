#ifndef BOOL_ITEM_H
#define BOOL_ITEM_H

#include <LiquidCrystal_I2C.h>
#include "Timers/Timer_SD.h"
#include "Menu/BaseItem.h"

//Класс для редактирования булевых переменных
class BoolItem : public BaseItem
{

    private:

        //Временная переменная
        bool temp_value;

        //Позиция курсора для вывода значения
        int CursorPositionValue = 12;

    public:

        /**
         *Конструктор
         *header - заголовок экрана
         *object - объект, связанный с текущим экраном
        **/
        BoolItem(char* header, void * object);

        char* BoolToStr(bool val);

        char* BoolToStr();

        //Метод для отображения
        void Display() override;

        //Добавляет к значению 1
        void UpValue();

        //Вычитает из значения 1
        void DownValue();

        //Начало редактирования
        void StartEdit();

        //Конец редактирования
        void EndEdit();

        //Переключение шага для редактирования
        //(меняем разряды)
        bool IsEdition() override;

        //Сообщаем модели, что нажата кнопка
        //вверх
        void BtnUpPressed() override;

        //Сообщаем модели, что нажата кнопка
        //вниз
        void BtnDownPressed() override;

        //Сообщаем модели, что нажата кнопка
        //выбора
        void BtnSelectPressed() override;

        //Текст при значении true
        char* TEXT_TRUE = " ON";

        //Текст при значении false
        char* TEXT_FALSE = "OFF";

};

#endif
