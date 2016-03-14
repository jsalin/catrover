#
# Get public IP address using external service to /tmp/public_ip file
#

curl -s http://whatismijnip.nl | cut -d " " -f 5 > /tmp/public_ip
