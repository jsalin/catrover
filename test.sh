#
# Test rotation of motors by full 360 degrees in different directions using the cgi-bin over HTTP
#
wget -O - "http://localhost/cgi-bin/motor?deg=360&dir1=0&dir2=1"
