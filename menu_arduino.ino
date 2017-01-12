#include <dht.h>

#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>

#include <Wire.h>
#include <EEPROM.h>



LiquidCrystal_I2C lcd(0x27,20,4);

#define resistencia 6
#define buzzer 10

#define butMais 8
#define butMenos 9
#define butUp 14
#define butDown 15
#define butOk 16
#define butBacklight 17




/* DEFINIÇÕES DHT */
dht dht22;
#define DHT22_PIN 7

//estados do relogio
#define stClockRunning 0
#define stSetDay 1
#define stSetMonth 2
#define stSetYear 3
#define stSetHour 4
#define stSetMinute 5
#define stSetSecond 6

/* Variaveis time*/
int stat = stClockRunning;

/*VARIAVEIS EEPROM*/


//Prototipos das funções
void read_buts();
void grava(float x, int p);
float programaBt(float value, int p);
float ler(int p);
void iniciarChoca();
void printTime();
void setMTime(int hour, int minute, int second, int day, int month, int year);
void getTempHum();
int getStat();
void menu1();
void menu2();
void menu3();
void menu4();
void menu5();

/*Variaveis tela */
unsigned short screen = 0x01;  //armazena a tela
boolean backlightOn = true;
boolean setupScreen = false;

//Variaveis DHT22
float temperature = 0.0, humidity = 0.0;
float TempProg;
float HumdProg;
const long interval = 6000; //intervalos das leituras
unsigned long previousMillis = 0;
unsigned long currentMillis; 


boolean t_up, t_down, t_menos, t_mais, t_ok, t_back;


byte tempChar[8] = {0b00100,0b01010,0b01010,0b01110,0b01110,0b11111,0b11111,0b01110};
byte humidChar[8] = {0b00100,0b00100,0b01110,0b01110,0b11111,0b11101,0b11011,0b01110};
byte horasChar[8] = {0b00000,0b01110,0b10101,0b10101,0b10011,0b10001,0b01110,0b00000};
byte grauChar[8] = {0b01100,0b10010,0b10010,0b01100,0b00000,0b00000,0b00000,0b00000};
byte calendarChar[8] = {0b01010,0b11111,0b00000,0b11111,0b00000,0b11111,0b00000,0b11111};
byte next[8] = {0b10000,0b01000,0b00100,0b00010,0b00010,0b00100, 0b01000,0b10000};
byte last[8] = {0b00001,0b00010,0b00100,0b01000,0b01000,0b00100,0b00010,0b00001};

uint32_t ref = 0;

void setup() {
  Wire.begin();
  lcd.init();  
  lcd.backlight();
  Serial.begin(9600);
  TempProg = ler(0);
  HumdProg = ler(4);

  setTime(12,30,00,1,1,2017);
  
  pinMode(butUp, INPUT);
  pinMode(butDown, INPUT);
  pinMode(butMais, INPUT);
  pinMode(butMenos, INPUT);
  pinMode(butOk, INPUT);
  pinMode(butBacklight, INPUT);
  
  pinMode(buzzer, OUTPUT);
  pinMode(resistencia, OUTPUT);

  t_up = 0x00;
  t_down = 0x00;
  t_mais = 0x00;
  t_menos = 0x00;
  t_ok = 0x00;
  t_back = 0x00;

  lcd.setCursor(0,0);
  lcd.print("Chocadeiras Ovo Bom");
  delay(2000);

  lcd.createChar(1, tempChar);
  lcd.createChar(0, humidChar);
  lcd.createChar(2, horasChar);
  lcd.createChar(3, grauChar);  
  lcd.createChar(4, calendarChar);
  lcd.createChar(6, next);
  lcd.createChar(5, last);

}

int oldStat = stat;
unsigned long t = millis();

void loop() {  
  currentMillis  = millis();
  getTempHum();
  read_buts();
  disp_menus();  
  comp_temp(TempProg, temperature);
}

