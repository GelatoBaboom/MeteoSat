const {
    SerialPort,
    SerialPortMock
} = require('serialport');

var moment = require('moment');
var port = null;
var datas = "";

initializePort();

//functions
function initializePort() {
    SerialPort.list().then(function (ports) {
        ports.forEach(function (port) {

            if (port.path == "COM7") {

                console.log("Port: ", port);
                port = new SerialPort({
                    path: port.path,
                    autoOpen: true,
                    baudRate: 9600
                });

                port.on('data', function (data) {
					 datas += data.toString();
                    if (datas.match(/\r\n/i)) {
						
                        console.log('Data: ' + datas.split("\r\n")[0]);
                        datas = datas.replace(datas.split("\r\n")[0] + "\r\n",'');
                    } 

                    port.flush();
                });
            }

        })
    });
}

