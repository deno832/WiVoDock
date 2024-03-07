#include <ESP8266WiFi.h>
#include <ezButton.h>  // the library to use for SW pin
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define CLK_PIN D5  // ESP8266 pin D7 connected to the rotary encoder's CLK pin
#define DT_PIN D6   // ESP8266 pin D6 connected to the rotary encoder's DT pin
#define SW_PIN D7 // ESP8266 pin D5 connected to the rotary encoder's SW pin
#define LED_PIN D8

#define DIRECTION_CW 0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction

int counter = 32;
int direction = DIRECTION_CW;
int CLK_state;
int prev_CLK_state;

#define PREV_PIN D3
#define NEXT_PIN D4

ezButton button(SW_PIN);  // create ezButton object that attach to pin 7;
ezButton nextButton(NEXT_PIN);
ezButton prevButton(PREV_PIN);

const char* ssid = "Zyxel_A421"; // WiFi ağının adı
const char* password = "my1977**"; // WiFi şifresi

WiFiServer server(80); // TCP bağlantıları için sunucu nesnesi, 80 numaralı port üzerinden iletişim sağlanacak

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  
  display.setTextSize(2);
  
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(22,0);
  display.println(F("deno832"));

  display.setCursor(18,20);
  display.println(F("Deniz E."));

  display.setCursor(18,40);
  display.println(F("YILDIRIM"));
  
  display.display();
  delay(3000);
  
  Serial.begin(115200);
  delay(10);

  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  button.setDebounceTime(50);  // set debounce time to 50 milliseconds
  nextButton.setDebounceTime(50);
  prevButton.setDebounceTime(50);
  
  // read the initial state of the rotary encoder's CLK pin
  prev_CLK_state = digitalRead(CLK_PIN);
  
  // WiFi ağına bağlanma
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  display.clearDisplay();
  
  display.setCursor(0,17);
  display.setTextSize(2);
  display.println(F("Connecting..."));
  
  display.display();

  
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, HIGH);
    delay(70);
    Serial.print(".");
    digitalWrite(LED_PIN, LOW);
    delay(70);
  }

  display.clearDisplay();
  
  display.setCursor(25,0);
  display.setTextSize(1);
  display.println(F("Connected to:"));
  
  display.setCursor(25,8);
  display.println(ssid);
  
  display.setCursor(25,18);
  display.println(F("IP Adress is:"));
  
  display.setCursor(25,26);
  display.println(WiFi.localIP());

  display.setCursor(0,40);
  display.println(F("Waiting for client..."));

  display.setCursor(0,55);
  display.println(F("Yes,texts are so tiny"));
  
  display.display();

  digitalWrite(LED_PIN, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  
  // IP adresini görüntüleme
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // Sunucuyu başlatma
  server.begin();
  Serial.println("Server started");
}

void loop() {
  
  
  WiFiClient client = server.available(); // Bağlantıları kabul etme

  if (client) {
    Serial.println("New client connected"); // Yeni bir istemci bağlandı

    display.clearDisplay();
  
    display.setCursor(0,9);
    display.setTextSize(7);
    display.println(F("OK!"));

    display.display();
    
    while (client.connected()) {
      button.loop();
      nextButton.loop();
      prevButton.loop();
      
      CLK_state = digitalRead(CLK_PIN);
      if (CLK_state != prev_CLK_state && CLK_state == HIGH) {
        if (digitalRead(DT_PIN) == HIGH) {
          if (counter > 0){
            counter -= 4;
          }
          
          direction = DIRECTION_CCW;
          client.println(counter);
          Serial.println("CCW");
        } else {
          // the encoder is rotating in clockwise direction => increase the counter
          if (counter < 100){
            counter += 4;
          }
          direction = DIRECTION_CW;
          client.println(counter);
          Serial.println("CW");
        }
      }
      prev_CLK_state = CLK_state;
      if (client.available()) { // İstemciden gelen veri varsa
        char c = client.read(); // Veriyi oku
        Serial.println(c); // Seri monitöre yazdır
        // İstemciye gelen veriyi geri gönderme
        client.write(c);
      }
      
      if (button.isPressed()) {
      client.println("1234");
      }
    
      if (nextButton.isPressed()) {
      client.println("1235");
      }

      if (prevButton.isPressed()) {
      client.println("1236");
      }
    }

    // Bağlantıyı kapat
    client.stop();
    Serial.println("Client disconnected");

    display.clearDisplay();
  
    display.setCursor(25,0);
    display.setTextSize(1);
    display.println(F("Connected to:"));
    
    display.setCursor(25,8);
    display.println(ssid);
    
    display.setCursor(25,18);
    display.println(F("IP Adress is:"));
    
    display.setCursor(25,26);
    display.println(WiFi.localIP());
  
    display.setCursor(0,40);
    display.println(F("Waiting for client..."));
  
    display.setCursor(0,55);
    display.println(F("Yes,texts are so tiny"));
    
    display.display();

    
  }else{
    digitalWrite(LED_PIN, LOW);
    delay(350);
    digitalWrite(LED_PIN, HIGH);
    delay(350);
  }
}
