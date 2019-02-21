var express = require('express'); 
var app = express();
var io = require('socket.io')(server);
const SerialPort = require('serialport'); 
const accountSid = 'AC9815b4ffe9df8df160e0ae0b6a76ac5c';
const authToken = 'be1d8a807be8e9f904a7efb5f597fd2b';
const client = require('twilio')(accountSid, authToken);

var server = app.listen(3000, () => { //Start the server, listening on port 3000.
    console.log("Listening to requests on port 3000...");
})

var io = require('socket.io')(server); //Bind socket.io to our express server.
app.use(express.static('public')); //Send index.html page on GET /

const Readline = SerialPort.parsers.Readline;
const port = new SerialPort('COM4'); //Connect serial port to port COM4
const parser = port.pipe(new Readline({delimiter: '\r\n'}));
var msgSent = false;
var sendMsgTo = '+18472748299';
var upperLimit = 30;
var lowerLimit = 20;

parser.on('data', (temp) => { //Read data
    console.log(temp);

    if ((temp < lowerLimit || temp > upperLimit) && !msgSent) {
        console.log("should be sending msg")
        msgSent = true;
        client.messages
            .create({
                body: 'The Tempature is out of bounds!',
                from: '+12242053083',
                to: sendMsgTo
            })
            .then(message => console.log(message.sid));
    }
    else if (msgSent && temp < upperLimit && temp > lowerLimit){
        msgSent = false;
    }

    var today = new Date();
    io.sockets.emit('tempUpdate', temp);
    io.sockets.emit('temp', {date: today.getDate()+"-"+today.getMonth()+1+"-"+today.getFullYear(),
                     time: (today.getHours())+":"+(today.getMinutes() + ":" + today.getSeconds()), temp:temp});
                     //emit the datd i.e. {date, time, temp} to all the connected clients.
});

io.on('connection', (socket) => {
    console.log("Someone connected."); //show a log as a new client connects.

	//when the server receives clicked message, do this
    socket.on('clicked', (data) => {
                port.write('1')
    });

    socket.on('released', (data) => {
                console.log(data)
                port.write('0')
    });
    
    socket.on('updateTwilio', (number, high,low) => {
        sendMsgTo = number;
        upperLimit = high;
        lowerLimit = low;
        console.log("phone number: " + sendMsgTo)
    });
})