void disp_menus(){
  switch(screen){
    case 0x01: menu1(); break;
    case 0x02: menu2(); break;
    case 0x03: menu3(); break;
    case 0x04: menu4(); break;
    case 0x05: menu5(); break;
  }
}

void getTempHum(){ 
  if (currentMillis - previousMillis >= interval){
    int chk = dht22.read22(DHT22_PIN);
    previousMillis = currentMillis;
    temperature = dht22.temperature;   
    humidity = dht22.humidity;
  }
}

void read_buts(){
  if(!digitalRead(butUp)) t_up = 0x01;
  if(!digitalRead(butDown)) t_down = 0x01;
  if(!digitalRead(butBacklight)) t_back = 0x01;
  
  if(digitalRead(butUp)&& t_up){ //botão menos solto e flag setada
    tone(buzzer,1500,100);
    t_up = 0x00;
    lcd.clear();
    screen++;
    if(screen > 0x05) screen = 0x01;
    
  }

  if(digitalRead(butDown)&& t_down){
    tone(buzzer,1500,100);
    t_down = 0x00;
    screen--;
    lcd.clear();
    if(screen < 0x01) screen = 0x05;       
  }  

  if(digitalRead(butBacklight) && t_back){
    if(backlightOn){
       lcd.noBacklight();
       backlightOn = false;
    }else{
       lcd.backlight();
       backlightOn = true;
    }
  }

}
//COMPARA TEMPERATURA E LIGA A RESISTENCIIA
void comp_temp(float temp_in, float temp_out){
  //comp_temp(TempProg, t);
  if(temp_out >= temp_in){
    digitalWrite(resistencia, LOW);
  } 
  if(temp_out < temp_in){
    digitalWrite(resistencia, HIGH);
  }
  return ;
}

void menu1(){  
  if(millis() - ref >= 1000){
    lcd.clear();
    //exibe a temperatura
    lcd.setCursor(0,0); //col lin
    lcd.write((uint8_t)1);
    lcd.setCursor(2,0);
    lcd.print(temperature,1);
    lcd.setCursor(7,0);
    lcd.write((uint8_t)3);
    lcd.print("C");
    lcd.setCursor(12,0);
    lcd.write((uint8_t)1);
    lcd.print("P");
    lcd.setCursor(15,0);
    lcd.print(TempProg,1);
    
    //exibe humidade
    lcd.setCursor(0,1); //col lin
    lcd.write((uint8_t)0);
    lcd.setCursor(2,1);
    lcd.print(humidity,1);
    lcd.setCursor(7,1);
    lcd.print("%");
    lcd.setCursor(12,1);
    lcd.write((uint8_t)0);
    lcd.print("P");
    lcd.setCursor(15,1);
    lcd.print(HumdProg,1);

    lcd.setCursor(0,2);
    lcd.write((uint8_t)4);
    lcd.setCursor(2,2);
    lcd.print("Dias de Choco");
    lcd.setCursor(17,2);
    lcd.print("10");
    lcd.setCursor(0,3);
    lcd.write((uint8_t)5); 
    lcd.print("Anterior");
    lcd.setCursor(12,3);
    lcd.print("Proximo");
    lcd.setCursor(19,3);    
    lcd.write((uint8_t)6);
    
    
    ref = millis();
  }
  //delay(50);
}


void menu2(){
  if(millis() - ref >= 1000){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Clique em Ok para"); 
    lcd.setCursor(0,1);
    lcd.print("iniciar choca");

    lcd.setCursor(0,3);
    lcd.write((uint8_t)5); 
    lcd.print("Anterior");
    lcd.setCursor(12,3);
    lcd.print("Proximo");
    lcd.setCursor(19,3);    
    lcd.write((uint8_t)6);
    
    if(!digitalRead(butOk)) t_ok = 0x01;
    if(digitalRead(butOk) && t_ok){
      tone(buzzer,1500,100);
      t_ok = 0x00;
     iniciarChoca();
    }
    
      
    ref = millis();
  }
}

