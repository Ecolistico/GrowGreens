from time import time, sleep

class Alert():
    def __init__(self, logger = None, mailObj = None):
        # Default variables
        self.log = logger
        self.mail = mailObj
        self.alerts = {}

        # Add Alerts
        self.addAlert('Bomba Encendida', ('Las bombas llevan demasiado tiempo encendidas.'
        ' Se recomienda revisar que el agua esta fluyendo correctamente, que las electroválvulas'
        ' se encuentran encendidas y que no están tapadas.'), 20)
        self.addAlert('Exceso Recirculado', ('El nivel del recirculado ha superado los niveles'
        ' permitidos. Se recomienda revisar que no sea una falla del sensor ultrasónico y si es'
        ' necesario vaciar el tanque de agua.'), 5)
        self.addAlert('Caudal Bajo', ('Las siguientes regiones de riego presentaron un caudal'
        ' por debajo de lo normal: '), 30)
        self.addAlert('Llenado de agua', ('El tiempo para rellenar los tanques a presión a sido'
        ' superado. Se recomienda revisar que es lo que está sucediendo.'), 30)
        self.addAlert('Conección de dispósitivos', ('Los siguientes dispósitivos no se han'
        ' podido conectar al sistema: '))

    def str2log(self, msg, level = 'DEBUG'):
        if self.log!=None:
            if (level==0 or level=='DEBUG'): self.log.debug(msg)
            elif (level==1 or level=='INFO'): self.log.info(msg)
            elif (level==2 or level=='WARNING'): self.log.warning(msg)
            elif (level==3 or level=='ERROR'): self.log.error(msg)
            elif (level==4 or level=='CRITICAL'): self.log.critical(msg)
        else:
            print(msg)

    def addAlert(self, alertName, alertMsg, waitTime = 10, extraInfo = ''):
        # waitTime is in minutes
        self.alerts[alertName] = {'msg': alertMsg, 'activate': False, 'time': 0, 'waitTime': waitTime, 'extraInfo': extraInfo}

    def activateAlert(self, alertName, addInfo = ''):
        if alertName in self.alerts:
            self.alerts[alertName].update(activate = True, time = time(), extraInfo = addInfo)

    def deactivateAlert(self, alertName):
        if alertName in self.alerts:
            self.alerts[alertName].update(activate = False, time = 0, extraInfo = '')

    def getEmail(self, alertName):
        if alertName in self.alerts:
            sub = 'ALERTA'
            msg = ' '*15
            msg += '{} - '.format(alertName)
            msg += self.alerts[alertName].get('msg')
            if self.alerts[alertName].get('extraInfo') != "": msg += self.alerts[alertName].get('extraInfo')
            return sub, msg
        else: return 'None', 'None'

    def sendAlert(self, alertName):
        if alertName in self.alerts and self.mail != None:
            sub, msg = self.getEmail(alertName)
            resp = self.mail.sendMail(sub, msg)
            self.alerts[alertName].update(time = time())
            if resp: return True
            else: return False
        else:
            self.str2log('Cannot send alert. Alert Name or Email not defined', 'ERROR')
            return False

    def activateAndSend(self, alertName, addInfo = ''):
        self.activateAlert(alertName, addInfo)
        self.sendAlert(alertName)

    def checkAlertState(self):
        actualTime = time()
        for alert in self.alerts:
            act = self.alerts[alert]['activate']
            alertTime = self.alerts[alert]['time']
            alertWaitTime = self.alerts[alert]['waitTime']
            if act and actualTime-alertTime>alertWaitTime*60:
                self.sendAlert(alert)

"""
from smtp import Mail
mail = Mail(["jmcasimar@sippys.com.mx", "trigger@applet.ifttt.com"], 'city', 'state', '15-113-001', None)
alert = Alert(None, mail)
alert.activateAndSend('Exceso Recirculado')
try:
    while True:
        alert.checkAlertState()
except Exception as e: print(e)
"""
