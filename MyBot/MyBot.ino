#include <pcmRF.h>
#include <TMRpcm.h>
#include <LedControl.h>
#include <Ultrasonic.h>
#include <Servo.h>
#include <SPI.h>
#include <SD.h>
#include <PCD8544.h>
#include "display.h"

// ---------------------------- Definação da pinagem --------------------------

//Sensor de voltagem
#define PinBattery      A0            //Pino do Sensor de Voltagem (Bateria - 11.1V).

//Sensores de distância
#define Trigger_Esq     3             //Pino do trigger de distância (Esquerda).
#define Echo_Esq        5             //Pino do echo de distância (Esquerda).
#define Trigger_Dir     3             //Pino do trigger de distância (Direita).
#define Echo_Dir        7             //Pino do echo de distância (Direita).
#define Trigger_Centro  3             //Pino do trigger de distância (Central).
#define Echo_Centro     9             //Pino do echo de distância (Central).

//LCD (Cristal líquido)
#define PIN_SCLK        38            //LCD pino de clock
#define PIN_SDIN        36            //LCD pino DIN
#define PIN_DC          34            //LCD pino DC
#define PIN_RESET       32            //LCD pino de reset
#define PIN_SCE         30            //LCD pino SCE

//Caixa de som
#define PIN_Speaker     11            //Pino do alto-falante

//Servo
#define ServoVer        13            //Pino do servo vertical

//Motores
#define AvancaMD        A9            //Pino avança motor direito.
#define RetrocedeMD     A10           //Pino retrocede motor direito.

#define AvancaME        A8            //Pino avança motor esquerdo.
#define RetrocedeME     A11           //Pino retrocede motor esquerdo.

//LEDS (Olhos)
#define dataPin         49            //Pino para alterar os dados.
#define csPin           40            //Pino para selecionar o device.
#define clockPin        47            //Pino de clock dos olhos.

//Farol
#define Farol           23            //Pino para ligar o farol.

//LCD
#define backLight_LCD   29            //Backlight do LCD.

//Desligar robô
#define Desligar        25            //Pino para desligar.

//Sensor
Ultrasonic sensorEsq(Trigger_Esq, Echo_Esq);            // Sensor Esquerdo.
Ultrasonic sensorCentro(Trigger_Centro, Echo_Centro);   // Sensor Central.
Ultrasonic sensorDir(Trigger_Dir, Echo_Dir);            // Sensor Direito.

//Cartão de memória
File root;                        //Caminho do SD Card.
static const byte iconLin = 82;   //Linha da bateria
static const byte iconCol = 40;   //Coluna da bateria

//Sensor de luminosidade
int LDRTime;                      //Tempo para repetir sobre a luminosidade.

//Servo servoHor;
Servo servoVer;

//Áudio do cartão do memória
TMRpcm audioCartaoSD;

//Sensor central
int disAlerta_Centro = 20;         //Distância de alerta de obstáculo central.
int disCentro;

//Sensor direita
int disAlerta_Dir = 15;            //Distância de alerta de obstáculo da direita.
int disDir;

//Sensor esquerda
int disAlerta_Esq = 15;            //Distância de alerta de obstáculo da esquerda.
int disEsq;

//Display Sensor Voltagem
double voltagePorc = 0.0;           //% da vontagem medida.
float sensorValor = 0.0f;           //Voltagem do sensor.

//Display LCD
PCD8544 lcd(PIN_SCLK,PIN_SDIN,PIN_DC,PIN_RESET,PIN_SCE);

LedControl display = LedControl(dataPin, clockPin, csPin, 1);