void menu3(){
  int value = 0;
  
 if(millis() - ref >= 1000){
     printTime();    
    
    //não modificar aqui
    ref = millis();
  }
   //delay(100);
  //ajustar relógio
    stat = getStat();

    if(stat != oldStat){
      t = millis();
      oldStat = stat;
    }else{
      if((stat != stClockRunning) && ((millis() - t) > 5000))
        stat = stClockRunning;
    }
    
    switch(stat){
      case stClockRunning:
        break;
      case stSetDay:
        value = day();
        while(readPin(butMais, 300)){
          value++;
          
          if(value > 31){
            value = 1;
          }
          setTime(hour(),minute(),second(),value,month(),year());
          printTime();
        }
        while(readPin(butMenos, 300)){
          
          value--;
          if(value < 1){
            value = 31;
          }
          setTime(hour(),minute(),second(),value,month(),year());
          printTime();
        }        
        break;
        case stSetMonth:
          value=month();
          while(readPin(butMais,300))
          {
            value++;
            if (value > 12)
              value=1;
            setTime(hour(),minute(),second(),day(),value,year());
              printTime();
            }
            while(readPin(butMenos,300))
            {
              value--;
              if (value < 1)
                value=12;
              setTime(hour(),minute(),second(),day(),value,year());
              printTime();
            }
            break;
        case stSetYear:
          value=year();
          while(readPin(butMais,300))
          {
            value++;
            if (value > 2030)
              value=2015;
            setTime(hour(),minute(),second(),day(),month(),value);
            printTime();
          }
          while(readPin(butMenos,300))
          {
            value--;
            if (value < 2015)
              value=2030;
            setTime(hour(),minute(),second(),day(),month(),value);
            printTime();
          }
          break;
          case stSetHour:
      value=hour();
      while(readPin(butMais,300))
      {
        value++;
        if (value > 23)
          value=0;
        setTime(value,minute(),second(),day(),month(),year());
        printTime();
      }
      while(readPin(butMenos,300))
      {
        value--;
        if (value < 1)
          value=24;
        setTime(value,minute(),second(),day(),month(),year());
        printTime();
      }
      break;
    case stSetMinute:
      value=minute();
      while(readPin(butMais,300))
      {
        value++;
        if (value > 59)
          value=0;
        setTime(hour(),value,second(),day(),month(),year());
        printTime();
      }
      while(readPin(butMenos,300))
      {
        value--;
        if (value < 0)
          value=59;
        setTime(hour(),value,second(),day(),month(),year());
        printTime();
      }
      break;
    case stSetSecond:
      value=second();
      while(readPin(butMais,300))
      {
        value++;
        if (value > 59)
          value=0;
        setTime(hour(),minute(),value,day(),month(),year());
        printTime();
      }
      while(readPin(butMenos,300))
      {
        value--;
        if (value < 0)
          value=59;
        setTime(hour(),minute(),value,day(),month(),year());
        printTime();
      }
      break;
    }   
    
    
  //fim ajuste
}

