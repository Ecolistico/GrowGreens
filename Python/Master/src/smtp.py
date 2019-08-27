#!/usr/bin/env python3

# Import modules
import smtplib
from time import strftime, localtime

class Mail:
    def __init__(self, logger, loggerMail, city, state, ID):
        self.log = logger
        self.logMail = loggerMail
        self.city = city
        self.state = state
        self.ID = ID
        
    def sendMail(self, subject, mssg):
        infoMail = "info@sippys.com.mx"
        message = "From: {0}\nSubject:{1}\n\nUbicación: {2}, {3}\nContainer:{4}\n{5} - {6}".format(
            "Ecolistico-info",
            subject,
            self.city,
            self.state,
            self.ID,
            strftime("%Y-%m-%d %H:%M:%S", localtime()),
            mssg)
        message = message.encode("utf-8")
        try:
            server = smtplib.SMTP_SSL("smtp.yandex.com.tr", 465)
            server.login(infoMail, "Kale5elak.")
            server.sendmail(infoMail, self.logMail, message)
            server.close()
            self.log.info("Email sent to {}, Subject={}, Message={}".format(self.logMail, subject,mssg))
        except:
            self.log.exception("Email Error, cannot send the email")
