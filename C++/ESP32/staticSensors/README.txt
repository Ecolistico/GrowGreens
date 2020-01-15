Se utiliza la librería AutoConnect para crear un Access Point que permite configurar previamente o en cualquier momento
el ESP32 en cuestión. Los pasos que se deben seguir para configurar el dispósitivo (en el siguiente orden) son:

1- Prender el ESP32, esperar aprox. minuto y medio a que cargue las configuraciones iniciales.
2- Conectarse con algún aparato equipado con WiFi (laptop/celular/tablet) a una red que se llamará ESP32-"ESP-ID", donde
"ESP-ID" es un número de serie. La contraseña genérica es "Kale5elak.".
3- Esperar a que el navegador se abra, en caso de que pase más de un minuto y no se abra o conecte correctamente, abrirlo
manualmente y entrar a la dirección http://192.168.10.1/_ac.
4- Buscar en el menú la pestaña llamada "MQTT Settings" y hacer click. Una vez que cargue la página se deberán introducir
los siguientes datos:
	* MQTT Broker IP Address: Es la dirección IP del dispósitivo/broker en formato IPV4.
	* Container ID: Es el número de serie asignado al contenedor en el cual trabajara el microcontrolador.
	Debe contener exactamente 10 carácteres y tener el siguiente formato "01-001-001".
	* Tipo de ESP32: Sólo existen tres tipos frontal(front), central(center) y trasero(back).
5- Una vez ingresados los datos se debe dar click en el botón "save", este guardará la información, en caso de que
algún dato sea erroneo no lo grabará y se deberá repetir el proceso. En caso de que se haya guardado con éxito
la información ingresada será desplegada en la pantalla. NOTA: Favor de verificar que la información sea correcta, en
caso de haber errores dar click en el botón que lleva el nombre de "Clear channel", esto borrará la información y podrá
empezar nuevamente desde el paso anterior.
6- Haga click en la pestaña llamada <SSDI Config> o ingrese en el navegador la dirección http://192.168.10.1/_ac/config
Una vez que se cargue la ventana favor de seleccionar la red WiFi pertinente e introducir su contraseña, para
posteriormente dar enter.
7- Si la red fue cargada con éxito lo podrá ver en la siguiente ventana. En caso de que haya fallado la conexión repetir 
paso 6.
8- Hacer click en la ventana llamada RESET, se desplegará otro botón bajo el nombre de RESET, hacer click en el 
nuevamente.
9- !Felicidades ha logrado configurar un dispósitivo!

El código para los ESP32 actualmente funciona para trabajar con 8 sensores DHT22(temp/hum).
Recolectan datos cada X segundos sin filtro por default, y la envían a través de el
protocolo MQTT cada que un cliente solicita la información.

Funciones a través de MQTT:
	* "sendData": envía la información de los sensores
	* "reboot": reinicia el dispósitivo
	* "hardReset": borra todos los datos guardados en la NVS (Non-Volatile Storage) como las credenciales de las
	redes WiFi y los parámetros configurados
	* "updateConstant,X": Modifica la frecuencia de captura de datos cada X segundos.
	La X debe ser un valor entre [2-200]
	* "notFilter": Quita la aplicación de cualquier filtro a la captura de datos.
	* "setExponentialFilter,X": Establece un filtro exponencial con parámetro alfa=(X/100)
	La X debe ser un valor entre [0-100]
	* "setKalmanFilter,X": Establece un filtro de Kalman con parámetro Kalman_Noise=(X/100)
	La X debe ser un valor entre [0-200]

PENDIENTES/LIMITANTES:
* Agregar programación OTA (Over The Air)
* Funciones pendientes MQTT: TBD
* Sensores por añadir: TBD