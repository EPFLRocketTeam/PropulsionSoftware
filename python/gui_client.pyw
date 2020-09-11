import os
import tkinter as tk
import tkinter.ttk as ttk
import serial
import math
import platform

from matplotlib.backends.backend_tkagg import (
    FigureCanvasTkAgg, NavigationToolbar2Tk)
# Implement the default Matplotlib key bindings.
from matplotlib.backend_bases import key_press_handler
from matplotlib.figure import Figure
from matplotlib.lines import Line2D
import matplotlib.animation as animation
from matplotlib import style
import matplotlib

font = {'family' : 'normal',
        'weight' : 'normal',
        'size'   : 7}

matplotlib.rc('font', **font)

import numpy as np


#functions

COM_PORT ='com14'

safety = 1
ser = serial.Serial()
connected = 0
heart_beat=500

#data_to_save

max_samples = 50
samples = 0
temp1_data = []
temp2_data = []
temp3_data = []
pres1_data = []
pres2_data = []
time_data = []
time_current = 0


def move_rel():
    targ = tmp_target_entry.get()
    out = 'move_rel {}\n'.format(targ)
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))
        ser.readline()

def move_abs():
    targ = tmp_target_entry.get()
    out = 'move_abs {}\n'.format(targ)
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))
        ser.readline()

def set_home():
    out = 'homing\n'
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))

def set_profile():
    vel = prof_spd_entry.get()
    acc = prof_acc_entry.get()
    dec = prof_dec_entry.get()
    out = 'ppm_profile {} {} {}\n'.format(vel, acc, dec)
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))
        ser.readline()
        ser.readline()
        ser.readline()

def set_operation():
    pre = op_wait0_entry.get()
    half = op_half_entry.get()
    wait1 = op_wait1_entry.get()
    full = op_full_entry.get()
    wait2 = op_wait2_entry.get()
    out = 'op_profile {} {} {} {} {}\n'.format(pre, half, wait1, full, wait2)
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))
        ser.readline()
        ser.readline()
        ser.readline()
        ser.readline()
        ser.readline()

def get_profile():
    out = 'short_ppm\n'
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))
        resp = ser.readline().decode('ascii')
        data = resp.split()
        print(data)
        if(len(data) == 3):
            prof_spd_entry.delete(0, tk.END)
            prof_spd_entry.insert(0, data[0])
            prof_acc_entry.delete(0, tk.END)
            prof_acc_entry.insert(0, data[1])
            prof_dec_entry.delete(0, tk.END)
            prof_dec_entry.insert(0, data[2])

def get_operation():
    out = 'short_op\n'
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))
        resp = ser.readline().decode('ascii')
        data = resp.split()
        print(data)
        if(len(data) == 5):
            op_wait0_entry.delete(0, tk.END)
            op_wait0_entry.insert(0, data[0])
            op_half_entry.delete(0, tk.END)
            op_half_entry.insert(0, data[1])
            op_wait1_entry.delete(0, tk.END)
            op_wait1_entry.insert(0, data[2])
            op_full_entry.delete(0, tk.END)
            op_full_entry.insert(0, data[3])
            op_wait2_entry.delete(0, tk.END)
            op_wait2_entry.insert(0, data[4])

def safety_toggle():
    global safety
    if safety == 1:
        safety=0
        operation_but['state'] = 'active'
        homing_but['state'] = 'active'
        saf_but['bg'] = 'lime'
    else:
        safety=1
        operation_but['state'] = 'disabled'
        homing_but['state'] = 'disabled'
        saf_but['bg'] = 'orange'

def operation():
    out = 'operation\n'
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))

def homing():
    out = 'homing\n'
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))

def abort():
    out = 'abort\n'
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))

def enable():
    if stat_enabled['bg'] == 'lime':
        out = 'disable\n'
        print(out)
        if ser.is_open:
            ser.write(bytes(out, 'ascii'))
    else:
        out = 'enable\n'
        print(out)
        if ser.is_open:
            ser.write(bytes(out, 'ascii'))

