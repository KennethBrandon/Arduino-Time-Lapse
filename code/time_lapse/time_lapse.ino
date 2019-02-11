
#include <LiquidCrystal_I2C.h>
#include "pitches.h"
#define STEP_PIN 4
#define DIR_PIN 5
#define DISTANCE 1600
#define BUTTON_RIGHT_PIN 10
#define BUTTON_LEFT_PIN 9
#define BUTTON_3_PIN 8
#define BUTTON_4_PIN 7
#define BUTTON_5_PIN 6
#define SPEAKER_PIN 11
LiquidCrystal_I2C lcd(0x27,20,4);

enum states {
  SET_LOCATIONS,
  SET_TIME,
  GOTO_START,
  GOTO_END,
  OVERVIEW,
  RUNNING
};

enum states state = SET_LOCATIONS;
enum states lastState = SET_LOCATIONS;
int stepCountStart = 0;
int stepCountEnd = 0;
bool wasLeftDown = false;
bool wasRightDown = false;
int currentStepCount = 0;

enum timeValues{
  SEC,
  MIN,
  HOUR
};
int seconds = 0;
int minutes = 0;
int hours = 0;

enum timeValues timeSelection = SEC;

int melody[] = { // notes in the melody:
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
int noteDurations[] = { // note durations: 4 = quarter note, 8 = eighth note, etc.:
  4, 8, 8, 4, 4, 4, 4, 4
};


void setup() {
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);

  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_3_PIN, INPUT_PULLUP);
  pinMode(BUTTON_4_PIN, INPUT_PULLUP);
  pinMode(BUTTON_5_PIN, INPUT_PULLUP);
  
  lcd.init(); // initialize the lcd 
  lcd.backlight();
  printLocations();
}

void loop() {
  switch(state){
    case SET_LOCATIONS:
      loopSetLocations();
      break;
    case SET_TIME:
      loopSetTime();
      break;
    case GOTO_START:
      loopGotoStart();
      break;
    case OVERVIEW:
      loopOverview();
      break;
    case RUNNING:
      loopRunning();
      break;
  }
  delay(1);
}

void printLocations() {
  lcd.clear();
  lcd.setCursor(15,0);
  lcd.print("Set A");
  lcd.setCursor(15,1);
  lcd.print("Set B");
  lcd.setCursor(16,2);
  lcd.print("Next");
  lcd.setCursor(0,3);
  lcd.print("<-      Move      ->");
}
void playMelody() {
    // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(SPEAKER_PIN, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(SPEAKER_PIN);
  }
}
void chirp(){
  tone(SPEAKER_PIN, NOTE_C5, 125);
}
void playBegin(){
  tone(SPEAKER_PIN, NOTE_C6, 250);
  delay(800);
  tone(SPEAKER_PIN, NOTE_C6, 250);
  delay(800);
  tone(SPEAKER_PIN, NOTE_C6, 250);
  delay(800);
  tone(SPEAKER_PIN, NOTE_C7, 750);
}

void loopSetLocations() {
  
  bool setStartDown = !digitalRead(BUTTON_3_PIN);
  bool setEndDown = !digitalRead(BUTTON_4_PIN);
  bool nextDown = !digitalRead(BUTTON_5_PIN);
  if(lastState!=SET_LOCATIONS){
    printLocations();
    lastState = SET_LOCATIONS;
  }
  if(digitalRead(BUTTON_LEFT_PIN) == LOW){
    if(!wasLeftDown){
      wasLeftDown = true;
      digitalWrite(DIR_PIN, LOW);
      chirp();
    }
    sendStep();
    currentStepCount = currentStepCount - 1;
    if(currentStepCount%250 == 0){
      printCount();
    }
  }
  else{
    if(wasLeftDown){
      wasLeftDown = false;
      lcd.setCursor(4,2);
      lcd.print("         ");
      chirp();
    }
  }
  if(digitalRead(BUTTON_RIGHT_PIN) == LOW){
    if(!wasRightDown){
      wasRightDown = true;
      digitalWrite(DIR_PIN, HIGH);
      chirp();
    }
    sendStep();
    currentStepCount = currentStepCount + 1;
    if(currentStepCount%250 == 0){
      printCount();
    }
  }
  else{
    if(wasRightDown){
      wasRightDown = false;
      lcd.setCursor(4,2);
      lcd.print("         ");
      chirp();
    }
  }
  if(!wasLeftDown && !wasRightDown){
    lcd.setCursor(0,2);
    lcd.print("loc:");
    lcd.setCursor(4,2);
    lcd.print(currentStepCount);
    
    lcd.setCursor(0,0);
    lcd.print("A:");
    lcd.setCursor(2,0);
    lcd.print(stepCountStart);

    lcd.setCursor(0,1);
    lcd.print("B:");
    lcd.setCursor(2,1);
    lcd.print(stepCountEnd);
  }
  if(setStartDown){
    stepCountStart = currentStepCount;
    chirp();
    lcd.setCursor(2,0);
    lcd.print("        "); 
    lcd.setCursor(2,0);
    lcd.print(stepCountStart);
    delay(500);
  }
  if(setEndDown){
    stepCountEnd = currentStepCount;
    chirp();
    
    lcd.setCursor(2,1);
    lcd.print("       ");
    lcd.setCursor(2,1);
    lcd.print(stepCountEnd);
    delay(500);
  }
  if(nextDown){
    if(stepCountEnd != 0 || stepCountStart !=0){
      state = SET_TIME;
      chirp();
      delay(500);
    }
  }

}
void loopSetTime() {
  if(lastState != SET_TIME){
    printSetTime();
    lastState = SET_TIME;
  }
  bool rightDown = !digitalRead(BUTTON_RIGHT_PIN);
  bool leftDown = !digitalRead(BUTTON_LEFT_PIN);
  bool backDown = !digitalRead(BUTTON_3_PIN);
  bool timeDown = !digitalRead(BUTTON_4_PIN);
  bool nextDown = !digitalRead(BUTTON_5_PIN);
  if(backDown) {
    state = SET_LOCATIONS;
    chirp();
    delay(500);
  }
  if(timeDown){ 
    switch(timeSelection){
      case MIN: timeSelection = HOUR; break;
      case SEC: timeSelection = MIN; break;
      case HOUR: timeSelection = SEC; break;
    }
    printTimeSelection();
    chirp();
    delay(500);
  }
  if(rightDown){
    switch(timeSelection){
      case MIN: if(minutes<59) minutes++; else minutes = 0;break;
      case SEC: if(seconds<59) seconds++; else seconds = 0; break;
      case HOUR: if(hours<999) hours++; break;
    }
    printDuration();
    chirp();
    delay(100);
  }
  if(leftDown){
    switch(timeSelection){
      case MIN: if(minutes>0) minutes--; else minutes = 59; break;
      case SEC: if(seconds>0) seconds--; else seconds = 59; break;
      case HOUR: if(hours>0) hours--; break;
    }
    printDuration();
    chirp();
    delay(100);
  }
  if(nextDown){
    if(minutes == hours && hours == seconds && seconds == 0) return;
    state = OVERVIEW;
    chirp();
    delay(500);
  }
}

