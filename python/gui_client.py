import os
import tkinter as tk
import tkinter.ttk as ttk
import serial

#functions

safety = 1

def move_rel():
    pass

def move_abs():
    pass

def set_home():
    pass

def update_profile():
    pass

def update_operation():
    pass

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
    pass

def homing():
    pass

def abort():
    pass

def main_update():
    pass















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

mov_rel_btn = tk.Button(pos_adj, text="Relative")
mov_rel_btn.grid(row=1, column=3, sticky="EW", pady=YPAD)

mov_rel_btn = tk.Button(pos_adj, text="Absolute")
mov_rel_btn.grid(row=1, column=4, sticky="EW", pady=YPAD)


cur_pos_label = tk.Label(pos_adj, text="current position = ")
cur_pos_label.grid(row=3, column = 0, sticky="E", pady=YPAD)

cur_pos_display = tk.Entry(pos_adj, justify='right')
cur_pos_display.grid(row=3, column = 1, sticky="E", pady=YPAD)
cur_pos_display.bind("<Key>", lambda e: "break")


cur_pos_label2 = tk.Label(pos_adj, text="[0.1 deg]")
cur_pos_label2.grid(row=3, column=2, sticky="W", pady=YPAD)

set_hom_btn = tk.Button(pos_adj, text="Home")
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

motor_stat = tk.Frame(motor)
motor_stat.grid(row=4, column=0, sticky="WE")

stat_label = tk.Label(motor_stat, text="MOTOR STATUS")
stat_label.grid(row=0, column=0, columnspan=5, pady=YPAD)

stat_power = tk.Button(motor_stat, text="Power", bg='white', fg='black', state='disabled')
stat_power.grid(row=1, column=0, pady=YPAD)

stat_enabled = tk.Button(motor_stat, text="Enabled", bg='white', fg='black', state='disabled')
stat_enabled.grid(row=1, column=1, pady=YPAD)

stat_fault = tk.Button(motor_stat, text="Fault", bg='white', fg='black', state='disabled')
stat_fault.grid(row=1, column=2, pady=YPAD)

stat_targ = tk.Button(motor_stat, text="Target reached", bg='white', fg='black', state='disabled')
stat_targ.grid(row=1, column=3, pady=YPAD)

err_label = tk.Label(motor_stat, text="Error code = ")
err_label.grid(row=1, column=4, pady=YPAD)

err_label = tk.Entry(motor_stat)
err_label.grid(row=1, column = 5, sticky="E", pady=YPAD)
err_label.bind("<Key>", lambda e: "break")


mot_ctrl = tk.Frame(motor)
mot_ctrl.grid(row=5, column=0, sticky="WE")

mot_ctrl_label = tk.Label(mot_ctrl, text="MOTOR OPERATION")
mot_ctrl_label.grid(row=0, column=0, columnspan=4, sticky="WE", pady=YPAD)

saf_but = tk.Button(mot_ctrl, text="Safety", bg='orange', command=safety_toggle)
saf_but.grid(row=1, column=0, sticky="WE", pady=YPAD, padx=BPAD)

operation_but = tk.Button(mot_ctrl, text="Operation", state='disabled')
operation_but.grid(row=1, column=2, sticky="WE", pady=YPAD, padx=BPAD)

homing_but = tk.Button(mot_ctrl, text="Homing", state='disabled')
homing_but.grid(row=1, column=3, sticky="WE", pady=YPAD, padx=BPAD)

abort_but = tk.Button(mot_ctrl, text="ABORT", bg='red')
abort_but.grid(row=1, column=4, sticky="WE", pady=YPAD, padx=BPAD)






window.mainloop()
