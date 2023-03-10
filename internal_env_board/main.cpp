#include "Arduino.h"
// #include <teensy_uavcan.hpp>
// #include "publisher.hpp"
#include "running_average.hpp"
// #include <uavcanNodeIDs.h>
#include <watchdog.h>
#include <Metro.h>
#include <Wire.h>
#include "lcd_functions.h"
#include "sensor_functions.h"
#include "FlexCAN.h"
#include "kinetis_flexcan.h"


static CAN_message_t msg;


// UAVCAN application settings
static constexpr float framerate = 100;
extern Running_Average<uint32_t, SAMPLES_PER_SECOND> avg_pressure;
extern Running_Average<float, SAMPLES_PER_SECOND> avg_temperature;
extern Running_Average<float, SAMPLES_PER_SECOND> avg_humidity;

#include "batteryStatus.hpp"
// #include "subscriber.hpp"
// #include <teensy_uavcan.hpp>

// UAVCAN Node settings
// static constexpr uint32_t nodeID = UAVCAN_NODE_ID_INTERNAL_SENSOR_BOARD;
static constexpr uint8_t swVersion = 1;
static constexpr uint8_t hwVersion = 1;
// static const char *nodeName = "org.arvp.internalSensor";

// instantiate the timer for reading values/publishing message
// interval in milliseconds
Metro timer = Metro(1000/SAMPLES_PER_SECOND);
uint8_t timerCounter;
uint16_t lcdCounter = 0;
boolean lcdMode = false;

const int LED_PIN = 13;


void setup() {
    Can0.begin(500000);
    Wire.begin();

    pinMode(LED_PIN, OUTPUT);

    // initLeds();

    // ensure counter starts at 0
    timerCounter = 0;

    Serial.begin(9600);
    delay(1000);
    Serial.println("Setup start");

    setupMPL();

    // setup_lcd();
    
    // Create a node
    // systemClock = &initSystemClock();
    // canDriver = &initCanDriver();
    // node = new Node<NodeMemoryPoolSize>(*canDriver, *systemClock);
    // initNode(node, nodeID, nodeName, swVersion, hwVersion);

	//init subscriber
	// initSubscriber(node);
	
    // init publisher
    // initPublisher(node);

    // start up node
    // configureCanAcceptanceFilters(*node);
    // node->setModeOperational();
    Serial.println("Setup complete"); 

      msg.id = 8;
        msg.ext = 1;

        msg.len = 8;

        msg.buf[0] = 0x01;
        msg.buf[1] = 0x02;
        msg.buf[2] = 0x03;
        msg.buf[3] = 0x04;
        msg.buf[4] = 0x05;
        msg.buf[5] = 0x06;
        msg.buf[6] = 0x07;
        msg.buf[7] = 0x08;
    
}

void loop() {
      
      digitalWrite(LED_PIN, HIGH);

      
    KickDog();
    if(timer.check() == 1) {
      
      timerCounter++;
      // read humidity and temperature
      measureHIH7120();
      avg_humidity.AddSample(humidity());
      avg_temperature.AddSample(temp());

      // read pressure
      readPressureMPL();
      avg_pressure.AddSample(pressure());

      // publish once every second
      if (timerCounter == SAMPLES_PER_SECOND) {
        // cyclePublisher();
        Serial.println("Reading...");
        Serial.print("Humidity: ");
        Serial.println(avg_humidity.Average());
        Serial.print("Temperature: ");
        Serial.println(avg_temperature.Average());
        Serial.print("Pressure: ");
        Serial.println(avg_pressure.Average());
		
        //Print out battery voltages

        Serial.print("Battery 1: ");
        Serial.println(batteryVoltage[0]);
        Serial.print("Battery 2: ");
        Serial.println(batteryVoltage[1]);
        Serial.print("Battery 3: ");
        Serial.println(batteryVoltage[2]);
        Serial.print("Battery 4: ");
        Serial.println(batteryVoltage[3]);

        // lcdCounter++;

        // if(lcdCounter%6 == 0){
        //     lcdMode = !lcdMode;
        // }

        //Alternate data displayed to LCD every six seconds
        // if(lcdMode == true){
        //     display_to_lcd(avg_temperature.Average(), avg_humidity.Average(), avg_pressure.Average());
        // } 
        // else {
        //     display_voltages_to_lcd(batteryVoltage[0], batteryVoltage[1], batteryVoltage[2], batteryVoltage[3]);
        // }


        timerCounter = 0;

        if(Can0.write(msg)){
          Serial.println(".");
        }  
        
      }
    }

    //--UAVCAN cycles--//
    // wait in cycle
    // cycleWait(framerate);

    // do some CAN stuff
    // cycleNode(node);

    // toggle heartbeat
    // toggleHeartBeat();
}
