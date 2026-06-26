 #include <LiquidCrystal.h>
#include <Servo.h>

const int TRIG_PIN= 7;
const int ECHO_PIN= 6;
const int SERVO_PIN= 8;
const int BUZZER_PIN = 10;
const int LED_PIN= 13;


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo radarServo;

const int SEUIL_DETECTION= 30;  
const int VITESSE_SERVO= 15;  
const int ANGLE_MIN= 0;
const int ANGLE_MAX=180;

int  angle= ANGLE_MIN;
int  direction= 1;           
bool alerteActive=false;

float mesurerDistance() {
  digitalWrite(TRIG_PIN,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duree = pulseIn(ECHO_PIN, HIGH, 30000); 
  if (duree == 0) return 999;                 
  return duree*0.0343 / 2.0;
}

void afficherLCD(int ang, float dist) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Angle: ");
  lcd.print(ang);
  lcd.print((char)223);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  if (dist >= 999) {
    lcd.print("Dist: ---  cm");
  } else {
    lcd.print("Dist: ");
    lcd.print((int)dist);
    lcd.print(" cm    ");
  }
}

void gererAlarme(bool activer) {
  alerteActive = activer;
  if (activer) {
    tone(BUZZER_PIN, 1000);   
    digitalWrite(LED_PIN, HIGH);
  } else {
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, LOW);
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN,   OUTPUT);
  pinMode(ECHO_PIN,   INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN,    OUTPUT);

  radarServo.attach(SERVO_PIN);
  radarServo.write(ANGLE_MIN);

  lcd.begin(16, 2);
  lcd.print("  RADAR ACTIF   ");
  lcd.setCursor(0, 1);
  lcd.print("  Initialise... ");
  delay(1500);
}

void loop() {
  angle += direction;
  if (angle >= ANGLE_MAX) direction = -1;
  if (angle <= ANGLE_MIN) direction =  1;
  radarServo.write(angle);

  delay(VITESSE_SERVO); 
  float distance = mesurerDistance();

  afficherLCD(angle, distance);

  if (distance < SEUIL_DETECTION && distance > 2) {
    gererAlarme(true);
  } else {
    gererAlarme(false);
  }


  Serial.print("Angle:"); Serial.print(angle);
  Serial.print("°  |  Distance: "); Serial.print(distance);
  Serial.println(" cm");
}
