clc;
clear;
figure(6)
clf(figure(6))
numOfMes = 800;

IMU = serialport("COM8",115200,"Timeout",5);

acc=zeros(1000,3);
gyro=zeros(1000,3);
mag=[];

disp('не трогайте датчик');
pause(1);
disp('началось')

magCnt=1;
for i=1:1000
   magTmp=[0 0 0];
   [acc(i,1),acc(i,2),acc(i,3), gyro(i,1), gyro(i,2), gyro(i,3), period, magTmp(1), magTmp(2), magTmp(3)]=getIMUdata(IMU);   
   if norm(magTmp) ~= 0
        mag=[mag ; magTmp];
        magCnt = magCnt+1;
   end
   if mod(i, 100) == 0
       disp(i) 
   end
end

accAvg = mean(acc, 1)
acc = acc - accAvg;
gyroAvg = mean(gyro,1)
gyro = gyro - gyroAvg;
magAvg = mean(mag,1)
mag = mag - magAvg;

%%%%%%%%%%%%%%%%%%%%%%%%%%
subplot(3,3,1)
hold on; title('ACC X');
% xlim([-0.08 0.08])
histogram(acc(:,1),100)

subplot(3,3,2)
hold on; title('ACC Y');
% xlim([-0.08 0.08])
histogram(acc(:,2),100)

subplot(3,3,3)
hold on; title('ACC Z');
% xlim([-0.08 0.08])
histogram(acc(:,3),100)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
subplot(3,3,4)
hold on; title('GYRO X');
% xlim([-0.08 0.08])
histogram(gyro(:,1),100)

subplot(3,3,5)
hold on; title('GYRO Y');
% xlim([-0.08 0.08])
histogram(gyro(:,2),100)

subplot(3,3,6)
hold on; title('GYRO Z');
% xlim([-0.08 0.08])
histogram(gyro(:,3),100)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
subplot(3,3,7)
hold on; title('MAG X');
% xlim([-0.08 0.08])
histogram(mag(:,1),100)

subplot(3,3,8)
hold on; title('MAG Y');
% xlim([-0.08 0.08])
histogram(mag(:,2),100)

subplot(3,3,9)
hold on; title('MAG Z');
% xlim([-0.08 0.08])
histogram(mag(:,3),100)


function [ACx, ACy, ACz, GYx, GYy, GYz, interval, MAx, MAy, MAz]=getIMUdata(port)
ACx=0; ACy=0; ACz=0; GYx=0; GYy=0; GYz=0; interval=0; MAx=0; MAy=0; MAz=0;
    s=read(port,11,'int16');
    if(s(8) == 23157)
        ACx=s(1)/1638.4;
        ACy=s(2)/1638.4;
        ACz=s(3)/1638.4;
        GYx=s(4)/32.8;
        GYy=s(5)/32.8;
        GYz=s(6)/32.8;
        interval=s(7)/1000000;
%         s(8);
        MAx=s(10);
        MAy=s(9);
        MAz=-s(11);
        
    else
        flush(port)
    end    
end

function a = myFilt(x, prevX, d)
    err = x-prevX;
    a = x - err*(1-2.5/sqrt(2*pi)*exp(-x^2/(2*d^2)));
end