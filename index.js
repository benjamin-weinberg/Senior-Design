<<<<<<< Updated upstream
var express = require('express'); 
=======
var express = require('express');

const accountSid = 'ACa57a80c5a71aaab26bd951e96e65c90b';
const authToken = '01f0a28be945ee3b3c3006247e321070';

const client = require('twilio')(accountSid, authToken);



>>>>>>> Stashed changes
var app = express();
var io = require('socket.io')(server);
const SerialPort = require('serialport'); 

var server = app.listen(3000, () => { //Start the server, listening on port 3000.
    console.log("Listening to requests on port 3000...");
})

var io = require('socket.io')(server); //Bind socket.io to our express server.
app.use(express.static('public')); //Send index.html page on GET /

const Readline = SerialPort.parsers.Readline;
const port = new SerialPort('COM4'); //Connect serial port to port COM3. Because my Arduino Board is connected on port COM3. See yours on Arduino IDE -> Tools -> Port
// const parser = port.pipe(new Readline({delimiter: '\r\n'})); //Read the line only when new line comes.
const parser = port.pipe(new Readline({delimiter: '\r\n'}));

parser.on('data', (temp) => { //Read data
    var radios = document.getElementsByName('temperature').value;
    if(temp = 'nan')// if temp is sending null data alert user that thermometer connection needs correction
    {
        alert("Thermometer not connected properly, Try again");
    }
    if(radios = 'fahrenheit')//if the user wishes to display data in fahrenheit, convert
    {
        temp = (temp * 9/5) + 32;
    }
    console.log(temp);
    var today = new Date();
    io.sockets.emit('tempUpdate', temp);
    io.sockets.emit('temp', {date: today.getDate()+"-"+today.getMonth()+1+"-"+today.getFullYear(), time: (today.getHours())+":"+(today.getMinutes() + ":" + today.getSeconds()), temp:temp}); //emit the datd i.e. {date, time, temp} to all the connected clients.
    if(temp > document.getElementsByName('maxtemp') || temp < document.getElementsByName('mintemp'))//if temp is outside specified range send text alert to users phone
    {
        client.messages.create({
            to: document.getElementsByName(phonenumber),
            from: '+16302837921',
            body: 'Warning Temperature:' + temp + 'is too high/low'
        })
            .then((message) => console.log(message.sid));
    }
});


io.on('connection', (socket) => {
    console.log("Someone connected."); //show a log as a new client connects.
<<<<<<< Updated upstream

	//when the server receives clicked message, do this
    socket.on('clicked', (data) => {
				console.log('the button was clicked!')
    });
    socket.on('released', (data) => {
				console.log('the button was released!')
    });
})

=======
})

//Use JQUeryUI to resize the div with IE 11
$(".isResizable").resizable();
>>>>>>> Stashed changes
