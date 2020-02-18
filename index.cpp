#include <Keypad.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 4;

const char hexaKeys[ROWS][COLS] = {
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
const byte rowPins[ROWS] = {
  9,
  8,
  7,
  6
};
const byte colPins[COLS] = {
  5,
  4,
  3,
  2
};
const Keypad tastatura = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const String lozinke[3] = {
  "5394",
  "0000",
  "1234"
};
int racuni[3] = {
  13000,
  24,
  -6
};

String zaIsplatu = "";
String sifra = "";
bool done = false;
bool finished = false;
bool biranje = false;
bool isplacuje = false;
int racun = 0;
int brojac = 0;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  resetuj();
}

// Provjerava da li postoji račun i da li je unešena šifra ispravna
bool verifikuj(int racun, String sifra) {
  if (racun < 1 || racun > 3)
    return false;

  return lozinke[racun - 1] == sifra;
}

// Oduzima novac sa računa i vraća novo stanje
int stanje(int racun, int novoStanje = 0) {
  if (racun < 1 || racun > 3)
    return -1;

  racuni[racun - 1] -= novoStanje;
  return racuni[racun - 1];
}

// Vraća sve vrijednosti na početnu vrijednost
void resetuj() {
  zaIsplatu = "";
  sifra = "";
  done = false;
  finished = false;
  biranje = false;
  isplacuje = false;
  racun = 0;
  brojac = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Upisite broj");
  lcd.setCursor(0, 1);
  lcd.print("racuna:");
}

void isplacivanje(char unos) {
  if (unos != 'A' && unos != 'B' && unos != 'C' && unos != '*' && unos != '#' && unos != 'D') {
    zaIsplatu += unos;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Isplatiti: ");
    lcd.setCursor(0, 1);
    lcd.print(zaIsplatu);
  } else if (unos == 'D') {
    int stanjeRac = stanje(racun, 0);

    if (zaIsplatu.toInt() % 10 == 0 && zaIsplatu.toInt() > 0 && stanjeRac >= zaIsplatu.toInt()) {
      stanje(racun, zaIsplatu.toInt());

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Isplaceno.");

      isplacuje = false;

      delay(5000);
      resetuj();
    } else {
      if (stanjeRac < zaIsplatu.toInt()) {
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
}

void meni(char unos) {
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
    if (verifikuj(racun, sifra)) {
      int stanjeRac = stanje(racun, 0);

      done = true;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("A - Stanje");
      lcd.setCursor(0, 1);
      lcd.print("B - Isplata");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pogresna sifra");
      lcd.setCursor(0, 1);
      lcd.print(sifra);
    }

  }
}

void unosenje(char unos) {
    if (unos != '1' && unos != '2' && unos != '3') {
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

        if (unos == '1')
          racun = 1;
        else if (unos == '2')
          racun = 2;
        else if (unos == '3')
          racun = 3;
      }
}

void loop() {
  char unos = tastatura.getKey();

  if (unos) {
    if (isplacuje) {
      isplacivanje(unos);
    } else if (biranje) {
        meni(unos);
    } else {
      unosenje();
    }
  }
}
