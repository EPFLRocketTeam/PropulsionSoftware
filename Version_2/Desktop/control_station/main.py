# File: main.py
import sys
import os
import platform
import re
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QFile, QIODevice, QTimer, QThread, QThreadPool, QRunnable

import struct
import msv2

window = []
m = msv2.msv2()


#COMMANDS

READ_STATE =    0x00
SET_PP_PARAMS = 0x01
GET_PP_PARAMS = 0x02
PP_MOVE =       0x03
CALIBRATE =     0x04
ARM =           0x05
DISARM =        0x06
IGNITE =        0x07
ABORT =         0x08
RECOVER =       0x09
GET_SENSOR =    0x0A
GET_STATUS =    0x0B

#MOVE MODES

ABSOLUTE            = 0x00
ABSOLUTE_IMMEDIATE  = 0x01
RELATIVE            = 0x02
RELATIVE_IMMEDIATE  = 0x03

status_state = 0;

def safe_int(d):
    try:
        return int(d)
    except:
        return 0

def connect():
    device = window.connect_device.text()
    window.connect_status.clear()
    if(m.is_connected()):
        if(m.disconnect()):
            window.connect_status.insert("")
            window.connect_btn.setText("Connect")
        else:
            window.connect_status.insert("Error")
    else:

        if(m.connect(device)):
            window.connect_status.insert("Connected")
            window.connect_btn.setText("Disconect")
        else:
            window.connect_status.insert("Error")

def read():
    data = m.send(READ_STATE, [0x00, 0x00])
    if(data and len(data) >= 1):
        state = data[0]
        window.status_state.clear()
        state_text = ['IDLE', 'CALIBRATION', 'ARMED', 'COUNTDOWN', 'IGNITION', 'THRUST', 'SHUTDOWN', 'GLIDE', 'ABORT', 'ERROR']
        window.status_state.insert(state_text[state])

def pp_motor_get():
    bin_data = m.send(GET_PP_PARAMS, [0x00, 0x00])
    if(bin_data and len(bin_data) == 32):
        data = struct.unpack("IIIIIIii", bytes(bin_data))

        window.pp_motor_acc.clear()
        window.pp_motor_dec.clear()
        window.pp_motor_speed.clear()
        window.pp_motor_cwait.clear()
        window.pp_motor_hwait.clear()
        window.pp_motor_fwait.clear()
        window.pp_motor_hangle.clear()
        window.pp_motor_fangle.clear()

        window.pp_motor_acc.insert(str(data[0]))
        window.pp_motor_dec.insert(str(data[1]))
        window.pp_motor_speed.insert(str(data[2]))
        window.pp_motor_cwait.insert(str(data[3]))
        window.pp_motor_hwait.insert(str(data[4]))
        window.pp_motor_fwait.insert(str(data[5]))
        window.pp_motor_hangle.insert(str(data[6]))
        window.pp_motor_fangle.insert(str(data[7]))


def pp_motor_set():
    acc = safe_int(window.pp_motor_acc.text())
    dec = safe_int(window.pp_motor_dec.text())
    spd = safe_int(window.pp_motor_speed.text())
    cwait = safe_int(window.pp_motor_cwait.text())
    hwait = safe_int(window.pp_motor_hwait.text())
    fwait = safe_int(window.pp_motor_fwait.text())
    hangle = safe_int(window.pp_motor_hangle.text())
    fangle = safe_int(window.pp_motor_fangle.text())
    bin_data = struct.pack("IIIIIIii", acc, dec, spd, cwait, hwait, fwait, hangle, fangle)
    resp = m.send(SET_PP_PARAMS, bin_data);


