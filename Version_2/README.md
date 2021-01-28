# Software for the propulsion subsystem of Bella Lui II




## Features

### Main FSM

#### IDLE
The mode in which the software starts
We will also be in this mode during fueling
Here only the solenoid valve may be actuated
#### CALIBRATION
Find zero on the kulite sensors
Perform main valve motor homing
#### ARMED
Once fueling is done, in this mode the main valve will not open
#### COUNTDOWN
Delay from the global egnition signal to let the ignitor burn a little bit
#### IGNITION
Half valve opening
Half open wait
Full valve opening
#### THRUST
Valve is fully open and controlled by the Thrust control algorithm
#### SHUTDOWN
Valve closing
#### GLIDE
The rocket "glides" in the air control of the airbrakes
#### ABORT
Emergency close of the main valve
#### ERROR
Error state in case there are any problems

### Sensor acquisition

### CAN communication

### Debug communication

### MAXON communication

#### MSV2 Driver
The Maxon Serial V2 protocol is used by the EPOS4 motor control board.
This driver allows the creation and decoding of data packets for the EPOS4

#### EPOS4 Driver
This driver sends the right sequences of commands to the motor to perform the desired actions.





