clc;clear;close all;

prefix = 'Data/frame_t';
suffix = '.png';
boatFile = 'calibrationData.mat';

N = 9;
boom_angle = zeros(1,N);

P = [148 283; 
     464 182; 
     678 171; 
    1122 148; 
     960 149; 
     771 158; 
     468 190; 
     416 159; 
     411 160]';


for ii = 1:N
    boom_angle(ii) = imgPoint2BoomAngle([prefix num2str(ii) suffix],boatFile, P(:,ii));
    %export_fig(['frame_t' num2str(ii) 'annotated.eps'],'-eps','-transparent')
end    
close all;
%% 
%t_act = [-50, -15, 0, 30, 20, 10, -15, -15, -10];
%plot(t_act,t,'.')
heading = 60; %deg
optimal_angle = sail_model(heading *pi/180)*ones(size(boom_angle));
%Calculate the suggested trim.
boom_angle
delta_phi = optimal_angle*180/pi - boom_angle