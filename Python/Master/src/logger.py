#!/usr/bin/env python3

# Import Directories
import logging
import coloredlogs
import logging.handlers

class logger:
    def __init__(self):
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
        self.logger_esp32front1 = logging.getLogger('esp32front1')
        self.logger_esp32center1 = logging.getLogger('esp32center1')
        self.logger_esp32back1 = logging.getLogger('esp32back1')
        self.logger_esp32front2 = logging.getLogger('esp32front2')
        self.logger_esp32center2 = logging.getLogger('esp32center2')
        self.logger_esp32back2 = logging.getLogger('esp32back2')
        self.logger_grower1 = logging.getLogger('grower1')
        self.logger_grower2 = logging.getLogger('grower2')
        self.logger_grower3 = logging.getLogger('grower3')
        self.logger_grower4 = logging.getLogger('grower4')
        self.logger_generalControl = logging.getLogger('generalControl')
        self.logger_motorsGrower1 = logging.getLogger('motorsGrower1')
        self.logger_motorsGrower2 = logging.getLogger('motorsGrower2')
        self.logger_ihp = logging.getLogger('iHP')
        self.logger_AirPrincipal = logging.getLogger('AirPrincipal')
        self.logger_AirReturn = logging.getLogger('AirReturn')
        # Create a file handler for each logger
        handler = logging.handlers.RotatingFileHandler('./temp/master.log', maxBytes = 4*1024*1024, backupCount = 2)
        handler.setLevel(logging.DEBUG)
        handler_esp32front1 = logging.handlers.RotatingFileHandler('./temp/esp32front1.log', maxBytes = 3*1024*1024, backupCount = 2)
        handler_esp32front1.setLevel(logging.DEBUG)
        handler_esp32center1 = logging.handlers.RotatingFileHandler('./temp/esp32center1.log', maxBytes = 3*1024*1024, backupCount = 2)
        handler_esp32center1.setLevel(logging.DEBUG)
        handler_esp32back1 = logging.handlers.RotatingFileHandler('./temp/esp32back1.log', maxBytes = 3*1024*1024, backupCount = 2)
        handler_esp32back1.setLevel(logging.DEBUG)
        handler_esp32front2 = logging.handlers.RotatingFileHandler('./temp/esp32front2.log', maxBytes = 3*1024*1024, backupCount = 2)
        handler_esp32front2.setLevel(logging.DEBUG)
        handler_esp32center2 = logging.handlers.RotatingFileHandler('./temp/esp32center2.log', maxBytes = 3*1024*1024, backupCount = 2)
        handler_esp32center2.setLevel(logging.DEBUG)
        handler_esp32back2 = logging.handlers.RotatingFileHandler('./temp/esp32back2.log', maxBytes = 3*1024*1024, backupCount = 2)
        handler_esp32back2.setLevel(logging.DEBUG)
        handler_grower1 = logging.handlers.RotatingFileHandler('./temp/grower1.log', maxBytes = 2*1024*1024, backupCount = 2)
        handler_grower1.setLevel(logging.DEBUG)
        handler_grower2 = logging.handlers.RotatingFileHandler('./temp/grower2.log', maxBytes = 2*1024*1024, backupCount = 2)
        handler_grower2.setLevel(logging.DEBUG)
        handler_grower3 = logging.handlers.RotatingFileHandler('./temp/grower3.log', maxBytes = 2*1024*1024, backupCount = 2)
        handler_grower3.setLevel(logging.DEBUG)
        handler_grower4 = logging.handlers.RotatingFileHandler('./temp/grower4.log', maxBytes = 2*1024*1024, backupCount = 2)
        handler_grower4.setLevel(logging.DEBUG)
        handler_generalControl = logging.handlers.RotatingFileHandler('./temp/generalControl.log', maxBytes = 5*1024*1024, backupCount = 2)
        handler_generalControl.setLevel(logging.DEBUG)
        handler_motorsGrower1 = logging.handlers.RotatingFileHandler('./temp/motorsGrower1.log', maxBytes = 3*1024*1024, backupCount = 2)
        handler_motorsGrower1.setLevel(logging.DEBUG)
        handler_motorsGrower2 = logging.handlers.RotatingFileHandler('./temp/motorsGrower2.log', maxBytes = 3*1024*1024, backupCount = 2)
        handler_motorsGrower2.setLevel(logging.DEBUG)
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
        handler_esp32front1.setFormatter(formatter)
        handler_esp32center1.setFormatter(formatter)
        handler_esp32back1.setFormatter(formatter)
        handler_esp32front2.setFormatter(formatter)
        handler_esp32center2.setFormatter(formatter)
        handler_esp32back2.setFormatter(formatter)
        handler_grower1.setFormatter(formatter)
        handler_grower2.setFormatter(formatter)
        handler_grower3.setFormatter(formatter)
        handler_grower4.setFormatter(formatter)
        handler_generalControl.setFormatter(formatter)
        handler_motorsGrower1.setFormatter(formatter)
        handler_motorsGrower2.setFormatter(formatter)
        handler_ihp.setFormatter(formatter)
        handler_AirPrincipal.setFormatter(formatter)
        handler_AirReturn.setFormatter(formatter)
        # Add the handler to the logger
        self.logger.addHandler(handler)
        self.logger_esp32front1.addHandler(handler_esp32front1)
        self.logger_esp32center1.addHandler(handler_esp32center1)
        self.logger_esp32back1.addHandler(handler_esp32back1)
        self.logger_esp32front2.addHandler(handler_esp32front2)
        self.logger_esp32center2.addHandler(handler_esp32center2)
        self.logger_esp32back2.addHandler(handler_esp32back2)
        self.logger_grower1.addHandler(handler_grower1)
        self.logger_grower2.addHandler(handler_grower2)
        self.logger_grower3.addHandler(handler_grower3)
        self.logger_grower4.addHandler(handler_grower4)
        self.logger_generalControl.addHandler(handler_generalControl)
        self.logger_motorsGrower1.addHandler(handler_motorsGrower1)
        self.logger_motorsGrower2.addHandler(handler_motorsGrower2)
        self.logger_ihp.addHandler(handler_ihp)
        self.logger_AirPrincipal.addHandler(handler_AirPrincipal)
        self.logger_AirReturn.addHandler(handler_AirReturn)
