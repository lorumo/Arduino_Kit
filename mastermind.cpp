#include "mastermind.h"
#include "lcd_wrapper.h"
#include <stdio.h>
#include <arduino.h>


char* generate_code(bool repeat, int length) {
    if (length < 1) {
        return NULL;
    }
    if (!repeat && length > 10) { //cause we cannot have 10 different numbers {1;9}
        return NULL;
    }

    char* resultcode = (char*) malloc((length + 1) * sizeof(char));
    if (resultcode == NULL) {
        return NULL;
    }

    randomSeed(analogRead(A2)); //random by port

    for (int i = 0; i < length; i++) { 
        int numero;
        bool uniqalnyi = false; //to chek if number is repeated

        while (!uniqalnyi) {          
            numero = random(0, 10);
            uniqalnyi = true;

            if (!repeat) {
                for (int j = 0; j < i; j++) {
                    if (numero == (resultcode[j] - '0')) {
                        uniqalnyi = false;
                        break;
                    }
                }
            }
        }

        resultcode[i] = '0' + numero; 
    }

    resultcode[length] = '\0'; //terminatORR

    return resultcode;
}

void get_score(char* secret, char* guess, int* peg_a, int* peg_b){
    int dlz = strlen(secret);
    *peg_a = 0;
    *peg_b = 0;

    if (secret != NULL && guess != NULL){
        if (strcmp(secret, guess) == 0)
            *peg_a = dlz;
        else{
            for (int i = 0; i < strlen(guess); i++) {
                if (secret[i] == guess[i]) {
                    (*peg_a)++;
                }
            }

            for (int i = 0; i < strlen(guess); i++) {
                if (secret[i] != guess[i]) {
                    for (int j = 0; j < dlz; j++) {
                        if (i != j && secret[i] == guess[j]) {
                            (*peg_b)++;
                            break;
                        }
                    }
                }
            }
        }
    }
}



void turn_off_leds(){
    digitalWrite(PIN_RED4, LOW);
    digitalWrite(PIN_BLUE4, LOW);
    digitalWrite(PIN_RED3, LOW);
    digitalWrite(PIN_BLUE3, LOW);
    digitalWrite(PIN_RED2, LOW);
    digitalWrite(PIN_BLUE2, LOW);
    digitalWrite(PIN_RED1, LOW);
    digitalWrite(PIN_BLUE1, LOW);
  }

void render_leds(const int peg_a, const int peg_b){
    int leds[4][2] = {
        {PIN_RED1, PIN_BLUE1},
        {PIN_RED2, PIN_BLUE2},
        {PIN_RED3, PIN_BLUE3},
        {PIN_RED4, PIN_BLUE4}
    };

	turn_off_leds(); 
    for(int i = 0; i < peg_a+peg_b; i++){
      	if(i < peg_a)
    		    analogWrite(leds[i][0], 255);
      	else
      		  analogWrite(leds[i][1], 255);
    }
}


void render_history(char * secret, char** history, const int entry){
  	int peg_a;
  	int peg_b;
  
  	get_score(secret, history[entry], &peg_a, &peg_b);

    render_leds(peg_a, peg_b);

  	char buffer[15];
    if(entry+1 < 10)
        sprintf(buffer, "Att%d: %s %dA%dB ", entry+1, history[entry], peg_a, peg_b);
    else
        sprintf(buffer, "Att%d: %s %dA%dB", entry+1, history[entry], peg_a, peg_b);

    //lcd_clear();
    lcd_set_cursor(0,0);
    lcd_print(buffer);
}


void play_game(char* secret) {
  int try_ = 1;
  
  int history_counter = 0;
  int a = 0;
  int b = 0;
  char* history[11];
  char* guess = (char*)calloc(5, sizeof(char));
  
  for (int i = 0; i < 4; i++)
    guess[i] = '0';
  
  guess[4] = '\0';
  
  while (try_ <= 10 && a < 4) {
    lcd_print_at(1, 1, "My guess: ");
    lcd_print(guess);
    
    if (digitalRead(BTN_1_PIN) == HIGH) {
      if (digitalRead(BTN_2_PIN) == HIGH && history_counter >= 1) {
        //Serial.print("slovo");
        history_counter--;
        render_history(secret, history, history_counter);
        delay(200);
      } else if (digitalRead(BTN_3_PIN) == HIGH && history_counter < try_ - 2) {
        //Serial.print(history_counter);
        history_counter++;
        render_history(secret, history, history_counter);
        delay(200);
      } else {
        if (guess[0] < '9') {
          guess[0]++;
        } else {
          guess[0] = '0';
        }
        delay(200);
      }
    } else if (digitalRead(BTN_2_PIN) == HIGH) {
      if (guess[1] < '9') {
        guess[1]++;
      } else {
        guess[1] = '0';
      }
      delay(200);
    } else if (digitalRead(BTN_3_PIN) == HIGH) {
      if (guess[2] < '9') {
        guess[2]++;
      } else {
        guess[2] = '0';
      }
      delay(200);
    } else if (digitalRead(BTN_4_PIN) == HIGH) {
      if (guess[3] < '9') {
        guess[3]++;
      } else {
        guess[3] = '0';
      }
      delay(200);
    } else if (digitalRead(BTN_ENTER_PIN) == HIGH) {
      history[try_ - 1] = new char[5];
      strcpy(history[try_ - 1], guess);
      get_score(secret, guess, &a, &b);
      turn_off_leds();
      render_leds(a, b);
      lcd_set_cursor(0, 0);
      
      char buffer[15];
      sprintf(buffer, "Try %d: %s %dA%dB", try_, history[try_ - 1], a, b);
      lcd_print(buffer);
      
      for (int i = 0; i < 4; i++)
        guess[i] = '0';
      history_counter = try_-1;
      try_++;
      
      delay(200);
    }
  }
  
  delay(1000);
  
  if (a == 4) {
    lcd_clear();
    turn_off_leds();
    lcd_print_at(0, 5, "YOU WON!");
    digitalWrite(PIN_BLUE1, HIGH);
    digitalWrite(PIN_BLUE2, HIGH);
    digitalWrite(PIN_BLUE3, HIGH);
    digitalWrite(PIN_BLUE4, HIGH);
    digitalWrite(PIN_RED1, HIGH);
    digitalWrite(PIN_RED2, HIGH);
    digitalWrite(PIN_RED3, HIGH);
    digitalWrite(PIN_RED4, HIGH);
    delay(2000);
  } else {
    lcd_clear();
    turn_off_leds();
    lcd_print_at(0, 4, "YOU L00SE!");
    delay(2000);
  }
}
