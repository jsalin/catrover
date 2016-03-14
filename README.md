Cat Rover
=========

Software for a custom Raspberry Pi based three wheeled droid which has a webcam, laser pointer and light source. It is controlled over network using a web browser. The droid also has a charger dock. 3D designs are at <http://www.thingiverse.com/thing:1252462>.
Microphone of the usb webcam can be listened to over a stream and user can use a text-to-speech synthesizer to speak back.

The web UI was originally created as a static html page and cgi-bins which are still included, but then moved to node.js when more api calls became necessary as well as some dynamic variable on the html page. Performance did not seem to suffer at all. There is also a deeper backend service made in C, which controls some features that have to be ran as root or which are time critical.

Copyright (C) 2015 Jussi Salin <salinjus@gmail.com> under GPLv3 license.

The status of the project is fully working, including the hardware with 3D parts uploaded to Thingiverse. I moved however to a new hardware & software design based on sturdier platform and motors, available on eBay, and Arduino and Bluetooth for superior power savings compared to RPi and WiFi. The newer parts and source are also available under project called Cat Rover 2.


Performance
-----------

Webcam and audio can be turned on and off from the UI. This seems to help saving energy when the camera goes to standby mode when not used. RPi does not seem to have enough speed to run mumble voice chat, and Ventrillo and Teamspeak are not supported. Instead the mic is listened to over mp2 stream over icecast.

With default configuration the webcam consumes about 300-400kB/s bandwidth and the audio about 13kB/s, so a standard 11M Wi-Fi with a 10M Internet connections is enough for lag free operation. The webcam picture lags a few hundred of ms only. Audio lags about 4 seconds because of buffering. Controls are only tens of ms.

There is no need to overclock RPi and even the first model is enough. It should be a model with two USB ports (one for camera, one for wi-fi), unless you configure for the official RPi camera that doesn't have a mic but doesn't occupy an usb port, or if you use a small hub to connect two devices to just one port. About 50% of CPU time is used on first generation RPi when having both webcam and audio streaming on while controlling the droid.

The webcam and audio stream are on separate ports so they are unfortunately referred to by absolute URLs in the web GUI. I thought of proxying them over Apache for simplifity and more security, but the proxying module adds way too much latency to the streams.


Security
--------

Webcam and audio stream are insecure and over http only. Other approach would at least add more latency (https->http proxying over Apache) or ruin the performance and battery life (more advanced daemons that have integrated encryption.)

SSH is used for making configuration changes remotely, so change the password of the "pi" user. Username and password of the web UI can be changed in config.js file.


Installation
------------

Install dependencies to freshly installed Raspbian (I might be missing some on the list).

    sudo apt-get install npm nodejs gcc make icecast2 darkice git

Compile and install mjpg-streamer, which is the lightest motion jpeg webcam http service.

    git clone https://github.com/jacksonliam/mjpg-streamer.git

Run 'make' to compile and run 'make install' to install Cat Rover software. Node.js backend is not installed because it is ran from the directory directly.

Use alsamixer to configure webcam for maximum capture volume. Icecast's default configuration is ok. Configure darkice so that it can connect to the icecast stream and encodes using mpeg2 encoder (light) and using the webcam's microphone as input. To acomplish this, see the sample configuration files in this project. It would be also a good idea to adjust the output volume for playing sounds and text-to-speech.

Add the startup script to /etc/rc.local:

    /usr/local/sbin/start_catrover

It is a good idea to configure the ethernet port so that you can use it for changing Wi-Fi connection when needed, so you don't need a HDMI monitor and USB keyboard for that. Configure Wi-Fi so that it is connected to the network of the location where the droid is used at. Make port redirections to TCP 80, 8000 and 8081 to your router, if you want to access the droid over Internet. Don't forget to change the username and password!

Connect to WPA2 Wi-Fi network by first generating a passphrase with `wpa_passphrase <my ssid> <my wpa2 key>`. Then copy the generated psk variable and put following lines to /etc/network/intefaces file (replace any existing default wlan0 configuration):

    auto wlan0
    iface wlan0 inet dhcp
        wpa-ssid <my ssid>
        wpa-psk <the generated psk key>

Reboot at the end to see that all works after power-on.


Tips
----

To compile mjpg-streamer on Raspbian at the time of writing, you first have to `sudo scp -rvp /usr/include/linux/videodev2.h /usr/include/linux/videodev.h`

You can use `cmdmotor` to test and use the motors from command line.


Hardware
--------

Connect two stepper motors with drivers to the GPIO pins. Connect optional laser and light source LEDs to GPIO pins also, using driving transistors if necessary. Also, connect an amplified speaker to raspberry's 3.5mm audio jack.

    Purpose                     GPIO number     Header pin number
    Motor 1 direction           23              16
    Motor 2 direction           25              22
    Step motor 1                24              18
    Step motor 2                8               24
    Enable motor 1              14              8
    Enable motor 2              15              10
    Laser enable                2               3
    Light enable                3               5
    +3.3V                       -               1 and 17
    GND                         -               6, 9 and 25

    2  4  6  8  10 12 14 16 18 20 22 24 26
    o  o  o  o  o  o  o  o  o  o  o  o  o
    o  o  o  o  o  o  o  o  o  o  o  o  o
    1  3  5  7  9  11 13 15 17 19 21 23 25

You can tweak the delay between pulses in config.h, and the amount of pulses needed for full rotation for the particular motor. Defaults are for ST28 motors.

Obiviously you need also a battery for the stepper motors (12V) and a power source for Raspberry Pi. Circuit board of a car USB charger will do nicely.

Connection of A4988/SP stepper motor board:

    VMOT o      o EN
    GND  o      o MS1
    2B   o      o MS2
    2A   o      o MS3
    1A   o      o RST
    1B   o      o SLP
    VDD  o      o STEP
    GND  o      o DIR

Connect VMOT and the following GND to +12V power source. 2B goes to orange wire of ST28 motor. 2A goes to pink, 1A to red and 2B to blue. VDD goes to +3.3V of Raspberry Pi and GND to the GND of Raspberry. EN, STEP and DIR connected to GPIO pins. RST and SLP are connected together by a jumper. MS1 to MS3 are unused.
