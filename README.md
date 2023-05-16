A gate system, ideally used for car parks, that requires authorization via an RFID card or tag to enter. While idle, the LCD module displays some generic weather and humidity information, though after being activated by an approaching object, it displays how many free spaces are left and some other useful information during the authorization process, such as if it fails, succeeds or requires additional details.

The project uses:
1. STM32F407G-DISC1 - the brain of the project, containing all of the logic for an efficient and resposive functioning;
2. DHT11 temperature and humidity sensor - reads and outputs information about the weather (temperature and humidity);
3. HC-SR04 ultrasonic distance sensor - detects if an object approaches the system and at the required distance activates the system to either ask for additional information required for authorization or simply opens the gate;
4. MFRC522 RFID scanner module - used for authorization. Detects and reads an RFID card or tag and if the ID matches in the dataase, grants permission to open the gate. Otherwise, blocks any attempts for entry;
5. 28BYJ-48 step motor - used for the physical implementation of the gate. Works in half drive mode, so it allows for an even more custom opening angle.
6. LCD1602 LCD screen module - used to display various output messages for clearer flow.

While most of the modules and sensors are fully capable of functioning while connected to the STM's 5V power line, the 28BYJ-48 step motor requires an independent power source, which was provided via a Arduino module, though an independed power source such as a battery would be much more portable and efficient.

There's also quite a lot of room for improvements. For example, I've lacked the module for the LCD1602, that would allow communications via I2C interface, also I believe some more efficient delays can be implemented to allow greater responsiveness of the system. 
