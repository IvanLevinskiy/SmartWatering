
#include <ESP8266WiFi.h>
#include "EEPROM.h"
#include <Ticker.h>
//#include <Valve/Valve.h>

//Максимальное количество HOLDING регистров
#define MAX_HOLDING_REGISTERS 37

//TCP порт
#define MODBUS_TCP_PORT  502

//Буфер целочисленных значений (HOLDING регистров)
uint16_t MB_HOLDING_REGISTERS[MAX_HOLDING_REGISTERS];

/**
 * ОПРЕДЕЛЕНИЕ ДЛЯ ЧАСОВ
 **/
#define CURRUNT_TIME  MB_HOLDING_REGISTERS[5] //Текущее время в минутах
#define SECONDS       MB_HOLDING_REGISTERS[6] //Количество секунд
 

//Температура воздуха
#define TEMPERATURE_AIR     MB_HOLDING_REGISTERS[0]

//Температура воды в баке
#define TEMPERATURE_WAITER  MB_HOLDING_REGISTERS[1]

//Температура воздуха
#define TEMPERATURE_T1      MB_HOLDING_REGISTERS[2]

//Температура воздуха
#define TEMPERATURE_T2      MB_HOLDING_REGISTERS[3]

//Уровень воды в баке
#define LEVEL_WAITER        MB_HOLDING_REGISTERS[4]

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

/**Функция MODBUS **/
#define MB_FUNCTION ByteArray[ MB_TCP_FUNC ]

extern "C" 
{
  #include "user_interface.h";
}

/**
    ##########  ИНИЦИАЛИЗАЦИЯ ЭКЗКМПЛЯРОВ ОБЪЕКТОВ И ПЕРЕМЕННЫХ ПРОГРАММЫ  #################
**/
//Таймер - счетчик
Ticker timer;

