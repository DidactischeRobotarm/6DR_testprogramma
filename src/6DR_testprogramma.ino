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
  Serial.println("Voer snelheid en versnelling in als percentage (0-100), gescheiden door een spatie:");
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

    int sepIndex = input.indexOf(' ');
    if (sepIndex > 0) {
      int snelheidPercent = input.substring(0, sepIndex).toInt();
      int versnellingPercent = input.substring(sepIndex + 1).toInt();

      if (snelheidPercent == 0) {
        digitalWrite(ENABLE_PIN, LOW);
        motor.stop();
        Serial.println("Motor uitgeschakeld. Voer een nieuwe snelheid en versnelling in:");
        wachtOpInvoer = true;
        motorBezig = false;
        return;
      }

      if (snelheidPercent >= 0 && snelheidPercent <= 100 &&
          versnellingPercent >= 0 && versnellingPercent <= 100) {

        // Percentages omzetten naar absolute waarden
        doelSnelheid = (snelheidPercent / 100.0) * maxSnelheid;
        doelVersnelling = (versnellingPercent / 100.0) * maxVersnelling;

        digitalWrite(ENABLE_PIN, HIGH);
        motor.setMaxSpeed(doelSnelheid);
        motor.setAcceleration(doelVersnelling);
        motor.move(stappenPerOmwenteling * 50);

        wachtOpInvoer = false;
        motorBezig = false;

        Serial.print("Motor draait op ");
        Serial.print(snelheidPercent);
        Serial.print("% (");
        Serial.print(doelSnelheid);
        Serial.print(") snelheid met ");
        Serial.print(versnellingPercent);
        Serial.print("% (");
        Serial.print(doelVersnelling);
        Serial.println(") versnelling.");
        return;
      }
    }

    Serial.println("⚠️ Ongeldige invoer! Geef snelheid en versnelling in % (0–100) of typ 'S' om te stoppen.");
    return;
  }

  if (!wachtOpInvoer) {
    motor.run();
    if (!motorBezig) {
      Serial.println("Motor aan het draaien...");
      motorBezig = true;
    }

    if (motor.distanceToGo() == 0) {
      Serial.println("\n✅ Motor gestopt. Voer een nieuwe snelheid en versnelling in:");
      wachtOpInvoer = true;
      motorBezig = false;
      Serial.println("Motor in rust.");
    }
  }
}
