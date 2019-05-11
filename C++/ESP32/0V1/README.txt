Los códigos para los ESP32 actualmente funcionan para trabajar con 8 sensores DHT22(temp/hum).
Recolectan datos cada 5 segundos utilizando un filtro exponencial, y la envían a un servidor mediante
el protocolo MQTT cada que el servidor pide la información.

Se utiliza el siguiente formato para transmisión de datos:
Servidor envía ---> Topic="alfa/sensor/X"    Mensaje="sendData"
Cliente responde ---> Topic="alfa/sensor/X"  Mensaje= Arreglo de datos separados por comas.

Donde X puede ser front, center o back, según sea el caso de cada ESP32.

LIMITANTES:
* Este código actualmente funciona únicamente si conoces de antemano el nombre y la contraseña de la Red WiFi
a la que se debe de conectar. Así como la dirección IP del servidor MQTT.
* Actualizaciones, usar AutoConnect para evitar conocer estos datos de antemano y poder configurar el esp32
como cliente en cualquier momento.