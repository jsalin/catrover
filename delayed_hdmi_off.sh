#
# Add to /etc/rc.local, so user has 3 minutes to kill this if he/she wants
# to do fix something locally. After that HDMI is turned off to save power.
#

sleep 180
/opt/vc/bin/tvservice -o