def toggle_solenoid():
    out = 'solenoid\n'
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))
        resp = ser.readline().decode('ascii')
        data = resp.split()
        print(data)
        if(len(data) == 1):
            if int(data[0]):
                sol_but['bg'] = 'lightblue'
                canvas.coords(solenoid_draw, solen_bbox(math.radians(90)))
            else:
                sol_but['bg'] = 'white'
                canvas.coords(solenoid_draw, solen_bbox(math.radians(0)))
                    

def get_obj():
	index = int(obj_index.get(), 0)
	subindex = int(obj_subindex.get(), 0)
	out = 'get_object {} {}\n'.format(index, subindex)
	print(out)
	if ser.is_open:
		ser.write(bytes(out, 'ascii'))

def set_obj():
	index = int(obj_index.get(), 0)
	subindex = int(obj_subindex.get(), 0)
	data = int(obj_in.get(), 0)
	out = 'set_object {} {} {}\n'.format(index, subindex, data)
	print(out)
	if ser.is_open:
		ser.write(bytes(out, 'ascii'))

    
def startup():
    if stat_power['bg'] == 'lime':
        out = 'shutdown\n'
        print(out)
        if ser.is_open:
            ser.write(bytes(out, 'ascii'))
    else:
        out = 'startup\n'
        print(out)
        if ser.is_open:
            ser.write(bytes(out, 'ascii'))

def fstartup():
    out = 'startup\n'
    print(out)
    if ser.is_open:
      ser.write(bytes(out, 'ascii'))
   

def twos_complement(hexstr,bits):
    value = int(hexstr,16)
    if value & (1 << (bits-1)):
        value -= 1 << bits
    return value 

def angle_half_mod(angle):
    while angle >= 180:
        angle -= 180
    while angle < -180:
        angle += 180
    return angle

def get_status():
    out = 'short_stat\n'
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))
        resp = ser.readline().decode('ascii')
        data = resp.split()
        print(data)
        if(len(data) == 9):
            if int(data[0]):
                stat_power['bg'] = 'lime'
            else:
                stat_power['bg'] = 'white'
            if int(data[1]):
                stat_enabled['bg'] = 'lime'
            else:
                stat_enabled['bg'] = 'white'
            if int(data[2]):
                stat_fault['bg'] = 'red'
            else:
                stat_fault['bg'] = 'white'
            if int(data[3]):
                stat_targ['bg'] = 'lime'
            else:
                stat_targ['bg'] = 'white'
            err_cod.delete(0, tk.END)
            err_cod.insert(0, data[4])
            cur_pos_display.delete(0, tk.END)
            cur_pos_display.insert(0, data[5])
            canvas.coords(valve_draw, valve_bbox(math.radians(int(data[5])/10)))
            if abs(angle_half_mod(int(data[5])/10)) > 28:
                canvas.itemconfig(s_flame, state='normal')
            else:
                canvas.itemconfig(s_flame, state='hidden')


            if abs(angle_half_mod(int(data[5])/10)) > 55:
                canvas.itemconfig(b_flame, state='normal')
            else:
                canvas.itemconfig(b_flame, state='hidden')

            psu_cod.delete(0, tk.END)
            psu_cod.insert(0, str(int(data[6])/10.0))
            torq_entry.delete(0, tk.END)
            torq_entry.insert(0, str(int(data[7])/1000.0))
            obj_out.delete(0, tk.END)
            obj_out.insert(0, '0x'+data[8])
            obj_out_int.delete(0, tk.END)
            obj_out_int.insert(0, str(twos_complement(data[8], 32)))

            