void printTime(){
    lcd.clear();
    lcd.setCursor(0,0); //col lin
    lcd.print("Hora/Data"); 
    lcd.setCursor(4,1);
    printnn(hour());
    lcd.setCursor(7,1);
    lcd.print(":");
    lcd.setCursor(9,1);
    printnn(minute());
    lcd.setCursor(12,1);
    lcd.print(":");
    lcd.setCursor(14,1);
    printnn(second()); 
    
    lcd.setCursor(3,2);
    printnn(day());
    lcd.setCursor(6,2);
    lcd.print("/");
    lcd.setCursor(8,2);
    printnn(month());
    lcd.setCursor(11,2);
    lcd.print("/");
    lcd.setCursor(13,2);
    lcd.print(year());

    //verifica o estado inicial
    if(stat != stClockRunning){
      lcd.setCursor(0,1);  //col lin
      lcd.print("*");}
    else{
      lcd.setCursor(0,1);  //col lin
      lcd.print(" ");
    }
    switch(stat){
      case stSetDay:
        lcd.setCursor(0,2);  //col lin
        lcd.print("d");
        lcd.setCursor(2,2);
        lcd.print(">");
        break;
      case stSetMonth:
        lcd.setCursor(0,2);  //col lin
        lcd.print("m");
        lcd.setCursor(7,2);
        lcd.print(">");
        break;
      case stSetYear:
        lcd.setCursor(0,2);  //col lin
        lcd.print("a");
        lcd.setCursor(12,2);
        lcd.print(">");
        break;
      case stSetHour:
        lcd.setCursor(0,1);  //col lin
        lcd.print("h");
        lcd.setCursor(3,1);
        lcd.print(">");
        break;
      case stSetMinute:
        lcd.setCursor(0,1);  //col lin
        lcd.print("M");
        lcd.setCursor(8,1);
        lcd.print(">");
        break;
      case stSetSecond:
        lcd.setCursor(0,1);  //col lin
        lcd.print("s");
        lcd.setCursor(13,1);
        lcd.print(">");
        break;
    }
}

void menu4(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humidade Ideal");
  lcd.setCursor(0,1);
  lcd.print(HumdProg,1);
  
  HumdProg = programaBt(HumdProg, 4);
  
}


void menu5(){
  lcd.clear();
  lcd.setCursor(0,0); //col lin
  lcd.print("Temp Choca");
  lcd.setCursor(0,1);
  
  lcd.print(TempProg,1);
  TempProg = programaBt(TempProg, 0);

  delay(50);
}

void iniciarChoca(){  
  setTime(20,25,0,11,1,2017);
}

float programaBt(float value, int p){

  if(!digitalRead(butMais)) t_mais = 0x01;
  if(!digitalRead(butMenos)) t_menos = 0x01;
  if(!digitalRead(butOk)) t_ok = 0x01;
  
  if(digitalRead(butMais)&& t_mais){
    tone(buzzer,1500,100);
    t_mais = 0x00;    
    value = value + 0.1;   
  }
  if(digitalRead(butMenos)&& t_menos){
    tone(buzzer,1500,100);
    t_menos = 0x00;    
    value = value - 0.1;     
  }
  if(digitalRead(butOk) && t_ok){
    tone(buzzer,1500,100);
    t_ok = 0x00;
    lcd.setCursor(9, 1);
    lcd.print("Gravando...");
    delay(50);
    grava(value, p);
    screen = 0x01;
  }
  
  return value;
}

void printnn(int n){
  String digitos = String(n);
  if(digitos.length()==1){
    digitos="0"+digitos;
  }
  lcd.print(digitos);
}

void grava(float x, int p){
  EEPROM.write(p, int(x));
  EEPROM.write(p+1, int((x-int(x))*100));
}

float ler(int p){
  return float(EEPROM.read(p)) + float(EEPROM.read(p+1))/100;
  
}


void Print(int number){
  lcd.print(number/10);
  lcd.print(number%10);
}

//pega novo estatus, se tiver algum
int getStat()
{
  if (readPinSwitch(butOk,300))
     if (stat == stSetSecond)
        return stClockRunning;
     else
        return stat+1;
  else
     return stat; 
}

int readPinSwitch(int pin, int threshold)
{
  if (digitalRead(pin))
  {
    unsigned long tt = millis();
    while (digitalRead(pin));
    if ((millis() - tt) > threshold)
       return 1;
    else
       return 0;  
  }
  else
    return 0;
}


int readPin(int pin, int threshold)
{
  if (digitalRead(pin))
  {
    unsigned long tt = millis();
    while ((digitalRead(pin) && (millis() - tt) <= threshold));
    if (digitalRead(pin))
       return 1;
    else
       return 0;  
  }
  else
    return 0;
}
