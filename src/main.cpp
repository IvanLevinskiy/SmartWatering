
/**
void setup()
{
}

void loop()
{
}
**/

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include <Menu/Menu.h>
#include <MODBUS/Modbus_TCP_Slave.h>
#include <RotaryEncoder.h>
#include "MODBUS/ModbusMaster.h"
#include "MODBUS/Modbus_RTU_Device.h"
#include "Menu/SymbolsDefinition.h"
#include <Valve/Valve.h>
#include "RTClib.h"

#include "esp_system.h"

const int wdtTimeout = 3000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 16, 2);

//Главное меню
Menu MainMenu = Menu(&LCD);

//Энкодер
RotaryEncoder* encoder;

//Экземпляр класса MODBUS
MODBUS_TCP_SLAVE* MODBUS = new MODBUS_TCP_SLAVE();

//Тики энкодера
int ticks;

//Клапана
Valve*  Valve_0;
Valve*  Valve_1;
Valve*  Valve_2;
Valve*  Valve_3;

int FreeRAM = 0;

//const int wdtTimeout = 3000;  //time in ms to trigger the watchdog
//hw_timer_t *timer = NULL;

//Плата расширения выходов
MODBUS_RTU_DEVICE ExtendedBoard_1 = MODBUS_RTU_DEVICE(1, &Serial, 19);

Button btn = Button(16);

RTC_DS3231 rtc;


//Время начала цикла loop
unsigned long CycleStart= 0;

//Время окончания цикла loop
unsigned long CycleEnd= 0;

//Время выполнения цикла
int CycleTime = 0;

//Пробный регистр для чтения
uint16_t Reg_0 = 0;

void IRAM_ATTR resetModule()
{
    ets_printf("reboot\n");
    esp_restart();
}

//Метод для обработки тиков энкодера
void encoderStep()
{
    //Обновляем состояние энкодера
    encoder->tick();

    //Получение тиков энкодера
    ticks = encoder->getPosition();
}

Timer_SD updateTimer = Timer_SD(2500);

/* EEPROM карта
Update modbus time 0 - 1

Клапан 1  14 - 19
Клапан 2  20 - 25
Клапан 3  26 - 31
Клапан 4  32 - 37
*/

