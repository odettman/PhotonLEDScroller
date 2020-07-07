
#include <FastLED.h>
#include <Custom_SSD1306.h>

FASTLED_USING_NAMESPACE;

/*  File: ssd1306_LEDMessage
 *  Author: O. Dettman
 *  Original Date: 2020-06-14
 *  Revised Date: 2020-07-06
 *  
 *  Summary:
 *      Displays some basic information on a 128x96 OLED:
 *          Current Time (corrected for time zone - not auto updating)
 *          Current IP Address
 *  
 *      Also displays a user-specified message
 *          This message is specified by calling the ledMessage method with a String variable argument
 *          
 *      The ledMessage method is exposed to the Particle environment. This code is connected to IFTTT.
 *      The following phrases, spoken to Google Assistant, will call the ledMessage method and change the displayed text:
 *          Set LED text to $
 *          Set LED Message text to $
 *          Scroll LED Message $
 * 
 *      The application also subscribes to the PRIVATE event "change_led_message"
 *          If a change_led_event message is published, the device retrieves the message and displays it on the OLED
 *
 *
 *  CHANGES
 *      2020-06-19  Removed references to IFTTT using the ledMessageText function. All IFTTT traffic goes through the publish event.
 *                  Function is still available to call to change an individual device message.
 *      2020-07-06  Integrated FastLED to light up a string of 30 WS2812B LEDs when then message changes
*/

// Rquired by the FastLED Library
#define LED_PIN     5
#define NUM_LEDS    256
CRGB leds[NUM_LEDS];

// Initialize objects from the lib
Custom_SSD1306 oled;

String ledMessageText; // = "No Message Set";
IPAddress localip;

void setup() {
  // FastLED initiation
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical  
  
  oled.begin();
  oled.clearDisplay();
  oled.display();
  oled.drawCenteredString(32, "Initializing");
  oled.display();

  // Expose a variable and a function to the Particle management console interface.
  //    By calling this function with a string argument, an individual device's message can be changed.
  //    The variable stores the currently displayed messgage.
  Particle.function("ledMessage", ledMessage);
  Particle.variable("ledMessageText", ledMessageText);
  
  // Subscribe to private events
  // All particle devices running this code will display the same message as specified in the Particle event 'change_led_message'
  Particle.subscribe("change_led_message", ledChangeMessage, MY_DEVICES);
  
  // Set up local time.
  // Not perfect. No automated DST updates - definitely needs some work.
  Time.zone(-5);
  Time.setDSTOffset(1.0);
  Time.beginDST();
  
  while (!Particle.connected()) {
      delay(1000);
  }
  
  // Switch below lines depending on WiFi vs Cellular connection method
  localip = WiFi.localIP();
  // localip = Cellular.localIP();
  
  oled.clearDisplay();
  oled.display();
    
}

void loop() {
  String ipAddress = localip.toString();
  
  String time = Time.format("%Y-%m-%d %H:%M:%S");
  oled.drawCenteredString(0,time);
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.drawCenteredString(16,"LED DISPLAY");
  oled.drawCenteredString(26,ipAddress);
  oled.setCursor(0,32);
  oled.println();
  oled.println(ledMessageText);
  oled.display();
  delay(50);
  
  //leds[5] = CRGB::Red;
  //FastLED.show();
  //delay(500);
  //leds[5] = CRGB::Black;
  //FastLED.show();
  //delay(500);
}

void ledUpdateRoutine() {
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i]=CRGB::Blue;
        FastLED.show();
        // delay(20);
    }
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i]=CRGB::Black;
        FastLED.show();
        // delay(20);
    }
    
}

int ledMessage(String messagetext) {
    ledMessageText = messagetext;
    oled.clearDisplay();
    // Switch below lines depending on WiFi vs Cellular connection method
    localip = WiFi.localIP();
    // localip = Cellular.localIP();
    
    ledUpdateRoutine();
}

void ledChangeMessage(String event, String data) {
    // TODO:    Insert some code here to process "off" statement
    //          The code should look for the "off" statement and send a blank screen to the LEDs
    //              and show "OFF" on the OLED.
    ledMessage(data);
}


