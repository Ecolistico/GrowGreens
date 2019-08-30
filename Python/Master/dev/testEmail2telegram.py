import sys
import logging
sys.path.insert(0, '../src/')
from smtp import Mail

logging.basicConfig(level = logging.DEBUG,
                            format = '%(asctime)s %(name)-15s %(levelname)-8s %(message)s',
                            datefmt = '%Y-%m-%d %H:%M:%S',
                            filename = 'test.log',
                            filemode = 'a')

mail = Mail(logging, ["jmcasimar@sippys.com.mx", "trigger@applet.ifttt.com"], "Valle de Bravo", "Edo. México", "15-113-001")

mail.sendMail("Ecolistico Alerta", "GrowGreens se detuvo")