def get_sensors():
    global samples
    global time_current
    out = 'short_sensors\n'
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))
        resp = ser.readline().decode('ascii')
        data = resp.split()
        print(data)
        if(len(data) == 6):
            pres1_entry.delete(0, tk.END)
            pres1_entry.insert(0, data[0])
            pres2_entry.delete(0, tk.END)
            pres2_entry.insert(0, data[1])
            temp1_entry.delete(0, tk.END)
            temp1_entry.insert(0, data[2])
            temp2_entry.delete(0, tk.END)
            temp2_entry.insert(0, data[3])
            temp3_entry.delete(0, tk.END)
            temp3_entry.insert(0, data[4])

            pres1_data.append(int(data[0]))
            pres2_data.append(int(data[1]))
            temp1_data.append(int(data[2]))
            temp2_data.append(int(data[3]))
            temp3_data.append(int(data[4]))
            time_data.append(int(data[5]))
            time_current = int(data[5])
            samples += 1

            if samples >= max_samples:
                pres1_data.pop(0)
                pres2_data.pop(0)
                temp1_data.pop(0)
                temp2_data.pop(0)
                temp3_data.pop(0)
                time_data.pop(0) 
                samples -= 1

def connect():
    global ser
    global connected
    if connected:
        ser.close()
        connected = 0
        serial_but['text']='Connect'
    else:
        com = serial_entry.get()
        ser = serial.Serial(com, 115200, timeout=1)      
        if ser.is_open:
            connected = 1
            serial_but['text']='Disconnect'
    

    



def valve_bbox(angle):
    center_x = o_top_x+o_rad
    center_y = o_top_y+o_rad
    bb_x1 = center_x + math.cos(angle)*o_rad
    bb_x2 = center_x - math.cos(angle)*o_rad
    bb_y1 = center_y + math.sin(angle)*o_rad
    bb_y2 = center_y - math.sin(angle)*o_rad
    return [bb_x1, bb_y1, bb_x2, bb_y2]

def solen_bbox(angle):
    center_x = sol_x
    center_y = r_top_y-r_arc-sol_height-2
    rad = 3
    bb_x1 = center_x - math.cos(angle)*rad
    bb_x2 = center_x + math.cos(angle)*rad
    bb_y1 = center_y - math.sin(angle)*rad
    bb_y2 = center_y + math.sin(angle)*rad
    return [bb_x1, bb_y1, bb_x2, bb_y2]


def main_update():

    get_status()
    get_sensors()
    window.after(heart_beat, main_update)







window = tk.Tk()

window.geometry('940x580')
window.title('Propulsion Control')


motor = ttk.Labelframe(window, text="motor control")
motor.grid(row=1, column=0, rowspan=2, sticky="NSEW")


sensor = ttk.Labelframe(window, text="sensors")
sensor.grid(row=1, column=1, sticky="NSEW")


other = ttk.Labelframe(window, text="other")
other.grid(row=2, column=1, sticky="NSEW")

YPAD = 2
BPAD = 5

comm_box = ttk.Labelframe(window, text='connection')
comm_box.grid(row=0, column=0, columnspan=2, sticky="NSEW")

serial_label= tk.Label(comm_box, text='Serial port =')
serial_label.grid(row=0, column=0, sticky="WE", pady=YPAD)

serial_entry= tk.Entry(comm_box)
serial_entry.insert(0, COM_PORT)
serial_entry.grid(row=0, column=1, sticky="WE", pady=YPAD)

serial_but= tk.Button(comm_box, text='Connect', command=connect)
serial_but.grid(row=0, column=2, sticky="WE", pady=YPAD, padx=BPAD)

pos_adj = ttk.Labelframe(motor, text='position adjustement')
pos_adj.grid(row=0, column = 0, sticky='WE')

#tmp_mov_label = tk.Label(pos_adj, text="POSITION ADJUSTEMENT")
#tmp_mov_label.grid(row = 0, column = 0, columnspan=5, pady=YPAD)

tmp_target_label = tk.Label(pos_adj, text="target = ")
tmp_target_label.grid(row=1, column=0, sticky="E", pady=YPAD)

