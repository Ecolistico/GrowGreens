#!/usr/bin/env python3

# Import modules
import smtplib
from time import strftime, localtime
from email.mime.text import MIMEText

class Mail:
    def __init__(self, logger, loggerMail, city, state, ID):
        self.log = logger
        self.logMail = loggerMail
        self.city = city
        self.state = state
        self.ID = ID
        
    def sendMail(self, subject, mssg):
        infoMail = "info@sippys.com.mx"
        msg = MIMEText("Ubicación: {0}, {1}\nContainer:{2}\n{3} - {4}".format(
            self.city, self.state, self.ID,strftime("%Y-%m-%d %H:%M:%S", localtime()),
            mssg))
        recipients = self.logMail
        msg['Subject'] = "#{} {}".format(self.ID, subject)
        msg['From'] = infoMail
        msg['To'] = ", ".join(recipients)
        try:
            server = smtplib.SMTP_SSL("smtp.yandex.com.tr", 465)
            server.login(infoMail, "Kale5elak.")
            server.sendmail(infoMail, recipients, msg.as_string())
            server.close()
            self.log.debug("Email sent to {}, Subject={}, Message={}".format(self.logMail, subject,mssg))
        except:
            self.log.exception("Email Error, cannot send the email")
