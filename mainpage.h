const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<TITLE>
HOME AUTOMATION
</TITLE>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
body{
	background-color: rgb(61,72,95);
	font-size: 16px;
	-webkit-user-select: none; /* Safari 3.1+ */
	-moz-user-select: none; /* Firefox 2+ */
	-ms-user-select: none; /* IE 10+ */
	user-select: none; /* Standard syntax */
}
#id1{
	font-family: century gothic; font-size: 4.5em;text-align: center;
	color:rgb(255,255,255);
	text-shadow:0 0 2px white;
	padding: 8px;
	border-radius:5px;
	margin: auto;
	max-width: 100%;
	background-color: rgb(255,128,0);
}
sup{
	font-size: 0.5em;
}
#id2,#id3{
	padding: 6px;
	border-radius:5px;
	width:7em;
}
#id2{
	background-color: rgba(0,0,0,0.8);
	background-clip:padding-box;
	color: white;
}
#id3{
	background-color: rgb(200,200,200);
	color: black;
	display:block;
	height:3em;
	justify-content:center;
	-webkit-transition-duration: 0.4s; 
	-moz-transition-duration: 0.4s; 
	transition-duration: 0.4s;
}
#spd{
	background-color: #000;
	color: #fff;
	padding: 5px;
	border-radius:8px;
}
ul{
	width:100%;
	list-style-type:none;
	font-size: 2.5em;
	font-family: calibri;
	overflow: hidden;
	margin: 8px auto;
	padding: 8px;
}
li{
	float: left;
	margin: 10px;
	text-align: center;
	padding: 8px 5px;
}
.table{
	display:table;
	margin: 0 auto;
}
input,.slider{
	position:relative;
	background-color: rgb(230,230,230);	
	-webkit-appearance:none;
	-moz-appearance:none;
	outline:none;
	border-radius: 34px;
	background: #99994d;
	transition:.4s;
	margin:auto;
	opacity:0.8;
}
input:hover{
	opacity:1;
	cursor:pointer;
}
input:disabled{
	cursor:not-allowed;
}
input[type=range]{
	width:200px;height:8px;
}
input[type=checkbox]{
  width:80px;height: 25px;
}
input:before{
	content:'';
	position:absolute;
	background: white;
	transition:.4s;
}
input[type="checkbox"]:before{
	width:35px;height:35px;
	border-radius: 50%;
	top: -5px;
	left:-1px;
	bottom: 5px;
}
.slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 20px;
  height: 40px;
  background: #ffffff;
  border-radius: 34px;
}
.slider::-moz-range-thumb {
  width: 20px;
  height: 40px;
  background: #ffffff;
  border-radius: 34px;
}
input:checked[type=checkbox]:before{
	left:50px;
}
@media only screen and (max-width: 600px) {
	ul{
		font-size: 2.2em;
	}
	#id1{
		font-size: 3em;
		width:8em;
	}
	#id2,#id3{
		width: 6em;
	}
}
@media only screen and (max-width: 992px) {
	ul{
		font-size: 2.4em;
	}
	#id1{
		font-size: 3.2em;
		width: 18em;
	}
	#id2,#id3{
		width: 6.5em;
	}
}
</style>
<script>
function ajaxwork(url, servfunc) {// Generalised ajax function to handle multiple function requests
  var xhttp;
  xhttp=new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      servfunc(this);
    }
  };
  xhttp.open("GET", url, true);
  xhttp.send();
}
//Get temperature value by AJAX::::::::::::::::::
setInterval(function(){ajaxwork("readTemphu",getdhtdata);},2000); //Get temp-humdity value update every 2s interval
function getdhtdata(xhttp) {
  document.getElementById("tempval").innerHTML = xhttp.responseText;
}
//Send FAN speed value by AJAX:::::::::::::::::::
function setspeed(s) {
  var Speed="altspd?speedval="+s.value;
  ajaxwork(Speed,getspeed);
  function getspeed(xhttp){
      document.getElementById("spd").innerHTML = xhttp.responseText;
    }
}
//Get door status by AJAX::::::::::::::::::::::::
setInterval(function(){ajaxwork("doorstate",doorstat);},500); //Call a function repetatively with 0.5 Second interval
function doorstat(xhttp){
      document.getElementById("door").innerHTML = xhttp.responseText;
}
//Get fire alarm value by AJAX:::::::::::::::::::
setInterval(function(){ajaxwork("fireAlert",flamesensor);}, 200); //Call a function repetatively with 0.5 Second interval
function flamesensor(xhttp) {
      document.getElementById("firealrt").innerHTML = xhttp.responseText;
}
//Get tank status by AJAX::::::::::::::::::::::::
setInterval(function(){ajaxwork("tankstate",tankstatus);}, 500);   //Call a function repetatively with 0.5 Second interval
function tankstatus(xhttp){
        var str = xhttp.responseText;
        var arr = str.split("@");
        document.getElementById("tank").innerHTML =arr[0];
        document.getElementById("tankst").innerHTML =arr[1];
    if(arr[0].includes("Full")){
		var y=document.getElementById("pumpsw");
		y.checked=false;check(y);y.disabled=true;
	  }
    else
      document.getElementById("pumpsw").disabled=false;
	}
