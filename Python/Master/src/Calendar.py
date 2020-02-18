from datetime import date
from workalendar.america import Mexico as MX
import workalendar.core as CalCore

class Calendar():
    def __init__(self):
        # Class variables
        self.cal = MX()
        self.reminders = []

        # All day reminders
        self.addReminder('EVERY DAY', 'Revisión General')
        self.addReminder('EVERY DAY', 'Rellenar Sales')
        self.addReminder('EVERY DAY', 'Sembrar')
        # Weekly reminders
        self.addReminder('EVERY WEEK', 'Drenar Compresor')
        self.addReminder('EVERY WEEK', 'Limpiar Tanques Recirculado')
        self.addReminder('EVERY WEEK', 'Nutrir Tanques Recirculado')
        # Half of month reminders
        self.addReminder('TWICE MONTH', 'TESTING')
        # Months reminders
        self.addReminder('EVERY MONTH', 'Cambiar aceite compresor')

    def addReminder(self, type, reminder):
        self.reminders.append((type, reminder))

    def checkReminders(self):
        now = date.today()
        todayReminders = []

        for rem in self.reminders:
            cmd = rem[0].split(' ')
            if cmd[0] == 'EVERY' and cmd[1] == 'DAY':
                compare = self.cal.find_following_working_day(now)
                if compare == now: todayReminders.append(rem[1])
            elif cmd[0] == 'EVERY' and cmd[1] == 'WEEK':
                Monday = self.cal.get_first_weekday_after(now, CalCore.MON)
                compare = self.cal.find_following_working_day(Monday)
                if compare == now: todayReminders.append(rem[1])
            elif cmd[0] == 'EVERY' and cmd[1] == 'MONTH':
                compare = self.cal.find_following_working_day(date(now.year, now.month, 1))
                if compare == now: todayReminders.append(rem[1])
            elif cmd[0] == 'TWICE' and cmd[1] == 'MONTH':
                compare1 = self.cal.find_following_working_day(date(now.year, now.month, 1))
                compare2 = self.cal.find_following_working_day(date(now.year, now.month, 15))
                if compare1 == now or compare2 == now: todayReminders.append(rem[1])

        return todayReminders

    def getEmail(self):
        rem = self.checkReminders()
        subject = 'Tareas del día'
        msg = ''
        for i, task in enumerate(rem): msg += '{}-{}     '.format(i+1, task)
            if i == 0: msg = ' '*15
        return subject, msg
