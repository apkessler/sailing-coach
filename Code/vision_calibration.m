clc;clear;close all;
%This script calculates the mapping p = MP for 3D points to 2D image
%points.

file_calib = 'Data/frame_t1.png';

%% Make the matrix A from known world calibration points.
%World coordinates for features in the first image
W = [0, 0, 0;
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
 
%% Choose the camera points manually, or load from file.

img_calib = imread(file_calib);
figure, image(img_calib);
title('Click a point on this image'); axis image;
C = zeros(2,N);
for ii = 1:N
    fprintf('Please choose point at (%.2f,%.2f,%.2f) in 3D space...',W(1,ii),W(2,ii),W(3,ii))
    [x y] = ginput(1);
    C(:,ii) = [x y];
    fprintf('(%.3f, %.3f)\n',x,y);
end

    
%% Calculate the lumped intrinsic/extrinsic camera matrix M = K[R t]
 c = reshape(C,2*N,1); %reshape all the image points into a column vector
 p = inv(A'*A)*A'*c; %find the least-squares solution
 M = [p(1) p(2) p(3) p(4); p(5) p(6) p(7) p(8); 0  0 0 1]; %reshape in 4x3
 
%% Calculate reprojection error
C_est = M*vertcat(W, ones(1,N));
C_est(1,:) = C_est(1,:)./C_est(3,:);
C_est(2,:) = C_est(2,:)./C_est(3,:);
C_err = (C - C_est(1:2,:)).^2; %calculate the difference between where we estimate the image is, and where is was
RMS_err = sqrt(sum(sum(C_err))/N); %calcualte RMS error 

%% Make circle in 3D space
  t= 0:0.1:pi;
  R = 50;
  P = [R*cos(t); zeros(size(t));R*sin(t); ones(size(t))];
  P_im = M*P; %project it into image 
  P_im(1,:) = P_im(1,:)./P_im(3,:);
  P_im(2,:) = P_im(2,:)./P_im(3,:);
  P_im = [P_im, P_im(:,1)]; %close the line
  
 %% Reconstruct points and plot as check
 imshow(img_calib)
 hold on; axis image;
 c1_est = M*vertcat(W, ones(1,N));
 plot(c1_est(1,:),c1_est(2,:),'o')
 plot(P_im(1,:),P_im(2,:),'r')
 title('Reconstructed World Points')

 %% Save this data to a calibration file. 
 save calibrationData.mat C W M RMS_err file_calib
 