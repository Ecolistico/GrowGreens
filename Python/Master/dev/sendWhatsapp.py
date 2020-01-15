from twilio.rest import Client 
 
account_sid = 'AC0a143b4eaf65eac20e9f3a05e95bb495' 
auth_token = '00ac9857f2443a03e1afa0868aef581f' 
client = Client(account_sid, auth_token) 
 
message = client.messages.create( 
                              from_='whatsapp:+14155238886',  
                              body='Ecolistico te da la bienvenida',      
                              to='whatsapp:+5213781056808' 
                          ) 
 
print(message.sid)