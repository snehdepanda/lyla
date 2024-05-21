// #include <Arduino.h>
// #include <LiquidCrystal_I2C.h>

// // set the LCD number of columns and rows
// int lcdColumns = 16;
// int lcdRows = 2;

// // set LCD address, number of columns and rows
// // if you don't know your display address, run an I2C scanner sketch
// LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

// void setup(){
//   // sda, scl
//   Wire.setPins(41, 42);
//   // initialize LCD
//   lcd.init();
//   // turn on LCD backlight                      
//   Serial.println("LCD initialized");
// }

// void loop(){
//   lcd.backlight();
//   // set cursor to first column, first row
//   lcd.setCursor(0, 0);
//   // print message
//   lcd.print("Hello, World!");
//   delay(1000);
//   // clears the display to print new message
//   lcd.clear();
//   lcd.noBacklight();
//   // set cursor to first column, second row
//   lcd.setCursor(0,1);
//   lcd.print("Hello, World!");
//   delay(1000);
//   lcd.clear(); 
// }