SONOFF WIFI SWITCH USING LOCAL SERVER

This is a custom & basic no frills firmware for Sonoff (and other ESP82XX based) switches.
This firmware allows you to remove the stock firmware and hence reliance on the 
manufacturer's cloud based control.  This may be desireable in air-gapped applications
where the switch wouldn't have access to the internet, or where maximum security is 
required.

IP address is set by DHCP on your network (you can set a static DHCP reservation), or hard code the 
IP address in the code before flashing to each switch, see comments in the code.

COULD USE SOME HELP: If you know how to implement OTA firmware updating, please get in touch with
me.   The goal of this firmware is simplicity and stability, but having OTA would be a
huge plus so the module doesn't require being pulled from service to update firmware
as mey be needed.

Enjoy!
