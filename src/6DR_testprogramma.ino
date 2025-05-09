#include <AccelStepper.h>

#define STEP_PIN 5
#define DIR_PIN 17
#define ENABLE_PIN 4

const int stappenPerOmwenteling = 400;
const int maxSnelheid = 4000;
const int maxVersnelling = 4000;

AccelStepper motor(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

bool wachtOpInvoer = true;
bool motorBezig = false;
int doelSnelheid = 0;
int doelVersnelling = 1000;

void setup() {
  Serial.begin(9600);
  Serial.println("Voer snelheid%, versnelling% en rotaties in (bijv: 50 75 10):");
  Serial.println("Typ 'S' voor een noodstop.");

  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);

  motor.setMaxSpeed(maxSnelheid);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.equalsIgnoreCase("S")) {
      motor.stop();
      digitalWrite(ENABLE_PIN, LOW);
      Serial.println("‼️ Noodstop geactiveerd. Motor uitgeschakeld.");
      wachtOpInvoer = true;
      motorBezig = false;
      return;
    }

    // Input splitsen op spaties
    int eersteSpatie = input.indexOf(' ');
    int tweedeSpatie = input.indexOf(' ', eersteSpatie + 1);

    if (eersteSpatie > 0 && tweedeSpatie > eersteSpatie) {
      int snelheidPercent = input.substring(0, eersteSpatie).toInt();
      int versnellingPercent = input.substring(eersteSpatie + 1, tweedeSpatie).toInt();
      int rotaties = input.substring(tweedeSpatie + 1).toInt();

      if (snelheidPercent >= 0 && snelheidPercent <= 100 &&
          versnellingPercent >= 0 && versnellingPercent <= 100 &&
          rotaties > 0) {

        doelSnelheid = (snelheidPercent / 100.0) * maxSnelheid;
        doelVersnelling = (versnellingPercent / 100.0) * maxVersnelling;

        digitalWrite(ENABLE_PIN, HIGH);
        motor.setMaxSpeed(doelSnelheid);
        motor.setAcceleration(doelVersnelling);
        motor.move(rotaties * stappenPerOmwenteling);

        wachtOpInvoer = false;
        motorBezig = false;

        Serial.print("Motor draait ");
        Serial.print(rotaties);
        Serial.print(" rotaties op ");
        Serial.print(snelheidPercent);
        Serial.print("% snelheid (");
        Serial.print(doelSnelheid);
        Serial.print(") met ");
        Serial.print(versnellingPercent);
        Serial.print("% versnelling (");
        Serial.print(doelVersnelling);
        Serial.println(").");
        return;
      }
    }

    Serial.println("⚠️ Ongeldige invoer! Gebruik bijv: 50 75 10 of typ 'S' voor noodstop.");
    return;
  }

  if (!wachtOpInvoer) {
    motor.run();
    if (!motorBezig) {
      Serial.println("Motor aan het draaien...");
      motorBezig = true;
    }

    if (motor.distanceToGo() == 0) {
      Serial.println("\n✅ Motor gestopt. Voer nieuwe snelheid%, versnelling% en rotaties in:");
      wachtOpInvoer = true;
      motorBezig = false;
      Serial.println("Motor in rust.");
    }
  }
}
