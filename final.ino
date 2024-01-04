#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <EEPROM.h>

const char *ssid = "Galaxya54";
const char *password = "nonenone";
int highestserverscore = 0;
ESP8266WebServer server(80);
WebSocketsServer chatSocket = WebSocketsServer(81);
WebSocketsServer imageSocket = WebSocketsServer(82);
WebSocketsServer gameSocket = WebSocketsServer(83);

const char *chatHtmlCode = R"=====(
<html>
<head><title>Anonymouse</title>

<style>
    h1{
        text-align: center;
        font-size: larger;
        font-family: 'Franklin Gothic Medium', 'Arial Narrow', Arial, sans-serif;
    }
    marquee{
        text-align: center;
        font-family: 'Times New Roman', Times, serif;
    }
    body{
        background: linear-gradient(to top, yellow, red);
    }
    #container{
        background-color: white;
        border-radius: 25px;
        height: 70vh;
        padding: 30px;
        margin: 25px;
        overflow: hidden;
    }
    #console{
        float: left;
        height: 65%;
        width: 85%;
        resize: none;
        border: 2px solid purple;
    }
    #sender{
        padding-top: 10px;
        display: flex;
        float: right;
    }
    #tx{
        outline: none;
        border: none;
        border-bottom: 1px solid;
    }
    a{
        float: left;
        display: block;
    }
    #read{
        display: flex;
        flex-direction: column;
        float: left;
    }
    footer{
        position: absolute;
        bottom: 0px;
        text-align: right;
        width: 100%;
        float:right;
    }

</style>
</head>
<body onload="javascript:init()">
    <h1>Anonymouse ChatRoom</h1>
    <marquee scrollamount ="20">Anonymouse is a simple chat room simulation and not a public use product. While chatting please maintain decency. Its not even secured 🥲</marquee>
    <div id = "container"><textarea id="console"></textarea>
    <div id="sender"><input type="text" id="tx" placeholder="Chat With others">
    <button onclick="javascript:sendText()">Send</button></div>
    <div id ="read">Rules of The ChatServer
        <ul>
            <li>This website is crappy so please dont worry it may not always work.</li>
            <li>New messages appear at the bottom of TextArea so if they dont appear scroll down.</li>
            <li>Thodi Maryaada Rakhiye</li>
        </ul>
        <a href="/image" target="_blank">Flash Image</a>
        <a href="/game" target="_blank">Play Game</a></div>
</div>
    <footer>Hello server v1.0.0</footer>
   
    <script>
    var Socket;
    function init() {
        Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
        Socket.onmessage = function (e) {
            document.getElementById("console").value += "Someone: "+e.data + "\n";
        }
    }
    function sendText() {
        Socket.send(document.getElementById("tx").value);
        document.getElementById("tx").value = '';
    }
</script>

</body>
</html>
)=====";

const char *imageHtmlCode = R"=====(
<html>
<head>
    <title>Image Upload</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-T3c6CoIi6uLrA9TneNEoa7RxnatzjcDSCmG1MXxSR1GAsXEV/Dwwykc2MPK8M2HN" crossorigin="anonymous">
<script>
    window.onload = function () {
        const form = document.getElementById('uploadForm');
        const imageElement = document.getElementById('uploadedImage');
        const imageSocket = new WebSocket('ws://' + window.location.hostname + ':82/image');

        const CHUNK_SIZE = 1024; // Adjust the chunk size as needed

        if (form && imageElement) {
            form.addEventListener('submit', (event) => {
                event.preventDefault();
                const formData = new FormData(form);

                const reader = new FileReader();
                reader.onload = function (e) {
                    const base64Img = e.target.result.split(',')[1];

                    // Send the base64 string in chunks
                    for (let i = 0; i < base64Img.length; i += CHUNK_SIZE) {
                        const chunk = base64Img.slice(i, i + CHUNK_SIZE);
                        imageSocket.send(chunk);
                    }

                    // Signal the end of transmission
                    imageSocket.send('EOF');
                };

                if (formData.get('image')) {
                    reader.readAsDataURL(formData.get('image'));
                }
            });

            imageSocket.addEventListener('message', (event) => {
                if (event.data === 'EOF') {
                    // End of transmission, update the image
                    imageElement.src = 'data:image/jpeg;base64,' + receivedChunks.join('');
                    receivedChunks = [];
                } else {
                    // Accumulate received chunks
                    receivedChunks.push(event.data);
                }
            });

            let receivedChunks = [];

            // Retrieve the last uploaded image from the server on page load
            imageSocket.send('REQUEST_LAST_IMAGE');
        }
    };
