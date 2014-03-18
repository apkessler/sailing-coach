function [theta_d] = imgPoint2BoomAngle(theImage, theCalibration, varargin)
    
p= [];
if (nargin == 3)
    p = varargin{1};
end


calibrationStruct = load(theCalibration);

M  = calibrationStruct.M;

geometryStruct = load('boatGeometry.mat');
L_B = geometryStruct.L_boom;
alph = geometryStruct.alph;

figure;
im = imread(theImage);
imshow(im);
axis image;

if (isempty(p))
    fprintf('Select where the end of the boom is!\n')
    [u v] = ginput(1);
    fprintf('Point in image is at (%.2f, %.2f)\n',u,v);
else
   u = p(1); 
   v = p(2); 
end

hold on;
plot(u,v,'o')

A = vertcat(M,[0 1 0 0]);
P_y = L_B*sind(alph);
y = [u; v; 1; P_y];
P = pinv(A)*y; %this is our guess for where the point is

%Now reproject back into image...

R = L_B*cosd(alph);
O_C = [0 P_y 0]'; %origin of circle
d = P(1:3) - O_C; 
P_fix = R*d/norm(d) + O_C;
theta = atan2(P_fix(1),-P_fix(3));
theta_d = theta*180/pi;
p_fix = M*[P_fix; 1];


%Make a circle in 3D space
t= pi:0.1:2*pi;
P = [R*cos(t); P_y*ones(size(t)); R*sin(t); ones(size(t))];
P_im = M*P; %project it into image 
P_im(1,:) = P_im(1,:)./P_im(3,:);
P_im(2,:) = P_im(2,:)./P_im(3,:);
plot(P_im(1,:),P_im(2,:),'y');

W_org = [733 523]';
%Draw a line over the boom.
line([W_org(1) p_fix(1)], [W_org(2), p_fix(2)],'Color','r','LineWidth',3)

 
 %Finally, plot our guess of the fixed point on the image.
 plot(p_fix(1),p_fix(2),'rx','LineWidth',3);
 title(['\theta = ' num2str(theta_d)]);

  