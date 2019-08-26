#!/usr/bin/env python3

# Import modules
import smtplib
from time import strftime, localtime

class Mail:
    def __init__(self, logger, loggerMail):
        self.log = logger
        self.logMail = loggerMail
        
    def sendMail(self, subject, mssg):
        infoMail = "info@sippys.com.mx"
        message = "From: {}\nSubject:{}\n\n{} - {}".format(infoMail,
                                                           subject,
                                                           strftime("%Y-%m-%d %H:%M:%S",localtime()),
                                                           mssg)
        try:
            server = smtplib.SMTP_SSL("smtp.yandex.com.tr", 465)
            server.login(infoMail, "Kale5elak.")
            server.sendmail(infoMail, self.logMail, message)
            server.close()
            self.log.info("Email sent to {}, Subject={}, Message={}".format(self.logMail, subject,mssg))
        except:
            self.log.error("Email Error, cannot send the email")