</script>
<style>
    body{
        background:linear-gradient(to bottom, yellow, white);
        /* display: flex;
        flex-direction: column; */
    }
    h2{
        text-align: center;
    }
    h6{
        text-align: center;
    }
    #contianer{
        display: flex;
        flex-direction:column ;
        justify-content: center;
        align-items: center;
    }
    form{
        border: 2px solid purple;
        display: inline-flex;
        align-items: center;
        justify-content: center;
        padding: 25px;
    }
    img{
        border: 1px solid navy;
    }
</style>
</head>
<body>
    <h2>Flash Image</h2>
    <h6>Make any image visible to people who have this page open at the upload time</h6>
    <div id="contianer"><form enctype="multipart/form-data" id="uploadForm">
        <input type="file" name="image" accept="image/*">
        <input type="submit" value="Upload">
    </form>
    <br>
    <img id="uploadedImage" style="max-width: 100%;" alt="Uploaded Image"></div>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-T3c6CoIi6uLrA9TneNEoa7RxnatzjcDSCmG1MXxSR1GAsXEV/Dwwykc2MPK8M2HN" crossorigin="anonymous">

</body>
</html>
)=====";
const char *game = R"=====(
<html>
    <head>
        <meta charset="UTF-8">
        <meta name="viewport", content="width=device-width, initial-scale=1.0">
        <title>Flappy Bird</title>
        <style>
            body {
                font-family: 'Courier New', Courier, monospace;
                text-align: center;
            }  

            #board {
                background-color: skyblue;
            }
        </style>
    </head>
    <body onload="javascript:init()">
        <canvas id="board" height="500px" width="400px"></canvas>
        <div id="rules">To play game press space or upArrow or click anywhere</div>
        <script>
            var Socket;
            let bird = {
                x: 50,
                y: 50,
                width: 50,
                height: 30,
            };

            let pipeArray = [];
            let pipeWidth = 80;
            let pipeHeight = 400;
            let pipeX = 800;
            let pipeY = 0;

            let velocityX = -4;
            let velocityY = 0;
            let gravity = 0.4;

            let gameOver = false;
            let score = 0;

            function init() {
                Socket = new WebSocket('ws://' + window.location.hostname + ':83/');

                setInterval(placePipes, 1500);
                document.addEventListener("keydown", handleKeyPress);
                update();
            }

            function update() {
                requestAnimationFrame(update);
                if (gameOver) {
                    return;
                }
                draw();
                moveBird();
                movePipes();
                detectCollision();
                updateScore(); // Add this line to update the score
            }

            function draw() {
                let board = document.getElementById("board");
                let context = board.getContext("2d");
                context.clearRect(0, 0, board.width, board.height);

                context.fillStyle = "green";
                context.fillRect(bird.x, bird.y, bird.width, bird.height);

                for (let i = 0; i < pipeArray.length; i++) {
                    let pipe = pipeArray[i];
                    context.fillStyle = "green";
                    context.fillRect(pipe.x, pipe.y, pipe.width, pipe.height);
                }

                context.fillStyle = "white";
                context.font = "45px sans-serif";
                context.fillText(score, 5, 45);

                if (gameOver) {
                    context.fillText("GAME OVER", 5, 90);
                }
            }

            function moveBird() {
                velocityY += gravity;
                bird.y = Math.max(bird.y + velocityY, 0);

                if (bird.y > 640) {
                    gameOver = true;
                    Socket.send(score);
                }
            }

            function placePipes() {
                if (gameOver) {
                    return;
                }

                let randomPipeY = pipeY - pipeHeight / 4 - Math.random() * (pipeHeight / 2);
                let openingSpace = 640 / 4;

                let topPipe = {
                    x: pipeX,
                    y: randomPipeY,
                    width: pipeWidth,
                    height: pipeHeight,
                    passed: false,
                }
                pipeArray.push(topPipe);

                let bottomPipe = {
                    x: pipeX,
                    y: randomPipeY + pipeHeight + openingSpace,
                    width: pipeWidth,
                    height: pipeHeight,
                    passed: false,
                }
                pipeArray.push(bottomPipe);
            }

            function movePipes() {
                for (let i = 0; i < pipeArray.length; i++) {
                    let pipe = pipeArray[i];
                    pipe.x += velocityX;
                }

                while (pipeArray.length > 0 && pipeArray[0].x < -pipeWidth) {
                    pipeArray.shift();
                }
            }

            function detectCollision() {
                for (let i = 0; i < pipeArray.length; i++) {
                    let pipe = pipeArray[i];
                    if (
                        bird.x < pipe.x + pipe.width &&
                        bird.x + bird.width > pipe.x &&
                        bird.y < pipe.y + pipe.height &&
                        bird.y + bird.height > pipe.y
                    ) {
                        gameOver = true;
                        Socket.send(score);
                        break;
                    }
                }
            }

            function updateScore() {
                // Check if the bird has passed a pipe
                for (let i = 0; i < pipeArray.length; i++) {
                    let pipe = pipeArray[i];
                    if (!pipe.passed && bird.x > pipe.x + pipe.width) {
                        score+= 0.5   ;
                        pipe.passed = true; // Mark the pipe as passed to avoid incrementing score continuously
                        Socket.send(score);
                    }
                }
            }

            function handleKeyPress(e) {
                if (e.code == "Space" || e.code == "ArrowUp" || e.code == "KeyX") {
                    velocityY = -6;
                    if (gameOver) {
                        resetGame();
                    }
                }
            }
            document.onclick = function(){
              velocityY = -6;
              if(gameOver){
                resetGame();
              }
            }

            function resetGame() {
                bird.y = 50;
                pipeArray = [];
                score = 0;
                gameOver = false;
            }
        </script>
    </body>
