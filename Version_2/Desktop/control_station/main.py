# File: main.py
import sys
import os
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QFile, QIODevice

import struct
import msv2

window = []
m = msv2.msv2()


#COMMANDS

READ_STATE = 0x00
MOVE_ABS = 0x01
MOVE_REL = 0x02
MOVE_ABS_IMM = 0x03
MOVE_REL_IMM = 0x04
SET_PP_PARAMS = 0x05
GET_PP_PARAMS = 0x06



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
    if(len(data) >= 1):
        state = data[0]
        window.status_state.clear()
        state_text = ['IDLE', 'CALIBRATION', 'ARMED', 'IGNITION', 'THRUST', 'SHUTDOWN', 'GLIDE', 'ABORT', 'ERROR']
        window.status_state.insert(state_text[state])

def pp_motor_get():
    bin_data = m.send(GET_PP_PARAMS, [0x00, 0x00]);
    print(bin_data)
    data = struct.unpack("IIIIIIIii", bytes(bin_data))

    window.pp_motor_acc.clear()
    window.pp_motor_dec.clear()
    window.pp_motor_speed.clear()
    window.pp_motor_hspeed.clear()
    window.pp_motor_cwait.clear()
    window.pp_motor_hwait.clear()
    window.pp_motor_fwait.clear()
    window.pp_motor_hangle.clear()
    window.pp_motor_fangle.clear()

    window.pp_motor_acc.insert(str(data[0]))
    window.pp_motor_dec.insert(str(data[1]))
    window.pp_motor_speed.insert(str(data[2]))
    window.pp_motor_hspeed.insert(str(data[3]))
    window.pp_motor_cwait.insert(str(data[4]))
    window.pp_motor_hwait.insert(str(data[5]))
    window.pp_motor_fwait.insert(str(data[6]))
    window.pp_motor_hangle.insert(str(data[7]))
    window.pp_motor_fangle.insert(str(data[8]))


def pp_motor_set():
    acc = int(window.pp_motor_acc.text())
    dec = int(window.pp_motor_dec.text())
    spd = int(window.pp_motor_speed.text())
    hspd = int(window.pp_motor_hspeed.text())
    cwait = int(window.pp_motor_cwait.text())
    hwait = int(window.pp_motor_hwait.text())
    fwait = int(window.pp_motor_fwait.text())
    hangle = int(window.pp_motor_hangle.text())
    fangle = int(window.pp_motor_fangle.text())
    bin_data = struct.pack("IIIIIIIii", acc, dec, spd, hspd, cwait, hwait, fwait, hangle, fangle)
    resp = m.send(SET_PP_PARAMS, bin_data);

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


    #connect all the callbacks
    window.connect_btn.clicked.connect(connect)
    window.status_read.clicked.connect(read)
    window.pp_motor_get.clicked.connect(pp_motor_get)
    window.pp_motor_set.clicked.connect(pp_motor_set)

    window.show()

    sys.exit(app.exec_())
