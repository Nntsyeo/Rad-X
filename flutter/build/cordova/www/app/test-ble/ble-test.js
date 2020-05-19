/* global ble, statusDiv, beatsPerMinute */

// check what is needed to import

var radiationRate = {
    service: '44a52222-1fcb-11ea-978f-2e728ce88125', 
    measurement: '44a5c9b6-1fcb-11ea-978f-2e728ce88125'
}

var app = {
    initialize: function(){
        this.bindEvents();
    },
    bindEvents: function(){
        //add an event listener to a certain widget to display 
        //when device ready
    },
    onDeviceReady: function(){
        app.scan();
    },
    scan: function(){
        app.status("Scanning for Radiation Detector");

        var foundLittleDetector = false;

        function onScan(peripheral){
            console.log("Found "+ JSON.stringify(peripheral));
            foundLittleDetector = true;
            ble.connect(peripheral.id, app.onConnect, app.onDisconnect);
            //connect to service uuid,
        }

        function scanFailure(reason){
            alert("BLE Scan Failed"); // check the compat of alert
        }

        ble.scan([radiationRate.service], 5, onScan, scanFailure);
        // scan for service uuuid, for 5 seconds, success, failure

        setTimeout(function(){
            if (!foundLittleDetector){
                app.status("Did not find a radiation detector");
            }
        }, 5000); //check the compat of scan and ble
    },
    onConnect: function(){
        app.status("Connected to " + peripheral.id); //check peri
        ble.startNotification(peripheral.id, radiationRate.service, radiationRate.measurement);
    },
    onDisconnect: function(){
        alert("Disconnected " + reason);
        // check what to change for this -> beatsPerMinute.innerHTML = "...";
        app.status("Disconnected");
    },
    onData: function(){
        var data = new Uint8Array(buffer);
        //check what to change for this -> beatsPerMinute.innerHTML = data[1];
    },
    onError: function(){
        alert("There was an error " + reason);
    },
    status: function(){
        console.log(message): //check console.log compat
        //check what to change for this -> statusDiv.innerHTML = message;
    }
};

app.initialize();