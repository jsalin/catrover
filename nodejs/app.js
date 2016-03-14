//
// Cat Rover backend, node.js version
// Copyright (C) 2015 Jussi Salin <salinjus@gmail.com>
//
// See config.js file to set up a username and password for the user.
// Run in background to serve node.js version of the Cat Rover web gui.
// Execute with "node app.js &"
// The catservice background service made in C is still required - this
// backend communicates to it with temporary files just like the cgi-bin
// backend does.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// Configuration settings come from config.js file
var config = require('./config');

// Use Express as web server, and basic-auth-connect for authentication
var express = require('express');
var app = express();
var basicAuth = require('basic-auth-connect');
app.use(basicAuth(config.username, config.password));
var http = require("http");
var server = http.createServer(app);

// Serve all static files under www folder, but we can add paths over
// it to set up some web api functions and dynamic pages
app.use(express["static"](__dirname + "/www"));

// Filesystem access and external process execution
var fs = require('fs');
var sys = require('sys');
var exec = require('child_process').exec;

// "IP" package for getting default IP address of the server
var ip = require("ip");

// Use Morgan for logging requests
var morgan = require('morgan');
app.use(morgan('combined'));

// Simple template engine (replace some parts of HTML page with variables
// coming from this backend)
app.engine('ntl', function (filePath, options, callback) {
  fs.readFile(filePath, function (err, content) {
    if (err) return callback(new Error(err));
    var rendered = content.toString();
    // Replace all occurrances of the template variables
    rendered = rendered.replace(new RegExp('#rover_ip#', 'g'), options.rover_ip);
    return callback(null, rendered);
  })
});
app.set('views', './views'); // specify the views directory
app.set('view engine', 'ntl'); // register the template engine

// Event for "motor" api call
app.get('/motor', function (req, res) {
  fs.writeFile("/tmp/cat_motor", "deg="+req.query.deg+"&dir1="+req.query.dir1+"&dir2="+req.query.dir2);
  res.send();
});

// Event for "light_on" api call
app.get('/light_on', function (req, res) {
  fs.writeFile("/tmp/cat_light_on", "");
  res.send();
});

// Event for "light_off" api call
app.get('/light_off', function (req, res) {
  fs.writeFile("/tmp/cat_light_off", "");
  res.send();
});

// Event for "laser_on" api call
app.get('/laser_on', function (req, res) {
  fs.writeFile("/tmp/cat_laser_on", "");
  res.send();
});

// Event for "laser_off" api call
app.get('/laser_off', function (req, res) {
  fs.writeFile("/tmp/cat_laser_off", "");
  res.send();
});

// Event for "audio_on" api call
app.get('/audio_on', function (req, res) {
  fs.writeFile("/tmp/cat_audio_on", "");
  res.send();
});

// Event for "audio_off" api call
app.get('/audio_off', function (req, res) {
  fs.writeFile("/tmp/cat_audio_off", "");
  res.send();
});

// Event for "camera_on" api call
app.get('/camera_on', function (req, res) {
  fs.writeFile("/tmp/cat_camera_on", "");
  res.send();
});

// Event for "camera_off" api call
app.get('/camera_off', function (req, res) {
  fs.writeFile("/tmp/cat_camera_off", "");
  res.send();
});

// Event for "speak" api call. Call Festival speech synthesizer directly.
app.get('/speak', function (req, res) {
  // Escape quotes, because they could be used for injection
  msg = req.query.msg.replace(/\"/g,'\\"');
  exec('flite -t "' + msg + '"');
  res.send();
});

// Event for loading Index page
app.get('/', function (req, res) {
  // Handled by template engine because we need to pass variables
  res.render('index', {rover_ip: public_ip});
});

// Get public IP
try {
  public_ip = fs.readFileSync("/tmp/public_ip", "utf8").replace(/\n$/, '');
} catch (ex) {
  public_ip = ip.address();
}

// Start server
server.listen(config.port, function () {
  console.log('Listening at http://' + public_ip + ":" + config.port + "/");
});
