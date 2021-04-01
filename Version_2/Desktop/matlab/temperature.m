

close all
alpha = 6.374e-3;
alpha_1 = 5.6474e-3;
alpha_2 = 6.695e-6;
alpha_3 = 5.688e-9;
V0 = 3.3;
Rref = 1000;
R0 = 1000;

T=(-20:0.01:30)';
dT = T-21.11111;
Rc = R0.*(1+alpha_1.*dT + alpha_2.*(dT.*dT) + alpha_3.*(dT.*dT.*dT));
%Rc = R0.*(1+alpha.*dT);
Vm1 = V0.*(Rc)./(R0+Rc);
Vm2 = V0.*(R0)./(R0+Rc);
[f1, r1] = fit(T,Vm1, 'a*x+b');
[f2, r2] = fit(T,Vm2, 'a*x+b');

l1 = f1(T);
e1 = abs(Vm1-l1);

l2 = f2(T);
e2 = abs(Vm2-l2);

V = (Vm1./3.3).*4096;


L = 1200:1:2799;
D = L;
for i=L
    [ d, ix ] = min( abs( V-i ) );
    D(i-1199) = T(ix);
    
    
end

csvwrite('test.txt',D.')

hold on
plot(T,V)
hold off
title(sprintf("sensor below | sensi: %f [V/K]", f1.a))


figure;
plot(T, Rc)

% figure;
% hold on
% plot(T,Vm2)
% plot(f2)
% plot(T, e1)
% hold off
% title(sprintf("sensor above | sensi: %f [V/K]", f2.a))
