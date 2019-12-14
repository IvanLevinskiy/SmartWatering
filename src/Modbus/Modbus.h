#include <ESP8266WiFi.h>

//Максимальное количество HOLDING регистров
#define MAX_HOLDING_REGISTERS 37

//TCP порт
#define MODBUS_TCP_PORT  502

//Определение функций модбас
#define MB_FC_NONE 0
#define MB_FC_READ_REGISTERS 3 //implemented
#define MB_FC_WRITE_REGISTER 6 //implemented
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16 //implemented
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
#define MB_DEVICE_ID 0

//Функция MODBUS 
#define MB_FUNCTION ByteArray[ MB_TCP_FUNC ]

//Modbus TCP Slave
class MODBUS_TCP_SLAVE
{

  public:

    //Инициализация точки доступа
    void WIFI_AP_INIT();

    //Инициализация точки доступа
    void AP_CONNECT();

    //Инициализация настроек для подключения к 
    //существующей сети WiFi
    void WIFI_STA_INIT();

    //Буфер целочисленных значений (HOLDING регистров)
    uint16_t MB_HOLDING_REGISTERS[MAX_HOLDING_REGISTERS];

    //Конструктор класса
    MODBUS_TCP_SLAVE();

    //Обновление данных. Метод вызывается в loop
    //Метод для чтения данных из TCP порта
    void MODBUS_UPDATE();
      
  
   private:

        //Установка статического IP
        IPAddress ip     (192, 168, 4, 1); 
        IPAddress gateway(192, 168, 4, 1);
        IPAddress subnet (255, 255, 255, 0);
        uint8_t MAC[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};


        //Буфер байтов
        byte ByteArray[260];

        //Клиент и сервер
        WiFiServer MBServer(MODBUS_TCP_PORT);
        WiFiClient client;

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
};