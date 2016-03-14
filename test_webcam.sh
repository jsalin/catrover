#
# Test bandwidth of webcam stream and that it is working
# Press ^C just once and wait to exit
#
sudo mjpg_streamer -b -i "/usr/local/lib/input_uvc.so -r 640x360 -f 10" -o "/usr/local/lib/output_http.so -p 8081"
wget -O /dev/null http://localhost:8081/?action=stream
sudo killall mjpg_streamer
