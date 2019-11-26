% ----------------------------------------------------------------------- %
%                              MAIN_GANDALF                               %
%                     (c) Computational Optics Group                      %
%                   https://biostat.wisc.edu/~compoptics/                 %
% ----------------------------------------------------------------------- %
%                            +++ DESCRIPTION +++                          %
% The following script provides the basic backprojection code which can   %
% be used to reconstruct our datasets                                     %
% Code is provided "as is"                                                %
% ----------------------------------------------------------------------- %
clear; close all; clc

%% PREAMBLE
% 1. Mex the backprojection C++ file (can be done just once per operating
% system)
mex backproject_fastOld.cpp;

% 2. Load the dataset
load('dataset_Gandalf')

% 3. Define the 3D reconstruction volume
% First design the desired volume centered in (0, 0, 0)
lefttopfront = [-44 -75 0]; 
rightbottomback = [44 75 300];
gridsize = 2; % This selects the desired "resolution"; the smaller the 
              % number, the higher the resolution (but the backprojection 
              % computation time increases). Using an i7-4770 CPU + 32 GB
              % of RAM, it takes ~250s to compute the backprojection with
              % above parameters
% Use "makegrid" to create a discretized volume space
[voxels, gridSize] = makegrid(lefttopfront, rightbottomback, gridsize);
% Rotate and shift the voxels into the desired position (target's
% neighborhood)
Rz = rotz(-25); % Rotation matrix
t = [375, 220, 0]; % Shift vector
voxels = Rz*(voxels.'); 
voxels = voxels';
for i = 1:length(voxels(:,1))
   voxels(i,:) = voxels(i,:)+t; 
end
% Plot the scenario
plotScenario(voxels, dataset)

% Following values are helpful for plotting purposes at the end
x = [min(voxels(:,1)) max(voxels(:,1))];
y = [min(voxels(:,2)) max(voxels(:,2))];
z = [min(voxels(:,3)) max(voxels(:,3))];

%% BACKPROJECTION
% 1. Raw backprojection
% Rearranging the dataset to feed variables into C
bpinput = backprojectprep(dataset, voxels); 

% Backproject
tic
Wraw = backproject_fastOld(double(bpinput.data), double(bpinput.laserPos), ...
    double(bpinput.pts), double(bpinput.cameraPos), ...
    double(bpinput.laserOrigin), double(bpinput.cameraOrigin),...
    double(dataset.deltat), double(dataset.t0)); % <-- This is the raw backprojection
toc
% Reshape the column vector into a 3D cube
px = gridSize(1); py = gridSize(2); pz = gridSize(3);
W = reshape(Wraw, [pz, py, px]); % Cube


loop = 3; % define N
for i=1:loop
    nW = W;
    maxi = max(max(max(W)));
    mini = min(min(min(W)));
    nW = nW-mini;
    nW(nW<1)=[];
    nW = nW/(maxi-mini);
    level = graythresh(nW)*(maxi-mini)+mini;
    W(W<level)=0;
end

%% VISUALIZATION
% 1. Raw backprojection
image2Draw = squeeze(max(W, [], 3));
figure() 
imagesc(y, z, image2Draw);
title('Gandalf - Filtered backprojection')
xlabel('y'); ylabel('z')
axis xy

saveas(gcf, ['results/' '1D_image2Dfilter' '.jpeg']);