tmp_target_entry = tk.Entry(pos_adj, justify='right')
tmp_target_entry.grid(row=1, column=1, sticky="E", pady=YPAD)

tmp_target_label = tk.Label(pos_adj, text="[0.1 deg]")
tmp_target_label.grid(row=1, column=2, sticky="W", pady=YPAD)

mov_rel_btn = tk.Button(pos_adj, text="Relative", command=move_rel)
mov_rel_btn.grid(row=1, column=3, sticky="EW", pady=YPAD)

mov_rel_btn = tk.Button(pos_adj, text="Absolute", command=move_abs)
mov_rel_btn.grid(row=1, column=4, sticky="EW", pady=YPAD)


cur_pos_label = tk.Label(pos_adj, text="current position = ")
cur_pos_label.grid(row=3, column = 0, sticky="E", pady=YPAD)

cur_pos_display = tk.Entry(pos_adj, justify='right')
cur_pos_display.grid(row=3, column = 1, sticky="E", pady=YPAD)
cur_pos_display.bind("<Key>", lambda e: "break")


cur_pos_label2 = tk.Label(pos_adj, text="[0.1 deg]")
cur_pos_label2.grid(row=3, column=2, sticky="W", pady=YPAD)

set_hom_btn = tk.Button(pos_adj, text="Home", command=set_home)
set_hom_btn.grid(row=3, column=3,  sticky="EW", pady=YPAD)


prof_sett = ttk.Labelframe(motor, text='profile settings')
prof_sett.grid(row = 2, column = 0, sticky='WE')

#prof_label = tk.Label(prof_sett, text="PROFILE SETTINGS")
#prof_label.grid(row = 2, column = 0, columnspan=5, pady=YPAD)

prof_spd_label = tk.Label(prof_sett, text="profile speed = ")
prof_spd_label.grid(row=3, column=0, sticky="E", pady=YPAD)

prof_acc_label = tk.Label(prof_sett, text="profile acceleration = ")
prof_acc_label.grid(row=4, column=0, sticky="E", pady=YPAD)

prof_dec_label = tk.Label(prof_sett, text="profile deceleration = ")
prof_dec_label.grid(row=5, column=0, sticky="E", pady=YPAD)

prof_spd_entry = tk.Entry(prof_sett, justify='right')
prof_spd_entry.grid(row=3, column=1, sticky="E", pady=YPAD)

prof_acc_entry = tk.Entry(prof_sett, justify='right')
prof_acc_entry.grid(row=4, column=1, sticky="E", pady=YPAD)

prof_dec_entry = tk.Entry(prof_sett, justify='right')
prof_dec_entry.grid(row=5, column=1, sticky="E", pady=YPAD)

prof_spd_label2 = tk.Label(prof_sett, text="[rpm]")
prof_spd_label2.grid(row=3, column=2, sticky="W", pady=YPAD)

prof_acc_label2 = tk.Label(prof_sett, text="[rpm/s]")
prof_acc_label2.grid(row=4, column=2, sticky="W", pady=YPAD)

prof_dec_label2 = tk.Label(prof_sett, text="[rpm/s]")
prof_dec_label2.grid(row=5, column=2, sticky="W", pady=YPAD)

prof_get_btn = tk.Button(prof_sett, text="Get", command=get_profile)
prof_get_btn.grid(row=5, column=3, sticky="W", pady=YPAD, padx=BPAD)

prof_set_btn = tk.Button(prof_sett, text="Set", command=set_profile)
prof_set_btn.grid(row=5, column=4, sticky="W", pady=YPAD, padx=BPAD)

op_sett = ttk.Labelframe(motor, text='operation settings')
op_sett.grid(row=3, column=0, sticky='WE')

#op_label = tk.Label(op_sett, text="OPERATION SETTINGS")
#op_label.grid(row = 2, column = 0, columnspan=5, pady=YPAD)

