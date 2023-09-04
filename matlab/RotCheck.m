clc;
clear;
figure(5)
clf(figure(5))
hold on; grid on;
axis equal;
view([30,40]); %azimuth, elevation
title('Инерциальный трекер');
xlabel('x');ylabel('y');zlabel('z');

basis=eye(3);
drawBasis([0 0 0],basis, 0.1);
plot3(0,0,0,'ob');
xlim([-0.125 0.125])
ylim([-0.125 0.125])
zlim([-0.125 0.125])

basis=basis*rotz(90);
drawBasis([0 0 0],basis, 0.05);
r=basis*rotx(45);
basis=r*basis;
drawBasis([0 0 0],basis, 0.05);


%отрисовка базиса
function res = drawBasis(coord, bas, scale)
       res = quiver3(coord(1),coord(2),coord(3), bas(1,1),bas(2,1),bas(3,1), scale,'r');
       res = [res,quiver3(coord(1),coord(2),coord(3), bas(1,2),bas(2,2),bas(3,2), scale,'g')];
       res = [res,quiver3(coord(1),coord(2),coord(3), bas(1,3),bas(2,3),bas(3,3), scale,'b')];
end