//Установка статического IP
//172.20.30.2  172.20.30.2
IPAddress ip     (192,168,4,1); 
IPAddress gateway(192,168,4,1);
IPAddress subnet (255,255,255,0);
uint8_t MAC[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

//Клиент и сервер
WiFiServer MBServer(MODBUS_TCP_PORT);
WiFiClient client;


//Определения для адресов регистров с данными
#define MB_CONTROL_WORD  MB_HOLDING_REGISTERS[firstRegister]     //Слово управления
#define MB_STRT_TIME     MB_HOLDING_REGISTERS[firstRegister + 1] //Время процесса начала
#define MB_STP_TIME      MB_HOLDING_REGISTERS[firstRegister + 2] //Время окончания процесса

//Класс клапана

class Valve
{
  public:
    //Активация ручного режима
    bool AUTO_MODE = true;

    //Разрешение, пока не применено
    bool Enable;

    //Форирование катшки реле
    bool Force;

    //Символьное имя клапана для отладки
    char* Name;

    //Время начала полива, мин
    uint16_t  StartTime;

    //Время окончания полива, мин
    uint16_t  EndTime;

    String DebagText = "";

    //Конструктор класса
    Valve()
    {
      
    }

    //Конструктор класса
    Valve(char* _name, int _pin, int _firstRegister)
    {
        //Очищаем текст, который используется для отладки
        DebagText = "";
             
        Name = _name;
        pin = _pin;
        firstRegister = _firstRegister;
        
        //Чтение настроек из EEPROM при запуске
        MB_STRT_TIME = StartTime =  word(EEPROM.read(_firstRegister), EEPROM.read(_firstRegister + 1));
        MB_STP_TIME = EndTime =     word(EEPROM.read(_firstRegister + 2), EEPROM.read(_firstRegister + 3));
        MB_CONTROL_WORD = CONTROL_WORD = word(EEPROM.read(_firstRegister + 4), EEPROM.read(_firstRegister + 5));

        //Настройка выхода реле
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
    }

    //Обновление данных. Метод вызывается в loop
    //Управляет логикой
    void Update()
    { 
      //Переменная для временного хранения результата состояния пина,
      //к которому полключается клапан
      bool tempStateValve = false;

      //Если слово управления пустое, пропускаем цикл
      if(MB_CONTROL_WORD == 0)
      {
          goto M1;
      }

      //Если в ручную подана команда включить, тогда
      //независимо ни от чего включаем порт
      if(Force == true)
      {
          tempStateValve = true;
          goto M1;
      }

      //Если режим работы ручной
      if(AUTO_MODE == true)
      {              
          //Проверяем есть попадает ли текущее время в диапазон
          //1. Если время начала меньше времени окончания полива
          if( StartTime < EndTime)
          {                    
            tempStateValve = StartTime <= CURRUNT_TIME && CURRUNT_TIME < EndTime;
            goto M1;
          }

          //2. Если время начала больше времени окончания полива, т.е полив продолжается и ночью
          if( StartTime > EndTime)
          {
              bool temp_1 = StartTime <= CURRUNT_TIME && CURRUNT_TIME < 1441;
              bool temp_2 = 0 <= CURRUNT_TIME && CURRUNT_TIME < EndTime;
              tempStateValve = temp_1 || temp_2;
              goto M1;
          }
      }

      //Если не включена FORCE и программа автоматики
      //тогда выключаем переменную
      if(Force == false && AUTO_MODE == false)
      {
          tempStateValve = false;
      }

      //Передаем состояние пина реле с инверсией,
      //так как логика у реле обратная
      M1: digitalWrite(pin, ! tempStateValve);
      
      //Записываем сотояние реле в регистр,
      //для того чтоб было видно его состояние
      bitWrite(MB_HOLDING_REGISTERS[firstRegister], 4, tempStateValve);
    }
    
    //Получаем регистр, СЛОВО УПРАВЛЕНИЯ и другие
    void UpdateRegister()
    {     
        //Проверяем, если регистр (слово управления) изменился,
        //тогда пытаемся его парсим
        if( MB_CONTROL_WORD != CONTROL_WORD) 
        {
          CONTROL_WORD = MB_CONTROL_WORD;
          Force = GetBit(CONTROL_WORD, 0);        
          AUTO_MODE = GetBit(CONTROL_WORD, 1);

          //Сохраняем состояние регистра в EEPROM
          EEPROM.write(firstRegister + 4, highByte(CONTROL_WORD));      
          EEPROM.write(firstRegister + 5, lowByte (CONTROL_WORD)); 
          EEPROM.commit();  

          Serial.print("CW Reccord: ");
          Serial.print("cell: ");
          Serial.print(firstRegister + 4);
          Serial.print(" value: ");
          Serial.println(CONTROL_WORD);
        }

        //Проверяем, если регистр (время включения) изменился, 
        //тогда его сохраняем в EEPROM
        if(MB_STRT_TIME != StartTime) 
        {
          StartTime =  MB_STRT_TIME;
          EEPROM.write(firstRegister + 0, highByte(StartTime));      
          EEPROM.write(firstRegister + 1, lowByte (StartTime)); 
          EEPROM.commit();

          Serial.print("Start time reccord: ");
          Serial.print("cell: ");
          Serial.print(firstRegister);
          Serial.print(" value: ");
          Serial.println(StartTime);
        }

        //Проверяем, если регистр (время выключения) изменился, 
        //тогда его сохраняем в EEPROM
        if(MB_STP_TIME != EndTime) 
        {
          EndTime = MB_STP_TIME;
          EEPROM.write(firstRegister + 2, highByte(EndTime));      
          EEPROM.write(firstRegister + 3, lowByte(EndTime));  
          EEPROM.commit(); 

          Serial.print("Stop time reccord: ");
          Serial.print("cell: ");
          Serial.print(firstRegister + 2);
          Serial.print(" value: ");
          Serial.println(EndTime);
        }
    }

   
   private:  
          //Пин, к которому подключен
          //клапан
          int pin;

          //Состояние HRegister
          uint16_t CONTROL_WORD;

          //Стартовый регистр
          int firstRegister;

          //Получаем бит из слова
          bool GetBit(uint16_t _value, int _bit)
          {
              uint16_t res = _value;
              res &= (uint16_t)(1<<_bit);

              //возвращаем результат
              if(res == 0) return false;
              return true;
          }
};


//Инициализация клапанов
Valve  Valve_1;
Valve  Valve_2;
Valve  Valve_3;
Valve  Valve_4;

//Буфер байтов
byte ByteArray[260];

//Обработка тиков
void timer_tick() 
{
  //Каждую секунду инкрементируем секунды
  SECONDS ++;

  //Если секунд больше 60,
  //тогда инкрементируем минуты,
  //секунды сбрасываем
  if(SECONDS >= 60)
  {
    //Инкрементируем минуты
    CURRUNT_TIME ++;

    //Обнуляем счетчик секунд
    SECONDS = 0;
  }

  //Если часы досчитали до 1440 минут, тогда их обнуляем
  //Это значит, что наступило 00:00 часов
  if(CURRUNT_TIME >= 1440)
  {
    CURRUNT_TIME = 0;  
  }

  //Обновляем состояние клапанов
  Valve_1.Update();
  Valve_2.Update();
  Valve_3.Update();
  Valve_4.Update();
}

void MB_NEW_DATA_RECIVED()
{
  //Обновление данных в клапанах
   Valve_1.UpdateRegister();
   Valve_2.UpdateRegister();
   Valve_3.UpdateRegister();
   Valve_4.UpdateRegister();
}

//Длина принятых данных
int RECIVED_BYTES_LENGTCH = 0;

bool _checkMacAddres(byte array[])
{
  bool result = 0;
  for (byte i = 0; i < 6; i++)
  {
    if (array[i] == 255) 
    {
      return 0;
    }
    if (array[i] > 0 ) 
    {
      result = 1;
    }
  }
  return result;
}

 void WIFI_AP_INIT()
{   
    WiFi.mode(WIFI_AP);
    //WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.softAP("Watering", "");
    
    if ( ! (_checkMacAddres(MAC))) 
    {
      WiFi.softAPmacAddress(MAC);
    }
    
    delay(3000);
    MBServer.begin();
}

void WIFI_STA_INIT()
{    
   //Подключение к сети
   WiFi.begin("MyAsus", "Ae3182126");
      
   //Конфигурация параметров сети модуля
   WiFi.config(ip, gateway, subnet);

  //WiFi.setAutoConnect(true);
  //WiFi.setAutoReconnect(true);
  //WiFi.persistent(false);

   //Ждем 10 секунд, если соединение не произошло
   //выходим из метода
   while(WiFi.status() != WL_CONNECTED) 
   {
      delay(500);
      Serial.print(".");
   }

   MBServer.begin();
}

 void AP_CONNECT()
{
    if ( _checkMacAddres(MAC)) 
    {
        //wifi_set_macaddr(1, const_cast<uint8*>(MAC));
    }

    WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.softAP("Watering", "");
    
    if ( ! (_checkMacAddres(MAC))) 
    {
        WiFi.softAPmacAddress(MAC);
    }

    delay(3000);
    MBServer.begin();
}

//Функция, котороя обрабатывает запрос от сервера
//на чтение HOLDING регистров
void READ_HOLDING_REGISTERS()
{
  //************ЗАПРОС
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х03)
  //ByteArray[8] -  адрес первого регистра (старший байт)
  //ByteArray[9] -  адрес первого регистра (младший байт)
  //ByteArray[10] - количество регистров (старший байт)
  //ByteArray[11] - количество регистров (младший байт)

  //Определяем адрес певого регистра, который надо прочиталь
  word FIRST_REGISTER_ADDRESS = word(ByteArray[8],ByteArray[9]);

  //Определяем количество регистров, которые будем читать
  word QUANTITY_REGISTERS = word(ByteArray[10],ByteArray[11]);

  //************ОТВЕТ
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х03)
  //ByteArray[8] -  Количество байт далее
  //ByteArray[9] -  Значение регистра 1 (старший байт)
  //ByteArray[10] - Значение регистра 1 (младший байт)
  //ByteArray[11] - Значение регистра 2 (старший байт)
  //ByteArray[12] - Значение регистра 2 (младший байт)
  //и так далее

  byte BYTE_COUNT = 0;      //Счетчик байтов
  word MESSAGE_LENGTCH = 3; //Длина сообщения

  //Наполняем телеграмму значениями регистров, которые надо прочиталь
  for(int i = 0; i < QUANTITY_REGISTERS; i++) 
  {
     ByteArray[9  + i * 2] = highByte(MB_HOLDING_REGISTERS[FIRST_REGISTER_ADDRESS + i]);
     ByteArray[10 + i * 2] = lowByte(MB_HOLDING_REGISTERS [FIRST_REGISTER_ADDRESS + i]);
     BYTE_COUNT += 2;
  }

   //Считаем, какая длина сообщения получилась
   MESSAGE_LENGTCH += word(0, BYTE_COUNT);

   //Переносим оставшиеся данные в телеграмму
   //Длина телеграммы
   ByteArray[4] = highByte(MESSAGE_LENGTCH);
   ByteArray[5] = lowByte(MESSAGE_LENGTCH);

   //Количество байт за заголовком
   ByteArray[8] = BYTE_COUNT;
    
   client.write((const uint8_t *)ByteArray, 6 + MESSAGE_LENGTCH);
   MB_FUNCTION = MB_FC_NONE;
   
   Serial.print("SEND: ");

   for(int i = 0; i < 6 + MESSAGE_LENGTCH; i++) 
  {
      Serial.print( ByteArray[i] );
      Serial.print(" ");
  }

  Serial.println("");
}