bool isRunning = false;
void loopRunning(){
  if(lastState != RUNNING){
    lastState = RUNNING;
    lcd.clear(); lcd.setCursor(7,1); lcd.print("Ready?");
    playBegin();
    lcd.clear(); lcd.setCursor(7,1); lcd.print("GO!");
  }
  bool pauseDown = !digitalRead(BUTTON_RIGHT_PIN);
  bool quitDown = !digitalRead(BUTTON_3_PIN);
  
  if(quitDown){
    state = OVERVIEW;
    chirp();
    delay(500);
  }
  if(pauseDown){
    chirp();
    delay(500);
    isRunning = !isRunning;
  }
  
}

bool goUp = true;
bool goToBegin = false;
void loopGotoStart(){
  if(lastState != GOTO_START){
    lastState = GOTO_START;
    if(currentStepCount > stepCountStart) {
      digitalWrite(DIR_PIN, LOW);
      goUp = false;
    } else {
      digitalWrite(DIR_PIN, HIGH);
      goUp = true;
    }
    lcd.clear(); lcd.setCursor(5,1); lcd.print("GOTO START!");
  }
  if(currentStepCount == stepCountStart){
    if(goToBegin) state = RUNNING;
    else state = OVERVIEW;
    return;
  }
  sendStep();
  if(goUp) currentStepCount++;
  else currentStepCount--;
 
  
}
void loopOverview(){
    if(lastState != OVERVIEW){
    lastState = OVERVIEW;
    printOverview();
  }
  
  bool beginDown = !digitalRead(BUTTON_RIGHT_PIN);
  bool gotoStartDown = !digitalRead(BUTTON_LEFT_PIN);
  bool backDown = !digitalRead(BUTTON_3_PIN);

  if(backDown){
    state = SET_TIME;
    chirp();
    delay(500);
  }
  if(gotoStartDown){
    goToBegin = false;
    state= GOTO_START;
    chirp();
    delay(500);
  }
  if(beginDown){
    goToBegin = true;
    state = GOTO_START;
    delay(500);
  }

}
void printOverview(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Location:");
  lcd.setCursor(9,0);
  lcd.print(currentStepCount);
  lcd.setCursor(16,0);
  lcd.print("Back"); 
  lcd.setCursor(0,1);
  lcd.print("Duration:");
  printDuration(9,1);
  lcd.setCursor(0,2);
  lcd.print("Start:");
  lcd.setCursor(6,2);
  lcd.print(stepCountStart);
  
  lcd.setCursor(11,2);
  lcd.print("End:");
  lcd.setCursor(15,2);
  lcd.print(stepCountEnd);
  lcd.setCursor(0,3);
  lcd.print("Goto Start     Begin");

}
void printTimeSelection(){
  switch(timeSelection){
    case HOUR:
      lcd.setCursor(0,3);
      lcd.print("<-     Hour       ->");
    break;
    case MIN:
      lcd.setCursor(0,3);
      lcd.print("<-     Minute     ->");
    break;
    case SEC:
      lcd.setCursor(0,3);
      lcd.print("<-     Second     ->");
    break;
  }
}
void printDuration(int col, int row ){
  lcd.setCursor(col,row);
  lcd.print(" 00:00:00");
  
  if(hours>99) lcd.setCursor(col,row);
  else if(hours>9) lcd.setCursor(col+1,row);
  else lcd.setCursor(col+2,row);
  lcd.print(hours);

  if(minutes>9) lcd.setCursor(col+4,row);
  else lcd.setCursor(col+5,row);
  lcd.print(minutes);
  
  if(seconds>9) lcd.setCursor(col+7,row);
  else lcd.setCursor(col+8,row);
  lcd.print(seconds);
}
void printDuration(){
  printDuration(0,1);
}

void printCount(){
  lcd.setCursor(4,2);
  lcd.print("         ");
  lcd.setCursor(4,2);
  lcd.print(currentStepCount);
}
void sendStep(){
  digitalWrite(STEP_PIN, HIGH);
  digitalWrite(STEP_PIN, LOW);
}
void printSetTime(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set duration");
  lcd.setCursor(16,0);
  lcd.print("Back"); 
  lcd.setCursor(0,1);
  lcd.print(" 00:00:00       Time");
  lcd.setCursor(16,2);
  lcd.print("Next");
  printDuration();
  printTimeSelection();
}
