# File: main.py
import sys
import os
import platform
import re
from PySide2.QtUiTools import QUiLoader
from PySide2.QtWidgets import QApplication, QWidget
from PySide2.QtCore import QFile, QIODevice, QThread, QObject, Slot, Signal, QTimer

from matplotlib.backends.qt_compat import QtWidgets

import matplotlib

from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas

import numpy as np

import struct
import msv2


pressure_axes = None
temperature_axes = None


DATA_BUFFER_LEN = 500
HEART_BEAT = 100


SENSOR_REMOTE_BUFFER = 5

counter = 0

pressure_data_1 = []
pressure_data_2 = []
temperature_data_1 = []
temperature_data_2 = []
temperature_data_3 = []

time_data = []
window = []
#m = msv2.msv2()
worker = []

total_data = 1

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
VENTING =       0x0C
DOWNLOAD =      0x0D
TVC_MOVE =      0x0E

#MOVE MODES
ABSOLUTE            = 0x00
ABSOLUTE_IMMEDIATE  = 0x01
RELATIVE            = 0x02
RELATIVE_IMMEDIATE  = 0x03

connection_status = "DISCONNECTED"
status_state = 0;


#recording
recording = 0
rec_file = None
start_rec = None

data_labels = ['pres_1 [mBar]', 'pres_2 [mBar]', 'temp_1 [0.1deC]', 'temp_2 [0.1deC]', 'temp_3 [0.1degC]', 'sensor_time [ms]']
remote_labels = ['data_id', 'temp_1', 'temp_2', 'temp_3', 'pres_1', 'pres_2', 'motor_pos', 'sensor_time', 'system state', 'counter_active', 'padding', 'counter']

def safe_int(d):
    try:
        return int(d)
    except:
        return 0

def safe_float(d):
    try:
        return float(d)
    except:
        return 0

def inc2deg(inc):
    return round(-inc/4/1024/66*1*360, 2)

def deg2inc(deg):
    return int(round(-deg*4*1024*66/1/360))

#4096 -> 360 and 2048 is the middle
def deg2dyn(deg):
    return int(round(deg*4096/360 + 2048))

def dyn2deg(dyn):
    return round((dyn - 2048)*360/4096, 2)

@Slot()
def connect_trig():
    device = window.connect_device.text()
    if connection_status == "DISCONNECTED":
        serial_worker.ser_connect(device)
        serial_worker.send_generic(GET_PP_PARAMS, [0x00, 0x00])
    else:
        serial_worker.ser_disconnect()
        

@Slot(str)
def connect_cb(stat):
    global connection_status
    connection_status = stat
    window.connect_status.setText(stat)
    print("connection_updated")


def pp_motor_get_trig():
   serial_worker.send_generic(GET_PP_PARAMS, [0x00, 0x00])

def pp_motor_get_cb(bin_data):
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
        window.pp_motor_hangle.insert(str(inc2deg(data[6])))
        window.pp_motor_fangle.insert(str(inc2deg(data[7])))


def pp_motor_set_trig():
    acc = safe_int(window.pp_motor_acc.text())
    dec = safe_int(window.pp_motor_dec.text())
    spd = safe_int(window.pp_motor_speed.text())
    cwait = safe_int(window.pp_motor_cwait.text())
    hwait = safe_int(window.pp_motor_hwait.text())
    fwait = safe_int(window.pp_motor_fwait.text())
    hangle = deg2inc(safe_float(window.pp_motor_hangle.text()))
    fangle = deg2inc(safe_float(window.pp_motor_fangle.text()))
    bin_data = struct.pack("IIIIIIii", acc, dec, spd, cwait, hwait, fwait, hangle, fangle)
    serial_worker.send_generic(SET_PP_PARAMS, bin_data);


def pp_motor_move_trig():
    target = deg2inc(safe_float(window.pp_motor_target.text()))
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
    serial_worker.send_generic(PP_MOVE, bin_data)

def tvc_motor_move_trig():
    target = deg2dyn(safe_float(window.tvc_motor_target.text()))
    bin_data = struct.pack("i", target)
    serial_worker.send_generic(TVC_MOVE, bin_data)

def calibrate_trig():
    serial_worker.send_generic(CALIBRATE, [0x00, 0x00])

