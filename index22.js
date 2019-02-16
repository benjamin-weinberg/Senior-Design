const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
const port = new SerialPort('COM4')
var plotly = require('plotly')('benjamin-weinberg', 'hjhccyH8DzjPiRw78XP5');

const parser = port.pipe(new Readline({ delimiter: '\r\n' }))
parser.on('data', data => {
    /*
    var date = new Date()
    var data = [
        {
          x: [date.getFullYear() + "-" + date.getMonth() + "-" + date.getDay() + " " + date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds()],
          y: [data],
          type: "scatter"
        }
      ];
      
      var graphOptions = {filename: "date-axes", fileopt: "overwrite"};
      plotly.plot(data, graphOptions, function (err, msg) {
          console.log(msg);
      });

      */
    console.log(data)
})

var data = [{x:[], y:[], stream:{token:'cnihgxiggq', maxpoints:300}}];
var graphOptions = {fileopt : "extend", filename : "nodenodenode"};

plotly.plot(data,graphOptions,function() {
  var stream = plotly.stream('cnihgxiggq', function (res) {
    console.log(res);
  });
  port.pipe(new Readline({ delimiter: '\r\n' })).pipe(stream);
});

