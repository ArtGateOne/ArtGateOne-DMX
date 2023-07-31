# ArtGateOne-DMX
This is simple artnet node.


Required:
Arduino UNO

Ethernet Shield with W5100/W5500

OLED display 128x32 I2C

module with max485

---------------------------------------

Universal Ethernet to DMX Interface

ONE DMX512 Port

Art-Net streaming DMX support

Can be used with any Art-Net compliant console or DMX software

Universe selectable by web browser


Stand-alone DMX buffer mode

---------------------------------------

WIRING

OLED DISPLAY I2C

VCC --> 5v or 3.3v

GND --> GND

SCL --> SCL

SDA --> SDA

----------
MAX485 module

RO --> not connnected

RE + DE + VCC --> 5v

DI --> TX (D1 PIN)

GND --> GND

A --> DMX OUT

B --> DMX OUT



-------
OLED REQUIRED TO CORRECT WORK


IF U DONT HAVE OLED DISPLAY _ USE NO OLED VERSION


----------

U can configure it - use web browser - enter node ip (default 2.0.0.10)

A3 pin to Ground ehwn boot = Factory Reset

Due to the limited amount of Arduino memory - the program recognizes only full ArtDMX frames - containing 512 channels, and ArtPool packages.

-----------
"Art-Net™ Designed by and Copyright Artistic Licence Engineering Ltd"
