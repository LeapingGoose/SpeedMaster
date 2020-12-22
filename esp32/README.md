## ToDo
- Allow option to not broadcast Access Point SSID.
- Get mesh units binding with base unit.
- Get handshake communication and/or mesh unit heartbeats working.
- Figure out best way to do OTA updates on mesh client units.
- Ensure task are able to bounce.
- Addbuffering to sensor unit message in
- Add blocking to message our in sensor if a message is already being sent.

- Rename "status" and "state" variables/enums to be more consistent.

## Done
- Switch between Access Point and Station for the Web Interface.
- Save and display IP and node addresses.
- Have no task start before preferences have been read.
- Exception when trying to send email before the task is ready.
- NTP should retrieve the time right away.
- Should start in STA mode. Currently seems to switch to AP.