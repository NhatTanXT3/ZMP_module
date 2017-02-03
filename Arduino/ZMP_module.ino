
#include "PID.h"
#include "IR_sensor.h"
#include "Communication.h"



/*====================================
          Biến chương trình,
          phục vụ truyền nhận lệnh
    ======================================*/
struct {
  unsigned char run: 1;
  unsigned char display: 1;
  unsigned char test_motor: 1;
} flag;
unsigned char run_mode = 1;

char command[100];
unsigned char command_index = 0;
boolean stringComplete = false;  // whether the string is complete


/*=====================================
            biến motor và encoder
    =====================================*/
const int LED = 13; //pin for LED
int ledState = LOW;

/*=====================================
            biến cho timer
    =====================================*/
unsigned long currentMicros;
unsigned long previousMicros_100Hz = 0;        // will store last time it was called for 100Hz timer
const long interval_100Hz = 10000;           // interval at which to do 100Hz task(micros)

unsigned long previousMicros_50Hz = 0;        // will store last time it was called for 50Hz timer
const long interval_50Hz = 20000;           // interval at which to do 50Hz task (micros)

unsigned long previousMicros_500Hz = 0;       // will store last time it was called for 50Hz timer
const long interval_500Hz = 2000;           // interval at which to do 500Hz task (micros)

/*=====================================
            biến cho ADC sensor
    =====================================*/
int adc_sensor[10];
float adc_sensor_filter[10];
float adc_sensor_offset[10];


/*=====================================
            biến cho ZMP
    =====================================*/
#define ZMP_POSITION_0_X -4
#define ZMP_POSITION_0_Y -10

#define ZMP_POSITION_1_X 4
#define ZMP_POSITION_1_Y -10

#define ZMP_POSITION_2_X 4
#define ZMP_POSITION_2_Y 10

#define ZMP_POSITION_3_X -4
#define ZMP_POSITION_3_Y 10

float zmp_position_x;
float zmp_position_y;

float zmp_amplitude;

/*=====================================
             Setup phần cứng
    =====================================*/
void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);  // chân xuất xung để test

  Serial.println("Done");
  //	while(Spring_calib());
  //while(Loadcell_calib());
}
/*==============================================
             Chương trình chính
             Task 500Hz: đọc cảm biến và lọc
             Task 100Hz:  hiễn thị dữ liệu
             Task 50Hz:
    ===============================================*/
unsigned char flag_force_control = 0;
void loop() {
  serialEvent();
  currentMicros = micros();
  //your code begin from here

  if (currentMicros - previousMicros_500Hz >= interval_500Hz)
  {
    previousMicros_500Hz = currentMicros;
    //your code begin from here
    Sensor_update();
  }

  if (currentMicros - previousMicros_100Hz >= interval_100Hz)
  {
    previousMicros_100Hz = currentMicros;
    //your code begin from here
    if (flag.display)
    {
      display();
    }
  }

  if (currentMicros - previousMicros_50Hz >= interval_50Hz)
  {
    previousMicros_50Hz = currentMicros;
    //your code begin from here

    ledState ^= 1;
    digitalWrite(LED, ledState);
  }
}//loop

/*============================================
         Chương trình hiễn thị dữ liệu
    =============================================*/
void display() {
  //your code start from here
  Serial.write(CN_1);
  Serial.print(adc_sensor[0]);
  Serial.write(CN_2);
  Serial.print(adc_sensor[1]);
  Serial.write(CN_3);
  Serial.print(adc_sensor[2]);
  Serial.write(CN_4);
  Serial.print(adc_sensor[3]);
  Serial.write(CN_5);
  Serial.print(zmp_position_x);
  Serial.write(CN_6);
  Serial.print(zmp_position_y);
  Serial.write(CN_7);
  Serial.println(zmp_amplitude);
  //	Serial.write(CN_8);e
  //	Serial.println(position_setpoint);
  // Serial.write(CN_9);
  // Serial.println(as_filter[5]);


  //  Serial1.print(xlt_filter);
  //  Serial1.write('a');
  //  Serial1.println(PID_x.output);


}
/*============================================
         Chương trình đọc và xữ lý cám biến
    =============================================*/
