<template>
  <div class="page-container">

    <md-app>
      <md-app-content>
        <!-- Pump State -->
        <md-card>

          <md-card-header>
            <div class="md-title">Pump</div>
          </md-card-header>

          <md-card-content>

            <md-field>
              <label>Currently</label>
              <md-input v-model="pumpOn" readonly></md-input>
            </md-field>

          </md-card-content>

          <md-card-content>
            <md-switch v-model="forceOn" v-on:change="setForceOn" class="md-primary">Force On</md-switch>
            <md-switch v-model="forceOff" v-on:change="setForceOff" class="md-primary">Force Off</md-switch>
          </md-card-content>

        </md-card>

        <!-- Water Level -->
        <md-card>
          <md-card-header>
            <div class="md-title">Water Level</div>
          </md-card-header>

          <md-card-content>
            <md-field>
              <label>Current</label>
              <md-input v-model="waterLevel" readonly></md-input>
            </md-field>

            <md-field>
              <label>Low Water Mark</label>
              <md-input v-model="lowWaterMark" type="number" readonly></md-input>
            </md-field>

            <md-field>
              <label>Change Low Water Mark</label>
              <md-input v-model="newLowWaterMark" type="number"></md-input>
              <md-button v-on:click="onUpdateLowWaterMarkClick" class="md-primary">Update</md-button>
            </md-field>

          <span class="md-helper-text">The pump turns on when 'Current' is greater than 'Low Water Mark'.</span>
          </md-card-content>
        </md-card>

        <!-- Timers -->
        <md-card>

          <md-card-header>
            <div class="md-title">Timers (dd:hh:mm:ss)</div>
          </md-card-header>

          <md-card-content>

            <md-field>
              <label>Running Time</label>
              <md-input v-model="appStartedSecsAgo" readonly></md-input>
            </md-field>

            <md-field>
              <label>Last On</label>
              <md-input v-model="lastOnSecsAgo" readonly></md-input>
            </md-field>

            <md-field>
              <label>Longest On Duration</label>
              <md-input v-model="longestOnSecs" readonly></md-input>
            </md-field>

            <md-field>
              <label>Last Off</label>
              <md-input v-model="lastOffSecsAgo" readonly></md-input>
            </md-field>

            <md-field>
              <label>Longest Off Duration</label>
              <md-input v-model="longestOffSecs" readonly></md-input>
            </md-field>

          </md-card-content>

          <md-button href="settings" class="md-primary">Settings</md-button>

        </md-card>

        <md-snackbar position="center" duration="Infinity" :md-active="wsDisconnected" md-persistent>
          <span>Connection lost, attempting to reconnect...</span>
        </md-snackbar>

      </md-app-content>
    </md-app>
  </div>
</template>

