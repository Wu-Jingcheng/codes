clear 
close all
clc

mex backproject_fast.cpp;


% loading dataset
data_str = ['./data/'];
data_str = [data_str 'data.mat'];
load(data_str);

% Reconstruction volume corner points, unit meter
minimalpos = [-1.2, 0.7, 0.5]; 
maximalpos = [ 0.0, 1.7, 1.2]; 

% reconstruction voxel resolution, unit meter
gridsize = 0.01; 

%% backprojection
% dataset.data = dataset.data(:,:,1 : 30000); % get rid of last several temporal bins, do not comment out this

[pts, pixels] = prep(dataset,minimalpos,maximalpos,gridsize);%¿Ô√Ê”–forwardprojection

bpinput = backprojectprep(dataset, pts); 

% Backproject
tic
Wraw = backproject_fast(double(bpinput.data), double(bpinput.laserPos), ...
    double(bpinput.pts), double(bpinput.cameraPos), ...
    double(bpinput.laserOrigin), double(bpinput.cameraOrigin),...
    double(dataset.deltat), double(dataset.t0), double(bpinput.laserNorm), double(bpinput.cameraNorm));
toc
                             
px=pixels(1);
py=pixels(2);
pz=pixels(3);                             
W = reshape(Wraw,[pz, py, px]);

%% FBP
% Filter in space (nullified)
% fw = nfbp(W,'log',[3 3 3]); 
fw=W;
loop = 2; % define N here, set to zero if not apply denoising
for i=1:loop
    nfw = fw;
    maxi = max(max(max(fw)));
    mini = min(min(min(fw)));
    nfw = nfw-mini;
    nfw(nfw<1)=[];
    nfw = nfw/(maxi-mini);
    level = graythresh(nfw)*(maxi-mini)+mini;
    fw(fw<level)=0;
end


% Maximum projection visualization
v_max_FBP;