function f = RotZ(a)
    a = deg2rad(a);
    f=[cos(a) -sin(a) 0; sin(a) cos(a) 0; 0 0 1];
end