//-----------------------------------------------------------------------------------------------
int bolFalaEscuro = 1;
void setup() {

  Serial.begin(9600);

  //Manter o robô ligado.
  digitalWrite(Desligar, LOW);     

  //Servo.
  servoVer.attach(ServoVer);
  servoVer.write(90);            //Posição inicial.
  delay(1000);

  //Configuração do alto-falante
  audioCartaoSD.speakerPin = PIN_Speaker;

  //Inicialização do cartão SD.
  if (SD.begin(53),2000) {
    Serial.println("###### Cartão SD inicializado com sucesso! ######");
    Serial.println(" ");
    root = SD.open("");
  }else{
    Serial.println("###### Cartão SD com erro! ######");
    Serial.println(" ");
  }

  //Display LCD
  lcd.begin();

  //Display Led
  display.clearDisplay(0);
  display.shutdown(0, false);
  display.setIntensity(0,1);

  pinMode(RetrocedeMD, OUTPUT);
  pinMode(RetrocedeME, OUTPUT);  
  pinMode(AvancaMD, OUTPUT);
  pinMode(AvancaME, OUTPUT);

  //Versão.
  Serial.println("############################################################");
  Serial.println("#          Versão 16.9        Data: 08/08/2020             #");
  Serial.println("############################################################");

  //Saudações
  Serial.println("Rodando saudações!");
  AbrirSDCard(root, "OLAMONO.WAV");
}

//------------------------------------------------------------------------------------------------------//
//                                          FLUXO PRINCIPAL                                             //
//------------------------------------------------------------------------------------------------------//

void loop() {

  //-------------------------------- Verifica bateria ----------------------------------------------------
  //Battery LiPO (11.1V)
  voltagePorc = fn_MedirTensao(PinBattery);
  lcd.setCursor(0, 1);
  IconeBateria(voltagePorc);
  Serial.println("############################################################");

  //------------------------------- Sensor de luminosidade -----------------------------------------------
  int LDR = analogRead(A15);
  Serial.print(LDR);
  Serial.println(" Sensor de luminosidade");

  if (LDR < 200 && bolFalaEscuro == 1) {

      fn_PararMotores();
      AbrirSDCard(root, "ESCURO~1.WAV");
      bolFalaEscuro = 0;
      
      //Ligar farol.
      AbrirSDCard(root, "LIGFAROL.WAV");
      digitalWrite(Farol, HIGH);
      //Ligar LCD.
      digitalWrite(backLight_LCD, HIGH);      
      fn_AvancarMotores(); 
      
  } else if (LDR > 200){
    bolFalaEscuro = 1;
    
    //Desligar farol.
    digitalWrite(Farol, LOW);

    //Desligar LCD.
    digitalWrite(backLight_LCD, LOW);
  }

  //-------------------------------------------------------------------------------------------------------

  //-------------------------------- Verifica obstáculo ---------------------------------------------------

  disCentro = sensorCentro.read();
  delay(10);
  disEsq = sensorEsq.read();
  delay(10);
  disDir = sensorDir.read();
  delay(10);

  Serial.print("Distância esquerda - ");
  Serial.println(disEsq);
  Serial.print("Distância centro - ");
  Serial.println(disCentro);
  Serial.print("Distância direita - ");
  Serial.println(disDir);

  if (disCentro < disAlerta_Centro){          //Obstáculo à frente.
    fn_PararMotores();
    AbrirSDCard(root, "FRENTE~1.WAV");
    if (disEsq < disDir){
      fn_VirarDir();                          //Vira à direita se tiver mais espaço. 
    }else{
      fn_VirarEsq();                          //Vira à esquerda se tiver mais espaço. 
    }
  } else if (disEsq < disAlerta_Esq){         //Vira à direita.
    fn_PararMotores();
    AbrirSDCard(root, "ESQUER~1.WAV");
    fn_VirarDir();
  } else if (disDir < disAlerta_Dir){         //Vira à esquerda.
    fn_PararMotores();
    AbrirSDCard(root, "DIREIT~1.WAV");
    fn_VirarEsq();
  }else{                                      //Continua avançando.
    fn_AvancarMotores();     
  }
    
  //-------------------------------------------------------------------------------------------------------  

  //Piscar
  if (piscar > 7){
    fn_Olhar(pisca, 8, 10);
    piscar = 0;
  }else{
    piscar++;
  }
  Serial.println("----------------------------------------------");
}

//------------------------------------------ FUNÇÕES --------------------------------------------

//-----------------------------------------------------------------------------
// Objetivo: Informar que a bateria está acabando e desligar.
// Parâmetros: Não há.
// Retorno: Não há.
//-----------------------------------------------------------------------------
void DesligarRobo(){  
   
  fn_Olhar(sono, 8, 600);

  Serial.println("Desligando o robô!");

  //Desligar robô.
  digitalWrite(Desligar, HIGH); //Transistor PNP.

}