def arm_trig():
    print("arm")
    if status_state == 2: # if armed
        serial_worker.send_generic(DISARM, [0x00, 0x00])
    else:
        serial_worker.send_generic(ARM, [0x00, 0x00])

def ignite_trig():
    serial_worker.send_generic(IGNITE, [0x00, 0x00])

def abort_trig():
    serial_worker.send_generic(ABORT, [0x00, 0x00])

def recover_trig():
    serial_worker.send_generic(RECOVER, [0x00, 0x00])

def ping_trig():
    serial_worker.send_ping()


def vent_open_trig():
    bin_data = struct.pack("H", 1)
    serial_worker.send_generic(VENTING, bin_data)

def vent_close_trig():
    bin_data = struct.pack("H", 0)
    serial_worker.send_generic(VENTING, bin_data)

def vent_cb(stat):
    if(stat and len(stat) == 2):
        data = struct.unpack("H", bytes(stat))
        window.vent_status.clear()
        if(data[0]):
            window.vent_status.setText("OPEN")
        else:
            window.vent_status.setText("CLOSED")

def id_2_mem(id):
    usage = float(id)*32
    u_str = "B"
    u_flt = usage
    if(usage > 1000):
        u_str = "KB"
        u_flt =usage / 1000
    if(usage > 1000000):
        u_str = "MB"
        u_flt =usage / 1000000
    return "{:.3f} {}".format(u_flt, u_str)


def ping_cb(stat, sens):
    global status_state
    global counter
    global total_data

    if(stat and len(stat) == 28):
        data = struct.unpack("HHHHiiIiHBb", bytes(stat))
        state = data[0]
        status_state = state
        window.status_state.clear()
        window.pp_error.clear()
        window.pp_psu.clear()
        window.tvc_psu.clear()
        window.tvc_motor_current.clear()
        window.tvc_error.clear()
        window.tvc_temperature.clear()
        window.status_counter.setText(str(round(float(data[5])/1000, 1)))
        #window.status_counter.display(round(float(data[5])/1000, 1))
        window.pp_motor_current.clear()
        state_text = ['IDLE', 'CALIBRATION', 'ARMED', 'COUNTDOWN', 'IGNITION', 'THRUST', 'SHUTDOWN', 'GLIDE', 'ABORT', 'ERROR']
        window.status_state.insert(state_text[state])
        window.pp_error.insert(hex(data[2]))
        window.pp_psu.insert(str(data[1]/10))
        window.pp_motor_current.insert(str(inc2deg(data[4])))
        window.dl_used.setText(id_2_mem(data[6]))
        total_data = data[6]
        window.tvc_psu.insert(str(data[8]/10))
        window.tvc_motor_current.insert(str(dyn2deg(data[7])))
        window.tvc_error.insert(hex(data[9]))
        window.tvc_temperature.insert(str(data[10]))
        if state == 1:
            temperature_data_1.clear()
            temperature_data_2.clear()
            temperature_data_3.clear()
            pressure_data_1.clear()
            pressure_data_2.clear()
            time_data.clear()
            counter = 0;

    if(sens and len(sens) == 24):

        data = struct.unpack("iiiiiI", bytes(sens))
        #print(data)

        record_sample(data)

        window.temp_1.setText("{:02.1f} °C".format(data[2]/10))
        window.temp_2.setText("{:02.1f} °C".format(data[3]/10))
        window.temp_3.setText("{:02.1f} °C".format(data[4]/10))

        window.pres_1.setText("{:02.3f} Bar".format(data[0]/1000))
        window.pres_2.setText("{:02.3f} Bar".format(data[1]/1000))

        temperature_data_1.append(data[2])
        temperature_data_2.append(data[3])
        temperature_data_3.append(data[4])
        pressure_data_1.append(data[0])
        pressure_data_2.append(data[1])
        time_current = data[5]
        time_data.append(data[5])
        counter = counter + 1

        if counter == DATA_BUFFER_LEN:
            temperature_data_1.pop(0)
            temperature_data_2.pop(0)
            temperature_data_3.pop(0)
            pressure_data_1.pop(0)
            pressure_data_2.pop(0)
            time_data.pop(0)




        temperature_axes.clear()
        pressure_axes.clear()
        pressure_axes.set_xticks([])
        pressure_axes.set_ylabel("Pressure [Bar]")
        temperature_axes.set_ylabel("Temperature [C]")
        temperature_axes.set_xlabel("Time [ms]")

        time_vec = (np.array(time_data)-time_current)/1000
        pressure_axes.set_xlim(-(HEART_BEAT*DATA_BUFFER_LEN-100)/1000, 0.1)
        temperature_axes.set_xlim(-(HEART_BEAT*DATA_BUFFER_LEN-100)/1000, 0.1)


        temperature_axes.plot(time_vec, np.array(temperature_data_1)/10, label='1')
        temperature_axes.plot(time_vec, np.array(temperature_data_2)/10, label='2')
        temperature_axes.plot(time_vec, np.array(temperature_data_3)/10, label='3')
        pressure_axes.plot(time_vec, np.array(pressure_data_1)/1000, label='1')
        pressure_axes.plot(time_vec, np.array(pressure_data_2)/1000, label='2')
        temperature_axes.figure.canvas.draw()
        pressure_axes.figure.canvas.draw()


