import os
import tkinter as tk
import tkinter.ttk as ttk
import serial

#functions

safety = 1
ser = serial.Serial()
connected = 0

def move_rel():
    targ = tmp_target_entry.get()
    out = 'move_rel {}\n'.format(targ)
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))

def move_abs():
    targ = tmp_target_entry.get()
    out = 'move_abs {}\n'.format(targ)
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))

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
    half = op_half_entry.get()
    wait1 = op_wait1_entry.get()
    full = op_full_entry.get()
    wait2 = op_wait2_entry.get()
    out = 'op_profile {} {} {} {}\n'.format(half, wait1, full, wait2)
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))
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
        if(len(data) == 4):
            op_half_entry.delete(0, tk.END)
            op_half_entry.insert(0, data[0])
            op_wait1_entry.delete(0, tk.END)
            op_wait1_entry.insert(0, data[1])
            op_full_entry.delete(0, tk.END)
            op_full_entry.insert(0, data[2])
            op_wait2_entry.delete(0, tk.END)
            op_wait2_entry.insert(0, data[3])

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
    

def get_status():
    out = 'short_stat\n'
    print(out)
    if ser.is_open:
        ser.write(bytes(out, 'ascii'))
        resp = ser.readline().decode('ascii')
        data = resp.split()
        print(data)
        if(len(data) == 7):
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
            psu_cod.delete(0, tk.END)
            psu_cod.insert(0, str(int(data[6])/10.0))
            


def connect():
    global ser
    global connected
    if connected:
        ser.close()
        connected = 0
        serial_but['text']='Connect'
    else:
        com = serial_entry.get()
        ser = serial.Serial(com, 115200)
        if ser.is_open:
            connected = 1
            serial_but['text']='Disconnect'
    

    
heart_beat=1000



def main_update():

    get_status()
    window.after(heart_beat, main_update)















window = tk.Tk()

window.geometry('800x500')
window.title('propulsion client')


motor = ttk.Labelframe(window, text="motor control")
motor.grid(row=0, column=0, rowspan=2, sticky="NSEW")


sensor = ttk.Labelframe(window, text="sensors")
sensor.grid(row=0, column=1, sticky="NSEW")


other = ttk.Labelframe(window, text="other")
other.grid(row=1, column=1, sticky="NSEW")

YPAD = 2
BPAD = 5

pos_adj = tk.Frame(motor)
pos_adj.grid(row=0, column = 0, sticky='WE')

tmp_mov_label = tk.Label(pos_adj, text="POSITION ADJUSTEMENT")
tmp_mov_label.grid(row = 0, column = 0, columnspan=5, pady=YPAD)

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


prof_sett = tk.Frame(motor)
prof_sett.grid(row = 2, column = 0, sticky='WE')

prof_label = tk.Label(prof_sett, text="PROFILE SETTINGS")
prof_label.grid(row = 2, column = 0, columnspan=5, pady=YPAD)

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

op_sett = tk.Frame(motor)
op_sett.grid(row=3, column=0, sticky='WE')

op_label = tk.Label(op_sett, text="OPERATION SETTINGS")
op_label.grid(row = 2, column = 0, columnspan=5, pady=YPAD)

op_half_label = tk.Label(op_sett, text="half angle = ")
op_half_label.grid(row=3, column=0, sticky="E", pady=YPAD)

op_wait1_label = tk.Label(op_sett, text="half wait = ")
op_wait1_label.grid(row=4, column=0, sticky="E", pady=YPAD)

op_full_label = tk.Label(op_sett, text="full angle = ")
op_full_label.grid(row=5, column=0, sticky="E", pady=YPAD)

op_wait2_label = tk.Label(op_sett, text="full wait = ")
op_wait2_label.grid(row=6, column=0, sticky="E", pady=YPAD)

op_half_entry = tk.Entry(op_sett, justify='right')
op_half_entry.grid(row=3, column=1, sticky="E", pady=YPAD)

op_wait1_entry = tk.Entry(op_sett, justify='right')
op_wait1_entry.grid(row=4, column=1, sticky="E", pady=YPAD)

op_full_entry = tk.Entry(op_sett, justify='right')
op_full_entry.grid(row=5, column=1, sticky="E", pady=YPAD)

op_wait2_entry = tk.Entry(op_sett, justify='right')
op_wait2_entry.grid(row=6, column=1, sticky="E", pady=YPAD)

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

motor_stat = tk.Frame(motor)
motor_stat.grid(row=4, column=0, sticky="WE")

stat_label = tk.Label(motor_stat, text="MOTOR STATUS")
stat_label.grid(row=0, column=0, columnspan=5, pady=YPAD)

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


mot_ctrl = tk.Frame(motor)
mot_ctrl.grid(row=5, column=0, sticky="WE")

mot_ctrl_label = tk.Label(mot_ctrl, text="MOTOR OPERATION")
mot_ctrl_label.grid(row=0, column=0, columnspan=4, sticky="WE", pady=YPAD)

saf_but = tk.Button(mot_ctrl, text="Safety", bg='orange', command=safety_toggle)
saf_but.grid(row=1, column=0, sticky="WE", pady=YPAD, padx=BPAD)

operation_but = tk.Button(mot_ctrl, text="Operation", state='disabled', command=operation)
operation_but.grid(row=1, column=2, sticky="WE", pady=YPAD, padx=BPAD)

homing_but = tk.Button(mot_ctrl, text="Homing", state='disabled', command=homing)
homing_but.grid(row=1, column=3, sticky="WE", pady=YPAD, padx=BPAD)

abort_but = tk.Button(mot_ctrl, text="ABORT", bg='red', command=abort)
abort_but.grid(row=1, column=4, sticky="WE", pady=YPAD, padx=BPAD)

motor_conn = tk.Frame(motor)
motor_conn.grid(row=6, column=0)

serial_label= tk.Label(motor_conn, text='Serial port =')
serial_label.grid(row=0, column=0, sticky="WE", pady=YPAD)

serial_entry= tk.Entry(motor_conn)
serial_entry.grid(row=0, column=1, sticky="WE", pady=YPAD)

serial_but= tk.Button(motor_conn, text='Connect', command=connect)
serial_but.grid(row=0, column=2, sticky="WE", pady=YPAD, padx=BPAD)


main_update()


window.mainloop()