</html>
)=====";

const int EEPROM_SIZE = 1024;
const int LAST_IMAGE_ADDRESS = 0;

void handleRoot() {
    server.send_P(200, "text/html", chatHtmlCode);
}

void handleImage() {
    server.send_P(200, "text/html", imageHtmlCode);
}
void handlegame(){
    server.send_P(200, "text/html", game);
}
void handleFileUpload() {
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        // Clear the last image stored in EEPROM
        for (int i = 0; i < EEPROM_SIZE; i++) {
            EEPROM.write(LAST_IMAGE_ADDRESS + i, 0);
        }
        EEPROM.commit();
    }
    else if (upload.status == UPLOAD_FILE_WRITE) {
        // Store the last uploaded image in EEPROM
        for (size_t i = 0; i < upload.currentSize; i++) {
            EEPROM.write(LAST_IMAGE_ADDRESS + i, upload.buf[i]);
        }
        EEPROM.commit();
    }
}

void webSocketEventChat(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    if (type == WStype_TEXT) {
        char c[length];
        for (int i = 0; i < length; i++) {
            c[i] = payload[i];
        }
        chatSocket.broadcastTXT(c, sizeof(c));
    }
}
void webSocketEventgame(uint8_t num, WStype_t type, uint8_t *payload, size_t length){
    if(type == WStype_TEXT){
        char c[length];
        for (int i = 0; i < length; i++) {
            c[i] = payload[i];
        }
        int x = atoi(c);
        if(x>highestserverscore){
          highestserverscore = x;
        chatSocket.broadcastTXT(c, sizeof(c));
        }
    }
}

void webSocketEventImage(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    if (type == WStype_TEXT) {
        String base64Img = String((char *)payload);

        if (base64Img == "REQUEST_LAST_IMAGE") {
            // Send the last uploaded image to the client
            String lastImage;
            for (int i = 0; i < EEPROM_SIZE; i++) {
                char byte = EEPROM.read(LAST_IMAGE_ADDRESS + i);
                lastImage += String(byte);
            }
            imageSocket.sendTXT(num, lastImage.c_str()); // Convert String to const char* before sending
        }
        else {
            // Broadcast the new image to all clients
            imageSocket.broadcastTXT(base64Img);
        }
    }
}

void setup() {
    Serial.begin(115200);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Initialize EEPROM
    EEPROM.begin(EEPROM_SIZE);

    // Set up web server
    server.on("/", HTTP_GET, handleRoot);
    server.on("/image", HTTP_GET, handleImage);
    server.on("/upload", HTTP_POST, handleFileUpload);
    server.on("/game", HTTP_GET, handlegame);

    // Set up WebSocket for chat
    chatSocket.begin();
    chatSocket.onEvent(webSocketEventChat);

    // Set up WebSocket for image upload
    imageSocket.begin();
    imageSocket.onEvent(webSocketEventImage);
    //Set up Websocket for game
    // gameSocket.begin();
    // gameSocket.onEvent(webSocketEventgame);
    server.begin();
}

void loop() {
    chatSocket.loop();
    imageSocket.loop();
    // gameSocket.loop();
    server.handleClient();
}