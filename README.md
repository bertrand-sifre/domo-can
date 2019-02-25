Arduino Domotic CAN library
---------------------------------------------------------

**Contents:**
* [Hardware](#hardware)
   * [CAN Shield](#can-shield)
* [Software Usage](#software-usage)
   * [Dependencies](#dependencies)
   * [Library Installation](#library-installation)
   * [Initialization](#initialization)

# Hardware:

## CAN Shield

The following code samples uses the CAN-BUS Shield, wired up as shown:

![MCP2515 CAN-Shield wiring](examples/wiring.png)

# Software Usage:

## Dependencies

The following library has two dependencies. These dependencies must install for compile your code.

* arduino-mcp2515 https://github.com/autowp/arduino-mcp2515
* TrueRandom https://github.com/sirleech/TrueRandom

## Library Installation

1. Download the ZIP file from https://github.com/bertrand-sifre/domo-can/archive/master.zip
2. From the Arduino IDE: Sketch -> Include Library... -> Add .ZIP Library...
3. Restart the Arduino IDE to see the new "domo-can" library with examples

## Initialization

Construct a DomoCan object on your global variable. On your setup function config your DomoCan, you can call function addXXX for indicate your configuration, after this just call function init.

For the moment the bitrate is hard coded at 500KBPS with a 8Mhz quartz.

## Example

To create a DomoCan with a relay just use this code:

```C++
#include "domo_can.h"

DomoCan domocan;

void setup() {
  domocan.addRelay(&(setRelay), &(resetRelay), &(getStatus));
  domocan.init();
}

void loop() {
  // do something.
}

boolean getStatus() {
  // do something
  return false;
}

void resetRelay() {
  // reset relay.
}

void setRelay() {
  // set relay.
}
````
