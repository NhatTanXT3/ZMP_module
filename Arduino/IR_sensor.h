#ifndef IR_SENSOR_
#define IR_SENSOR_

#define ANALOG_CHANNEL_1  A0  // Analog input pin that the potentiometer is attached to
#define ANALOG_CHANNEL_2  A1 
#define ANALOG_CHANNEL_3  A2 
#define ANALOG_CHANNEL_4  A3  
//#define SPRING_CHANNEL  A0



#define MIN_A0  80//38
#define MIN_A1  50//34
#define MIN_A2  50//34
#define MIN_A3  50//34
#define MIN_A4  50//37
#define MIN_A5  50//37
#define MIN_A6  80//36

//#define MAX_A0  800
//#define MAX_A1
//#define MAX_A2
//#define MAX_A3
//#define MAX_A4
//#define MAX_A5
//#define MAX_A6
//
#define constrain_lower(amt,low) (amt<low?low:amt)
void irSensor_update();


#endif