op_wait0_label = tk.Label(op_sett, text="pre wait = ")
op_wait0_label.grid(row=2, column=0, sticky="E", pady=YPAD)

op_half_label = tk.Label(op_sett, text="half angle = ")
op_half_label.grid(row=3, column=0, sticky="E", pady=YPAD)

op_wait1_label = tk.Label(op_sett, text="half wait = ")
op_wait1_label.grid(row=4, column=0, sticky="E", pady=YPAD)

op_full_label = tk.Label(op_sett, text="full angle = ")
op_full_label.grid(row=5, column=0, sticky="E", pady=YPAD)

op_wait2_label = tk.Label(op_sett, text="full wait = ")
op_wait2_label.grid(row=6, column=0, sticky="E", pady=YPAD)

op_wait0_entry = tk.Entry(op_sett, justify='right')
op_wait0_entry.grid(row=2, column=1, sticky="E", pady=YPAD)

op_half_entry = tk.Entry(op_sett, justify='right')
op_half_entry.grid(row=3, column=1, sticky="E", pady=YPAD)

op_wait1_entry = tk.Entry(op_sett, justify='right')
op_wait1_entry.grid(row=4, column=1, sticky="E", pady=YPAD)

op_full_entry = tk.Entry(op_sett, justify='right')
op_full_entry.grid(row=5, column=1, sticky="E", pady=YPAD)

op_wait2_entry = tk.Entry(op_sett, justify='right')
op_wait2_entry.grid(row=6, column=1, sticky="E", pady=YPAD)

op_wait0_label2 = tk.Label(op_sett, text="[ms]")
op_wait0_label2.grid(row=2, column=2, sticky="W", pady=YPAD)

op_half_label2 = tk.Label(op_sett, text="[0.1 deg]")
op_half_label2.grid(row=3, column=2, sticky="W", pady=YPAD)

op_wait1_label2 = tk.Label(op_sett, text="[ms]")
op_wait1_label2.grid(row=4, column=2, sticky="W", pady=YPAD)

op_full_label2 = tk.Label(op_sett, text="[0.1 deg]")
op_full_label2.grid(row=5, column=2, sticky="W", pady=YPAD)

op_wait2_label2 = tk.Label(op_sett, text="[ms]")
op_wait2_label2.grid(row=6, column=2, sticky="W", pady=YPAD)

op_get_btn = tk.Button(op_sett, text="Get", command=get_operation)
op_get_btn.grid(row=6, column=3, sticky="W", pady=YPAD, padx=BPAD)

op_set_btn = tk.Button(op_sett, text="Set", command=set_operation)
op_set_btn.grid(row=6, column=4, sticky="W", pady=YPAD, padx=BPAD)

motor_stat = ttk.Labelframe(motor, text='motor status')
motor_stat.grid(row=4, column=0, sticky="WE")

#stat_label = tk.Label(motor_stat, text="MOTOR STATUS")
#stat_label.grid(row=0, column=0, columnspan=5, pady=YPAD)

stat_power = tk.Button(motor_stat, text="Power", bg='white', fg='black', command=startup)
stat_power.grid(row=1, column=0, pady=YPAD)

stat_enabled = tk.Button(motor_stat, text="Enabled", bg='white', fg='black', command=enable)
stat_enabled.grid(row=1, column=1, pady=YPAD)

stat_fault = tk.Button(motor_stat, text="Fault", bg='white', fg='black', command=fstartup)
stat_fault.grid(row=1, column=2, pady=YPAD)

stat_targ = tk.Button(motor_stat, text="Target reached", bg='white', fg='black')
stat_targ.grid(row=1, column=3, pady=YPAD)

err_label = tk.Label(motor_stat, text="Error = ")
err_label.grid(row=1, column=4, pady=YPAD)

err_cod = tk.Entry(motor_stat, width=5)
err_cod.grid(row=1, column = 5, sticky="E", pady=YPAD)
err_cod.bind("<Key>", lambda e: "break")

