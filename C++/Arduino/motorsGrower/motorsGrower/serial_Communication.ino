/*****   Lecture on serial Events   *****/
/*
 * The structure of the commands is specified on each function
 * 
 * Notes: Values of 1, 2, 3, 4, 5, 7, 9, etc... are char type, and its value is assigned according to the ASCII Table starting at char(48)="0"
 * For further information see ASCII Table
*/
const char zero_char = char(48);

void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC

  
  if(input_string_complete==true){
    if(inputstring.charAt(0)==zero_char){} // function --> '0' 
    else if(inputstring.charAt(0)==zero_char+1){} // function --> '1' 
    else if(inputstring.charAt(0)==zero_char+2){} // function --> '2'
    // etc...
  }
  input_string_complete = false;
}
