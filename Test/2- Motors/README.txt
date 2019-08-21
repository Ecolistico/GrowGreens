Este procedimiento puede ser utilizado para provar por separado cada par de motores o en su conjunto los 6 motores de un piso, siempre que las conecciones eléctricas esten correctamente realizadas.

1- Conecte los 6 motores a sus drivers.
2- Conecte los drivers (básandose en el diagrama "arduinoShieldConnection") a los siguientes pines del arduino:
Motor		->	 Pin
X1		->	 step=27,dir=29
X2		->	 step=33,dir=35
Y		->	 step=39,dir=41
Enable(Todos)	->	 31

Nota: 	Recuerde que X1, X2 y Y tienen 2 motores cada uno, los pines de señal del arduino son los mismos para cada par.
	Todos los drivers se habilitan/deshabilitan al mismo tiempo por eso solo se utiliza 1 pin como enable

3- Cargue el programa "testCode" dentro de esta carpeta al arduino.
4- Abra el monitor serial para ver los resultados de la prueba, configure el baudrate en 9600.
5- Reinicie el arduino y asegúrese que en el monitor aparezca en la primera línea "Starting test".
6- Posterior a eso el programa puede mostrar en el monitor algunas líneas que correspondan al set up de todos los motores, no se preocupe por ellas.

A continuación se proveeran todos los comandos a los que responden los motores, usted es libre de jugar con ellos siempre que cumpla lo siguiente:
a) Tanto en X como en Y se manejan desplazamiento positivos y negativos, al iniciar nunca podrá hacer un desplazamiento negativo porque el programa considera que
arranco en el límite del contenedor y las condiciones de frontera se lo impiden.
b) Los desplazamiento se miden en mm, usted puede decidir moverse dentro de los límites físicos del contenedor siempre que provea en los comandos el desplazamiento de mm como un número entero.
c) Es importante que conozca las direcciones de desplazamiento en ambos ejes para que pueda juzgar correctamente el comportamiento de los motores. Para hacerlo tomaremos como referencia que la persona que realiza la prueba lo hace
desde el frente de la puerta principal viendo hacia el fondo del contenedor. Desde esta perspectiva se tiene:
	En X, un desplazamiento a la izquierda es positivo y la derecha es negativo.
	En Y, un desplazamiento hacia el fondo del contenedor es positivo y hacia la puerta principal es negativo.
d) Después de jugar con los comandos usted debe poder responder las siguientes preguntas básicas:
	d.1) Los pares de motores X1, X2 y Y, ¿se mueven coordinadamente?
	d.1) ¿Los motores se desplazan en la dirección correcta?
	d.2) ¿Los motores se desplazan los mm solicitados?
	d.3) ¿Los motores presentan un comportamiento adecuado, es decir, no bailan, vibran o se traban mientras esta en curso un desplazamiento?
	d.5) Si todas las respuestas anteriores son afirmativas usted puede considerar como satisfactoria la prueba y proceder, en caso contrario reporte los resultados.

Comandos: Para enviar los comandos debe escribirlos en el monitor serial del Arduino IDE y presionar la tecla enter
'02x' - Movimiento relativo en eje X, x es una variable entera que expresa el desplazamiento esperado en mm (puede ser positivo o negativo)
'03y' - Movimiento relativo en eje Y, y es una variable entera que expresa el desplazamiento esperado en mm (puede ser positivo o negativo)
'04x' - Movimiento absoluto en eje X, x es una variable entera que expresa la posición esperada en mm (No puede ser negativo porque esto supondría que sale de los límites físicos del contenedor)
'05y' - Movimiento relativo en eje Y, y es una variable entera que expresa el desplazamiento esperado en mm (No puede ser negativo porque esto supondría que sale de los límites físicos del contenedor)
'11'  - Imprime en la pantallas las posiciones actuales guardadas desde el comienzo de la prueba.
'20'  - Deshabilita todos los motores, después de enviar este comando debería ser posible que mueva los motores con la mano.
'21'  - Habilita todos los motores, después de enviar este comando los motores deberían trabarse y sería imposible moverlos con la mano.
