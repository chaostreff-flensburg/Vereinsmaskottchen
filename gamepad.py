from evdev import InputDevice
import serial
import threading
import time
import sys


class MoveState:
    lf = 0
    lr = 0
    rf = 0
    rr = 0
    sdrop = 0

    def buffer(self):
        return (str(self.lf) + ';' + str(self.lr) + ';' + str(self.rf) + ';' + str(self.rr) + ';' + str(self.sdrop) + '*').encode('utf-8')

serialDevice = '/dev/ttyUSB0'

if len(sys.argv) == 2:
    serialDevice = sys.argv[1]

dev = InputDevice('/dev/input/event0')
print(dev)

c = threading.Condition()
moveState = MoveState()


class Thread_B(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        global moveState

        connection = serial.Serial(serialDevice, 9600)
        connection.timeout = 0.5
        print(connection.isOpen())
        print(connection.name)

        while True:
            c.acquire()

            buffer = moveState.buffer()
            print(buffer)
            connection.write(buffer)
            #print(connection.read(20))

            time.sleep(0.5)
            c.notify_all()
            c.release()


b = Thread_B()
b.start()
#b.join()

for event in dev.read_loop():
    #print(self.device.fn, categorize(event), sep=': ')

    if event.code == 288:
        if event.value == 1:
            print("rechts vorwaerts marsch!")
            moveState.rf = 1
        else:
            print("rechts vorwaerts stop!")
            moveState.rf = 0
    elif event.code == 290:
        if event.value == 1:
            print("rechts rueckwaerts marsch!")
            moveState.rr = 1
        else:
            print("rechts rueckwaerts stop!")
            moveState.rr = 0
    elif event.code == 1 and event.type == 3:
        if event.value == 0:
            print("links vorwaerts marsch!")
            moveState.lf = 1
        elif event.value == 255:
            print("links rueckwaerts marsch!")
            moveState.lr = 1
        else:
            print("links rueck/vorwaerts stop!")
            moveState.lf = 0
            moveState.lr = 0
    elif event.code == 294 and event.value == 1:
        print("drop the banana")
        moveState.sdrop = 1
    elif event.code == 294 and event.value == 0:
        print("dropped the banana?")
        moveState.sdrop = 0
