#include <AccelStepper.h>

#define STEP_PIN 5
#define DIR_PIN 17
#define ENABLE_PIN 4

const int stappenPerOmwenteling = 400;
const int maxSnelheid = 4000;    // MAX SNELHEID
const int maxVersnelling = 4000; // MAX VERSNELLING

AccelStepper motor(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

bool wachtOpInvoer = true;
int doelSnelheid = 0;
int doelVersnelling = 1000; // Standaard versnelling

void setup() {
  Serial.begin(9600);
  Serial.println("Voer snelheid (0 - 4000) en versnelling (1 - 4000) in, gescheiden door een spatie:");

  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);

  motor.setMaxSpeed(maxSnelheid);
}

void loop() {
  if (wachtOpInvoer) {
    if (Serial.available() > 0) {
      int nieuweSnelheid = Serial.parseInt();
      int nieuweVersnelling = Serial.parseInt();

      // Controle op geldige invoer
      if (Serial.available() > 0 || nieuweSnelheid == 0 || nieuweVersnelling == 0) {
        Serial.println("Ongeldige invoer! Voer een snelheid (0-4000) en versnelling (1-4000) in:");
        return;
      }

      if (nieuweSnelheid == 0) {
        // **Motor uitzetten**
        digitalWrite(ENABLE_PIN, LOW);
        motor.stop();
        Serial.println("Motor uitgeschakeld. Voer een nieuwe snelheid en versnelling in:");
        return;
      }

      if (nieuweSnelheid > 0 && nieuweSnelheid <= maxSnelheid && nieuweVersnelling > 0 && nieuweVersnelling <= maxVersnelling) {
        doelSnelheid = nieuweSnelheid;
        doelVersnelling = nieuweVersnelling;

        digitalWrite(ENABLE_PIN, HIGH); // **Motor aanzetten**
        motor.setMaxSpeed(doelSnelheid);
        motor.setAcceleration(doelVersnelling);
        motor.move(stappenPerOmwenteling * 50);

        wachtOpInvoer = false;

        Serial.print("Motor draait op snelheid: ");
        Serial.print(doelSnelheid);
        Serial.print(" met versnelling: ");
        Serial.println(doelVersnelling);
      } else {
        Serial.println("Ongeldige invoer! Voer een snelheid (0-4000) en versnelling (1-4000) in:");
      }
    }
  } else {
    motor.run();
    if (motor.distanceToGo() == 0) {
      Serial.println("\nMotor gestopt. Voer een nieuwe snelheid en versnelling in:");
      wachtOpInvoer = true;
      Serial.println("Motor in rust.");
    } else {
      Serial.println("Motor aan het draaien...");
    }
  }
}
