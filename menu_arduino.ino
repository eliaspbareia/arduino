#include <LiquidCrystal.h>
#include <DHT.h>
#include <Wire.h>
#include <EEPROM.h>


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define resistencia 6
#define butDown 14
#define butUp 15
#define butMenos 16
#define butMais 17
#define butOk 18


/* DEFINIÇÕES DHT */
#define DHTPIN
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

/*VARIAVEIS EEPROM*/
int addr_temp = 0;

//Prototipos das funções
void read_buts();
void menu1();
void menu2();
void menu3();
void menu4();
void menu5();
void segundos();

/*Variaveis globais */
unsigned short screen = 0x01;  //armazena a tela

float t = 0.0, h = 0.0;
float TempProg;
boolean t_up, t_down, t_menos, t_mais, t_ok;

void setup() {
  Wire.begin();
  dht.begin();
  lcd.begin(16, 2);  
  
  pinMode(butUp, INPUT);
  pinMode(butDown, INPUT);
  pinMode(butMais, INPUT);
  pinMode(butMenos, INPUT);
  pinMode(butOk, INPUT);

  pinMode(resistencia, OUTPUT);

  t_up = 0x00;
  t_down = 0x00;
  t_mais = 0x00;
  t_menos = 0x00;
  t_ok = 0x00;

  
}

void loop() {
  t = dht.readTemperature();
  h = dht.readHumidity();
  VERIFICAR SE O ENDEREÇO NA EPROM ESTA VAZIO, SE NÃO BUSCA O VALOR
 // EEPROM.get(0,TempProg);
  read_buts();
  disp_menus();  
  comp_temp(TempProg, t);
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

void read_buts(){
  if(!digitalRead(butUp)) t_up = 0x01;
  if(!digitalRead(butDown)) t_down = 0x01;

  
 
  if(digitalRead(butUp)&& t_up){ //botão menos solto e flag setada
    t_up = 0x00;
    lcd.clear();
    screen++;
    if(screen > 0x05) screen = 0x01;
    
  }

  if(digitalRead(butDown)&& t_down){
    t_down = 0x00;
    screen--;
    lcd.clear();
    if(screen < 0x01) screen = 0x05;       
  }  

}

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
  lcd.clear();
  lcd.setCursor(0,0); //col lin
  lcd.print("TEMPERATURA");
  lcd.setCursor(0,1);
  lcd.print(t,1);
  delay(50);
}

void menu2(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("UMIDADE");
  lcd.setCursor(0,1);
  lcd.print("INT: ");
  //lcd.print(h);
  delay(50);
}

void menu3(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("AJUSTE DATA");
  lcd.setCursor(0,1);
  lcd.print("DATA: ");
  //lcd.print(t);
  delay(50);
}

void menu4(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("AJUSTE HORA");
  lcd.setCursor(0,1);
  lcd.print("HORA: ");
  //lcd.print(t);
  delay(50);
}

void menu5(){
  lcd.clear();
  lcd.setCursor(0,0); //col lin
  lcd.print("Temp Choca");
  lcd.setCursor(0,1);
  lcd.print(TempProg,1);
  
  if(!digitalRead(butOk)) t_ok = 0x01;
  if(!digitalRead(butMais)) t_mais = 0x01;
  if(!digitalRead(butMenos)) t_menos = 0x01;
  
  if(digitalRead(butMais)&& t_mais){
    t_mais = 0x00;    
    TempProg = TempProg + 0.1;   
  }
  if(digitalRead(butMenos)&& t_menos){
    t_menos = 0x00;    
    TempProg = TempProg - 0.1; 
  }
  if(digitalRead(butOk) && t_ok){
    t_ok = 0x00;
    lcd.setCursor(10, 1);
    lcd.print("Gravando...");
    if(EEPROM.read(0) != TempProg){
      EEPROM.put(0, TempProg);
    }
  }
  delay(50);
}
