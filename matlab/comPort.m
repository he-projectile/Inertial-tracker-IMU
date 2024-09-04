%clc;
clear;
figure(5)
clf(figure(5))
hold on; grid on;
% writematrix([], 'IMU_DATA.txt', 'WriteMode', 'overwrite');
axis equal;
view([30,40]); %azimuth, elevation
title('Инерциальный трекер');
xlabel('x');ylabel('y');zlabel('z');

basis=[1 0 0; 0 1 0; 0 0 1];
drawBasis([0 0 0],basis, 0.1);
plot3(0,0,0,'ob');
xlim([-0.125 0.125])
ylim([-0.125 0.125])
zlim([-0.125 0.125])
% xlim([-1 1])
% ylim([-1 1])
% zlim([-1 1])

IMU = serialport("COM3",115200,"Timeout",15);
PCazimuth=15;
north=[1; 0; 0];
north=RotZ(PCazimuth)*north;
quiver(0,0, north(1), north(2), 0.1, 'r--');

acc=[0 0 0];
gyro=[0 0 0];
period=0;
mag=[0 0 0];
magInit=[0 0 0];
magOrig=[0 0 0];
accOffs=[(-10 +10)/2, (-9.8 +10.25)/2, (-9.28 +10.88)/2];
accMult=[20/(10+10), 20/(9.8+10.25), 20/(9.28+10.88)];
gyroOffs=[1.5807 1.6857 0.3176];
gyroMult=[1 1 1];
magOffs=[(-360+212)/2,(-280+285)/2, (-170+397)/2];
magMult=[500/(212+360), 500/(280+285), 500/(170+397)];
sensorCorrectionMatrix = [accOffs accMult; gyroOffs gyroMult; magOffs magMult];

cnt = 0;
magCnt=0;

gravity=[0 0 0];
velocity=zeros(3,1);
coordinate=zeros(3,1);

staticCnt=0;

disp('выравнивание по вертикали')
for i=1:100
   [acc(1),acc(2),acc(3), gyro(1), gyro(2), gyro(3), period, mag(1), mag(2), mag(3)]=getIMUdata(IMU,sensorCorrectionMatrix);
   gravity=gravity+acc;
   if(norm(mag)~=0)
    magInit=magInit+mag;
    magCnt = magCnt+1;
   end
end
gravity = -gravity./100;
gravity = gravity';
GRAVITY = [0; 0; -norm(gravity)];
l=cross(gravity, GRAVITY);
l=l/norm(l);
c=dot(gravity, GRAVITY)/(norm(gravity)*norm(gravity));
s=sqrt(1-c^2);
lx=l(1); ly=l(2); lz=l(3);
Rinit=[ c+(1-c)*lx^2 (1-c)*lx*ly-s*lz (1-c)*lz*lx+s*ly; (1-c)*lx*ly+s*lz c+(1-c)*ly^2 (1-c)*lz*ly-s*lx; (1-c)*lx*lz-s*ly (1-c)*ly*lz+s*lx c+(1-c)*lz^2];
basis=Rinit*basis;
gravity=basis*gravity;
norm(gravity)
clear GRAVITY i l lx ly lz c s Rinit

disp('выравнивание по азимуту')
magInit=magInit/magCnt;
magInit=basis*magInit';
magInit(3) = 0;
magInit=magInit/(norm(magInit));
theta = -angle(north, magInit);
rad2deg(theta)
Rmag=[cos(theta) -sin(theta) 0; sin(theta) cos(theta) 0; 0 0 1];
basis=Rmag*basis;
clear Rmag magInit magCnt theta


prewBas=[];
prewVec=[];
disp('готово')
while 1
    [accTmp(1),accTmp(2),accTmp(3), gyroTmp(1), gyroTmp(2), gyroTmp(3), period, mag(1), mag(2), mag(3)]=getIMUdata(IMU,sensorCorrectionMatrix);
    
    acc(1) = myFilt(accTmp(1), acc(1), 0.5);
    acc(2) = myFilt(accTmp(2), acc(2), 0.5);
    acc(3) = myFilt(accTmp(3), acc(3), 0.5);
    gyro(1) = myFilt(gyroTmp(1), gyro(1), 5);
    gyro(2) = myFilt(gyroTmp(2), gyro(2), 5);
    gyro(3) = myFilt(gyroTmp(3), gyro(3), 5);
    
    gyro=gyro.*period; 
    accOrig=basis*acc';
    basis = restoreVertical(basis, -gravity, accOrig);
    accOrig = accOrig+gravity;  
    
    if(norm(mag) ~= 0)  
        magOrig=basis*mag';
        magOrig(3)=0;
        magOrig=magOrig/(norm(magOrig));
    end
%    basis=restoreNorth(basis, mag, north);
    
    basis=basis*rotate(gyro(1),gyro(2),gyro(3),mod(cnt,2));     

    if mod(cnt,5)==0
       delete(prewBas); delete(prewVec);
       prewBas = drawBasis(coordinate, basis, 0.05);
       prewVec = drawVector(coordinate, accOrig, 0.01, 'b--'); 
    end  

    if (abs(acc(1))>19 || abs(acc(2))>19 || abs(acc(3))>19)
        disp('overload acc');
    end
    if (abs(gyro(1))>deg2rad(180) || abs(gyro(2))>deg2rad(180) || abs(gyro(3))>deg2rad(180))
        disp('overload gyro');
    end    