//Get led status by AJAX:::::::::::::::::::::::::
function ledonoff(xhttp){
	document.getElementById("lightst").innerHTML =xhttp.responseText;
}
//Get tank status by AJAX::::::::::::::::::::::::
function pumponoff(xhttp){
	document.getElementById("tankst").innerHTML =xhttp.responseText;
}
// function for on-off switches=============
function tog_onoff(x){
	var ledurl="togled?ledval=",pumpurl="togpmp?pmpval=";
		if(x.checked==true){
			var on=x.getAttribute("name");
			if(on == "led")
				ajaxwork(String(ledurl+"ON"),ledonoff);
			else if(on == "pump")
				ajaxwork(String(pumpurl+"ON"),pumponoff);
				check(x);
		}
		else{
			var off=x.getAttribute("name");
			if(off == "led")
				ajaxwork(String(ledurl+"OFF"),ledonoff);
			else if(off == "pump")
				ajaxwork(String(pumpurl+"OFF"),pumponoff);
				check(x);
		}
	}
//function for styling buttons==============
function check(x){
	if(x.checked==true){
			x.parentNode.style.backgroundColor="#359aff";
			x.parentNode.style.color="#ffffff";
			x.parentNode.style.textShadow="0 0 2px white";
		}
	else{
			x.parentNode.style.backgroundColor="rgb(200,200,200)";
			x.parentNode.style.color="#000000";
			x.parentNode.style.textShadow="none";
	}
}
</script>
</head>
<BODY>
<div id="id1">Home Automation System</div></br>
<div class="table"><ul>
<li id="id2"><a id="tempval"></a></li>
<li id="id2">Door:<span id="door"></span></li>
<li id="id2">Tank:<span id="tank"></span></li>
<li id="id2">System:<span id="firealrt"></span></li>
</ul></div>
<div class="table"><ul>
<li><!===================================load 1><div id="id3"><label>LIGHT:<a id="lightst"></a></label><br>
<input type="checkbox" id="lightsw" class="chkbox" name="led" onclick="tog_onoff(this)">
</div>
</li>
<li><!===================================Pump><div id="id3"><label>MOTOR:<a id="tankst"></a></label><br>
<input type="checkbox" id="pumpsw" class="chkbox" name="pump" onclick="tog_onoff(this)">
</div>
</li>
<li><!===================================fan><div id="id3"><label>FAN SPEED:<span id="spd">0</span></label><br>
<input type="range" min="0" max="50"value="0" id="myRange" class="slider" onchange="setspeed(this)">
</div>
</li>
</ul></div>
</BODY>
</HTML>
)=====";