#define  ADC_SENSOR_NOISE_RANGE  10
unsigned char Sensor_calib() {
  unsigned long sum = 0;
  unsigned int numOfsamples = 50;

  unsigned int buffer;
  unsigned int max_value;
  unsigned int min_value;
  unsigned char i, j;


  buffer = analogRead(ANALOG_CHANNEL_1);
  max_value = buffer;
  min_value = buffer;

  for (i = 1; i <= numOfsamples; i++)
  {
    buffer = analogRead(ANALOG_CHANNEL_1);
    if (buffer > max_value) max_value = buffer;
    else if (buffer < min_value)  min_value = buffer;
    if ((max_value - min_value) >= ADC_SENSOR_NOISE_RANGE)
    {
      Serial.println("error sensor calib");
      Serial.println(max_value - min_value);
      return 1;
    }

    // code sum
    sum += buffer;
    delay(10);
  }


  adc_sensor_offset[0] = sum / numOfsamples;

  Serial.print(" adc sensor offset: ");
  Serial.println(adc_sensor_offset[0]);
  return 0;

}


void Sensor_update() {
  adc_sensor[0] = (analogRead(ANALOG_CHANNEL_1) + adc_sensor_offset[0]);
  adc_sensor[1] = (analogRead(ANALOG_CHANNEL_2) + adc_sensor_offset[1]);
  adc_sensor[2] = (analogRead(ANALOG_CHANNEL_3) + adc_sensor_offset[2]);
  adc_sensor[3] = (analogRead(ANALOG_CHANNEL_4) + adc_sensor_offset[3]);


  adc_sensor_filter[0] = adc_sensor_filter[0] * 0.8 + adc_sensor[0] * 0.2;
  adc_sensor_filter[1] = adc_sensor_filter[1] * 0.8 + adc_sensor[1] * 0.2;
  adc_sensor_filter[2] = adc_sensor_filter[2] * 0.8 + adc_sensor[2] * 0.2;
  adc_sensor_filter[3] = adc_sensor_filter[3] * 0.8 + adc_sensor[3] * 0.2;
  //  zmp_position=(adc_sensor_filter[0]*ZMP_POSITION_1+adc_sensor_filter[1]*ZMP_POSITION_2)/(adc_sensor_filter[0]+adc_sensor_filter[1]);
  zmp_amplitude = adc_sensor_filter[0] + adc_sensor_filter[1] + adc_sensor_filter[2] + adc_sensor_filter[3];

  if (zmp_amplitude < 4)
  {
    zmp_position_x = 0;
    zmp_position_y = 0;
  }
  else
  {
    zmp_position_x = (adc_sensor_filter[0] * ZMP_POSITION_0_X + adc_sensor_filter[1] * ZMP_POSITION_1_X + adc_sensor_filter[2] * ZMP_POSITION_2_X + adc_sensor_filter[3] * ZMP_POSITION_3_X) / zmp_amplitude;
    zmp_position_y = (adc_sensor_filter[0] * ZMP_POSITION_0_Y + adc_sensor_filter[1] * ZMP_POSITION_1_Y + adc_sensor_filter[2] * ZMP_POSITION_2_Y + adc_sensor_filter[3] * ZMP_POSITION_3_Y) / zmp_amplitude;
  }
}//adcSensor_update


/*============================================
             Chương trình giao tiếp, nhận
             dữ liệu từ máy tính, được
             gọi giữa mỗi loop, cho nên
             hạn chế sử dụng delay trong
             để không mất dữ liệu đọc được
    ==============================================*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    command[command_index] = inChar;
    command_index++;

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }

  if (stringComplete) {
    Serial.print(command);
    switch (command[0])
    {
      case DISPLAY_ON_:
        flag.display = 1;
        break;
      case DISPLAY_OFF_:
        flag.display = 0;
        break;
      default:
        break;
    }
    memset(command, 0, sizeof(command));
    command_index = 0;
    // clear the string:
    stringComplete = false;
  }
}