void WRITE_SINGLE_HOLDING_REGISTER()
{
  //************ЗАПРОС
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х10)
  //ByteArray[8] -  адрес регистра (старший байт)
  //ByteArray[9] -  адрес регистра (младший байт)
  //ByteArray[10] - значение (старший байт)
  //ByteArray[11] - значение (младший байт)

  //Определяем адрес певого регистра, который надо прочиталь
  word REGISTER_ADDRESS = word(ByteArray[8],ByteArray[9]);

  //Записываем принятое значение в регистр
  MB_HOLDING_REGISTERS[REGISTER_ADDRESS] = word(ByteArray[10],ByteArray[11]);

  //************ОТВЕТ
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х10)
  //ByteArray[8] -  Адрес регистра 1 (старший байт)
  //ByteArray[9] -  Адрес регистра 1 (младший байт)
  //ByteArray[10] - Значение (старший байт)
  //ByteArray[11] - Значение (младший байт)
  
  //Отвечаем (телеграмма та же, которую приняли)
  client.write((const uint8_t *)ByteArray, 12);
  MB_FUNCTION = MB_FC_NONE;
}

void WRITE_MULTI_HOLDING_REGISTER()
{
  //************ЗАПРОС
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х10)
  //ByteArray[8] -  адрес первого регистра (старший байт)
  //ByteArray[9] -  адрес первого регистра (младший байт)
  //ByteArray[10] - количество регистров (старший байт)
  //ByteArray[11] - количество регистров (младший байт)
  //ByteArray[12] - количество байт далее
  //ByteArray[13] - значение 1 (старший байт)
  //ByteArray[14] - значение 1 (младший байт)
  //ByteArray[15] - значение 2 (старший байт)
  //ByteArray[16] - значение 2 (младший байт)

  //Определяем адрес певого регистра, который надо прочиталь
  word FIRST_REGISTER_ADDRESS = word(ByteArray[8],ByteArray[9]);

  //Определяем количество регистров, которые будем читать
  word QUANTITY_REGISTERS = word(ByteArray[10],ByteArray[11]);

  //************ОТВЕТ
  //ByteArray[0] -  идентификатор транзакции(старший байт)
  //ByteArray[1] -  идентификатор транзакции(младший байт)
  //ByteArray[2] -  идентификатор протокола (старший байт)
  //ByteArray[3] -  идентификатор протокола (младший байт)
  //ByteArray[4] -  длина сообщения (старший байт)
  //ByteArray[5] -  длина сообщения (младший байт)
  //ByteArray[6] -  адрес устройства
  //ByteArray[7] -  код функции (0х10)
  //ByteArray[8] -  Адрес регистра 1 (старший байт)
  //ByteArray[9] -  Адрес регистра 1 (младший байт)
  //ByteArray[10] - Количество записаных регистров (старший байт)
  //ByteArray[11] - Количество записаных регистров (младший байт)

  //Устанавливаем длину сообщения, она всегда фиксированая
  ByteArray[4] = 0;
  ByteArray[5] = 6;
  
  //Записываем данные из телеграммы в регистры
  for(int i = 0; i < QUANTITY_REGISTERS; i ++)
  {
      MB_HOLDING_REGISTERS[FIRST_REGISTER_ADDRESS + i] = word(ByteArray[ 13 + i * 2],ByteArray[14 + i * 2]);
  }
  
   client.write((const uint8_t *)ByteArray, 12); 
   MB_FUNCTION = MB_FC_NONE;
}

