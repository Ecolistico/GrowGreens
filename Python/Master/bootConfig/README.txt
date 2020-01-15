1- Para configurar una red WiFi debe configurar el archivo /etc/wpa_supplicant/wpa_supplicant.conf.
- Observe el archivo de ejemplo que lleva el mismo nombre dentro de esta carpeta.
- Cambie el ssid por el nombre de su red y psk por la contraseña de la misma.

2- Para configurar una IP estática se debe configurar el archivo /etc/dhcpcd.conf
- Observe el archivo de ejemplo que lleva el mismo nombre dentro de esta carpeta.
- Busqué las palabras frases "interface eth0" y "interface wlan0".
- Cambie la variable static ip_address por la IP que desea fijar en su dispósitivo seguida de "/24".
- Cambie las variable static routers y static domain_name_servers por la dirección IP del router al cual 
estará conectado.

3- Para configurar las reglas de los micros se debe crear el archivo /etc/udev/rules.d/microcontroller.rules
- Obsrevé el archivo de ejemplo que lleva el mismo nombre dentro de esta carpeta.
- Puede configurarlo automáticamente si sigue el procedimiento dentro 
de ../GrowGreens/Python/Master/attach_USBdevices.py
- Compruebe que el archivo fue creado correctamente y que es similar al ejemplo de esta carpeta.

4- Para configurar el sistema GrowGreens se deben tener los archivos ./GrowGreens/Python/Master/config.json
y ./GrowGreens/Python/Master/irrigation.json con parámetros correctos
- Este archivo debe ser configurado manualmente, en caso de no saber como proceder 
contacte a jmcasimar@sippys.com.mx

* Nota: Recuerde que la imagen de la raspberry master viene con OpenCV instalado.