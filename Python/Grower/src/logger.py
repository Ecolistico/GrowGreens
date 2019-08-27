#!/usr/bin/env python3

# Import Directories
import logging
import logging.handlers

class logger:
    def __init__(self):
        # Logging config to file
        logging.basicConfig(level = logging.DEBUG,
                            format = '%(asctime)s %(name)-40s %(levelname)-8s %(message)s',
                            datefmt = '%Y-%m-%d %H:%M:%S',
                            filename = './temp/growGrower.log',
                            filemode = 'a')
        
        # Define a Handler which writes WARNING messages or higher to the sys.stderr
        console = logging.StreamHandler()
        console.setLevel(logging.INFO)
        
        # Set a format for console use
        formatter = logging.Formatter('%(name)-6s: %(message)s')
        
        # Tell the handler to use this format
        console.setFormatter(formatter)
        
        # Add the handler to the root logger
        logging.getLogger('').addHandler(console)
        
        # Define loggers for differents areas
        self.logger = logging.getLogger('grower')
        self.logger.setLevel(logging.INFO)
        
        # Create a file handler for each logger
        handler = logging.handlers.RotatingFileHandler('./temp/master.log', maxBytes = 5000000, backupCount = 3)
        handler.setLevel(logging.DEBUG)
        
        # Create logging format and linking it to all the handlers
        formatter = logging.Formatter(fmt = '%(asctime)s %(levelname)-8s %(message)s',
                                      datefmt = '%Y-%m-%d %H:%M:%S')
        handler.setFormatter(formatter)

        # Add the handler to the logger
        self.logger.addHandler(handler)