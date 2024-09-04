%вращение вокруз 0Y
function f = RotY(a)
    a = deg2rad(a);
    f=[cos(a) 0 sin(a); 0 1 0; -sin(a) 0 cos(a)];
end