El c�digo para los ESP32 actualmente funciona para trabajar con 8 sensores DHT22(temp/hum).
Recolectan datos cada X segundos utilizando un filtro exponencial, y la env�an a un servidor mediante
el protocolo MQTT cada que el servidor pide la informaci�n.

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
	Debe contener exactamente 12 car�cteres.
	* Tipo de ESP32: S�lo existen tres tipos frontal(front), central(center) y trasero(back).
5- Una vez ingresados los datos se debe dar click en el bot�n "save", este guardara la informaci�n, en caso de que
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


PENDIENTES/LIMITANTES:
* Falta crear mayor n�mero de funciones que sirvan mediante la llegada de mensajes MQTT, como:
	1. Una funci�n que permita modificar los par�metros de cualquier y la configuraci�n de los filtros.
	2. Una funci�n que permita modificar el par�metro del tiempo entre mediciones.
	***3. Una funci�n que permita modificar la longitud del ID del contenedor para posteriores reconfiguraciones.

* Analizar si se anexaran m�s cantidad de DHT o alg�n otro tipo de sensores.