<script>
export default {
  name: 'Dashboard',
  props: {},
  methods: {

    // xhr: function() {
    //   window.console.log("Requesting ...");
    //   var rq = new XMLHttpRequest();

    //   rq.onreadystatechange = function(vm) {
    //     if (this.readyState === XMLHttpRequest.DONE) {
    //       if (this.status === 200) {
    //         vm.info = this.responseText;
    //       } else {
    //         window.console.log("Requesting Failed.");
    //       }
    //     }
    //   }.bind(rq, this);

    //   rq.open("GET", this.url);
    //   rq.send();
    // },

    setForceOn(val) {
      window.console.log("setForceOn(" + val + ")");
      this.forceOn = val;
      this.sendMsgKeyVal("forceOn", val);
    },

    setForceOff(val) {
      this.forceOff = val;
      this.sendMsgKeyVal("forceOff", val);
    },

    onUpdateLowWaterMarkClick() {
      this.sendMsgKeyVal("newLowWaterMark", this.newLowWaterMark);
    },

    sendMsgKeyVal(key, val) {
      var msg = JSON.stringify({ [key]: val });
      window.console.log("sendMsgKeyVal(...) " + msg);
      this.connection.send(msg);
    },

    onSckMsgIn(event) {
      var self = this;

      JSON.parse(event.data, function(key, val) {
        // We seem to be getting an empty string key and empty object value
        // as an extra last entry even though self's not what we see in
        // the raw data. Ignore it and move on.
        if (key === "") {
          return;
        }

        window.console.log(key + " === " + val);

        if (key === "pumpOn") {
          self.pumpOn = val === true ? "On" : "Off";
        } else if (key === "waterLevel") {
          self.waterLevel = val;
        } else if (key === "lowWaterMark") {
          self.lowWaterMark = val;
        } else if (key === "forceOff") {
          self.forceOff = val; // ? 1 : 0;
        } else if (key === "forceOn") {
          self.forceOn = val; // ? 1 : 0;
        } else if (key === "newLowWaterMark") {
          self.newLowWaterMark = val;
        } else if (key === "appStartedSecsAgo") {
          self.appStartedSecsAgo = self.secsToDdHhMmSs(val);
        } else if (key === "lastOnSecsAgo") {
          self.lastOnSecsAgo = self.secsToDdHhMmSs(val);
        } else if (key === "lastOffSecsAgo") {
          self.lastOffSecsAgo = self.secsToDdHhMmSs(val);
        } else if (key === "longestOffSecs") {
          self.longestOffSecs = self.secsToDdHhMmSs(val);
        } else if (key === "longestOnSecs") {
          self.longestOnSecs = self.secsToDdHhMmSs(val);
        }

      });
      window.console.log("\n");
    },

    connectWebSockets() {
      var self = this;
      if (window.__debug === true) {
        window.console.log('connect to ws://192.168.0.204/ws');
        this.connection = new WebSocket('ws://192.168.0.204/ws');
      } else {
        window.console.log('connect to ws://' + window.location.hostname + '/ws');
        this.connection = new WebSocket('ws://' + window.location.hostname + '/ws');
      }

      this.connection.onopen = function(event) {
        window.console.log("WS| Open");
        window.console.log(event);
        self.wsDisconnected = false;
      }

      this.connection.onclose = function(event) {
        window.console.log("WS| Close");
        self.wsDisconnected = true;
        window.console.log(event);
        self.connectWebSockets();
      }

      this.connection.onmessage = function(event) {
        window.console.log("\nWS| MSG: " + event.data);
        self.onSckMsgIn(event);
        // Restart our missing messages interval timer.
        clearInterval(self.missingMsgInInterval);
        self.missingMsgInInterval = setInterval(self.msgInTimeout, 5000);
      }

      this.connection.onerror = function(event) {
        window.console.log('WS| ERROR: [error] ${error.message}');
        window.console.log(event);
        self.connection.close();
        self.connection = null;
      }
    },

    msgInTimeout() {
      window.console.log("msgInTimeout ------------------------------------");
      this.wsDisconnected = true;
    },

    secsToDdHhMmSs(secs) {
      var days = Math.floor(secs / 60 / 60 / 24);
      secs = secs - (days * 24 * 60 * 60);
      var hours = Math.floor(secs / 60 / 60);
      var minutes = Math.floor(secs / 60) - (hours * 60);
      var seconds = secs % 60;
      return days.toString().padStart(2, '0') + ":" +
             hours.toString().padStart(2, '0') + ':' +
             minutes.toString().padStart(2, '0') + ':' +
             seconds.toString().padStart(2, '0');
    },

  },
  data() {
    return {
      initialized: false,
      // Status values
      pumpOn: "Off",
      waterLevel: "",
      forceOff: false,
      forceOn: false,
      lowWaterMark: "",
      newLowWaterMark: "",
      appStartedSecsAgo: "",
      lastOnSecsAgo: "",
      lastOffSecsAgo: "",
      longestOffSecs: "",
      longestOnSecs: "",
      wsDisconnected: true,
      missingMsgInInterval: setInterval(this.msgInTimeout, 5000),
    }
  },
  watch: {
    'lowWaterMark': function(val) {
      this.newLowWaterMark = val;
    },
    'forceOff': function(val) {
      window.console.log("val = " + val);
      this.sendMsgKeyVal("forceOff", val ? 1 : 0);
    },
    'wsDisconnected': function(val) {
      window.console.log('wsDisconnected = ' + val);
    }
  },
  created() {
    var self = this;
    window.console.log("Starting Connection to WebSocket Server");

    self.connectWebSockets();

    // ReadyState - implement these.
    // 0 – “CONNECTING”: the connection has not yet been established,
    // 1 – “OPEN”: communicating,
    // 2 – “CLOSING”: the connection is closing,
    // 3 – “CLOSED”: the connection is closed.
    /** @todo Create another interval to check how long it has been
     * since we got a message, if less than expected, close our current
     * connection, if any, and attempt to reconnect.
     */
    setInterval(() => {
      window.console.log("State: " + self.connection.readyState);

      if (self.connection.readyState === 3) { // CLOSED
        window.console.log("WebSockets are CLOSED. Connecting...");
        self.wsDisconnected = true;
        // self.connectWebSockets();
      }
    }, 3000);

  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h3 {
  margin: 40px 0 0;
}
ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
a {
  color: #42b983;
}

.md-card {
  width: 320px;
  margin: 4px;
  display: inline-block;
  vertical-align: top;
}
</style>