def pp_motor_move():
    target = safe_int(window.pp_motor_target.text())
    imm = window.pp_motor_immediate.isChecked()
    rel = window.pp_motor_relative.isChecked()
    if(rel and imm):
        mode = RELATIVE_IMMEDIATE
    if(not rel and imm):
        mode = ABSOLUTE_IMMEDIATE
    if(rel and not imm):
        mode = RELATIVE
    if(not rel and not imm):
        mode = ABSOLUTE
    bin_data = struct.pack("iH", target, mode)
    resp = m.send(PP_MOVE, bin_data)

def calibrate():
    m.send(CALIBRATE, [0x00, 0x00])

def arm():
    print(status_state)
    if status_state == 2: # if armed
        m.send(DISARM, [0x00, 0x00])
    else:
        m.send(ARM, [0x00, 0x00])

def ignite():
    m.send(IGNITE, [0x00, 0x00])

def abort():
    m.send(ABORT, [0x00, 0x00])

def recover():
    m.send(RECOVER, [0x00, 0x00])

def ping():
    global status_state
    bin_data = m.send(GET_STATUS, [0x00, 0x00])
    if(bin_data and len(bin_data) == 12):
        data = struct.unpack("HHHHi", bytes(bin_data))
        state = data[0]
        status_state = state
        window.status_state.clear()
        state_text = ['IDLE', 'CALIBRATION', 'ARMED', 'COUNTDOWN', 'IGNITION', 'THRUST', 'SHUTDOWN', 'GLIDE', 'ABORT', 'ERROR']
        window.status_state.insert(state_text[state])

def ping2():
    worker = Worker(ping)
    threadpool.start(worker)

class Worker(QRunnable):
    '''
    Worker thread

    Inherits from QRunnable to handler worker thread setup, signals and wrap-up.

    :param callback: The function callback to run on this worker thread. Supplied args and
                     kwargs will be passed through to the runner.
    :type callback: function
    :param args: Arguments to pass to the callback function
    :param kwargs: Keywords to pass to the callback function

    '''

    def __init__(self, fn, *args, **kwargs):
        super(Worker, self).__init__()
        # Store constructor arguments (re-used for processing)
        self.fn = fn
        self.args = args
        self.kwargs = kwargs

    def run(self):
        '''
        Initialise the runner function with passed args, kwargs.
        '''
        self.fn(*self.args, **self.kwargs)



if __name__ == "__main__":
    app = QApplication(sys.argv)

    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    ui_file_name = "mainwindow.ui"
    ui_file = QFile(ui_file_name)
    if not ui_file.open(QIODevice.ReadOnly):
        print("Cannot open {}: {}".format(ui_file_name, ui_file.errorString()))
        sys.exit(-1)
    loader = QUiLoader()
    window = loader.load(ui_file)
    ui_file.close()
    if not window:
        print(loader.errorString())
        sys.exit(-1)

    res = None

    if platform.system() == 'Darwin':
        dev_dir = os.listdir('/dev');
        res = None
        for d in dev_dir:
            res = re.search(r'cu.usbmodem[0-9]', d), d
            if(res[0] is not None):
                res = "/dev/{}".format(d)
                break

    if(res is not None):
        COM_PORT = res
    else:
        COM_PORT = 'COM18'


    window.connect_device.clear()
    window.connect_device.insert(COM_PORT)

    threadpool = QThreadPool()

    thread = QThread()
    timer = QTimer()
    timer.setInterval(500) #use multithreading to remove GUI overhead
    timer.timeout.connect(ping)
    timer.start()
    #timer.moveToThread(thread)







    #connect all the callbacks
    window.connect_btn.clicked.connect(connect)
    window.pp_motor_get.clicked.connect(pp_motor_get)
    window.pp_motor_set.clicked.connect(pp_motor_set)
    window.pp_motor_move.clicked.connect(pp_motor_move)
    window.status_calibrate.clicked.connect(calibrate)
    window.status_arm.clicked.connect(arm)
    window.status_ignite.clicked.connect(ignite)
    window.status_abort.clicked.connect(abort)
    window.status_recover.clicked.connect(recover)

    window.show()

    sys.exit(app.exec_())
