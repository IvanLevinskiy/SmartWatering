#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#else
#include <WiFi.h>
#include <ESPmDNS.h>

#endif

#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include"Timers/Timer_SD.h"
#include <LiquidCrystal_I2C.h>

//Максимальное количество HOLDING регистров
#define MAX_HOLDING_REGISTERS 37

//TCP порт
#define MODBUS_TCP_PORT  502

//Определение функций модбас
#define MB_FC_NONE 0
#define MB_FC_READ_REGISTERS 3 //Чтение HOLDING регистра
#define MB_FC_WRITE_REGISTER 6 //Запись одного HOLDING регистра
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16 //Запись нескольких HOLDING регистров
//
// MODBUS Error Codes
//
#define MB_EC_NONE 0
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3
#define MB_EC_SLAVE_DEVICE_FAILURE 4

//
// MODBUS MBAP offsets
//
#define MB_TCP_TID 0
#define MB_TCP_PID 2
#define MB_TCP_LEN 4
#define MB_TCP_UID 6
#define MB_TCP_FUNC 7

//ID устройства
#define MB_DEVICE_ID 1

//Функция MODBUS
#define MB_FUNCTION ByteArray[ MB_TCP_FUNC ]

//Modbus TCP Slave
class MODBUS_TCP_SLAVE
{

  public:

    //IP в строковом представлении
    String STR_IP = "";

    //Флаг, разрешающий работу WiFi
    bool WiFi_State;

    //Бит, что клиент подключен
    bool ClientConnected = false;

    //WiFi в режиме AP
    bool AP_MODE = false;

    //Инициализация точки доступа
    void WIFI_AP_INIT();

    //Инициализация настроек для подключения к
    //существующей сети WiFi
    void WIFI_STA_INIT();

    //Получение IP
    void GetIP();

    //Конструктор класса
    MODBUS_TCP_SLAVE();

    //Обновление данных. Метод вызывается в loop
    //Метод для чтения данных из TCP порта
    void MODBUS_UPDATE();

    //Буфер целочисленных значений (HOLDING регистров)
    uint16_t MB_HOLDING_REGISTERS[MAX_HOLDING_REGISTERS];

   private:

        //IP адрес
        //IPAddress ip = IPAddress(192,168,4,1);

        IPAddress ip = IPAddress(192, 168, 100, 100);

        //Шлюз
        IPAddress gateway  = IPAddress(192, 168, 100, 1);

        //Маска сети
        IPAddress subnet  = IPAddress(255, 255, 255, 0);

        //MAC адрес
        uint8_t MAC[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

        //Флаг, помнящий предыдущее состояние флага, разрешающего работу WiFi
        bool WiFi_Old_State = false;

        bool AP_MODE_Old_State = true;

        //Буфер байтов
        byte ByteArray[128];

        //Клиент и сервер
        WiFiServer MBServer = WiFiServer(MODBUS_TCP_PORT);

        //Клиент
        WiFiClient client;

        //Таймер для чтения регистров
        Timer_SD timer_Update = Timer_SD(10);

        //Таймер для проверкри клиента
        Timer_SD timer_CheckClient = Timer_SD(2000);

        //Последнее время сессии
        unsigned long LastTime = 0;

        //Переменная для временного
        //хранения результата
        unsigned long currentTime = 0;

        //Функция для управления WiFi
        void WiFi_CONTROL();

        //Проверка MAC адреса
        bool _checkMacAddres(byte array[]);

        //Длина принятых данных
        int RECIVED_BYTES_LENGTCH = 0;

        //Метод для чтения HOLDING регистров
        void READ_HOLDING_REGISTERS();

        //Метод для изменения значения одного HOLDING регистра
        void WRITE_SINGLE_HOLDING_REGISTER();

        //Метод для изменения значений нескольких HOLDING регистров
        void WRITE_MULTI_HOLDING_REGISTER();

        //Метод для обработки пакета принятых данных
        void MB_NEW_DATA_PROCESSING();

         //Программный таймер
         bool _isTimer(unsigned long period);

};

#endif
