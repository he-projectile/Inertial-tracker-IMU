%вращение вокруз 0X
function f = RotX(a)
    a = deg2rad(a);
    f=[1 0 0; 0 cos(a) -sin(a); 0 sin(a) cos(a)];
end

