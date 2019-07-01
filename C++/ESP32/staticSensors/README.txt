Se utiliza la librer�a AutoConnect para crear un Access Point que permite configurar previamente o en cualquier momento
el ESP32 en cuesti�n. Los pasos que se deben seguir para configurar el disp�sitivo (en el siguiente orden) son:

1- Prender el ESP32, esperar aprox. minuto y medio a que cargue las configuraciones iniciales.
2- Conectarse con alg�n aparato equipado con WiFi (laptop/celular/tablet) a una red que se llamar� ESP32-"ESP-ID", donde
"ESP-ID" es un n�mero de serie. La contrase�a gen�rica es "Kale5elak.".
3- Esperar a que el navegador se abra, en caso de que pase m�s de un minuto y no se abra o conecte correctamente, abrirlo
manualmente y entrar a la direcci�n http://192.168.10.1/_ac.
4- Buscar en el men� la pesta�a llamada "MQTT Settings" y hacer click. Una vez que cargue la p�gina se deber�n introducir
los siguientes datos:
	* MQTT Broker IP Address: Es la direcci�n IP del disp�sitivo/broker en formato IPV4.
	* Container ID: Es el n�mero de serie asignado al contenedor en el cual trabajara el microcontrolador.
	Debe contener exactamente 10 car�cteres y tener el siguiente formato "01-001-001".
	* Tipo de ESP32: S�lo existen tres tipos frontal(front), central(center) y trasero(back).
5- Una vez ingresados los datos se debe dar click en el bot�n "save", este guardar� la informaci�n, en caso de que
alg�n dato sea erroneo no lo grabar� y se deber� repetir el proceso. En caso de que se haya guardado con �xito
la informaci�n ingresada ser� desplegada en la pantalla. NOTA: Favor de verificar que la informaci�n sea correcta, en
caso de haber errores dar click en el bot�n que lleva el nombre de "Clear channel", esto borrar� la informaci�n y podr�
empezar nuevamente desde el paso anterior.
6- Haga click en la pesta�a llamada <SSDI Config> o ingrese en el navegador la direcci�n http://192.168.10.1/_ac/config
Una vez que se cargue la ventana favor de seleccionar la red WiFi pertinente e introducir su contrase�a, para
posteriormente dar enter.
7- Si la red fue cargada con �xito lo podr� ver en la siguiente ventana. En caso de que haya fallado la conexi�n repetir 
paso 6.
8- Hacer click en la ventana llamada RESET, se desplegar� otro bot�n bajo el nombre de RESET, hacer click en el 
nuevamente.
9- !Felicidades ha logrado configurar un disp�sitivo!

El c�digo para los ESP32 actualmente funciona para trabajar con 8 sensores DHT22(temp/hum).
Recolectan datos cada X segundos sin filtro por default, y la env�an a trav�s de el
protocolo MQTT cada que un cliente solicita la informaci�n.

Funciones a trav�s de MQTT:
	* "sendData": env�a la informaci�n de los sensores
	* "reboot": reinicia el disp�sitivo
	* "hardReset": borra todos los datos guardados en la NVS (Non-Volatile Storage) como las credenciales de las
	redes WiFi y los par�metros configurados
	* "updateConstant,X": Modifica la frecuencia de captura de datos cada X segundos.
	La X debe ser un valor entre [2-200]
	* "notFilter": Quita la aplicaci�n de cualquier filtro a la captura de datos.
	* "setExponentialFilter,X": Establece un filtro exponencial con par�metro alfa=(X/100)
	La X debe ser un valor entre [0-100]
	* "setKalmanFilter,X": Establece un filtro de Kalman con par�metro Kalman_Noise=(X/100)
	La X debe ser un valor entre [0-200]

PENDIENTES/LIMITANTES:
* Agregar programaci�n OTA (Over The Air)
* Funciones pendientes MQTT: TBD
* Sensores por a�adir: TBD