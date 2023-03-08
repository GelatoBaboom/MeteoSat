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
                    process.stdout.write(data);
                    datas += data.toString();
                    if (datas.match(/\r\n/i)) {
                        if (datas.match(/DV_DATA:T:([\d]{1,2}\.[\d]{1,2}),P:([\d]{4,6}\.[\d]{1,2}),V:([\d]{1}\.[\d]{1,3})/i)) {
                            console.log('------------------------------------------');
                            console.log('Data process at ' + moment().format('YYYY/MM/DD - HH:mm:ss'));
                            var str = datas.split("\r\n")[0];
                            console.log("Temp: %s C°", (/T:([\d]{1,2}\.[\d]{1,2})/i).exec(str)[1]);
                            console.log("Atmospheric pressure: %s Pa", (/P:([\d]{4,6}\.[\d]{1,2})/i).exec(str)[1]);
                            console.log("Device Voltage: %s V ", (/V:([\d]{1}\.[\d]{1,3})/i).exec(str)[1]);
                            console.log('------------------------------------------');
                        }

                        datas = datas.replace(datas.split("\r\n")[0] + "\r\n", '');
                    }
                    port.flush();
                });
            }

        })
    });
}
