%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                    %
%      [STR - Point Cloud Visualization on Matlab] LABWORK#2         %
%                                                                    %
%                   Frederico Vaz, nº 2011283029                     %
%                   Pedro Eugénio, nº 2012128473                     %
%                                                                    %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clear all 
close all
clc

%% Read point cloud files

% Original Point Cloud 1
filename = 'point_cloud1.txt';
PointCloud1 = dlmread(filename,' ',1,0);

% Filtered Point Cloud 1
filename = 'point_cloud1_filtered.txt';
PointCloud_filtered1 = dlmread(filename,' ',1,0);


% Original Point Cloud 2
filename = 'point_cloud2.txt';
PointCloud2 = dlmread(filename,' ',1,0);

% Filtered Point Cloud 2
filename = 'point_cloud2_filtered.txt';
PointCloud_filtered2 = dlmread(filename,' ',1,0);


% Original Point Cloud 3
filename = 'point_cloud3.txt';
PointCloud3 = dlmread(filename,' ',1,0);

% Filtered Point Cloud 3
filename = 'point_cloud3_filtered.txt';
PointCloud_filtered3 = dlmread(filename,' ',1,0);


%% Point Cloud Visualization

for i=1:3
    % Original Point Cloud
    figure(i)
    subplot(2,2,1)
    if i == 1
        plot(PointCloud1(:,2), PointCloud1(:,1));
    elseif i == 2
        plot(PointCloud2(:,2), PointCloud2(:,1));
    else
        plot(PointCloud3(:,2), PointCloud3(:,1));
    end
    title(['Original Point Cloud ', num2str(i),' - 2D']);
    xlabel('eixo Y')
    ylabel('eixo X')
    grid on
    
    subplot(2,2,2)
    if i == 1
        pcshow(PointCloud1(:,:), 'r');
    elseif i == 2
        pcshow(PointCloud2(:,:), 'r');
    else
        pcshow(PointCloud3(:,:), 'r');
    end
    title(['Original Point Cloud ', num2str(i),' - 3D']);
    xlabel('eixo X')
    ylabel('eixo Y')
    zlabel('eixo Z')
    grid on
    
    % Filtered Point Cloud
    subplot(2,2,3)
    if i == 1
        plot(PointCloud_filtered1(:,2), PointCloud_filtered1(:,1));
    elseif i == 2
        plot(PointCloud_filtered2(:,2), PointCloud_filtered2(:,1));
    else
        plot(PointCloud_filtered3(:,2), PointCloud_filtered3(:,1));
    end
    title(['Filtered Point Cloud ', num2str(i),' - 2D']);
    xlabel('eixo Y')
    ylabel('eixo X')
    grid on
    
    subplot(2,2,4)
    if i == 1
        pcshow(PointCloud_filtered1(:,:), 'r');
    elseif i == 2
        pcshow(PointCloud_filtered2(:,:), 'r');
    else
        pcshow(PointCloud_filtered3(:,:), 'r');
    end
    title(['Filtered Point Cloud ', num2str(i),' - 3D']);
    xlabel('eixo X')
    ylabel('eixo Y')
    zlabel('eixo Z')
    grid on
    
end

%% Attemp to delete the outliers, 2.3)

% ptCloud = pointCloud([P2(:,1),P2(:,2),P2(:,3)]);
% 
% ptCloudOut = pcdenoise(ptCloud);
% 
% figure
% pcshow(ptCloud);
% title('Original Data');



