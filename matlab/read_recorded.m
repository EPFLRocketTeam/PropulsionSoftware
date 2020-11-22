close all;
clear all;
clc;

T = readtable("");



sensor_diff = diff(T.("sensor_time"));
motor_diff = diff(T.("motor_time"));

sensor_sampling = mean(sensor_diff(sensor_diff>0));
motor_sampling = mean(motor_diff(motor_diff>0));

fprintf('motor sampling rate: %f ms (%f Hz)\n', motor_sampling, 1/motor_sampling*1000);
fprintf('sensor sampling rate: %f ms (%f Hz)\n', sensor_sampling, 1/sensor_sampling*1000);



hold on
plot(T.("sensor_time"), T.("temp_1")./10)
plot(T.("sensor_time"), T.("temp_2")./10)
plot(T.("sensor_time"), T.("temp_3")./10)

hold off
legend("temp 1", "temp 2", "temp 3");
xlabel("time [ms]");
ylabel("temperature [°C]");
ylim([0 30])

figure;

hold on
plot(T.("sensor_time"), T.("press_1"))
plot(T.("sensor_time"), T.("press_2"))

hold off
legend("press 1", "press 2");
xlabel("time [ms]");
ylabel("pressure [mBar]");
ylim([900 2000]);


figure;
plot(T.("motor_time"), T.("motor_pos"))
xlabel("time [ms]");
ylabel("angle [ddeg]");
ylim([-1000 100]);

figure;
hold on
plot(T.("motor_time"), T.("motor_ipos"))
plot(T.("motor_time"), T.("motor_icmd"))
hold off
legend("motor pos", "motor cmd");
xlabel("time [ms]");
ylabel("pos [inc]");
ylim([-100000 100]);



