//Iniciando bibliotecas
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//Definindo variaveis
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define LED1 45
#define LED2 35
#define LED3 37
#define LED4 46
#define BUZZER 18


void setup() {
  //Definindo tipo de retorno de cada uma das portas usadas
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.begin(115200);

//Iniciando o display
  Wire.begin();
  Serial.println("\nI2C Scanner");
  lcd.init();                
  lcd.backlight();
}

void loop() {
  //Definindo a partir de onde começará a escrever 
  lcd.setCursor(6, 0);
  //Mandando a mensagem
  lcd.print("262");

  //Acendendo o LED
  digitalWrite(LED3, HIGH);
}

//Função para identificar em qual endereço está o display, e verificar se ele está conectado.
void scanner()
{
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);   
}