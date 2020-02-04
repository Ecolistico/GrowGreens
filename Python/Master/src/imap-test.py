import imaplib
import email

imap_host = 'imap.yandex.com'
imap_user = 'USER@DOMAIN'
imap_pass = 'PASSWORD'

# connect to host using SSL
imap = imaplib.IMAP4_SSL(imap_host, 993)

## login to server
imap.login(imap_user, imap_pass)
imapCount = imap.select('Inbox')
tmp, data = imap.search(None, '(FROM info@sippys.com.mx)')

for num in data[0].split():
	tmp, data = imap.fetch(num, '(RFC822)')
	raw_email = data[0][1]
	msg = email.message_from_bytes(raw_email)
	print('From: {}'.format(msg['From']))
	print('Subject: {}'.format(msg['Subject']))
	print((msg.get_payload(decode=True)).decode('utf-8'))
	break
imap.close()
imap.logout()