//-----------------------------------------------------------------------------
// Objetivo: Abrir SD Card e tocar arquivo.
// Parâmetros: Arquivo que será tocado.
// Retorno: Não há.
//-----------------------------------------------------------------------------
void AbrirSDCard(File dir, char arquivo[]){
  
    File entry = dir.openNextFile();
    audioCartaoSD.setVolume(5);  
    audioCartaoSD.quality(1); 
    digitalWrite(PIN_Speaker, LOW); 
    audioCartaoSD.play(arquivo); //16Khz - Mono - 8Bits PCM
    while (audioCartaoSD.isPlaying()){
      delay(1);
    }
    digitalWrite(PIN_Speaker, HIGH); 
    entry.close();
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Objetivo: Abrir SD Card e mostrar no LDC.
// Parâmetros: Arquivo que será apresentado.
// Retorno: Texto HEX para apresentar.
//-----------------------------------------------------------------------------
void AbrirSDImagem(File dir, char arquivo[]){ 

  int QtdeChar = 820;              // Quantidade máxima de itens na matriz para caber na tela.
  char matriz1[QtdeChar] = {};     // Matriz temporária para conveter String em HEX.
  byte matriz2[QtdeChar] = {};     // Matriz para apresentar na tela do LCD.
  String recebido;
  int ContMatrizLCD = 0;

  //Lê a linha do arquivo.
  dir = SD.open(arquivo);
   if (dir){
     while (dir.available()){
       recebido = dir.readStringUntil('\n');
     }
   }
  dir.close();

  //Desloca byte para concatenar com 16 bits.
  recebido.toCharArray(matriz1,QtdeChar);
  for (int i = 0; i < QtdeChar ; i++){
    if (matriz1[i] > 0x40){
      matriz1[i] = matriz1[i] - 0x37;
    }else{
      matriz1[i] = matriz1[i] & 0x0F;
    }
  }

  //Monta a matriz para apresentar no LCD.  
  for (int j = 0; j < QtdeChar; j++){
    matriz2[ContMatrizLCD] = matriz1[j] << 4 | matriz1[j + 1];
    j++;
    ContMatrizLCD++;
  }
  
   lcd.drawBitmap(matriz2, iconLin, iconCol);
}
 
//-----------------------------------------------------------------------------
// Objetivo: Medidor de tensão.
// Parâmetros: PinVoltagem = Pino que recebe a voltagem.
// Retorno: String da voltagem medida.
//-----------------------------------------------------------------------------
float fn_MedirTensao(int PinVoltagem){

  sensorValor = analogRead(PinVoltagem);
  double voltagePorc = map(sensorValor,0,1023, 0, 2500);
  
  voltagePorc /=100;
  Serial.print("Voltage: ");
  Serial.print(voltagePorc);
  Serial.println("V");

  delay(500);

  return voltagePorc;
}

//-----------------------------------------------------------------------------
// Objetivo: Olhar.
// Parâmetros: byte com as posições, tempo desejado e duração da apresentação.
// Retorno: Não há.
//-----------------------------------------------------------------------------
void fn_Olhar(byte conteudo[], int tempoDes, int duracao){

  int tempoDec;    //Tempo decorrido
  tempoDec = 0;

  while (tempoDec < tempoDes){
    display.setRow(0,tempoDec,conteudo[tempoDec]);
    tempoDec = tempoDec+1;
  }
  delay(duracao);
}

//-----------------------------------------------------------------------------
// Objetivo: Avançar motores.
// Parâmetros: Não há.
// Retorno: Não há.
//-----------------------------------------------------------------------------
void fn_AvancarMotores(){  

    fn_Olhar(centro, 8, 600);

    Serial.println("^^^ - Avançando motores - ^^^");
    
    analogWrite(RetrocedeMD, 0);  
    analogWrite(RetrocedeME, 0);  
    analogWrite(AvancaMD, 255);     
    analogWrite(AvancaME, 255);
    delay(3000);
}

//-----------------------------------------------------------------------------
// Objetivo: Parar motores.
// Parâmetros: Não há.
// Retorno: Não há.
//-----------------------------------------------------------------------------
void fn_PararMotores (){   

    fn_Olhar(exclamacao, 8, 600);
    
    Serial.println("XXX - Parando motores - XXX");

    analogWrite(AvancaMD, 0);    
    analogWrite(AvancaME, 0);      
    analogWrite(RetrocedeMD, 0);    
    analogWrite(RetrocedeME, 0);
    delay(100);
}

//-----------------------------------------------------------------------------
// Objetivo: Virar à direita.
// Parâmetros: Não há.
// Retorno: Não há.
//-----------------------------------------------------------------------------
void fn_VirarDir(){  

    servoVer.write(0);
    delay(1000);

    fn_Olhar(direito, 8, 600);

    servoVer.write(90);
    delay(1000);

    do{
      fn_Olhar(direito, 8, 600);
      
      Serial.println(">>> - Virando à direita - >>>");
       
      if (sensorEsq.read() <= disAlerta_Esq){
        analogWrite(AvancaME, 0); 
        analogWrite(RetrocedeMD, 0);   
        analogWrite(AvancaMD, 255); 
        analogWrite(RetrocedeME, 255);

        Serial.print("Distância do objeto encontrado / Distância de Alerta:  ");
        Serial.print(sensorEsq.read());
        Serial.print(" / ");
        Serial.println(disAlerta_Esq);
        sensorEsq.read();
      }else{
        Serial.println("### Obstáculo superado! ###");
      }
      
    }while (sensorDir.read() <= disAlerta_Dir);

    servoVer.write(90);
    delay(1000);
}

//-----------------------------------------------------------------------------
// Objetivo: Virar à esquerda.
// Parâmetros: Não há.
// Retorno: Não há.
//-----------------------------------------------------------------------------
void fn_VirarEsq(){ 

    servoVer.write(180);
    delay(1000);

    fn_Olhar(esquerdo, 8, 600);

    servoVer.write(90);
    delay(1000);

    do{
      fn_Olhar(esquerdo, 8, 600);
      
      Serial.println("<<< - Virando à esquerda - <<<");
       
      if (sensorDir.read() <= disAlerta_Dir){
        analogWrite(AvancaMD, 0); 
        analogWrite(RetrocedeME, 0);    
        analogWrite(AvancaME, 255); 
        analogWrite(RetrocedeMD, 255);
 
        Serial.print("Distância do objeto encontrado / Distância de Alerta:  ");
        Serial.print(sensorDir.read());
        Serial.print(" / ");
        Serial.println(disAlerta_Dir);
        sensorDir.read();
      }else{
        Serial.println("### Obstáculo superado! ###");
      }
      
    }while (sensorEsq.read() <= disAlerta_Esq);

    servoVer.write(90);
    delay(1000);
}

//-----------------------------------------------------------------------------
// Objetivo: Posicionamento no display LCD.
// Parâmetros: Valor da tensão.
// Retorno: Não há.
//-----------------------------------------------------------------------------
void IconeBateria(int valor){
  if (valor >= 10){
    AbrirSDImagem(root, "100.txt"); 
  }else if ((valor <10) and (valor >= 9)){
    AbrirSDImagem(root, "90.txt"); 
  }else if ((valor < 9) and (valor >= 8)){
    AbrirSDImagem(root, "80.txt"); 
  }else if ((valor < 8) and (valor >= 7)){
    AbrirSDImagem(root, "70.txt"); 
  }else if ((valor < 7) and (valor >= 6)){
    AbrirSDImagem(root, "60.txt"); 
  }else if ((valor < 6) and (valor >= 5)){
    AbrirSDImagem(root, "50.txt"); 
    DesligarRobo();
  }else if ((valor < 5) and (valor >= 4)){
    AbrirSDImagem(root, "40.txt"); 
    DesligarRobo();
  }else if ((valor < 4) and (valor >= 3)){
    AbrirSDImagem(root, "30.txt"); 
    DesligarRobo();
  }else if ((valor < 3) and (valor >= 2)){
    AbrirSDImagem(root, "20.txt"); 
    DesligarRobo();
  }else if ((valor < 2) and (valor >= 1)){
    AbrirSDImagem(root, "10.txt"); 
    DesligarRobo();
  }else if (valor <= 1){
    AbrirSDImagem(root, "00.txt");
    DesligarRobo();
  }
}