psu_label = tk.Label(motor_stat, text="Psu = ")
psu_label.grid(row=1, column=6, pady=YPAD)

psu_cod = tk.Entry(motor_stat, width=5)
psu_cod.grid(row=1, column = 7, sticky="E", pady=YPAD)
psu_cod.bind("<Key>", lambda e: "break")

psu2_cod = tk.Label(motor_stat, text="[V]")
psu2_cod.grid(row=1, column = 8, sticky="E", pady=YPAD)

obj = ttk.Labelframe(motor, text="object access")
obj.grid(row=5, column = 0, sticky="WE")

obj_index = tk.Entry(obj, width=10, justify="right")
obj_index.grid(row=0, column = 0, sticky="E", pady=YPAD)

obj_subindex = tk.Entry(obj, width=5, justify="right")
obj_subindex.grid(row=0, column = 1, sticky="E", pady=YPAD)

obj_label = tk.Label(obj, text=" : ")
obj_label.grid(row=0, column = 2, sticky="E", pady=YPAD)

obj_in = tk.Entry(obj, width=10, justify="right")
obj_in.grid(row=0, column = 3, sticky="E", pady=YPAD)

obj_get = tk.Button(obj, text="Get", command=get_obj)
obj_get.grid(row=0, column = 4, sticky="W", pady=YPAD, padx=BPAD)

obj_set = tk.Button(obj, text="Set", command=set_obj)
obj_set.grid(row=0, column = 5, sticky="W", pady=YPAD, padx=BPAD)

obj_out = tk.Entry(obj, width=10, justify="right")
obj_out.grid(row=0, column = 6, sticky="E", pady=YPAD)
obj_out.bind("<Key>", lambda e: "break")

obj_out_int = tk.Entry(obj, width=10, justify="right")
obj_out_int.grid(row=0, column = 7, sticky="E", pady=YPAD)
obj_out_int.bind("<Key>", lambda e: "break")

mot_ctrl = ttk.Labelframe(motor, text='motor operation')
mot_ctrl.grid(row=6, column=0, sticky="WE")

#mot_ctrl_label = tk.Label(mot_ctrl, text="MOTOR OPERATION")
#mot_ctrl_label.grid(row=0, column=0, columnspan=4, sticky="WE", pady=YPAD)

saf_but = tk.Button(mot_ctrl, text="Safety", bg='orange', command=safety_toggle)
saf_but.grid(row=1, column=0, sticky="WE", pady=YPAD, padx=BPAD)

operation_but = tk.Button(mot_ctrl, text="Operation", state='disabled', command=operation)
operation_but.grid(row=1, column=2, sticky="WE", pady=YPAD, padx=BPAD)

homing_but = tk.Button(mot_ctrl, text="Homing", state='disabled', command=homing)
homing_but.grid(row=1, column=3, sticky="WE", pady=YPAD, padx=BPAD)

abort_but = tk.Button(mot_ctrl, text="ABORT", bg='red', command=abort)
abort_but.grid(row=1, column=4, sticky="WE", pady=YPAD, padx=BPAD)

pres1_label = tk.Label(sensor, text='pressure 1 = ')
pres1_label.grid(row=0, column=0, sticky="E", pady=YPAD)

pres1_entry = tk.Entry(sensor, justify='right')
pres1_entry.grid(row=0, column = 1, sticky="E", pady=YPAD)
pres1_entry.bind("<Key>", lambda e: "break")

pres1_label2 = tk.Label(sensor, text='[RAW]')
pres1_label2.grid(row=0, column=2, sticky="E", pady=YPAD)

pres2_label = tk.Label(sensor, text='pressure 2 = ')
pres2_label.grid(row=1, column=0, sticky="E", pady=YPAD)

pres2_entry = tk.Entry(sensor, justify='right')
pres2_entry.grid(row=1, column = 1, sticky="E", pady=YPAD)
pres2_entry.bind("<Key>", lambda e: "break")

