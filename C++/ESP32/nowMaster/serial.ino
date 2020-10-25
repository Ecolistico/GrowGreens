/*****   Lecture on serial Events   *****/

void serialEvent() {
  if (Serial.available() > 0) { // If we recieved some data via serial
    inputstring = Serial.readStringUntil(13);                 //read the string until we see a <CR>

    String parameter[5];
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
        //parameter[k] = inputstring.substring(l,j); // Check this line to work properly with python in raspberry pi 
      }
    }

    if(parameter[0]==F("newID") || parameter[0]==F("newID\n")){
      if (parameter[2].length()>0){
        if(parameter[1].length()==container_ID_length && testContainerId(parameter[1])){
          if (parameter[1]==container_ID) Serial.println(F("warning,[SERIAL] ID is already saved"));
          else {
            container_ID = parameter[1];
            saveID();
            memoryCleanSlaves(false); // Prevents that older slaves with different configuration continue existing
            memoryCleanNonSlaves(); // Prevents that older nonSlaves with different configuration continue existing
          }
        } 
        else Serial.println(F("error,[SERIAL] ID regex does not match"));
      } else Serial.println(F("error,[SERIAL] ID needs an extra argument"));
    } else if(parameter[0]==F("delete") || parameter[0]==F("delete\n")){
      if(parameter[1]==F("slaves") || parameter[1]==F("slaves\n")) memoryCleanSlaves();               // Erases all Slaves from the NVS
      else if(parameter[1]==F("id") || parameter[1]==F("id\n")) memoryCleanID();                      // Erases all id from the NVS
      else if(parameter[1]==F("nonSlaves") || parameter[1]==F("nonSlaves\n")) memoryCleanNonSlaves(); // Erases all nonSlaves from the NVS
      else Serial.println(F("error,[SERIAL] delete() parameter 1 wrong"));
    } else if (parameter[0]==F("reboot") || parameter[0]==F("reboot\n")){
      Serial.println(F("warning,[SERIAL] Rebooting..."));
      ESP.restart();
      delay(1000);  
    } else if(parameter[0]==F("home") || parameter[0]==F("home\n")) {
      Serial.println(F("HOME TEST"));
    }
    else{ Serial.println(F("warning,[SERIAL] Command Unknown")); }
  }
}


bool testContainerId(String ID){
  bool resp = false;
  int cont = 0;

  if(ID.length()==container_ID_length){
    for(int i=0; i<container_ID_length; i++){
      int test = -1;
      int prevTest = -1;
      int prevTest1 = -1;
      
      if(i==1){
        test = int(ID[i])-48;
        prevTest = int(ID[i-1])-48;
        if(prevTest==0){
          if(test>0 && test<=9){cont++;}
        }
        else if(test>=0 && test<=9){cont++;}
      }
      else if(i==5 || i==9){
        test = int(ID[i])-48;
        prevTest = int(ID[i-1])-48;
        prevTest1 = int(ID[i-2])-48;
        if(prevTest==0 && prevTest1==0){
          if(test>0 && test<=9){cont++;}
        }
        else if(test>=0 && test<=9){cont++;}
      }
      else if(i==2 || i==6){
        if(ID[i]=='-'){cont++;}
      }
      else{
        test = int(ID[i])-48;
        if(test>=0 && test<=9){cont++;}
      }
    }
  }

  if(cont==container_ID_length){resp=true;}
  
  return resp;
}
