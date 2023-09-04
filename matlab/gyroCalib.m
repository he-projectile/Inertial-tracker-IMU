clc;
clear;
disp('калибровка гироскопа')

IMU = serialport("COM8",115200,"Timeout",5);
acc=[0 0 0];
gyro=[0 0 0];
period=0;
mag=[0 0 0];

gyroSum=[0 0 0];

disp('не трогайте датчик');
pause(3);
disp('началось')

cnt=0;
for i=1:1000
   [acc(1),acc(2),acc(3), gyro(1), gyro(2), gyro(3), period, mag(1), mag(2), mag(3)]=getIMUdata(IMU);
   if norm(gyro) ~= 0
       gyroSum = gyroSum + gyro;
   end
   cnt = cnt + 1;
   if cnt == 800
       break
   end
   if mod(cnt, 100) == 0
      disp(cnt) 
   end
       
end

disp('результат')
gyroSum = gyroSum/cnt


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