pres2_label2 = tk.Label(sensor, text='[RAW]')
pres2_label2.grid(row=1, column=2, sticky="E", pady=YPAD)

temp1_label = tk.Label(sensor, text='temperature 1 = ')
temp1_label.grid(row=2, column=0, sticky="E", pady=YPAD)

temp1_entry = tk.Entry(sensor, justify='right')
temp1_entry.grid(row=2, column = 1, sticky="E", pady=YPAD)
temp1_entry.bind("<Key>", lambda e: "break")

temp1_label2 = tk.Label(sensor, text='[RAW]')
temp1_label2.grid(row=2, column=2, sticky="E", pady=YPAD)

temp2_label = tk.Label(sensor, text='temperature 2 = ')
temp2_label.grid(row=3, column=0, sticky="E", pady=YPAD)

temp2_entry = tk.Entry(sensor, justify='right')
temp2_entry.grid(row=3, column = 1, sticky="E", pady=YPAD)
temp2_entry.bind("<Key>", lambda e: "break")

temp2_label2 = tk.Label(sensor, text='[RAW]')
temp2_label2.grid(row=3, column=2, sticky="E", pady=YPAD)

temp3_label = tk.Label(sensor, text='temperature 3 = ')
temp3_label.grid(row=4, column=0, sticky="E", pady=YPAD)

temp3_entry = tk.Entry(sensor, justify='right')
temp3_entry.grid(row=4, column = 1, sticky="E", pady=YPAD)
temp3_entry.bind("<Key>", lambda e: "break")

temp3_label2 = tk.Label(sensor, text='[RAW]')
temp3_label2.grid(row=4, column=2, sticky="E", pady=YPAD)

torq_label = tk.Label(sensor, text='torque = ')
torq_label.grid(row=5, column=0, sticky="E", pady=YPAD)

torq_entry = tk.Entry(sensor, justify='right')
torq_entry.grid(row=5, column = 1, sticky="E", pady=YPAD)
torq_entry.bind("<Key>", lambda e: "break")

torq_label2 = tk.Label(sensor, text='[Nm]')
torq_label2.grid(row=5, column=2, sticky="E", pady=YPAD)

#matplotlib graph stuff



fig = Figure(figsize=(3, 3), dpi=100)
ax1 = fig.add_axes([0.2, 0.57, 0.75, 0.4])
ax2 = fig.add_axes([0.2, 0.15, 0.75, 0.4])
ax1.set_xticks([])
ax1.set_ylabel("Pressure [RAW]")
ax2.set_ylabel("Temperature [RAW]")
ax2.set_xlabel("Time [ms]")


def data_update(i):
    ax1.clear()
    ax2.clear()
    ax1.set_xticks([])
    ax1.set_ylabel("Pressure [RAW]")
    ax2.set_ylabel("Temperature [RAW]")
    ax2.set_xlabel("Time [ms]")
    ax1.set_xlim(-heart_beat*max_samples-100, 100)
    ax2.set_xlim(-heart_beat*max_samples-100, 100)
    ax1.plot(np.array(time_data)-time_current, pres1_data, label='1')
    ax1.plot(np.array(time_data)-time_current, pres2_data, label='2')
    ax2.plot(np.array(time_data)-time_current, temp1_data, label='1')
    ax2.plot(np.array(time_data)-time_current, temp2_data, label='2')
    ax2.plot(np.array(time_data)-time_current, temp3_data, label='3')

plot_canvas = FigureCanvasTkAgg(fig, master=sensor)  # A tk.DrawingArea.
plot_canvas.draw()
plot_canvas.get_tk_widget().grid(row=6, column=0, columnspan=3, sticky="E", pady=YPAD)
ani = animation.FuncAnimation(fig, data_update, interval=500)

