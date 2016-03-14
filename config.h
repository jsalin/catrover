// Raspberry Pi GPIO numbering
#define DIR_PIN1 23	// Pin 16
#define DIR_PIN2 25	// Pin 22
#define STEP_PIN1 24	// Pin 18
#define STEP_PIN2 8	// Pin 24
#define ENABLE_PIN1 14	// Pin 8
#define ENABLE_PIN2 15	// Pin 10
#define LASER_PIN 2     // GPIO pin to control laser
#define LIGHT_PIN 3     // GPIO pin to control light
//
//  2  4  6  8  10 12 14 16 18 20 22 24 26
//  o  o  o  o  o  o  o  o  o  o  o  o  o
//  o  o  o  o  o  o  o  o  o  o  o  o  o
//  1  3  5  7  9  11 13 15 17 19 21 23 25
//

// Fine tune for motor, these are for ST28
#define STEPS_PER_DEGREE 4096/360
#define STEP_INTERVAL 800	// usec of one step

#define CAMERA_TIME 900000	// ms to wait till turning the camera off
#define LASER_TIME 30000	// ms to wait till turning off the laser
#define LIGHT_TIME 300000	// ms to wait till turning off the light
#define AUDIO_TIME 900000	// ms to wait till turning off audio

#define POLLING_INTERVAL 20	// how of then (in ms) to poll for flags of what to do
#define FILE_OPEN_DELAY 40	// how many ms to wait for flag file to finish writing

// Commands executed to perform different actions on external programs
#define CAMERA_ON_CMD "mjpg_streamer -i \"/usr/local/lib/input_uvc.so -r 640x360 -f 10\" -o \"/usr/local/lib/output_http.so -p 8081\""
#define CAMERA_OFF_CMD "killall mjpg_streamer"
#define AUDIO_ON_CMD "darkice"
#define AUDIO_OFF_CMD "killall darkice"

