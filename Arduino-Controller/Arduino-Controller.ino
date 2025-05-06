// Arduino-Controller.ino
// William Garrity
// Last modified 5/6/2026

// This is the Arduino code for the Robot that interfaces with the XBee
// Controller and the Python Flask Server

// input constants
const int M0ON = 2;
const int M1ON = 3;
const int M0CTLA = 22;
const int M0CTLB = 23;
const int M1CTLA = 24;
const int M1CTLB = 25;

const int RED = 4;
const int GREEN = 5;
const int BLUE = 6;

// motor constants
const int LMOTOR = 0;
const int RMOTOR = 1;

// sonar constants
const int TRIG = 30;
const int FRONTECHO = 26;
const int LEFTECHO = 28;
const int BACKECHO = 27;
const int RIGHTECHO = 29;

const int SONAR_COLL_LIMIT = 2;


// variables for robot status
int wall_detected = 0;
int collision_detected = 0;
int wall_following_state = 0;
int front_sonar = 1;
int back_sonar = 2;
int left_sonar = 3;
int right_sonar = 4;
int front_sensor = 1001;
int back_sensor = 1002;
int left_motor_speed = 100;
int right_motor_speed = 100;

// States
// 0: Off
// 1: Forward
// 2: Backward
// 3: Left
// 4: Right
int motor_state = 0;


// byte buffers
byte input_buffer[4];

void setup() {
  // put your setup code here, to run once:

  pinMode(M0ON, OUTPUT);
  pinMode(M1ON, OUTPUT);
  pinMode(M0CTLA, OUTPUT);
  pinMode(M0CTLB, OUTPUT);
  pinMode(M1CTLA, OUTPUT);
  pinMode(M1CTLB, OUTPUT);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(FRONTECHO, INPUT);
  pinMode(LEFTECHO, INPUT);
  pinMode(BACKECHO, INPUT);
  pinMode(RIGHTECHO, INPUT);

  Serial.begin(9600);
  Serial3.begin(57600);

  // inital state is to have the robot off
  stop();
  
}

void loop() {
  // put your main code here, to run repeatedly:

  // check for near collisions, stop robot if a collision is about to happen
  // only check if going forwards or backwards
  /* if (motor_state == 1 && front_sonar <= SONAR_COLL_LIMIT){
    motor_state = 0;
  } else if (motor_state == 2 && back_sonar <= SONAR_COLL_LIMIT) {
    motor_state = 0;
  } */

  switch (motor_state) {
    case 0: // motor stop
      stop();
      break;
    case 1: // go forward
      go_forward();
      break;
    case 2: // go backward
      go_backward();
      break;
  }

  if (Serial3.available() > 0) {
    Serial3.readBytes(input_buffer, 4);

    switch (input_buffer[0]) {
      // read instructions
      case 0x60:
        front_sonar = read_sonar(0);
        send_data(front_sonar);
        break;
      case 0x61:
        back_sonar = read_sonar(1);
        send_data(back_sonar);
        break;
      case 0x62:
        left_sonar = read_sonar(2);
        send_data(left_sonar);
        break;
      case 0x63:
        right_sonar = read_sonar(3);
        send_data(right_sonar);
        break;
      case 0x20:
        send_data(motor_state);
        break;
      case 0x21:
        send_data(right_motor_speed);
        break;
      case 0x22:
        send_data(left_motor_speed);
        break;
      case 0x01:
        send_data(collision_detected);
        break;
      
      // write instructions
      case 0xA0: // write motor left speed
        left_motor_speed = input_buffer[1];
        update_speed();
        break;
      case 0xA1: // write motor right speed
        right_motor_speed = input_buffer[1];
        update_speed();
        break;
      case 0x88: // go forward
        stop();
        motor_state = 1;
        break;
      case 0x84: // go backward
        stop();
        motor_state = 2;
        break;
      case 0x82: // go left
        go_ccw(250);
        break;
      case 0x81: // go right
        go_cw(250);
        break;
      case 0xB0: // motor stop
        motor_state = 0;
        break;
      case 0xFF: // rest
        Serial.println("Reset");

    }

    // Serial.print("Input: ");
    // Serial.println(c);

    // if (c = 'c') {
    //   for (int i = 0; i < 8; i++) {
    //     values[i]++;
    //     Serial3.write(values[i]);
    //     Serial.write(values[i]);
    //   }
    //   Serial3.print('\n');
    // } else {
    //   Serial.print(c);
    //   Serial3.print(c);
    // }
    
  }


}



