var express = require('express'); 
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
    console.log(temp);
    var today = new Date();
    io.sockets.emit('tempUpdate', temp);
    io.sockets.emit('temp', {date: today.getDate()+"-"+today.getMonth()+1+"-"+today.getFullYear(), time: (today.getHours())+":"+(today.getMinutes() + ":" + today.getSeconds()), temp:temp}); //emit the datd i.e. {date, time, temp} to all the connected clients.
});

io.on('connection', (socket) => {
    console.log("Someone connected."); //show a log as a new client connects.

	//when the server receives clicked message, do this
    socket.on('clicked', (data) => {
				console.log('the button was clicked!')
    });
    socket.on('released', (data) => {
				console.log('the button was released!')
    });
})

