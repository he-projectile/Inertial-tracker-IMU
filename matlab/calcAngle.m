clc; clear;

a=[1 0 0];
b1=[1 1 0]; b2=[0 1 0]; b3=[-1 1 0]; b4=[-1 0 0]; b5=[-1 -1 0]; b6=[0 -1 0]; b7=[1 -1 0]; b8=[1 0 0];

angle(a, b1)
angle(a, b2)
angle(a, b3)
angle(a, b4)
angle(a, b5)
angle(a, b6)
angle(a, b7)
angle(a, b8)


function alpha = angle(x, y)
    thirdVec=cross(x,y);
    if (thirdVec(3) ~= 0)
        alpha=acos( dot(x,y)/( norm(x)*norm(y) ) )* sign(thirdVec(3));
    else 
        alpha=acos( dot(x,y)/( norm(x)*norm(y) ) );
    end
    alpha = rad2deg(alpha);
end


function alpha = scal(x, y)
    alpha=( dot(x,y)/( norm(x)*norm(y) ) );
end

function alpha = vec(x, y)
    alpha=( norm(cross(x,y))/( norm(x)*norm(y) ) );
end