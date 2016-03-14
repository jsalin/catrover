#
# Startup script, to be executed as root from /etc/rc.local
#

# Turn light and laser off at startup because the GPIOs might be in 3 state
touch /tmp/cat_light_off
touch /tmp/cat_laser_off

# Start the background service
/usr/local/sbin/catservice &

# Start the web backend
cd /home/pi/catrover/nodejs
sudo -u pi node app.js &

# Turn off HDMI after a delay to save power (user can cancel by killing)
/usr/local/sbin/delayed_hdmi_off.sh &

# Inform that startup is complete
flite -t ready &