void setup()
{
  //Инициализируем последовательный порт
  //для вывода информации диагностики
  Serial.begin(9600);
  //Serial.setTimeout(500);

  updateTimer.Start();

  //Строим меню
  /*
    РЕЛЕ 1
  */
  ContentItem* item_1 = new ContentItem("PE\3E 1          ");

  //РЕЖИМ
  ControlWordItem* ControlWordItem_10 = new ControlWordItem("PE\3E 1 PE\7\5M     ",     &MODBUS->MB_HOLDING_REGISTERS[14], 1);
  ControlWordItem_10->TEXT_TRUE = "ABTO";
  ControlWordItem_10->TEXT_FALSE = "P\2\6";
  item_1->AddScreen(ControlWordItem_10);

  //ВРЕМЯ ВКЛ
  TimeItem* TimeItem_10 = new TimeItem("PE\3E 1 BP. BK\3  ", &MODBUS->MB_HOLDING_REGISTERS[15]);
  TimeItem_10->IsVisibility = &ControlWordItem_10->VisibilityNextItem;
  item_1->AddScreen(TimeItem_10);

  //ВРЕМЯ ВЫКЛ
  TimeItem* TimeItem_11 = new TimeItem("PE\3E 1 BP. B\4K\3 ", &MODBUS->MB_HOLDING_REGISTERS[16]);
  TimeItem_11->IsVisibility = &ControlWordItem_10->VisibilityNextItem;
  item_1->AddScreen(TimeItem_11);

  //ВКЛЮЧЕНИЕ РЕЛЕ В РУЧНОМ РЕЖИМЕ
  ControlWordItem* ControlWordItem_11 = new ControlWordItem("PE\3E 1     ",     &MODBUS->MB_HOLDING_REGISTERS[14], 0);
  ControlWordItem_11->IsVisibility = &ControlWordItem_10->VisibilityNextItem;
  ControlWordItem_11->VisibilityInverse = true;
  item_1->AddScreen(ControlWordItem_11);

  //ОКНО ВОЗВРАТА
  ReturnItem * ReturnItem_10 = new ReturnItem(item_1);
  item_1->AddScreen(ReturnItem_10);

  //ДОБАВЛЕНИЕ ЭКРАНА В МЕНЮ
  MainMenu.AddScreen(item_1);


  /*
    РЕЛЕ 2
  */
  ContentItem* item_2 = new ContentItem("PE\3E 2          ");

  //РЕЖИМ
  ControlWordItem* ControlWordItem_20 = new ControlWordItem("PE\3E 2 PE\7\5M",     &MODBUS->MB_HOLDING_REGISTERS[20], 1);
  ControlWordItem_20->TEXT_TRUE = "ABTO";
  ControlWordItem_20->TEXT_FALSE = "P\2\6";
  item_2->AddScreen(ControlWordItem_20);

  //ВРЕМЯ ВКЛ
  TimeItem* TimeItem_20 = new TimeItem("PE\3E 2 BP. BK\3  ", &MODBUS->MB_HOLDING_REGISTERS[21]);
  TimeItem_20->IsVisibility = &ControlWordItem_20->VisibilityNextItem;
  item_2->AddScreen(TimeItem_20);

  //ВРЕМЯ ВЫКЛ
  TimeItem* TimeItem_21 = new TimeItem("PE\3E 2 BP. B\4K\3 ", &MODBUS->MB_HOLDING_REGISTERS[22]);
  TimeItem_21->IsVisibility = &ControlWordItem_20->VisibilityNextItem;
  item_2->AddScreen(TimeItem_21);

  //ВКЛЮЧЕНИЕ РЕЛЕ В РУЧНОМ РЕЖИМЕ
  ControlWordItem* ControlWordItem_21 = new ControlWordItem("PE\3E 2     ",     &MODBUS->MB_HOLDING_REGISTERS[20], 0);
  ControlWordItem_21->IsVisibility = &ControlWordItem_20->VisibilityNextItem;
  ControlWordItem_21->VisibilityInverse = true;
  item_2->AddScreen(ControlWordItem_21);

  //ОКНО ВОЗВРАТА
  ReturnItem * ReturnItem_20 = new ReturnItem(item_2);
  item_2->AddScreen(ReturnItem_20);

  //ДОБАВЛЕНИЕ ЭКРАНА В МЕНЮ
  MainMenu.AddScreen(item_2);


  /*
    РЕЛЕ 3
  */
  ContentItem* item_3 = new ContentItem("PE\3E 3          ");

  //РЕЖИМ
  ControlWordItem* ControlWordItem_30 = new ControlWordItem("PE\3E 3 PE\7\5M",     &MODBUS->MB_HOLDING_REGISTERS[26], 1);
  ControlWordItem_30->TEXT_TRUE = "ABTO";
  ControlWordItem_30->TEXT_FALSE = "P\2\6";
  item_3->AddScreen(ControlWordItem_30);

  //ВРЕМЯ ВКЛ
  TimeItem* TimeItem_30 = new TimeItem("PE\3E 3 BP. BK\3  ",  &MODBUS->MB_HOLDING_REGISTERS[27]);
  TimeItem_30->IsVisibility = &ControlWordItem_30->VisibilityNextItem;
  item_3->AddScreen(TimeItem_30);

  //ВРЕМЯ ВЫКЛ
  TimeItem* TimeItem_31 = new TimeItem("PE\3E 3 BP. B\4K\3 ", &MODBUS->MB_HOLDING_REGISTERS[28]);
  TimeItem_31->IsVisibility = &ControlWordItem_30->VisibilityNextItem;
  item_3->AddScreen(TimeItem_31);

  //ВКЛЮЧЕНИЕ РЕЛЕ В РУЧНОМ РЕЖИМЕ
  ControlWordItem* ControlWordItem_31 = new ControlWordItem("PE\3E 3     ",     &MODBUS->MB_HOLDING_REGISTERS[26], 0);
  ControlWordItem_31->IsVisibility = &ControlWordItem_30->VisibilityNextItem;
  ControlWordItem_31->VisibilityInverse = true;
  item_3->AddScreen(ControlWordItem_31);

  //ОКНО ВОЗВРАТА
  ReturnItem * ReturnItem_3 = new ReturnItem(item_3);
  item_3->AddScreen(ReturnItem_3);

  //ДОБАВЛЕНИЕ ЭКРАНА В МЕНЮ
  MainMenu.AddScreen(item_3);

  /*
    РЕЛЕ 4
  */
  ContentItem* item_4 = new ContentItem("PE\3E 4          ");

  //РЕЖИМ
  ControlWordItem* ControlWordItem_40 = new ControlWordItem("PE\3E 4 PE\7\5M",     &MODBUS->MB_HOLDING_REGISTERS[32], 1);
  ControlWordItem_40->TEXT_TRUE = "ABTO";
  ControlWordItem_40->TEXT_FALSE = "P\2\6";
  item_4->AddScreen(ControlWordItem_40);

  //ВРЕМЯ ВКЛ
  TimeItem* TimeItem_40 = new TimeItem("PE\3E 4 BP. BK\3  ", &MODBUS->MB_HOLDING_REGISTERS[33]);
  TimeItem_40->IsVisibility = &ControlWordItem_40->VisibilityNextItem;
  item_4->AddScreen(TimeItem_40);

  //ВРЕМЯ ВЫКЛ
  TimeItem* TimeItem_41 = new TimeItem("PE\3E 4 BP. B\4K\3 ", &MODBUS->MB_HOLDING_REGISTERS[34]);
  TimeItem_41->IsVisibility = &ControlWordItem_40->VisibilityNextItem;
  item_4->AddScreen(TimeItem_41);

  //ВКЛЮЧЕНИЕ РЕЛЕ В РУЧНОМ РЕЖИМЕ
  ControlWordItem* ControlWordItem_41 = new ControlWordItem("PE\3E 4     ",     &MODBUS->MB_HOLDING_REGISTERS[32], 0);
  ControlWordItem_41->IsVisibility = &ControlWordItem_40->VisibilityNextItem;
  ControlWordItem_41->VisibilityInverse = true;
  item_4->AddScreen(ControlWordItem_41);

  //ОКНО ВОЗВРАТА
  ReturnItem * ReturnItem_4 = new ReturnItem(item_4);
  item_4->AddScreen(ReturnItem_4);

  //ДОБАВЛЕНИЕ ЭКРАНА В МЕНЮ
  MainMenu.AddScreen(item_4);

  /*
    СИСТЕМА
  */
  ContentItem* item_5 = new ContentItem("C\5CTEMA         ");

  //Установка системного времени
  SystemTimeItem* SystemTimeItem_50 = new SystemTimeItem("C\5CTEMH\4E \6AC\4", &rtc);
  item_5->AddScreen(SystemTimeItem_50);

  ReturnItem * ReturnItem_5 = new ReturnItem(item_5);
  item_5->AddScreen(ReturnItem_5);

  MainMenu.AddScreen(item_5);

  /*
    IoT
  */
  ContentItem* item_6 = new ContentItem("IoT         ");

  BoolItem* BoolItem_60 = new BoolItem("WIFI            ", &MODBUS->WiFi_State);
  item_6->AddScreen(BoolItem_60);

  BoolItem* BoolItem_61 = new BoolItem("WIFI MODE       ", &MODBUS->AP_MODE);
  BoolItem_61->TEXT_TRUE =  "ACCES POINT";
  BoolItem_61->TEXT_FALSE = "STA";
  BoolItem_61->IsVisibility = &MODBUS->WiFi_State;
  item_6->AddScreen(BoolItem_61);

  //IP
  TextViewItem* TextViewItem_60 = new TextViewItem("IP ADDRESS      ", &MODBUS->STR_IP);
  TextViewItem_60->ReadOnly = true;
  TextViewItem_60->IsVisibility = &MODBUS->WiFi_State;
  item_6->AddScreen(TextViewItem_60);

  //Окно возврата
  ReturnItem * ReturnItem_6 = new ReturnItem(item_6);
  item_6->AddScreen(ReturnItem_6);

  MainMenu.AddScreen(item_6);

  /*
    ДИАГНОСТИКА
  */
  ContentItem* item_7 = new ContentItem("DIAGNOSTIC      ");

  //Бит, что клиент подключен
  BoolItem* BoolItem_70 = new BoolItem("CLIENT CONNECTED", &MODBUS->ClientConnected);
  BoolItem_70->ReadOnly = true;
  BoolItem_70->IsVisibility = &MODBUS->WiFi_State;
  item_7->AddScreen(BoolItem_70);

  //Свободная память
  NumericViewItem* NumericViewItem_70 = new NumericViewItem("FREE RAM", &FreeRAM);
  NumericViewItem_70->ReadOnly = true;
  item_7->AddScreen(NumericViewItem_70);

  //Время цикла loop
  NumericViewItem* NumericViewItem_71 = new NumericViewItem("CYCLE TIME", &CycleTime);
  NumericViewItem_71->ReadOnly = true;
  item_7->AddScreen(NumericViewItem_71);

  ReturnItem * ReturnItem_7 = new ReturnItem(item_7);
  item_7->AddScreen(ReturnItem_7);

  MainMenu.AddScreen(item_7);


  //Выключаем WiFi
  MODBUS->WiFi_State = false;

  //Запускаем меню
  MainMenu.Begin();

  //Привязка часов реального времени
  //к меню
  MainMenu.BindingRTC(&rtc);

  //Инициализируем энкодер
  encoder = new RotaryEncoder(5, 17);

  //Подписываемся на прерывание при изменении энкодера
  attachInterrupt(5, encoderStep, FALLING);
  attachInterrupt(17, encoderStep, FALLING);

  //Инициализация EEPROM
  EEPROM.begin(512);

  //Инициализация клапан 1
  Valve_0 = new Valve(14, MODBUS->MB_HOLDING_REGISTERS);

  //Инициализация клапан 2
  Valve_1 = new Valve(20, MODBUS->MB_HOLDING_REGISTERS);

  //Инициализация клапан 3
  Valve_2 = new Valve(26, MODBUS->MB_HOLDING_REGISTERS);

  //Инициализация клапан 4
  Valve_3 = new Valve(32, MODBUS->MB_HOLDING_REGISTERS);

  ArduinoOTA.onStart([]()
  {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("UPLOAD FIRMWARE ");
    LCD.backlight();
    //LCD.setCursor(0,0);
    //LCD.print("Start");  //  "Начало OTA-апдейта"
  });


  ArduinoOTA.onEnd([]()
  {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("UPLOAD SUCCES ");
    LCD.setCursor(0, 1);
    LCD.print("SUCCESSFULLY");
    delay(2000);
    //LCD.print("\nEnd");  //  "Завершение OTA-апдейта"
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    LCD.setCursor(0, 1);
    LCD.printf("PROGRESS: %u%%", (progress / (total / 100)));
  });

  /**
  ArduinoOTA.onError([](ota_error_t error)
  {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      //  "Ошибка при аутентификации"
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      //  "Ошибка при начале OTA-апдейта"
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      //  "Ошибка при подключении"
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      //  "Ошибка при получении данных"
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
      //  "Ошибка при завершении OTA-апдейта"
  });
  **/



  if (! rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  //Включаем по умолчанию MODBUS
  MODBUS->WiFi_State = true;


  //Загружаем символ в lcd
    MainMenu.createSymbol(0, symbol_Edit);
    MainMenu.createSymbol(1, symbol_Arrow);
    //MainMenu.createSymbol(2, symbol_YA);    //Я
    MainMenu.createSymbol(2, symbol_Y);     //У
    MainMenu.createSymbol(3, symbol_L);     //Л
    MainMenu.createSymbol(4, symbol_A);     //Ы
    MainMenu.createSymbol(5, symbol_I);     //И
    MainMenu.createSymbol(6, symbol_CH);    //Ч
    //MainMenu.createSymbol(6, symbol_Y);     //У
    //MainMenu.createSymbol(7, symbol_D);     //Д
    MainMenu.createSymbol(7, symbol_ZH);     //Ж


//  MainMenu.createSymbol(7, symbol_P);     //П

  //Строка для установки даты времени после прошивки
  //rtc.adjust(DateTime(__DATE__, __TIME__));

  pinMode(19, OUTPUT);

  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
}

int oldTicks = 0;



//Бесконечный цикл
void loop()
{

  //Получение времени начала
  //цикла (для определени)
  //времени выполнения одного цикла loop
  CycleStart = millis();

  updateTimer.Update();

  //Обработка тиков энкодера
  if(ticks > oldTicks)
  {
    MainMenu.UpCMD();
    oldTicks = ticks;
  }

  if(ticks < oldTicks)
  {
    MainMenu.DownCMD();
    oldTicks = ticks;
  }

  if(btn.FrontPositive())
  {
    MainMenu.SelectCMD();
  }

  Reg_0 = ExtendedBoard_1.Temperature;

  MainMenu.Update();

  MODBUS->MODBUS_UPDATE();

  //Вычисляем количество минут
  DateTime now = rtc.now();
  uint16_t minutes = now.hour() * 60 + now.minute();
  MODBUS->MB_HOLDING_REGISTERS[5] = minutes;
  MODBUS->MB_HOLDING_REGISTERS[6] = now.second();


  Valve_0->Update();
  Valve_1->Update();
  Valve_2->Update();
  Valve_3->Update();


  //Переносим переменнные в регистр
  ExtendedBoard_1.RELAY_0 = Valve_0->State;
  ExtendedBoard_1.RELAY_1 = Valve_1->State;
  ExtendedBoard_1.RELAY_2 = Valve_2->State;
  ExtendedBoard_1.RELAY_3 = Valve_3->State;
  ExtendedBoard_1.Update();

  FreeRAM = ESP.getFreeHeap();

  //Вычисляем время выполнения программы
  CycleEnd = millis();
  CycleTime = CycleEnd - CycleStart;

  timerWrite(timer, 0);
}
