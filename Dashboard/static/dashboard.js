// Arduino Robot Control Dashboard - dashboard.js
// William Garrity
// Last modified 5/6/2025

// This provides Javascript capabilites for the dashboard, including
// continuous updating of sensor variables

// Slider variables
// from https://www.w3schools.com/howto/howto_js_rangeslider.asp

var left_speed_slider = document.getElementById("left-speed");
var right_speed_slider = document.getElementById("right-speed");

var left_speed_output = document.getElementById("left-speed-out");
var right_speed_output = document.getElementById("right-speed-out");

// display default slider outputs
left_speed_output.innerHTML = left_speed_slider.value;
right_speed_output.innerHTML = right_speed_slider.value;


// variables for control buttons
var forward_button = document.getElementById("forward-button");
var backward_button = document.getElementById("backward-button");
var left_button = document.getElementById("left-button");
var right_button = document.getElementById("right-button");
var stop_button = document.getElementById("stop-button");

// updates slider values upon input

left_speed_slider.onchange = function() {

    fetch("write/motor/speed/left/" + this.value);
};

left_speed_slider.oninput = function() {
    left_speed_output.innerHTML = left_speed_slider.value;
    fetch("write/motor/speed/left/" + this.value);
};


right_speed_slider.onchange = function() {
    // right_offset_output.innerHTML = this.value;
    //right_speed_output.innerHTML = right_speed_slider.value;
    fetch("write/motor/speed/right/" + this.value);
};

right_speed_slider.oninput = function() {
    right_speed_output.innerHTML = right_speed_slider.value;
    fetch("write/motor/speed/left/" + this.value);
};

// button on input functions
forward_button.onclick = function () {
    fetch("write/motor/direction/forward");
};

backward_button.onclick = function () {
    fetch("write/motor/direction/backward");
};

left_button.onclick = function () {
    fetch("write/motor/direction/left");
};

right_button.onclick = function () {
    fetch("write/motor/direction/right");
};

stop_button.onclick = function () {
    fetch("write/motor/stop");
};



// https://www.w3docs.com/snippets/javascript/how-to-make-http-get-request-in-javascript.html

/* async function fillTheTitle() {
        const post = await fetch("https://jsonplaceholder.typicode.com/posts/1").then((res) => res.json());
        document.getElementById("spanId").innerText = post.title;
      } */

// gets robot data every second
async function getData() {
    
    // get all readings
    var motor_status = await fetch("read/motor/all")
        .then((response) => response.json());
    
    var left_motor_speed = await fetch("read/motor/left")
        .then((response) => response.json());
    
    var right_motor_speed = await fetch("read/motor/right")
        .then((response) => response.json());
    
    var front_sonar = await fetch("read/sonar/0")
        .then((response) => response.json());
    
    var back_sonar = await fetch("read/sonar/1")
        .then((response) => response.json());
    
    var left_sonar = await fetch("read/sonar/2")
        .then((response) => response.json());
    
    var right_sonar = await fetch("read/sonar/3")
        .then((response) => response.json());
    
    // set values
    document.getElementById("motor-status").innerHTML = motor_status.reading;
    document.getElementById("left-speed-out").innerHTML = left_motor_speed.reading;
    document.getElementById("left-speed").value = left_motor_speed.reading;
    document.getElementById("right-speed-out").innerHTML = right_motor_speed.reading;
    document.getElementById("right-speed").value = right_motor_speed.reading;
    document.getElementById("front-sonar-reading").innerHTML = front_sonar.reading + " in";
    document.getElementById("back-sonar-reading").innerHTML = back_sonar.reading + " in";
    document.getElementById("left-sonar-reading").innerHTML = left_sonar.reading + " in";
    document.getElementById("right-sonar-reading").innerHTML = right_sonar.reading + " in";
    
}

getData();

const interval = setInterval(getData, 1000);

//clearInterval(interval);