def write_csv(file, data):
    for i, d in enumerate(data):
        file.write(str(d))
        if i == len(data)-1:
            file.write('\n')
        else:
            file.write(';')

def start_record():
    global recording
    global rec_file
    global start_rec
    if recording:
        recording=0
        window.local_record_stat.setText("")
        rec_file.close()
    else:
        recording=1
        window.local_record_stat.setText("Recording...")
        fn = window.local_record_fn.text()
        if(fn == ''):
            fn = 'local'
        num = 0
        fnam = "{}{}.csv".format(fn, num);
        while(os.path.isfile(fnam)):
            num += 1
            fnam = "{}{}.csv".format(fn, num);
        rec_file = open(fnam, 'w')
        write_csv(rec_file, data_labels)


def record_sample(data):
    if recording:
        write_csv(rec_file, data)
        data_sampled = 0

def download_trig():
    global rem_file
    fn = window.dl_name.text()
    if(fn == ''):
        fn = 'remote'
    num = 0
    fnam = "{}{}.csv".format(fn, num);
    while(os.path.isfile(fnam)):
        num += 1
        fnam = "{}{}.csv".format(fn, num);
    rem_file = open(fnam, 'w')
    write_csv(rem_file, remote_labels)
    serial_worker.download()


def download_cb(data, cnt):
    progress = cnt/total_data*100
    print(progress)
    print(data)
    window.dl_bar.setValue(progress)
    for d in data:
        if(d[0] != 0xffff):
            write_csv(rem_file, d)
    if(cnt > total_data):
        rem_file.close()



class Serial_worker(QObject):
    update_status_sig = Signal(list, list) #status, sensor
    update_pp_params_sig = Signal(list)
    update_venting_sig = Signal(list)
    connect_sig = Signal(str)
    download_sig = Signal(list, int)

    def __init__(self):
        QObject.__init__(self)
        self.msv2 = msv2.msv2()
        self.downloading = 0

    @Slot(str)
    def ser_connect(self, port):
        if(self.msv2.connect(port)):
            self.connect_sig.emit("CONNECTED")
        else:
            self.connect_sig.emit("ERROR")
    @Slot()
    def ser_disconnect(self):
        if(self.msv2.disconnect()):
            self.connect_sig.emit("DISCONNECTED")
        else:
            self.connect_sig.emit("ERROR")

    @Slot(int, list)
    def send_generic(self, opcode, data):
        if self.msv2.is_connected():
            resp = self.msv2.send(opcode, data)
            print("generic:",resp)
            if opcode == GET_PP_PARAMS:
                self.update_pp_params_sig.emit(resp)

            if opcode == VENTING:
                self.update_venting_sig.emit(resp)

    @Slot()
    def download(self):
        if self.msv2.is_connected():
            last_recv = 0
            err_counter = 0
            self.downloading = 1
            while 1:
                bin_data = struct.pack("I", last_recv)
                recv_data = []
                data = self.msv2.send(DOWNLOAD, bin_data)
                if(not data):
                    err_counter += 1
                    if(err_counter > 10):
                        last_recv += 1
                        err_counter = 0
                    continue
                for i in range(5):
                    tmp_data = struct.unpack("HhhhiiiIBBHi", bytes(data[i*32:(i+1)*32]))
                    #print(bytes(data[i*32:(i+1)*32]))
                    recv_data.append(tmp_data)
                last_recv += 5
                self.download_sig.emit(recv_data, last_recv)
                if(last_recv > total_data):
                    self.downloading = 0
                    break
               



    @Slot()
    def send_ping(self):
        if self.msv2.is_connected() and not self.downloading:
            stat = self.msv2.send(GET_STATUS, [0x00, 0x00])
            sens = self.msv2.send(GET_SENSOR, [0x00, 0x00])
            if sens == -1 or sens==0:
                self.connect_sig.emit("RECONNECTING...")
            else:
                self.connect_sig.emit("CONNECTED")
            self.update_status_sig.emit(stat, sens)

    @Slot()
    def start_ping(self, period):
        self.timer = QTimer()
        self.timer.timeout.connect(self.send_ping)
        self.timer.start(period)





