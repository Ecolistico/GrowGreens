#!/usr/bin/env python3

# Import Directories
import logging
import coloredlogs
import logging.handlers

class logger:
    def __init__(self, floors = 8):
        # Logging config to file
        root_handler = logging.handlers.RotatingFileHandler(filename='./temp/growMaster.log', mode='a', maxBytes = 100*1024*1024, backupCount = 2)
        root_handler.setLevel(logging.DEBUG)
        root_formatter = logging.Formatter('%(asctime)s %(name)-15s %(levelname)-8s %(message)s',
                                           datefmt = '%Y-%m-%d %H:%M:%S')
        root_handler.setFormatter(root_formatter)
        
        logging.basicConfig(level = logging.DEBUG,
                            format = '%(asctime)s %(name)-15s %(levelname)-8s %(message)s',
                            datefmt = '%Y-%m-%d %H:%M:%S',
                            handlers = [root_handler])
        
        # Define a Handler which writes WARNING messages or higher to the sys.stderr
        console = logging.StreamHandler()
        console.setLevel(logging.INFO)
        
        # Set a format for console use
        formatter = logging.Formatter('%(asctime)s %(name)-15s %(message)s', '%Y-%m-%d %H:%M:%S')
        # Add Color to log
        colorFormatter = coloredlogs.ColoredFormatter(fmt='%(asctime)s %(name)-15s %(message)s',
                                         datefmt='%Y-%m-%d %H:%M:%S',
                                         level_styles={'debug':{'color':'cyan'},
                                          'info':{'color':'white'},
                                          'warning':{'color':'yellow'},
                                          'error':{'color':'red'},
                                          'critical':{'color':'white', 'bold':True, 'background':'red'}
                                                       },
                                         field_styles={'asctime':{'color':'white'},
                                                       'name':{'color':'green', 'bold':True}
                                                       })
        
        # Tell the handler to use this format
        console.setFormatter(colorFormatter)
        
        # Add the handler to the root logger
        logging.getLogger('').addHandler(console)
        
        # Integrate the main logger with warning Module
        logging.captureWarnings(True)
        
        # Define loggers for differents areas
        self.logger = logging.getLogger('master')
        self.logger.setLevel(logging.DEBUG)

        # ESP32 and motorsGrower needs a logger each 4 floors
        espFloors = int((floors)/4)
        self.logger_esp32 = []
        self.logger_motorsGrower = []
        for i in range(espFloors):
            front = logging.getLogger('esp32front{}'.format(i+1))
            center = logging.getLogger('esp32center{}'.format(i+1))
            back = logging.getLogger('esp32back{}'.format(i+1))
            self.logger_esp32.append([front, center, back])
            self.logger_motorsGrower.append(logging.getLogger('motorsGrower{}'.format(i+1)))

        # Grower needs a logger for each floor
        self.logger_grower = []
        for i in range(floors): self.logger_grower.append(logging.getLogger('grower{}'.format(i+1)))
        
        # Individual devices and loggers
        self.logger_generalControl = logging.getLogger('generalControl')
        self.logger_Tucan = logging.getLogger('Tucan')
        self.logger_Cloud = logging.getLogger('Cloud')
        self.logger_ihp = logging.getLogger('iHP')
        self.logger_AirPrincipal = logging.getLogger('AirPrincipal')
        self.logger_AirReturn = logging.getLogger('AirReturn')

        # Create a file handler for each logger
        handler = logging.handlers.RotatingFileHandler('./temp/master.log', maxBytes = 4*1024*1024, backupCount = 2)
        handler.setLevel(logging.DEBUG)

        handler_esp32 = []
        handler_motorsGrower = []
        for i in range(espFloors):
            front = logging.handlers.RotatingFileHandler('./temp/esp32front{}.log'.format(i+1), maxBytes = 3*1024*1024, backupCount = 2)
            front.setLevel(logging.DEBUG)
            center = logging.handlers.RotatingFileHandler('./temp/esp32center{}.log'.format(i+1), maxBytes = 3*1024*1024, backupCount = 2)
            center.setLevel(logging.DEBUG)
            back = logging.handlers.RotatingFileHandler('./temp/esp32back{}.log'.format(i+1), maxBytes = 3*1024*1024, backupCount = 2)
            back.setLevel(logging.DEBUG)
            handler_esp32.append([front, center, back])
            motorsGrower = logging.handlers.RotatingFileHandler('./temp/motorsGrower{}.log'.format(i+1), maxBytes = 3*1024*1024, backupCount = 2)
            motorsGrower.setLevel(logging.DEBUG)
            handler_motorsGrower.append(motorsGrower)
            
        handler_grower = []
        for i in range(floors): 
            handler_grower.append(logging.handlers.RotatingFileHandler('./temp/grower{}.log'.format(i+1), maxBytes = 3*1024*1024, backupCount = 2))
            handler_grower[i].setLevel(logging.DEBUG)
        
        handler_generalControl = logging.handlers.RotatingFileHandler('./temp/generalControl.log', maxBytes = 5*1024*1024, backupCount = 2)
        handler_generalControl.setLevel(logging.DEBUG)
        handler_Tucan = logging.handlers.RotatingFileHandler('./temp/Tucan.log', maxBytes = 5*1024*1024, backupCount = 2)
        handler_Tucan.setLevel(logging.DEBUG)
        handler_Cloud = logging.handlers.RotatingFileHandler('./temp/Cloud.log', maxBytes = 5*1024*1024, backupCount = 2)
        handler_Cloud.setLevel(logging.DEBUG)
        handler_ihp = logging.handlers.RotatingFileHandler('./temp/iHP.log', maxBytes = 4*1024*1024, backupCount = 2)
        handler_ihp.setLevel(logging.DEBUG)
        handler_AirPrincipal = logging.handlers.RotatingFileHandler('./temp/AirPrincipal.log', maxBytes = 1*1024*1024, backupCount = 2)
        handler_AirPrincipal.setLevel(logging.DEBUG)
        handler_AirReturn = logging.handlers.RotatingFileHandler('./temp/AirReturn.log', maxBytes = 1*1024*1024, backupCount = 2)
        handler_AirReturn.setLevel(logging.DEBUG)
        
        # Create logging format and linking it to all the handlers                                   
        formatter = logging.Formatter(fmt = '%(asctime)s %(levelname)-8s %(message)s',
                                      datefmt = '%Y-%m-%d %H:%M:%S')
        handler.setFormatter(formatter)
        for i in range(espFloors):
            for j in range(3): handler_esp32[i][j].setFormatter(formatter)
            handler_motorsGrower[i].setFormatter(formatter)
        for i in range(floors): handler_grower[i].setFormatter(formatter)
        handler_generalControl.setFormatter(formatter)
        handler_Tucan.setFormatter(formatter)
        handler_Cloud.setFormatter(formatter)
        handler_ihp.setFormatter(formatter)
        handler_AirPrincipal.setFormatter(formatter)
        handler_AirReturn.setFormatter(formatter)

        # Add the handler to the logger
        self.logger.addHandler(handler)
        for i in range(espFloors):
            for j in range(3): self.logger_esp32[i][j].addHandler(handler_esp32[i][j])
            self.logger_motorsGrower[i].addHandler(handler_motorsGrower[i])
        for i in range(floors): self.logger_grower[i].addHandler(handler_grower[i])
        self.logger_generalControl.addHandler(handler_generalControl)
        self.logger_Tucan.addHandler(handler_Tucan)
        self.logger_Cloud.addHandler(handler_Cloud)
        self.logger_ihp.addHandler(handler_ihp)
        self.logger_AirPrincipal.addHandler(handler_AirPrincipal)
        self.logger_AirReturn.addHandler(handler_AirReturn)