// Обработка запроса
void MB_NEW_DATA_PROCESSING()
{
  //Если адрес уствойства не то, тогда 
  //пропускае
  if(ByteArray[MB_TCP_UID] != MB_DEVICE_ID)
  {
     return;
  }
        
  switch(MB_FUNCTION) 
      {
        //Если нет функции
        case MB_FC_NONE:  break;

        //ЧТЕНИЕ НЕСКОЛЬКИХ РЕГИСТРОВ КОД: 0х03
        case MB_FC_READ_REGISTERS: READ_HOLDING_REGISTERS(); break; 

        //ЗАПИСЬ ОДНОГО РЕГИСТРА КОД: 0х06
        case MB_FC_WRITE_REGISTER: WRITE_SINGLE_HOLDING_REGISTER(); break; 
      
        //ЗАПИСЬ НЕСКОЛЬКИХ РЕГИСТРОВ КОД: 0х10
        case MB_FC_WRITE_MULTIPLE_REGISTERS: WRITE_MULTI_HOLDING_REGISTER(); break;
      }

      MB_NEW_DATA_RECIVED();
}

void MODBUS_POLL()
{ 
  //Если клиента нет, пытаемся его подключить
  if(! client)
  {
      client = MBServer.available();
      Serial.println(WiFi.softAPIP());
      Serial.println(":  MODBUS  not client");
      delay(100);
      return;   
  }
  
    //Modbus TCP/IP
    //Читаем телеграмму
    if(client.available() > 0) 
    {
       Serial.print("\n\n\nRECIVED: ");   
       RECIVED_BYTES_LENGTCH = 0;

       
       while(client.available()) 
       {
         ByteArray[RECIVED_BYTES_LENGTCH] = client.read();
         Serial.print(ByteArray[RECIVED_BYTES_LENGTCH]);
         Serial.print(" ");
         RECIVED_BYTES_LENGTCH++;
       }
         
        Serial.print("\n");
        client.flush();

        MB_NEW_DATA_PROCESSING();
    }
}

