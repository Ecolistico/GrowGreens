
import os
import base64
import os.path
from credentials import email
from time import strftime, localtime
from email.message import EmailMessage

from googleapiclient.discovery import build
from googleapiclient.errors import HttpError
from google.oauth2.credentials import Credentials
from google.auth.transport.requests import Request
from google_auth_oauthlib.flow import InstalledAppFlow

# If modifying these scopes, delete the file token.json.
SCOPES = ["https://www.googleapis.com/auth/gmail.send"]

class Mail:
    def __init__(self, loggerMail, city, state, ID, logger = None):
        self.log = logger
        self.logMail = loggerMail
        self.city = city
        self.state = state
        self.ID = ID
        self.trigger = "99-999-999"

    def sendMail(self, subject, mssg):
        # Tables variables
        actualDirectory = os.getcwd()
        if actualDirectory.endswith('src'):
            token_file = "token.json"
            cred_file = "credentials-google.json"
        else:
            token_file = "./src/token.json"
            cred_file = "./src/credentials-google.json"

        # AUTENTHICATION SCRIPT
        creds = None
        # The file token.json stores the user's access and refresh tokens, and is
        # created automatically when the authorization flow completes for the first
        # time.
        if os.path.exists(token_file):
            creds = Credentials.from_authorized_user_file(token_file, SCOPES)
        # If there are no (valid) credentials available, let the user log in.
        if not creds or not creds.valid:
            if creds and creds.expired and creds.refresh_token:
                creds.refresh(Request())
            else:
                flow = InstalledAppFlow.from_client_secrets_file(
                    cred_file, SCOPES
                )
                creds = flow.run_local_server(port=0)
            # Save the credentials for the next run
            with open(token_file, "w") as token:
                token.write(creds.to_json())
            
        try:
            service = build("gmail", "v1", credentials=creds)
            message = EmailMessage()
            message.set_content("Ubicación: {0}, {1}\nSistema: {2}\n{3} - {4}".format(
            self.city, self.state, self.ID, strftime("%Y-%m-%d %H:%M:%S", localtime()),
            mssg))

            recipients = self.logMail
            message["To"] = ", ".join(recipients)
            message["From"] = email['username']
            message["Subject"] = subject

            # encoded message
            encoded_message = base64.urlsafe_b64encode(message.as_bytes()).decode()

            create_message = {"raw": encoded_message}
            # pylint: disable=E1101
            send_message = (
                service.users()
                .messages()
                .send(userId="me", body=create_message)
                .execute()
            )

            if self.log != None: self.log.info(f'Gmail Oauth Email sent with Message Id: {send_message["id"]}')
            else: print(f'Gmail Oauth Email sent with Message Id: {send_message["id"]}')
        except HttpError as error:
            if self.log != None: self.log.error(f"An error occurred: {error}")
            else: print(f"An error occurred: {error}")
            send_message = None
        return send_message

if __name__ == "__main__":
  myMail = Mail(["jmcasimar@ecolistico.com", "eleazardg@ecolistico.com", "chemanuel_95@hotmail.com"], "Cancun", "Quintana Roo", "00-00-000")
  myMail.sendMail("Esto es una prueba", "Prueba de envío de correos con google gmail usando oauth2")