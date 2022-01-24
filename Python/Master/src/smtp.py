#!/usr/bin/env python3

# Import modules
import smtplib
from time import strftime, localtime
from email.mime.text import MIMEText
from credentials import email

class Mail:
    def __init__(self, loggerMail, city, state, ID, logger = None):
        self.log = logger
        self.logMail = loggerMail
        self.city = city
        self.state = state
        self.ID = ID
        self.trigger = "99-999-999"

    def sendMail(self, subject, mssg):
        msg = MIMEText("Ubicación: {0}, {1}\nContainer:{2}\n{3} - {4}".format(
            self.city, self.state, self.ID,strftime("%Y-%m-%d %H:%M:%S", localtime()),
            mssg))
        recipients = self.logMail
        msg['Subject'] = "#{} {}".format(self.trigger, subject)
        msg['From'] = email['username']
        msg['To'] = ", ".join(recipients)
        try:
            server = smtplib.SMTP_SSL(email['server'], email['port'])
            server.login(email['username'], email['password'])
            server.sendmail(email['username'], recipients, msg.as_string())
            server.close()
            if self.log != None:
                self.log.debug("Email sent to {}, Subject={}, Message={}".format(self.logMail, subject,mssg))
            return True
        except:
            if self.log != None:
                self.log.critical("Email Error, cannot send the email", exc_info=True)
            return False