//ПОКА ЧТО ЗНАЧЕНИЯ ЗАДАНЫ СТАТИЧНО
void DataProcess_Update()
{
    TEMPERATURE_AIR = 23;
    TEMPERATURE_WAITER = 13; 
    TEMPERATURE_T1 = 22;     
    TEMPERATURE_T2 = 21;    
    LEVEL_WAITER = analogRead(A0);
}


void setup() 
{ 
  //Инициализируем последовательный порт
  //для вывода информации диагностики
  Serial.begin(115200);

  //Инициализация EEPROM
  EEPROM.begin(512);

  Valve_1 = Valve("", 13, 14);
  Valve_2 = Valve("", 12, 20);
  Valve_3 = Valve("", 14, 26);
  Valve_4 = Valve("", 4,  32);


  //Создаем сеть
  WIFI_AP_INIT();
  //WIFI_STA_INIT();
  //AP_CONNECT();

  //Подпись на событие тика
  timer.attach(1, timer_tick);

  //Выводим диагностическую информацию,
  //что сервер устройство запущено
  Serial.println("\nDevice is starting");
}



void loop() 
{
    //Чтение данных от датчиков 
    //и передача их в регистры
    //DataProcess_Update();
      
    //Обработка MODBUS 
    MODBUS_POLL();

}