#!/usr/bin/env python3

# Import Directories
import logging
import coloredlogs
import logging.handlers

class logger:
    def __init__(self):
        # Logging config to file
        root_handler = logging.handlers.RotatingFileHandler(filename='./temp/positioningSystem.log', mode='a', maxBytes = 10*1024*1024, backupCount = 2)
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
        self.logger = logging.getLogger('Positioning Server')
        self.logger.setLevel(logging.DEBUG)

        # Create a file handler for each logger
        handler = logging.handlers.RotatingFileHandler('./temp/positioning.log', maxBytes = 10*1024*1024, backupCount = 2)
        handler.setLevel(logging.DEBUG)

        # Create logging format and linking it to all the handlers                                   
        formatter = logging.Formatter(fmt = '%(asctime)s %(levelname)-8s %(message)s',
                                      datefmt = '%Y-%m-%d %H:%M:%S')
        
        handler.setFormatter(formatter)

        # Add the handler to the logger
        self.logger.addHandler(handler)