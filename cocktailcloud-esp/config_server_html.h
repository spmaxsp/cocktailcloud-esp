#include <Arduino.h>

const char PAGE_NetworkConfiguration[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
    .card {
        margin: 3%;
        padding: 2%;
        justify-content: center;
        box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2);
        transition: 0.3s;
        width: 90%;
        border-radius: 5px;
    }

    .button {
        position: relative;
        background-color: #9258ad;
        border: none;
        font-size: 28px;
        color: #FFFFFF;
        padding: 20px;
        width: 200px;
        text-align: center;
        transition-duration: 0.4s;
        text-decoration: none;
        overflow: hidden;
        cursor: pointer;
    }

    .button:after {
        content: "";
        background: #f1f1f1;
        display: block;
        position: absolute;
        padding-top: 300%;
        padding-left: 350%;
        margin-left: -20px !important;
        margin-top: -120%;
        opacity: 0;
        transition: all 0.8s
    }

    .button:active:after {
        padding: 0;
        margin: 0;
        opacity: 1;
        transition: 0s
    }

    input[type=text], select {
        width: 100%;
        padding: 12px 20px;
        margin: 8px 0;
        display: inline-block;
        border: 1px solid #ccc;
        border-radius: 4px;
        box-sizing: border-box;
    }

</style>
</head>
<body>

    <div class="card">
    <h2>Current Configuration</h2>
        <p>SSID: <span id="ssid"></span></p>
        <p>Password: **********</p>
        <p>SocketIO Server: <span id="socketio_server"></span></p>
    </div>

    <div class="card">
    <h2>Wifi Status</h2>
        <p WIFI: <span id="wifi_status"></span></p>
        <p>IP: <span id="ip"></span></p>
    </div>

    <div class="card">
    <h2>Set WIFI</h2>
        <form action="/set_wifi" method="post">
            <label for="ssid_input">SSID:</label>
            <input type="text" id="ssid_input" name="ssid_input" required>
            <label for="password_input">Password:</label>
            <input type="text" id="password_input" name="password_input" required>
        </form>
        <button class="button" onclick="setWifi()">Set WIFI</button>

    <h2>Set SocketIO Server</h2>
        <form action="/set_socketio_server" method="post">
            <label for="socketio_server_input">SocketIO Server:</label>
            <input type="text" id="socketio_server_input" name="socketio_server_input" required>
        </form>
        <button class="button" onclick="setSocketIO()">Set SocketIO</button>
    </div>

    <div class="card">
    <h2>Actions</h2>
        <button class="button" onclick="restart()">Restart</button>
    </div>

    <script>
        function setWifi() {
            var ssid = document.getElementById("ssid_input").value;
            var password = document.getElementById("password_input").value;

            var xhr = new XMLHttpRequest();
            xhr.open("POST", "/set_wifi", true);
            xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            xhr.send("ssid=" + ssid + "&password=" + password);
        }

        function setSocketIO() {
            var socketio_server = document.getElementById("socketio_server_input").value;

            var xhr = new XMLHttpRequest();
            xhr.open("POST", "/set_socketio", true);
            xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            xhr.send("socketio_server=" + socketio_server);
        }

        function restart() {
            var xhr = new XMLHttpRequest();
            xhr.open("POST", "/reboot", true);
            xhr.send();
        }

        function updateStatus() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/get_status", true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    var response = JSON.parse(xhr.responseText);
                    document.getElementById("wifi_status").innerHTML = response.status;
                    document.getElementById("ip").innerHTML = response.ip;
                }
            }
            xhr.send();
        }

        function updateConfig() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/get_config", true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    var response = JSON.parse(xhr.responseText);
                    document.getElementById("ssid").innerHTML = response.ssid;
                    document.getElementById("socketio_server").innerHTML = response.socketio_server;
                }
            }
            xhr.send();
        }

        updateStatus();
        updateConfig();
    </script>
</body>
</html>
)=====";