def clean_quit():
    worker_thread.quit()
    sys.exit(0)


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
        COM_PORT = 'COM17'

    print(type(COM_PORT))
    if(type(COM_PORT)==str):
        window.connect_device.clear()
        window.connect_device.insert(COM_PORT)

    #CONNECT THREADED CALLBACKS

    #create worker and thread
    worker_thread = QThread()
    serial_worker = Serial_worker()
    serial_worker.moveToThread(worker_thread)

    #CONNECT CB SIGNALS


    serial_worker.connect_sig.connect(connect_cb)
    serial_worker.update_pp_params_sig.connect(pp_motor_get_cb)
    serial_worker.update_status_sig.connect(ping_cb)
    serial_worker.update_venting_sig.connect(vent_cb)
    serial_worker.download_sig.connect(download_cb)

    #start worker thread
    worker_thread.start()
    #worker_thread.setPriority(QThread.TimeCriticalPriority)

    serial_worker.start_ping(HEART_BEAT)

    '''
    all the serial communication should be in a separate thread,
    Some signals from the gui indicate what needs to be sent to the HOSTBOARD
    Some signals from the serial comm thread indicate what needs to be changed

    The communication can be implemented using two queues:
    One queue for data [each field string preferably, ....]
    One queue for commands one command at a time using the command codes

    the buttons callbacks just signal the serial
    There is one main timer function which updates the gui each 0.2sec or something like that

    the periodic access for status and sensor data is done in the serial comm thread
    '''

    #CREATE OSCILLO GRAPH

    pressure_figure = Figure(figsize=(3, 4), dpi=70)
    pressure_axes = pressure_figure.add_subplot(111)
    pressure_canvas = FigureCanvas(pressure_figure)
    window.sensors_layout.addWidget(pressure_canvas)


    temperature_figure = Figure(figsize=(3, 4), dpi=70)
    temperature_axes = temperature_figure.add_subplot(111)
    temperature_canvas = FigureCanvas(temperature_figure)
    window.sensors_layout.addWidget(temperature_canvas)

    pressure_axes.set_xticks([])
    pressure_axes.set_ylabel("Pressure [mBar]")
    temperature_axes.set_ylabel("Temperature [°C]")
    temperature_axes.set_xlabel("Time [ms]")



    window.connect_btn.clicked.connect(connect_trig)
    window.pp_motor_get.clicked.connect(pp_motor_get_trig)
    window.pp_motor_set.clicked.connect(pp_motor_set_trig)
    window.pp_motor_move.clicked.connect(pp_motor_move_trig)
    window.status_calibrate.clicked.connect(calibrate_trig)
    window.status_arm.clicked.connect(arm_trig)
    window.status_ignite.clicked.connect(ignite_trig)
    window.status_abort.clicked.connect(abort_trig)
    window.status_recover.clicked.connect(recover_trig)
    window.quit.clicked.connect(clean_quit)
    window.local_record.clicked.connect(start_record)
    window.vent_open.clicked.connect(vent_open_trig)
    window.vent_close.clicked.connect(vent_close_trig)
    window.download.clicked.connect(download_trig)
    window.tvc_motor_move.clicked.connect(tvc_motor_move_trig)

    window.show()

    sys.exit(app.exec_())
