#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0


int tempo = 180;
int buzzer = 56;
int bestScore = 0;
int melody[] = {

  // Nokia Ringtone 
  // Score available at https://musescore.com/user/29944637/scores/5266155
  // preluat de la https://github.com/robsoncouto/arduino-songs/blob/master/nokia/nokia.ino
  NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4, 
  NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4, 
  NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
  NOTE_A4, 2, 
};
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

typedef struct Snake Snake;
struct Snake{
  int head[3];
  int body[40][3];
  int len;
  int dir[3];
};

typedef struct Apple Apple;
struct Apple{
  int pos0;
  int pos1;
  int pos2;
};
const int butonjoy = 19;
const int butonjump = 2;
const int varXPin = A0;
const int varYPin = A1;
Snake snake;
Apple apple;

float oldTime = 0;
float timer = 0;
float updateRate = 1;

int score = 0;

int i,j;

int leds[] = {
    38, 44, 5, 52,
    34, 47, 53, 17,
    10, 22, 36, 48,
    39, 37, 27, 8,
    //
    14, 42, 4, 6,
    32, 49, 16, 18,
    12, 26, 28, 3,
    23, 35,29, 25,
    // 
    15, 40, 46, 7,
    43, 45, 51, 13,
    11, 50, 30, 24,
    41, 33, 31, 9
};
bool ledBool[48];



LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  pinMode(varXPin, INPUT);
  pinMode(varYPin, INPUT);
  pinMode(butonjoy, INPUT);
  pinMode(butonjump, INPUT);
	lcd.begin(16,2);
  lcd.backlight();
	lcd.print("score = " + String(score));
  lcd.setCursor(0,1);
  lcd.print("best is " + String(0));
  for(i = 0; i <=53; i++){
    if (i != 20 && i != 21 && i != 19 && i != 2){
      pinMode(i, OUTPUT);
    }
  }

  for (i = 0;i<48;i++) {
    ledBool[i] = false;
  }
  snake = {{1,0,2},{{0,0,2}, {1,0,2}}, 2, {1,0,0}};
  apple = {(int)random(0,4),(int)random(0,4),(int)random(0,3)};
  ledBool[apple.pos0+4*apple.pos1+16*apple.pos2] = true;

}

void loop() {
  float deltaTime = calculateDeltaTime();
  timer += deltaTime;
  int bVal = digitalRead(butonjoy);
  int jumpVal = digitalRead(butonjump);
  int xVal = analogRead(varXPin);
  int yVal = analogRead(varYPin);
  if (bVal == 1 && snake.head[2] != 0) {
    snake.dir[2] = -1;
  } else if(jumpVal == 1 && snake.head[2] != 2) {
    snake.dir[2] = 1;
  } else {
    snake.dir[2] = 0;
  }

  if(xVal<100 && snake.dir[1]==0){
    snake.dir[0] = 0;
    snake.dir[1] = -1;
  }else if(xVal >920 && snake.dir[1]==0){
    snake.dir[0] = 0;
    snake.dir[1] = 1;
  }else if(yVal<100 && snake.dir[0]==0){
    snake.dir[0] = -1;
    snake.dir[1] = 0;
  }else if(yVal >920 && snake.dir[0]==0){
    snake.dir[0] = 1;
    snake.dir[1] = 0;
  }

  if(timer > 1000/updateRate){
    score(xVal,yVal);
    timer = 0;
    Update();
  }
  
}

float calculateDeltaTime(){
  float currentTime = millis();
  float dt = currentTime - oldTime;
  oldTime = currentTime;
  return dt;
}

void Update(){
  
  int newHead[3] = {snake.head[0]+snake.dir[0], snake.head[1]+snake.dir[1],snake.head[2]+snake.dir[2]};
  snake.dir[2] = 0;

  //Handle Borders
  if(newHead[0]==4){
    newHead[0]=0;
  }else if(newHead[0]==-1){
    newHead[0] = 3;
  }else if(newHead[1]==4){
    newHead[1]=0;
  }else if(newHead[1]==-1){
    newHead[1]=3;
  }
  
   for(j=0;j<snake.len;j++){
    if(snake.body[j][0] == newHead[0] && snake.body[j][1] == newHead[1] && snake.body[j][2] == newHead[2]){
      song();
      delay(1000);
      snake = {{1,0,2},{{0,0,2}, {1,0,2}}, 2, {1,0,0}};
      apple = {(int)random(0,4),(int)random(0,4),(int)random(0,3)};
      for (i = 0;i<48;i++) {
        ledBool[i] = false;
        }
      ledBool[apple.pos0+4*apple.pos1+16*apple.pos2] = true;
      turnLeds();
      score = 0;
      return;
    }
  }

  if(newHead[0] == apple.pos0 && newHead[1] ==apple.pos1 && apple.pos2 == newHead[2]) {
    score++;
    snake.len = snake.len+1;
    apple.pos0 = (int)random(0,4);
    apple.pos1 = (int)random(0,4);
    apple.pos2 = (int)random(0,3);
    ledBool[apple.pos0+4*apple.pos1+16*apple.pos2] = true;
  }else{
    removeFirst();
  }
  
  snake.body[snake.len-1][0]= newHead[0];
  snake.body[snake.len-1][1]= newHead[1];
  snake.body[snake.len-1][2]= newHead[2];

  snake.head[0] = newHead[0];
  snake.head[1] = newHead[1];
  snake.head[2] = newHead[2];
  
  printit();
  // printleds();
  turnLeds();
}



void removeFirst() {
  ledBool[snake.body[0][0]+snake.body[0][1]*4+snake.body[0][2]*16] = false;
  for(j=1;j<snake.len;j++) {
    snake.body[j-1][0] = snake.body[j][0];
    snake.body[j-1][1] = snake.body[j][1];
    snake.body[j-1][2] = snake.body[j][2];
  }

}

void printit() {
    for(j=0;j<snake.len;j++){
      ledBool[snake.body[j][0]+snake.body[j][1]*4+snake.body[j][2]*16] = true;
    }
}

void score(int xVal, int yVal) {

  lcd.setCursor(0,0);
  lcd.print("score = " + String(score));
  lcd.setCursor(0,1);
  if (bestScore > score) {
    bestScore = score;
  }
  lcd.print("best is " + String(bestScore));
}

void printleds() {
  int i;
  int j;
  int k;
  for(k = 0; k < 3; k++){
    Serial.println(" "); 
    for(j = 0; j < 4; j++){
      Serial.println(" ");
      for(i = 0; i < 4;i++) {
        Serial.print(ledBool[i+j*4+k*16]);
      }}}
    Serial.println("end");

}

void turnLeds() {
  for (i = 0; i<48;i++){
    if(ledBool[i] == true) {
      digitalWrite(leds[i], HIGH);
    } else {
      digitalWrite(leds[i], LOW);
    }
  }
}

void song() {
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }

    tone(buzzer, melody[thisNote], noteDuration * 0.9);

    delay(noteDuration);

    noTone(buzzer);
  }
}

