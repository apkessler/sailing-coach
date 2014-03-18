function phi = sail_model(varargin)
%Get the optimal sail angle (phi) based on current heading (theta).
%If no inputs to function, calibration curve is plotted.
%If a vector of headings t is given, the optimal sail will be calculated
%for each heading. 


phi =[];
%These are the points to fit.
x = [pi/4, pi/2, 3*pi/4, pi]';
y = [0,    pi/4,   pi/3, pi/2]';

n = length(x);
A = zeros(n,n);

A = [x.^3 x.^2 x.^1 x.^0];

a = inv(A)*y;
if (numel(varargin) == 0)
   
    figure;
    hold on;

    rectangle('Position',[-pi/4, -2, pi/2, 4],'FaceColor',[0.4 0.4 0.4])
    xs = linspace(pi/4,pi,100);
    ys = a(1)*xs.^3 + a(2)*xs.^2  + a(3)*xs + a(4)*xs.^0;
    plot(xs,ys,'r');
    plot(-xs,-ys,'b')
    plot(x,y,'r.');
    plot(-x,-y,'b.');
    xlabel('Heading');
    ylabel('Sail Angle');
    title('Calibration Curve');
    grid on;

    text(x(1)+.1, y(1)+.05, 'Close-Hauled', 'Color', 'r'); 
    text(x(2)+.05, y(2), 'Beam-Reach', 'Color', 'r'); 
    text(x(3)+.05, y(3), 'Broad-Reach', 'Color', 'r'); 
    text(x(4)+.05, y(4), 'Running', 'Color', 'r'); 


    text(-x(1)-1.5, -y(1), 'Close-Hauled', 'Color', 'b'); 
    text(-x(2)-1.5, -y(2), 'Beam-Reach', 'Color', 'b'); 
    text(-x(3)-1.5, -y(3), 'Broad-Reach', 'Color', 'b'); 
    text(-x(4)-.2, -y(4)-.1, 'Running', 'Color', 'b');


    text(-.6,0,'No-Go Zone','Color','w')
else
   t = varargin{1};
   phi = a(1)*t.^3 + a(2)*t.^2  + a(3)*t + a(4)*t.^0;
end
