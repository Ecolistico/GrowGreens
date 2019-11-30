#!/usr/bin/env python3

# Import Directories
import logging
import logging.handlers

class logger:
    def __init__(self):
        # Logging config to file
        logging.basicConfig(level = logging.DEBUG,
                            format = '%(asctime)s %(name)-15s %(levelname)-8s %(message)s',
                            datefmt = '%Y-%m-%d %H:%M:%S',
                            filename = './temp/growMaster.log',
                            filemode = 'a')
        
        # Define a Handler which writes WARNING messages or higher to the sys.stderr
        console = logging.StreamHandler()
        console.setLevel(logging.WARNING)
        
        # Set a format for console use
        formatter = logging.Formatter('%(asctime)s %(name)-15s %(message)s', '%Y-%m-%d %H:%M:%S')
        
        # Tell the handler to use this format
        console.setFormatter(formatter)
        
        # Add the handler to the root logger
        logging.getLogger('').addHandler(console)
        
        # Integrate the main logger with warning Module
        logging.captureWarnings(True)
        
        # Define loggers for differents areas
        self.logger = logging.getLogger('master')
        self.logger.setLevel(logging.INFO)
        self.logger_esp32front = logging.getLogger('esp32front')
        self.logger_esp32center = logging.getLogger('esp32center')
        self.logger_esp32back = logging.getLogger('esp32back')
        self.logger_grower1 = logging.getLogger('grower1')
        self.logger_grower2 = logging.getLogger('grower2')
        self.logger_grower3 = logging.getLogger('grower3')
        self.logger_grower4 = logging.getLogger('grower4')
        self.logger_generalControl = logging.getLogger('generalControl')
        self.logger_motorsGrower = logging.getLogger('motorsGrower')
        self.logger_solutionMaker = logging.getLogger('solutionMaker')
        
        # Create a file handler for each logger
        handler = logging.handlers.RotatingFileHandler('./temp/master.log', maxBytes = 5000000, backupCount = 3)
        handler.setLevel(logging.DEBUG)
        handler_esp32front = logging.handlers.RotatingFileHandler('./temp/esp32front.log', maxBytes = 5000000, backupCount = 3)
        handler_esp32front.setLevel(logging.DEBUG)
        handler_esp32center = logging.handlers.RotatingFileHandler('./temp/esp32center.log', maxBytes = 5000000, backupCount = 3)
        handler_esp32center.setLevel(logging.DEBUG)
        handler_esp32back = logging.handlers.RotatingFileHandler('./temp/esp32back.log', maxBytes = 5000000, backupCount = 3)
        handler_esp32back.setLevel(logging.DEBUG)
        handler_grower1 = logging.handlers.RotatingFileHandler('./temp/grower1.log', maxBytes = 5000000, backupCount = 3)
        handler_grower1.setLevel(logging.DEBUG)
        handler_grower2 = logging.handlers.RotatingFileHandler('./temp/grower2.log', maxBytes = 5000000, backupCount = 3)
        handler_grower2.setLevel(logging.DEBUG)
        handler_grower3 = logging.handlers.RotatingFileHandler('./temp/grower3.log', maxBytes = 5000000, backupCount = 3)
        handler_grower3.setLevel(logging.DEBUG)
        handler_grower4 = logging.handlers.RotatingFileHandler('./temp/grower4.log', maxBytes = 5000000, backupCount = 3)
        handler_grower4.setLevel(logging.DEBUG)
        handler_generalControl = logging.handlers.RotatingFileHandler('./temp/generalControl.log', maxBytes = 5000000, backupCount = 3)
        handler_generalControl.setLevel(logging.DEBUG)
        handler_motorsGrower = logging.handlers.RotatingFileHandler('./temp/motorsGrower.log', maxBytes = 5000000, backupCount = 3)
        handler_motorsGrower.setLevel(logging.DEBUG)
        handler_solutionMaker = logging.handlers.RotatingFileHandler('./temp/solutionMaker.log', maxBytes = 5000000, backupCount = 3)
        handler_solutionMaker.setLevel(logging.DEBUG)
        
        # Create logging format and linking it to all the handlers
        formatter = logging.Formatter(fmt = '%(asctime)s %(levelname)-8s %(message)s',
                                      datefmt = '%Y-%m-%d %H:%M:%S')
        handler.setFormatter(formatter)
        handler_esp32front.setFormatter(formatter)
        handler_esp32center.setFormatter(formatter)
        handler_esp32back.setFormatter(formatter)
        handler_grower1.setFormatter(formatter)
        handler_grower2.setFormatter(formatter)
        handler_grower3.setFormatter(formatter)
        handler_grower4.setFormatter(formatter)
        handler_generalControl.setFormatter(formatter)
        handler_motorsGrower.setFormatter(formatter)
        handler_solutionMaker.setFormatter(formatter)
        # Add the handler to the logger
        self.logger.addHandler(handler)
        self.logger_esp32front.addHandler(handler_esp32front)
        self.logger_esp32center.addHandler(handler_esp32center)
        self.logger_esp32back.addHandler(handler_esp32back)
        self.logger_grower1.addHandler(handler_grower1)
        self.logger_grower2.addHandler(handler_grower2)
        self.logger_grower3.addHandler(handler_grower3)
        self.logger_grower4.addHandler(handler_grower4)
        self.logger_generalControl.addHandler(handler_generalControl)
        self.logger_motorsGrower.addHandler(handler_motorsGrower)
        self.logger_solutionMaker.addHandler(handler_solutionMaker)
