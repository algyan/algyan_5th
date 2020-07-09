#include <Servo.h>
#include <WiFi.h>

// Pin define
#define PIN_LED     12
#define PIN_SW1     35
#define PIN_CDS     34
#define PIN_SERVO   13
#define PIN_BUZZAR  14

#define BUZZAR_LEN  5 // Buzzar length (20 x 5 = 100msec)
Servo servo;
#define AVERAGE_BUF_SIZE 32
int deg_average_buf[AVERAGE_BUF_SIZE];
int deg_average_pos = 0;

void setup() {
  int i, cnt;

  // Setup Serial
  Serial.begin(115200);

  // Setup Pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SW1, INPUT);
  pinMode(PIN_CDS, INPUT);
  pinMode(PIN_BUZZAR, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_BUZZAR, LOW);

  // Setup Servo
  servo.attach(PIN_SERVO);
  for (i = 0; i<AVERAGE_BUF_SIZE; i++) deg_average_buf[i] = 90;

  // Scan Wifi Access Points
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Scanning WiFi ...");
  cnt = WiFi.scanNetworks();
  for (i = 0; i<cnt; i++)
  {
    Serial.print(i + 1);
    Serial.print(") \"");
    Serial.print(WiFi.SSID(i));
    Serial.print("\" rssi=");
    Serial.println(WiFi.RSSI(i));
    delay(10);
  }
  Serial.print(cnt);
  Serial.println(" WiFi AP found");
}

void loop() {
  static bool onoff = false;
  static int sw_before = false;
  static int timer_cnt = 0;
  int deg, deg_sum, adval;
  int sw;
  int i;

  // Reading Switch
  sw = digitalRead(PIN_SW1);
  if ((sw != sw_before) && sw) {
    onoff = (onoff)? false: true;
    timer_cnt = BUZZAR_LEN; // Push-SW Buzzar
  }
  sw_before = sw;

  // Reading Light Sensor
  adval = analogRead(PIN_CDS);

  // Set Light Servo
  deg = map(adval, 0, 4095, 0, 180);
  deg = max(deg, 0);
  deg = min(deg, 180);

  // Buffering for Average
  deg_average_buf[deg_average_pos] = deg;
  if (deg_average_pos < AVERAGE_BUF_SIZE-1) deg_average_pos++; else deg_average_pos = 0;

  // Calc average
  for (i = 0, deg_sum = 0; i<AVERAGE_BUF_SIZE; i++) deg_sum += deg_average_buf[i];
  deg = deg_sum / AVERAGE_BUF_SIZE;

  // output values to Serial for graph
  if (onoff) {
    Serial.print(adval); Serial.print(" "); Serial.println(deg);
  }
  servo.write(deg);

  if (onoff)
  {
    digitalWrite(PIN_LED, HIGH);  // LED on
  }
  else
  {
    digitalWrite(PIN_LED, LOW);   // LED off
  }

  // Buzzar On when key Pressed
  if (timer_cnt>0) 
  {
    digitalWrite(PIN_BUZZAR, HIGH);
    timer_cnt--;
  }
  else
  {
    digitalWrite(PIN_BUZZAR, LOW);
  }

  delay(20);
}
