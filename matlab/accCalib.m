clc;
clear;
figure(7)
clf(figure(7))
hold on; grid on;
title('Калибровка акселерометра');
numOfMes = 800;
xlim([0 numOfMes]);
ylim([-25 25])

IMU = serialport("COM3",115200,"Timeout",5);

acc=[0 0 0];
gyro=[0 0 0];
period=0;
mag=[0 0 0];
cnt=1;
firstHalf=zeros(3,numOfMes/2);
secHalf=zeros(3,numOfMes/2);
halfSelect=0;

accFilt=0;
accFilt2=0

while 1
%     [acc(1),acc(2),acc(3), gyro(1), gyro(2), gyro(3), period, mag(1), mag(2), mag(3)]=getIMUdata(IMU);
    [accTmp(1),accTmp(2),accTmp(3), gyroTmp(1), gyroTmp(2), gyroTmp(3), period, mag(1), mag(2), mag(3)]=getIMUdata(IMU);
%     accFilt = myFilt(gyro(1), accFilt, 15);
%     accFilt2 = myFilt(gyro(1), accFilt2, 20);
    acc(1) = myFilt(accTmp(1), acc(1), 0.5);
    acc(2) = myFilt(accTmp(2), acc(2), 0.5);
    acc(3) = myFilt(accTmp(3), acc(3), 0.5);
    gyro(1) = myFilt(gyroTmp(1), gyro(1), 10);
    gyro(2) = myFilt(gyroTmp(2), gyro(2), 10);
    gyro(3) = myFilt(gyroTmp(3), gyro(3), 10);   
    
    gyro=gyro.*period; 
    
    
    if(cnt<=numOfMes/2)
        firstHalf(:,cnt)=plot(cnt,norm(gyro),'.r', cnt,5*(norm(acc)-10),'.g', cnt,0,'.b');
    end
    if(cnt>numOfMes/2)
        secHalf(:,cnt-200)=plot(cnt,norm(gyro),'.r', cnt,5*(norm(acc)-10),'.g', cnt,0,'.b');
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
    a = prevX + err*(1-2.5/sqrt(2*pi)*exp(-err^2/(2*d^2)));
end