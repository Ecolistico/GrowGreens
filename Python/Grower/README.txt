Automáticamente se ejecuta un programa que revisa por el estado de la conexión WiFi en caso de que esta fallé en reiteradas
ocasiones se crea un AP (Access Point que permite configurarla).

Para configurarla por primera vez es necesario:
1- Encenderla y esperar aproximadamente 3 minutos para que arranque verifique que no cuenta con internet y configure
   el AP.
2- Una vez configurado el AP usted podrá ingresar a el mediante un equipo que cuente con conexión a internet, busque
   la lista de redes dispónibles y acceda a la que lleve por nombre "tocani-xxxxxxxxxx" donde las x representan el número
   serie del dispósitivo en cuestión, la contraseña por default será "123tocani456".
3- Una vez que este conectado ingrese a un navegador de internet e ingrese la siguiente dirección: "http://192.168.1.1"
4- Se abrirá una página con las instrucciones para completar el registro, por favor siga cada paso con cuidado e ingrese
   correctamente todos los datos. En caso de no entender a que se hace referancia consulte su manual de operación o en su
   defecto a un superior.
5- Si termino de llenar el formulario correctamente, ¡Felicidades a configurado un dispósitivo con éxito".	

El código de la Raspberry actualmente funciona para trabajar con 2 cámaras térmicas (AMG8833), una cámara raspberry
(o en su defecto una cámara usb), un filtro IR motorizado, un LED blanco 5VDC, una lámpara Xenon 5VDC, un LED IR 5VDC
y un sensor de la familia Cozir de Co2meter. Además, tiene capacidad para conectar un sensor adicional (que use 
I2C como protocolo de comunicación) y un motor 5VDC o un filtro IR adicional, ninguno de los componentes adicionales
están programados por default.

Funciones a través de MQTT:
	* On/Off (IR/XENON/LED/IRCUT): Activa o desactiva cualquiera de los actuadores
	* Enable/DisableIRCUT: Habilita/Deshabilita de manera permanente el filtro IR
	* takePicture,mode,name: Toma una foto en cualquera de los 4 modos de funcionamiento y les asigna por nombre {name}
	* thermalPhoto,name: Toma una imagen de la temperatura con las 2 cámaras térmicas, la une y la guarde en .csv
	bajo el nombre de {name}
	* PhotoSequence,name: Realiza la secuencia de fotos programadas por default y les asigna por nombre {name}
	* Enable/DisableStream: Habilita/Deshabilita de manera permanente el Streaming para agilizar la toma de fotos.
	* whatIsMyIP: Devuelve a la RPi principal la IP del dispósitivo
	* sendPhotos,host,name,password: Envía la carpeta de fotos más recientes a cualquier dispositivo dentro de la red
	que tenga una carpeta configurada para recibirlas. Es necesario pasar la IP, el nombre de usuario y la contraseña
	del dispósitivo que recibe la información.
	* cozirData: Devuelve por default el la humedad, la temperatura y el CO2 a la RPi principal
	* updateGrowerDate: Actualiza la fecha en la clase Grower para la creación y almacenamiento correcto de las fotos.
	* reboot: Reinicia el dispósitivo
	* forgetWiFi: Olvida las redes WiFi previamente configuradas e inmediatamente configura el AP.

PENDIENTES/LIMITANTES:
* Ajustar concatenación de imágenes térmicas.
* Funciones pendientes MQTT: 
	a) Funciones de calibración cozir
* Sensores por añadir: TBD
* Creación librería para sensor AMG88xx (cámara térmica)
* Sincronizar actualizaciones de software automáticamente con gitHub
* Cambiar RPi4 y programar procesamiento de imagenes con OpenCV y PlantCV

Esta carpeta hace uso de los siguientes recursos:
 * Adafruit_AMG88xx (Adafruit/pip): Librería de python bajo licensia MIT para interactuar con sensor AMG88xx, propiedad de 
 Adafruit Industries Copyright (c) 2017, a quien se le reconoce por facilitar su uso. 
 Actualmente está obsoleta, es decir, se dejo de dar soporte a la misma.
 * MotionEye: Paquete escrito por Calin Crisan y que pertenece a Copyright (C) 2007 Free Software Foundation, Inc. A quien 
 se le reconoce por facilitar su uso. Es una interfaz web para interactuar con Motion software de Linux para monitoreo de
 señales de video.
 * Cozir: Libería de creación propia por lo cual aplica a la misma la licencia de este proyecto. Es una librería escrita
 en python para interactuar con los sensores de la familia Cozir desarrollados por Co2meter.
