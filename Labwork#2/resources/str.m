clear all 
close all
clc

%%

filename = 'point_cloud1_filtered.txt';
M = dlmread(filename,' ',1,0)

filename = 'point_cloud1.txt';
MO = dlmread(filename,' ',1,0)

%%
x = M(:,1); y = M(:,2); z = M(:,3);

figure
plot(y, x);

figure
pcshow(M, 'r');


figure
plot(MO(:,2), MO(:,1));

figure
pcshow(MO, 'r');

%% 

ptCloud = pointCloud([M(:,1),M(:,2),M(:,3)]);

ptCloudOut = pcdenoise(ptCloud);

figure
pcshow(ptCloud);
title('Original Data');



