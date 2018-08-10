import sys
import time
from nums import*
import serial
from serial.tools import list_ports
#finding_port = false

def close_all(*args):
    for a in args:
        a.close()
    sys.stdout.write("Experiment Ended\n")


def get_user_go(*args):
    try:
        assert input("Type [go] to continue: ")=="go"

    except (Exception, KeyboardInterrupt):
        close_all(*args)
        sys.stdout.write("User Exit\n")
        sys.exit(0)
def get_serial():
    ports = [p for p in list_ports.comports()]
    #port = None

    for p,descr,hwid in ports:
        print(p)
        print(descr)
        print(hwid)

    
class MotorController(object):
    
    def __init__(self, port=None, baudrate=ard_baud_rate, timeout=3, finding_port=False):
        #self.port = port if port is not None else self.get_linear_stage_usb_port()
        self.baud = baudrate
        self.timeout = timeout
        sys.stdout.write("Finding Port...\n")
        sys.stdout.flush()
        self.get_linear_stage_usb_port()
        while self.port==None:
            time.sleep(0.1)
        self.open()
        time.sleep(1.75)
        self.home()
        #self.ser.write(("Home\n").encode('utf-8'))
        #self.ser.flush()
       

    def open(self):
        self.ser = serial.Serial(self.port, baudrate=self.baud, timeout=self.timeout)
        
    
    def get_linear_stage_usb_port(self):
    # from serial list, expect the linear stage board to return:
    # ('COM#', 'USB-SERIAL CH340 (COM#)', 'USB VID:PID=1A86:7523 SER=5 LOCATION=1-#')

        ports = [p for p in list_ports.comports()]

        for p,descr,hwid in ports:

            if 'USB-SERIAL CH340' in descr and 'USB VID:PID=1A86:7523' in hwid :
            #linear stage default board
                self.port = p
                self.open()
                if self.get_linear_stage_port():
                    break
                else:
                    self.ser.close()
            
                if p is None:
                    raise ValueError("No Control Boards Found on USB Serial")
        #return port

    def get_linear_stage_port(self):
        self.ser.write(("Get Arduino\n").encode('utf-8'))
        self.ser.flush()
        strInput = self.ser.readline()
        if b'Linear Stage' in strInput:
            return True
        else:
            return False
    def home(self):
        self.ser.write(("Home\n").encode('utf-8'))
        self.ser.flush()
        sys.stdout.write("Homing Motor...\n")
        strInput = ""
        sys.stdout.write("  Response: ")
        while len(strInput)<5:
            time.sleep(0.05)
            strInput = self.ser.readline()
        sys.stdout.write(strInput)
        sys.stdout.flush()

    def goto(self, x0):
        floatStr = '%.4f' % x0
        sys.stdout.write("Sending piston to %s mm\n" % floatStr)
        self.ser.write(("goTo: %f\n" % x0).encode('utf-8'))
        self.ser.flush()
        sys.stdout.write("  Response: ")
        strInput = ""
        while len(strInput)<5:
            time.sleep(0.05)
            strInput = self.ser.readline()
        sys.stdout.write(strInput)
        sys.stdout.flush()

    def step(self, x1):
        floatStr = '%.4f' % x1
        sys.stdout.write("Stepping motor %s mm\n" % floatStr)
        self.ser.write(("Step: %f\n" % x1).encode('utf-8'))
        self.ser.flush()
        sys.stdout.write("  Response: ")
        strInput = ""
        while len(strInput)<5:
            time.sleep(0.05)          
            strInput = self.ser.readline()
        sys.stdout.write(strInput)
        sys.stdout.flush()


    def set_speed(self, v0):
        if v0>=0.1 and v0<=1:
            floatStr = '%.2f' % v0
            sys.stdout.write("Setting speed to %s\n" % floatStr)
            self.ser.write(("Set Speed: %f\n" % v0).encode('utf-8'))
            self.ser.flush()
            time.sleep(0.05)
            sys.stdout.write("  Response: ")
            sys.stdout.write(self.ser.readline())
            sys.stdout.flush()
        else:
            sys.stdout.write("Speed needs to be between 0.1 and 1.\n")

    def get_speed(self):
        self.ser.write(("Get Speed\n").encode('utf-8'))
        self.ser.flush()
        time.sleep(0.05)
        sys.stdout.write("  Response: ")
        sys.stdout.write(self.ser.readline())
        sys.stdout.write("\n")
        sys.stdout.flush()

    def get_position(self):
        self.ser.write(("Get Position\n").encode('utf-8'))
        self.ser.flush()
        time.sleep(0.05)
        sys.stdout.write("  Response: ")
        sys.stdout.write(self.ser.readline())
        sys.stdout.flush()


    def enable(self):
        sys.stdout.write("Enabling Motor\n")
        self.ser.write(("Enable\n").encode('utf-8'))
        self.ser.flush()
        time.sleep(0.05)
        sys.stdout.write("  Response: ")
        sys.stdout.write(self.ser.readline())
        sys.stdout.write("\n")
        sys.stdout.flush()
        
    def disable(self):
        sys.stdout.write("Disabling Motor\n")
        self.ser.write(("Disable\n").encode('utf-8'))
        self.ser.flush()
        time.sleep(0.05)
        sys.stdout.write("  Response: ")
        sys.stdout.write(self.ser.readline())
        sys.stdout.write("\n")
        sys.stdout.flush()
    def get_enabled(self):
        self.ser.write(("Get Enabled\n").encode('utf-8'))
        self.ser.flush()
        time.sleep(0.05)
        sys.stdout.write("  Response: ")
        sys.stdout.write(self.ser.readline())
        sys.stdout.write("\n")
        sys.stdout.flush()
    def get_status(self):
        self.ser.write(("Get Status\n").encode('utf-8'))
        time.sleep(0.05)
        sys.stdout.write("  Response: ")
        sys.stdout.write(self.ser.readline())
        for i in range(2):
            sys.stdout.write("            ")
            sys.stdout.write(self.ser.readline())
            sys.stdout.flush()
    def _read(self):
        sys.stdout.write("  Response: ")
        sys.stdout.write(self.ser.readline())
        sys.stdout.write("\n")
        sys.stdout.flush()
    def close(self):
        self.disable()
        self.ser.close()
        sys.stdout.write("Motor Controller Closed\n")
    def _silent_close(self):
        self.ser.close()

    
        
 
