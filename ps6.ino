#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//#include "mastermind.cpp"
#include "mastermind.h"
#include "lcd_wrapper.h"
LiquidCrystal_I2C lcd(0x27, 16, 2);
void lcd_init(){
    lcd.begin();
    lcd.backlight();
}

void lcd_clear(){
    lcd.clear();
}


void lcd_set_cursor(int y, int x){
    lcd.setCursor(y,x);
}


void lcd_print(char* text){
    lcd.print(text);
}


void lcd_print_at(int y, int x, char* text){
    lcd_set_cursor(y, x);
    lcd_print(text);
}


void setup(){
  Serial.begin(9600);
  randomSeed(analogRead(A1));
  lcd_init();
  lcd_print_at(3, 0, "Welcome to");
  lcd_print_at(3, 1, "MasterMind");
  delay(2000);
  lcd_clear();
  lcd_print_at(0,0,"Your goal is to");
  lcd_print_at(0,1,"guess my secret");
  delay(3000);
  lcd_clear();
  lcd_print_at(1,0,"I am thinking");
  lcd_print_at(4,1,"a number");
  delay(2000);
  lcd_clear();
}

void loop(){ 
  lcd_clear();
  turn_off_leds();
  
  lcd_print_at(3,1,"Good luck!");
  delay(1000);
  lcd_clear();

  lcd_print_at(0,0,"Allow to repeat?");
  lcd_print_at(0,1,"1(y)");
  lcd_print_at(12,1,"2(n)");
  bool dbl = false;
  while(digitalRead(BTN_1_PIN) == LOW && digitalRead(BTN_2_PIN) == LOW){};
  if(digitalRead(BTN_1_PIN) == HIGH)
      dbl = true;
  lcd_clear();
  char* code = generate_code(dbl, 4);
  Serial.print("CODE: ");
  Serial.println(code);
  play_game(code);
  free(code);
}

