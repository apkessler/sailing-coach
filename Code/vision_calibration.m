clc;clear;close all;
%This script calculates the mapping p = MP for 3D points to 2D image
%points.

sourceIm = 'frame_calib1.png';


%World coordinates for features in the first image
W = [0, 0, 0;
     %0, 0, -41;
     0, 10, 0;
     0, 7,  0;
     0, 5,  0;
     0, 3,  0;
     10, -17, -17.5;
     -10, -17, -17.5;
     0,  -15, 76;
     ]'; 
 N = length(W);
 A = zeros(2*N, 8);
 
 for ii = 1:N %now fill A
     A(2*ii - 1, :) = [W(1,ii) W(2,ii) W(3,ii) 1 0 0 0 0];
     A(2*ii    , :) = [0 0 0 0 W(1,ii) W(2,ii) W(3,ii) 1];
 end
     
 %Camera features
C =[ 678.6615, 517.8665;
     %621.4193, 532.177;
     670.7112, 427.2329;
     672.3012, 454.264;
     675.4814, 468.5745;
     677.0714, 487.6553;
     789.9658, 662.5621;
     608.6988, 678.4627;
     739.0839, 807.2578;
     %823.3571, 797.7174;
     %837.6677, 920.1522;
     %681.8416, 936.0528;
     ]';
 c = reshape(C,2*N,1); %reshape all the image points into a column vector
 
 p = inv(A'*A)*A'*c; %find the least-squares solution
 M = [p(1) p(2) p(3) p(4); p(5) p(6) p(7) p(8); 0  0 0 1]
 
 %Calculate error
C_est = M*vertcat(W, ones(1,N));
C_est(1,:) = C_est(1,:)./C_est(3,:);
C_est(2,:) = C_est(2,:)./C_est(3,:);
C_err = (C - C_est(1:2,:)).^2; %calculate the difference between where we estimate the image is, and where is was
RMS_err = sqrt(sum(sum(C_err))/N) %calcualte RMS error 

 %Draw circle in 3D space
  t= 0:0.1:pi;
  R = 50;
  P = [R*cos(t); zeros(size(t));R*sin(t); ones(size(t))];
  P_im = M*P; %project it into image 
  P_im(1,:) = P_im(1,:)./P_im(3,:);
  P_im(2,:) = P_im(2,:)./P_im(3,:);
  P_im = [P_im, P_im(:,1)]; %close the line
  
 %Reconstruct points and plot as check
 imshow(sourceIm)
 hold on; axis image;
 c1_est = M*vertcat(W, ones(1,N));
 plot(c1_est(1,:),c1_est(2,:),'o')
 plot(P_im(1,:),P_im(2,:),'r')
 title('Reconstructed World Points')

 