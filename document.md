# Development Guide for 637-Clock

## Config locations

time server protocol:
server 192.168.137.1
port 3333
response_data *Unix Timestamp*

time server/tcp server address and port:
Line 544-546: 
CONFIG_EXAMPLE_IPV4_ADDR="192.168.137.1"
CONFIG_EXAMPLE_PORT=3333
CONFIG_EXAMPLE_SOCKET_IP_INPUT_STRING=y

## Config check

We should check following configurations:
- CONFIG_EXAMPLE_IPV4_ADDR="192.168.137.1"
- CONFIG_EXAMPLE_PORT=3333
- CONFIG_EXAMPLE_SOCKET_IP_INPUT_STRING=y
- CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY=y

Apparently, these options function as its name said.

It is recommended that you should edit *sdkconfig* using 
```bash
idf.py menuconfig
```
rather than just edit it in editor(like vscode or vim).
That's because sometimes(when edited version of sdkconfig does not conform to the specifications), esp-idf would undo the changes and resume the sdkconfig to the original version.
