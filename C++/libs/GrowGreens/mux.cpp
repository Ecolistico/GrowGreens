// mux.cpp
//
// Copyright (C) 2019 Grow

#include "mux.h"

/***   MUX   ***/
uint8_t MUX::_total =  0; // Start Count at zero

MUX::MUX(Mux config) // Constructor
  { _config = config;
    _numStates = 0;
    _en = false;
    _stcpEn = true;
    _false = false;
    _delayTimer = false;
    _delayStart = false;
    _number = _total++;
    for(int i = 0; i < OUT_PER_PCB; i++) _off[i] = false;
  }

void MUX::muxPrint(String act, uint8_t level /*= 0*/)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Mux "));
    Serial.print(_number+1);
    Serial.print(F(": "));
    Serial.println(act);
  }

void MUX::muxPrint(String act1, String act2, String act3, String act4, uint8_t level /*= 0*/)
  { if(level==0){ Serial.print(F("debug,")); } // Debug
    else if(level==1){ Serial.print(F("info,")); } // Info
    else if(level==2){ Serial.print(F("warning,")); } // Warning
    else if(level==3){ Serial.print(F("error,")); } // Error
    else if(level==4){ Serial.print(F("critical,")); } // Error
    Serial.print(F("Mux "));
    Serial.print(_number+1);
    Serial.print(F(": "));
    Serial.print(act1);
    Serial.print(act2);
    Serial.print(act3);
    Serial.println(act4);
  }

void MUX::begin()
  { _timer = millis();
    _printTimer = millis();

    pinMode(_config.stcp, OUTPUT);
    pinMode(_config.shcp, OUTPUT);
    pinMode(_config.ds, OUTPUT);

    pinMode(_config.stcp1, OUTPUT);
    pinMode(_config.shcp1, OUTPUT);
    pinMode(_config.ds1, INPUT);
  }

void MUX::activeDelay()
  { _delayTimer = true; }

void MUX::addState(bool &state, uint8_t order)
  { if(_numStates < MAX_MODULES*OUT_PER_PCB){
      _mux[_numStates].number = order;
      _mux[_numStates++].state = &state;  // Check pointer assigment
      //muxPrint(F("addState = "), String(*_mux[_numStates-1].state), F(" in position "), String(_mux[_numStates-1].number));
    } else { muxPrint(F("Error addState() cannot add more than MAX_MODULES*OUT_PER_PCB states"), 3);}
  }

void MUX::printState(uint8_t quotient, uint8_t remainder)
  { muxPrint(F("printState "), String(quotient), String(remainder), String(*_muxOut[quotient].st[remainder])); }

void MUX::orderMux()
  { for(int i = 0; i<_config.pcb_mounted*OUT_PER_PCB; i++) {
      int quotient = i / OUT_PER_PCB;
      int remainder = i % OUT_PER_PCB;
      _muxOut[quotient].st[remainder] = &_false; // Check pointer assigment
      //muxPrint(F("orderMux all to false = "), String(*_muxOut[quotient].st[remainder]), F(""), F(""));
    }

    for(int i = 0; i<_numStates; i++){
      int num = _mux[i].number;
      int quotient = num / OUT_PER_PCB;
      int remainder = num % OUT_PER_PCB;
      _muxOut[quotient].st[remainder] = _mux[i].state; // Check pointer assigment
      //muxPrint(F("orderMux to state = "), String(*_muxOut[quotient].st[remainder]), F(""), F(""));
    }

  }

void MUX::codificationMultiplexer() {
  int value_Multiplexer[_config.pcb_mounted];

  for(int i = 0; i<_config.pcb_mounted; i++) value_Multiplexer[i] = 0;

  for(int i = 0; i<_config.pcb_mounted; i++) {
      for(int j = 0; j<OUT_PER_PCB; j++) {
          if(_en) value_Multiplexer[i]  |= (*_muxOut[i].st[j])  << j;
          else value_Multiplexer[i]  |= _off[j]  << j;
      }
  }

  digitalWrite(_config.stcp, LOW);
  for(int i = 0; i<_config.pcb_mounted; i++) shiftOut(_config.ds, _config.shcp, MSBFIRST, value_Multiplexer[i]);
  if(_stcpEn) digitalWrite(_config.stcp, HIGH);

  _stcpEn = _en; // Copy _en to latch enable. This allow to write one last time the state before turn off latch

  if(millis() - _printTimer>60000){
    _printTimer = millis();
    //for(int i = 0; i<_config.pcb_mounted; i++) muxPrint(F("74HC595-"), String(i), F("= "), String(value_Multiplexer[i]));
  }

}

void MUX::enable(bool en)
  { if(en!=_en) {
      _en = en;
      if(en) muxPrint(F("Enabled"), 1);
      else muxPrint(F("Disabled"), 1);
    }
  }

void MUX::update()
  { if(millis() - _timer > 50) {
      _timer = millis();
      if(_delayTimer) { // If delay is active
        if(!_delayStart) { // If delay is not started
          // Set output to false
          _en = false;
          codificationMultiplexer();
          codificationMultiplexer();
          _en = true;
          // Start delay
          _delayStart = true;
          _delayTime = millis();
        } 
        else if(millis() - _delayTime > 30000) { // If delay is started and time is over
          _delayTimer = false;
          _delayStart = false;
        }
      } 
      else {
        codificationMultiplexer();
        codificationMultiplexer();
      }
    }
  }

/***   muxController   ***/
// Constructor
muxController::muxController(uint8_t muxNumber, dynamicMem & myMem)
  { _muxNumber = muxNumber;
    for(int i = 0; i < _muxNumber; i++){
      Mux _mconfig = myMem.read_mux(i);
      _myMux[i] = new MUX(_mconfig);
      _myMux[i]->begin();
    }
  }

void muxController::update()
  { for(int i = 0; i < _muxNumber; i++) _myMux[i]->update(); }
