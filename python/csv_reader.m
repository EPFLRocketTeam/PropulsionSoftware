T = readtable("test_2.csv");

hold on
plot(T.(15), T.(10))
plot(T.(15), T.(11))
legend("actual position", "demand position")
xlim([195000 203300]);
xlabel("time [ms]");
ylabel("position [inc]");
hold off
saveas(gcf,'1.png')
figure;
hold on
plot(T.(15), T.(12))
plot(T.(15), T.(13))
legend("actual current", "demand current")
xlim([195000 203300]);
xlabel("time [ms]");
ylabel("current [mA]");
hold off
saveas(gcf,'2.png')

figure;
hold on
plot(T.(15), T.(14))
legend("actual velocity")
xlim([195000 203300]);
xlabel("time [ms]");
ylabel("velocity [rpm]");
hold off
saveas(gcf,'3.png')

figure;
hold on
plot(T.(15), T.(9))
legend("actual torque")
hold off