%     if ( norm(gyro) < 0.005)
%         staticCnt = staticCnt+1;
%         if (staticCnt == 10)
%             accOrig = accOrig.*0;
%             velocity = velocity*.0;
%             disp('stopped')
%         end
%     else     
%         coordinate=coordinate+velocity.*period+accOrig.*(period^2/2);
%         velocity=velocity+accOrig.*period; 
%         drawPoint(coordinate);
%         if mod(cnt,40)==0
%             drawBasis(coordinate, basis, 0.03);
%             drawVector(coordinate, accOrig, 0.01, 'm');
%             drawVector(coordinate, velocity, 0.1, 'm-.');
%         end    
%         staticCnt = 0;
%     end 
    


    cnt = cnt+1;
end

function [ACx, ACy, ACz, GYx, GYy, GYz, interval, MAx, MAy, MAz]=getIMUdata(port, corrects)
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
        
%        writematrix([ACx, ACy, ACz, GYx, GYy, GYz, interval, MAx, MAy, MAz], 'IMU_DATA.txt', 'WriteMode', 'append');
        
        ACx=(ACx-corrects(1,1))*corrects(1,4);
        ACy=(ACy-corrects(1,2))*corrects(1,5);
        ACz=(ACz-corrects(1,3))*corrects(1,6);
        GYx=(GYx-corrects(2,1))*corrects(2,4);
        GYy=(GYy-corrects(2,2))*corrects(2,5);
        GYz=(GYz-corrects(2,3))*corrects(2,6);
        if (norm([MAx, MAy, MAz]) ~=0)
            MAx=(MAx-corrects(3,1))*corrects(3,4);
            MAy=(MAy-corrects(3,2))*corrects(3,5);
            MAz=(MAz-corrects(3,3))*corrects(3,6);
        end
    else
        flush(port)
    end    
end
%матрицы поворота
function Q=rotate(x,y,z,d)
Rx=RotX(x);
Ry=RotY(y);
Rz=RotZ(z);
if d==0
 Q=(Rx*Ry)*Rz;
else 
 Q=(Rz*Ry)*Rx;
end
end
%вращение вокруз 0X
function f = RotX(a)
    a = deg2rad(a);
    f=[1 0 0; 0 cos(a) -sin(a); 0 sin(a) cos(a)];
end
%вращение вокруз 0Y
function f = RotY(a)
    a = deg2rad(a);
    f=[cos(a) 0 sin(a); 0 1 0; -sin(a) 0 cos(a)];
end
%вращение вокруз 0Z
function f = RotZ(a)
    a = deg2rad(a);
    f=[cos(a) -sin(a) 0; sin(a) cos(a) 0; 0 0 1];
end
%отрисовка базиса
function res = drawBasis(coord, bas, scale)
       res = quiver3(coord(1),coord(2),coord(3), bas(1,1),bas(2,1),bas(3,1), scale,'r');
       res = [res,quiver3(coord(1),coord(2),coord(3), bas(1,2),bas(2,2),bas(3,2), scale,'g')];
       res = [res,quiver3(coord(1),coord(2),coord(3), bas(1,3),bas(2,3),bas(3,3), scale,'b')];
end
function res =  drawVector(cord,vec,scale,style)
       res = quiver3(cord(1),cord(2),cord(3), vec(1),vec(2),vec(3), scale,style);
end
function drawPoint(coord)
        plot3(coord(1),coord(2),coord(3),'.c');
end
function alpha = angle(x, y)
    thirdVec=cross(x,y);
    if (thirdVec(3) ~= 0)
        alpha=acos( dot(x,y)/( norm(x)*norm(y) ) )* sign(thirdVec(3));
    else 
        alpha=acos( dot(x,y)/( norm(x)*norm(y) ) );
    end
end
function a = myFilt(x, prevX, d)
    err = x-prevX;
    a = prevX + err*(1-2.5/sqrt(2*pi)*exp(-err^2/(2*d^2)));
end
function b=restoreVertical(bas, grav, acc)
    if( norm(acc) > 9.8 && norm(acc) <  10.2)
        l=cross(acc, grav);
        l=l/norm(l);
        angle = acos(dot(acc, grav)/(norm(acc)*norm(grav)));
        angle = angle/20;
        c=cos(angle);
        s=sin(angle);
        lx=l(1); ly=l(2); lz=l(3);
        Rinit=[ c+(1-c)*lx^2 (1-c)*lx*ly-s*lz (1-c)*lz*lx+s*ly; (1-c)*lx*ly+s*lz c+(1-c)*ly^2 (1-c)*lz*ly-s*lx; (1-c)*lx*lz-s*ly (1-c)*ly*lz+s*lx c+(1-c)*lz^2];
        b=Rinit*bas;
    else
        b=bas;
    end
end
function b=restoreNorth(bas, mag, north)
    if (norm(mag) ~= 0)     
        magInit=bas*mag';
        magInit(3) = 0;
        magInit=magInit/(norm(magInit));
        theta = -angle(north, magInit);
        theta = theta/20;
        rad2deg(theta);
        Rmag=[cos(theta) -sin(theta) 0; sin(theta) cos(theta) 0; 0 0 1];
        b=Rmag*bas;
    else
       b=bas; 
    end
end