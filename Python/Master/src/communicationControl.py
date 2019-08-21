class communicationControl:
    def __init__(self, serialGC, serialGM, serialSM, mqttE1, mqttE2, mqttE3, mqttE4, mqttG1, mqttG2, mqttG3, mqttG4):
        self.generalControl = 0 # Define generalControl
        self.growerMotors = 1 # Define growerMotors
        self.solutionMaker = 2 # Define solutionMaker
        self.esp32Front = 3 # Define Frontal ESP32
        self.esp32Center = 4 # Define Center ESP32
        self.esp32Back = 5 # Define Back ESP32
        self.grower1 = 6 # Define Grower 1
        self.grower2 = 7 # Define Grower 2
        self.grower3 = 8 # Define Grower 3
        self.grower4 = 9 # Define Grower 4
        self.functionNames = []
        self.functions = []
        
    def addCallback(self, functionName, function):
        self.fNames.append(functionName)
        self.functions.append(function)
        
    def run(self, functionName):
        for i, functionNames in enumerate(self.fNames):
            if(functionNames==functionName):
                self.functions[i]()
                
def
    
test = communicationControl("test")
test.addCallback("test", testFunction)

