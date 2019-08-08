1- Conecte los fines de carrera (básandose en el diagrama "arduinoShieldConnection") a los siguientes pines del arduino:
Fin de carrera	->	 Pin
Piso 1 X1	->	 27
Piso 1 X2	->	 29
Piso 1 Y	->	 31
Piso 2 X1	->	 33
Piso 2 X2	->	 35
Piso 2 Y	->	 37
Piso 3 X1	->	 39
Piso 3 X2	->	 41
Piso 3 Y	->	 43
Piso 4 X1	->	 45
Piso 4 X2	->	 47
Piso 4 Y	->	 49

2- Cargue el programa "testCode" dentro de esta carpeta al arduino.
3- Abra el monitor serial para ver los resultados de la prueba, configure el baudrate en 9600.
4- Reinicie el arduino y asegúrese que en el monitor aparezca en la primera línea "Starting test".
5- Proceda con la prueba para cada uno de los fines de carrera.

Notas: Cada fin de carrera deberá ser probado durante al menos 1 minuto con ayuda de una persona para revisar lo siguiente en el monitor:
a) No deben de aparecer nuevas líneas en el monitor mientras no se presione ningún fin de carrera.
b) Cada que se presione uno de los fines de carrera debe aparecer el nombre del fin de carrera en cuestión seguido de la palabra "pressed".
b.1) En caso de que cuando se presione el fin de carrera no aparezca nada en el monitor serial, proceda a revisar la conección del fin de carrera.
b.2) En caso de que cuando se presione el fin de carrera aparezca la palabra pressed antecedida de un fin de carrera que no corresponde, proceda a revisar que se conecto en el pin adecuado.
b.3) En caso de que aún cuando ningún fin de carrera haya sido presionado aparezcan líneas nuevas en el monitor serial, identifique el tipo de carrera que se supone fue presionado y repórtelo como falla durante la prueba.
