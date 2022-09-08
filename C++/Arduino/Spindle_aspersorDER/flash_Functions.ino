void write_Flash(float Central, float Left, float Right, float PressF, float PressM){
    /* read configuration struct from flash */
  byte* b = dueFlashStorage.readAddress(4); // byte array which is read from flash at adress 4
  Configuration configurationFromFlash; // create a temporary struct
  memcpy(&configurationFromFlash, b, sizeof(Configuration)); // copy byte array to temporary struct
  
    
  configurationFromFlash.a = Central;
  configurationFromFlash.b = Left;
  configurationFromFlash.c = Right; 
  configurationFromFlash.d = PressF;
  configurationFromFlash.e = PressM;
  
    // write configuration struct to flash at adress 4
  byte b2[sizeof(Configuration)]; // create byte array to store the struct
  memcpy(b2, &configurationFromFlash, sizeof(Configuration)); // copy the struct to the byte array
  dueFlashStorage.write(4, b2, sizeof(Configuration)); // write byte array to flash

    // print the content
  Serial.print("Centralmm:");
  Serial.print(configurationFromFlash.a);
  
  Serial.print(" Leftmm:");
  Serial.print(configurationFromFlash.b);

  Serial.print(" Rightmm:");
  Serial.print(configurationFromFlash.c);
  
  Serial.print(" PressFmm:");
  Serial.print(configurationFromFlash.d);
  
  Serial.print(" PressMmm:");
  Serial.print(configurationFromFlash.e);
  
  Serial.println();
  Serial.println();
  
  delay(1000);
}





