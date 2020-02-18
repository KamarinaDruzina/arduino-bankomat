#include <Keypad.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
// Define the Keymap
char hexaKeys[ROWS][COLS] = {
  {
    '1',
    '2',
    '3',
    'A'
  },
  {
    '4',
    '5',
    '6',
    'B'
  },
  {
    '7',
    '8',
    '9',
    'C'
  },
  {
    '*',
    '0',
    '#',
    'D'
  }
};
byte rowPins[ROWS] = {
  9,
  8,
  7,
  6
}; //connect to the Rows of the keypad pin 8, 7, 6, 5 respectively
byte colPins[COLS] = {
  5,
  4,
  3,
  2
}; //connect to the Columns of the keypad pin 4, 3, 2, 1 respectively

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int PASSWORD1 = "5394";
int PASSWORD2 = "0000";
int PASSWORD3 = "1234";

int RACUN1 = 13000;
int RACUN2 = 24;
int RACUN3 = -6;

bool done = false;
bool finished = false;
bool biranje = false;
bool isplacuje = false;
int brojac = 0;
String zaIsplatu = "";
int racun = 0;
String sifra = "";

void setup() {
  Serial.begin(9600);
  lcd.init();
  // turn on the backlight
  lcd.backlight();
  resetuj();
}

bool verifikuj(int racun, String sifra) {
  switch (racun) {
  case 1:
    {
      return sifra == PASSWORD1;
    }
  case 2:
    {
      return sifra == PASSWORD2;
    }
  case 3:
    {
      return sifra == PASSWORD3;
    }
  default:
    {
      return false;
    };
  }
}

int stanje(int racun, int novoStanje = 0) {

  switch (racun) {
  case 1:
    {
      RACUN1 -= novoStanje;
      return RACUN1;
    }
  case 2:
    {
      RACUN2 -= novoStanje;
      return RACUN2;
    }
  case 3:
    {
      RACUN3 -= novoStanje;
      return RACUN3;
    }
  default:
    {
      return false;
    };
  }

}

void resetuj() {
  sifra = "";
  brojac = 0;
  racun = 0;
  biranje = false;
  done = false;
  zaIsplatu = "";
  finished = false;
  isplacuje = false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Upisite broj");
  lcd.setCursor(0, 1);
  lcd.print("racuna:");
}

int multiplier = 1;
void loop() {
  char unos = customKeypad.getKey();

  if (unos) {
    if (isplacuje) {
      if(unos != 'A' && unos != 'B' && unos != 'C' && unos != '*' && unos != '#' && unos != 'D') {
          zaIsplatu += unos;
          
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Isplatiti: ");
          lcd.setCursor(0, 1);
          lcd.print(zaIsplatu);
      } else if(unos == 'D') {
        int stanjeRac = stanje(racun, 0);
          if(zaIsplatu.toInt() % 10 == 0 && zaIsplatu.toInt() > 0 && stanjeRac >= zaIsplatu.toInt()  ) {
             stanje(racun, zaIsplatu.toInt());
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Isplaceno.");
            isplacuje = false;
            delay(5000);
            resetuj();
          } else {
            if(stanjeRac < zaIsplatu.toInt()) {
              lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Nema dovoljno");
            lcd.setCursor(0, 1);
            lcd.print("sredstava.");
            delay(5000);
            resetuj();
            } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Pogresan unos.");  
            delay(5000);
            resetuj();
            }
            
          }
      }
    } else if (biranje) {
      if (unos == 'D' || (done && unos == 'D') || (finished && unos == 'D')) {
        resetuj();
        return;
      }

      if (done && unos == 'A') {
        finished = true;
        int stanjeRac = stanje(racun, 0);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Racun ");
        lcd.print(racun);
        lcd.setCursor(0, 1);
        lcd.print("BAM ");
        lcd.print(stanjeRac);
        return;
      } else if (done && unos == 'B') {
        //finished = true;
        //int stanjeRac = stanje(racun, 0);
        isplacuje = true;
        done = true;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Isplatiti: ");
        lcd.setCursor(0, 1);
        lcd.print(zaIsplatu);
        return;
      }

      brojac++;
      sifra += unos;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Unesite sifru:");
      lcd.setCursor(0, 1);
      lcd.print(sifra);
      if (brojac == 4) {
        Serial.println(sifra);
        if (verifikuj(racun, sifra)) {
          int stanjeRac = stanje(racun, 0);
          done = true;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("A - Stanje");
          lcd.setCursor(0, 1);
          lcd.print("B - Isplata");
          /*
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Racun ");
          lcd.print(racun);
          lcd.setCursor(0, 1);
          lcd.print("BAM ");
          lcd.print(stanjeRac); */

        } else {
          Serial.println("Pogresna sifra");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Pogresna sifra");
          lcd.setCursor(0, 1);
          lcd.print(sifra);
          //resetuj();
        }

      }
    } else {
      if (unos != '1' && unos != '2' && unos != '3') {
        Serial.println("Ne postoji racun");

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Ne postoji!");

        delay(5000);
        resetuj();
      } else {
        biranje = true;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Unesite sifru:");
        lcd.setCursor(0, 1);
        //lcd.print("racuna:");
        Serial.println("Unesite sifru: ");

        if (unos == '1')
          racun = 1;

        if (unos == '2')
          racun = 2;

        if (unos == '3')
          racun = 3;

        if (unos == '4')
          racun = 4;

      }
    }
  }
}
