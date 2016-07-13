# Durry

The only GPU-less driver for X.org that works without modelines!

#### Build and Install

To build and install the latest version:

    $ git clone https://github.com/angt/durry
    $ cd durry
    $ ./autogen.sh
    $ ./configure --prefix=/usr
    $ make
    # make install

#### Configure

Just use this `xorg.conf` and you will never see a modeline again :)

```
Section "Device"
    Identifier "device"
    Driver "durry"
EndSection

Section "Monitor"
    Identifier "monitor"
EndSection

Section "Screen"
    Identifier "screen"
    Monitor "monitor"
    Device "device"
    DefaultDepth 24
    SubSection "Display"
        Viewport 0 0
        Depth 24
        Virtual 1920 1080
    EndSubSection
EndSection

Section "InputDevice"
    Identifier "pointer"
    Option "CorePointer" "true"
    Driver "void"
EndSection

Section "InputDevice"
    Identifier "keyboard"
    Option "CoreKeyboard" "true"
    Driver "void"
EndSection

Section "ServerFlags"
    Option "DontVTSwitch" "true"
    Option "AllowMouseOpenFail" "true"
    Option "PciForceNone" "true"
    Option "AutoEnableDevices" "false"
    Option "AutoAddDevices" "false"
EndSection

Section "ServerLayout"
    Identifier "serverlayout"
    Screen "screen"
    InputDevice "pointer"
    InputDevice "keyboard"
EndSection
```
