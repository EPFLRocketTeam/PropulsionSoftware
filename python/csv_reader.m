T = readtable("test_2.csv");

hold on
plot(T.(15), T.(10))
plot(T.(15), T.(11))
legend("actual position", "demand position")
hold off
figure;
hold on
plot(T.(15), T.(12))
plot(T.(15), T.(13))
legend("actual current", "demand current")
hold off

figure;
hold on
plot(T.(15), T.(14))
legend("actual velocity")
hold off

figure;
hold on
plot(T.(15), T.(9))
legend("actual torque")
hold off
