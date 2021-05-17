T = readtable("test0.csv");

hold on
plot(T.(15), T.(10))
plot(T.(15), T.(11))
legend("actual position", "demand position")

xlabel("time [ms]");
ylabel("position [inc]");
hold off

figure;
hold on
plot(T.(15), T.(12))
plot(T.(15), T.(13))
legend("actual current", "demand current")

xlabel("time [ms]");
ylabel("current [mA]");
hold off


figure;
hold on
plot(T.(15), T.(14))
legend("actual velocity")

xlabel("time [ms]");
ylabel("velocity [rpm]");
hold off


figure;
hold on
plot(T.(15), T.(9))
legend("actual torque")
hold off
