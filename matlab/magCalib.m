clc;
clear;
figure(4)
clf(figure(4))
hold on; grid on;
title('Калибровка магнитометра');
numOfMes = 800;
xlim([0 numOfMes]);
ylim([-500 500])

IMU = serialport("COM10",115200,"Timeout",5);

acc=[0 0 0];
gyro=[0 0 0];
period=0;
mag=[0 0 0];
cnt=1;
firstHalf=zeros(3,numOfMes/2);
secHalf=zeros(3,numOfMes/2);
halfSelect=0;

while 1
   magTmp=[0 0 0];
   [acc(1),acc(2),acc(3), gyro(1), gyro(2), gyro(3), period, mag(1), mag(2), mag(3)]=getIMUdata(IMU);
   if (norm(mag) ~= 0)
    if(cnt<=numOfMes/2)
        firstHalf(:,cnt)=plot(cnt,mag(1),'.r', cnt,mag(2),'.g', cnt,mag(3),'.b');
    end
    if(cnt>numOfMes/2)
        secHalf(:,cnt-200)=plot(cnt,mag(1),'.r', cnt,mag(2),'.g', cnt,mag(3),'.b');
    end    
    if (cnt > numOfMes/2 && halfSelect==false)
       delete(secHalf);
       halfSelect = true;
    end
    if (cnt < numOfMes/2 && halfSelect==true)
       delete(firstHalf);
       halfSelect = false;
    end    
    cnt = cnt+1;
    if cnt > numOfMes
        cnt = 1;
    end
   end
end


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