const char zero_char = char(48);

void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
  Serial.println(F("Request accepted"));
  if(input_string_complete==true){
    String parameter[6];
    int k = 0;
    int l = 0;
    // Split the parameters
    for (int j = 0; j<inputstring.length(); j++){
      if(inputstring[j] == ','){ // Looking for ','
        parameter[k] = inputstring.substring(l,j);
        k++;
        l = j+1;
      }
      else if(j==inputstring.length()-1){
        parameter[k] = inputstring.substring(l,j+1);
      }
    }

    if(parameter[0]==F("config")){
      Serial.print("Saving...");
      float central = parameter[1].toFloat();
      float left = parameter[2].toFloat();
      float right = parameter[3].toFloat();
      float pressF = parameter[4].toFloat();
      float pressM = parameter[5].toFloat();
      write_Flash(central, left, right, pressF, pressM);            
    }
      else{Serial.println(F("error,Spindle(): Parameter[1] does not match a type"));}
    }
}
