Los c�digos para los ESP32 actualmente funcionan para trabajar con 8 sensores DHT22(temp/hum).
Recolectan datos cada 5 segundos utilizando un filtro exponencial, y la env�an a un servidor mediante
el protocolo MQTT cada que el servidor pide la informaci�n.

Se utiliza el siguiente formato para transmisi�n de datos:
Servidor env�a ---> Topic="alfa/sensor/X"    Mensaje="sendData"
Cliente responde ---> Topic="alfa/sensor/X"  Mensaje= Arreglo de datos separados por comas.

Donde X puede ser front, center o back, seg�n sea el caso de cada ESP32.

LIMITANTES:
* Este c�digo actualmente funciona �nicamente si conoces de antemano el nombre y la contrase�a de la Red WiFi
a la que se debe de conectar. As� como la direcci�n IP del servidor MQTT.
* Actualizaciones, usar AutoConnect para evitar conocer estos datos de antemano y poder configurar el esp32
como cliente en cualquier momento.