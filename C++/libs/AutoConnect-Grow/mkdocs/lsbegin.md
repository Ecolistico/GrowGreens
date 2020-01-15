## AutoConnect::begin logic sequence

Several parameters as follows of [AutoConnectConfig](apiconfig.md) affect the behavior of [AutoConnect::begin](api.md#begin) function. Each parameter affects the behaves in interacted order with the priority and apply to the logic sequence of AutoConnect::begin.

- [immediateStart](apiconfig.md#immediatestart) : The captive portal start immediately, without first WiFi.begin.
- [autoReconenct](apiconfig.md#autoreconnect) : Attempt re-connect with past SSID by saved credential.
- [portalTimeout](apiconfig.md#portaltimeout) : Time out limit for the portal.
- [retainPortal](apiconfig.md#retainportal) : Keep DNS server functioning for the captive portal.

You can use these parameters in combination with sketch requirements and need to understand correctly the behavior caused by the parameters. The following chart shows the AutoConnect::begin logic sequence including the effect of these parameters.

<img align="center" src="images/process_begin.svg">

For example, AutoConnect::begin will not exits without the **portalTimeout** while the connection not establishes, but WebServer will start to work. So, your sketch may work seemingly, but it will close with inside a loop of the AutoConnect::begin function. Especially when invoking AutoConnect::begin in the **setup()**, execution control does not pass to the **loop()**.

As different scenes, you may use the **immediateStart** effectively. Equipped the external switch to activate the captive portal with the ESP module, combined with the **portalTime** and the **retainPortal** it will become WiFi active connection feature. You can start AutoConnect::begin at any point in the **loop()**, which allows your sketch can behave both the offline mode and the online mode.

Please consider these kinds of influence when you make sketches.

!!! info "The AutoConnect::begin 3rd parameter"
    Another parameter as the [3rd parameter](api.md#begin) of AutoConnect::begin related to timeout constrains the connection wait time after WiFi.begin. It is the **CONNECTED** judgment of the above chart that it has an effect.