// send data function
// will send an integer out of Serial3 in the following format
// Bytes [Length] + [Integer]
void send_data(int input) {

  Serial3.write(sizeof(input));
  Serial3.write(highByte(input));
  Serial3.write(lowByte(input));

}



// read sonar sensor
long read_sonar(int sensor) {

  int sensor_echo = 0;
  long duration;
  long inches;

  switch (sensor) {
    case 0: // front
      sensor_echo = FRONTECHO;
      break;
    case 1: // = back
      sensor_echo = BACKECHO;
      break;
    case 2: // left 
      sensor_echo = LEFTECHO;
      break;
    case 3: // right
      sensor_echo = RIGHTECHO;
      break;
  }

  // send LOW pulse to ensure a clean HIGH pulse
  // then give a 10 millisecond HIGH pulse
  digitalWrite(TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // read signal from sensor
  duration = pulseIn(sensor_echo, HIGH);

  // convert time to distance in inches
  inches = (duration/2) / 74; 

  Serial.print(sensor);
  Serial.print(" : ");
  Serial.println(inches);

  return inches;

}



/* Control Functions */

void update_speed() {

  if (motor_state == 1 || motor_state == 2) {
    set_speed(LMOTOR, left_motor_speed);
    set_speed(RMOTOR, right_motor_speed);
  }
}

// stop the robot
void stop() {

  set_speed(LMOTOR, 0);
  set_speed(RMOTOR, 0);

  delay(100);

}


void go_forward() {
  

  set_forward(LMOTOR);
  set_forward(RMOTOR);

  set_speed(LMOTOR, left_motor_speed);
  set_speed(RMOTOR, right_motor_speed);

}


void go_backward() {

  set_backward(LMOTOR);
  set_backward(RMOTOR);

  set_speed(LMOTOR, left_motor_speed);
  set_speed(RMOTOR, right_motor_speed);

}


// go counter-clockwise
void go_ccw(int time) {

  stop();

  int prior_motor_state = motor_state;

  motor_state = 3;

  set_ccw();
  set_speed(LMOTOR, 150);
  set_speed(RMOTOR, 150);
  delay(time);

  stop();

  motor_state = prior_motor_state;

}


// go clockwise
void go_cw(int time) {

  stop();

  int prior_motor_state = motor_state;

  motor_state = 4;

  set_cw();
  set_speed(LMOTOR, 150);
  set_speed(RMOTOR, 150);

  delay(time);
  
  stop();

  motor_state = prior_motor_state;

}


// set counter clockwise
void set_ccw() {

  set_forward(RMOTOR);
  set_backward(LMOTOR);

}

// set clockwise
void set_cw() {

  set_forward(LMOTOR);
  set_backward(RMOTOR);

}



// move the motor forward
void set_forward(int motor) {

  if (motor == LMOTOR) {
    digitalWrite(M0CTLA, 1);
    digitalWrite(M0CTLB, 0);
  } else if (motor == RMOTOR) {
    digitalWrite(M1CTLA, 1);
    digitalWrite(M1CTLB, 0);
  }

}


// move the motor backwards
void set_backward(int motor) {

  if (motor == LMOTOR) {
    digitalWrite(M0CTLA, 0);
    digitalWrite(M0CTLB, 1);
  } else if (motor == RMOTOR) {
    digitalWrite(M1CTLA, 0);
    digitalWrite(M1CTLB, 1);
  }

}


// set motor speed
void set_speed(int motor, int speed) {

  if (motor == LMOTOR) {
    analogWrite(M0ON, speed);
  } else if (motor == RMOTOR) {
    analogWrite(M1ON, speed);
  }

}



// turn the motor on
void motor_on(int motor) {

  if (motor == LMOTOR){
    digitalWrite(M0ON, 1);
  } else if (motor == RMOTOR) {
    digitalWrite(M1ON, 1);
  }

}


// turn the motor on
void motor_off(int motor) {

  if (motor == LMOTOR){
    digitalWrite(M0ON, 0);
  } else if (motor == RMOTOR) {
    digitalWrite(M1ON, 0);
  }

}