sol_but = tk.Button(other, text='Solenoid', bg='white', command=toggle_solenoid)
sol_but.grid(row=0, column=0, pady=YPAD)


#plumbing diagram
canv = ttk.Labelframe(window, text='system')
canv.grid(row=0, column=2, rowspan=3, sticky='NSEW')

canvas = tk.Canvas(canv, width=220, height=500)

r_top_x = 70
r_top_y = 100
r_width = 40
r_height = 80
r_arc = 10
#reservoir
canvas.create_line(r_top_x, r_top_y, r_top_x, r_top_y+r_height)
canvas.create_line(r_top_x+r_width, r_top_y, r_top_x+r_width, r_top_y+r_height)
canvas.create_arc(r_top_x, r_top_y-r_arc, r_top_x+r_width, r_top_y+r_arc, style=tk.ARC, start=0, extent = 180)
canvas.create_arc(r_top_x, r_top_y+r_arc+r_height, r_top_x+r_width, r_top_y+r_height-r_arc, style=tk.ARC, start=-180, extent = 180)

sol_height = 20
sol_x = r_top_x+r_width/2
canvas.create_line(sol_x, r_top_y-r_arc, sol_x, r_top_y-r_arc-sol_height)
solenoid_draw = canvas.create_line(solen_bbox(math.radians(0)))
canvas.create_line(sol_x, r_top_y-r_arc-sol_height-5, sol_x, r_top_y-r_arc-2*sol_height-5)


l_top_x = r_top_x+r_width/2
l_top_y = r_top_y+r_height+r_arc
l_height = 20

canvas.create_line(l_top_x, l_top_y, l_top_x, l_top_y+l_height)


o_rad = 10
o_top_x = l_top_x-o_rad
o_top_y = l_top_y+l_height

canvas.create_oval(o_top_x, o_top_y, o_top_x+2*o_rad, o_top_y+2*o_rad)


valve_draw = canvas.create_line(valve_bbox(math.radians(0)))

l2_top_x = l_top_x
l2_top_y = o_top_y+2*o_rad
l2_height = 20

canvas.create_line(l2_top_x, l2_top_y, l2_top_x, l2_top_y+l2_height)

s_width = 30
s_height = 100
s_top_x = l_top_x-s_width/2
s_top_y = l2_top_y+l2_height

canvas.create_line(s_top_x, s_top_y, s_top_x+s_width, s_top_y)
canvas.create_line(s_top_x, s_top_y+s_height, s_top_x+s_width, s_top_y+s_height)

canvas.create_line(s_top_x, s_top_y, s_top_x, s_top_y+s_height)
canvas.create_line(s_top_x+s_width, s_top_y, s_top_x+s_width, s_top_y+s_height)

b_width = 20
b_height = 50
b_top_x = s_top_x
b_end_x = s_top_x+s_width
b_top_y = s_top_y+s_height

canvas.create_arc(b_top_x, b_top_y, b_top_x+b_width, b_top_y+b_height, style=tk.ARC, start=180, extent = -90)
canvas.create_arc(b_end_x-b_width, b_top_y, b_end_x, b_top_y+b_height, style=tk.ARC, start=0, extent = 90)
canvas.create_line(b_top_x, b_top_y+b_height/2, b_end_x, b_top_y+b_height/2)

f_top_x = b_top_x+6
f_top_y = b_top_y+b_height/2+5
f_width = b_width
f_height= 40
b_flame_bm = tk.BitmapImage(file="b_flame.xbm", foreground='red')
b_flame = canvas.create_image(f_top_x, f_top_y, image=b_flame_bm, anchor=tk.NW, state='hidden')
s_flame_bm = tk.BitmapImage(file="s_flame.xbm", foreground='orange')
s_flame = canvas.create_image(f_top_x, f_top_y, image=s_flame_bm, anchor=tk.NW, state='hidden')


canvas.grid(row=0, column=0, sticky='NSEW')

main_update()


window.mainloop()
