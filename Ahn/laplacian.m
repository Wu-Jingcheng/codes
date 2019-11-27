%% "Convolutional Approximations to the General NLOS Imaging operator"
% Compatible with MATLAB 2016a or higher (lower versions have not been tested)
% Run ./lib/lib_convnlos/mex/compile_mex.m before running this code
% Email bahn@cmu.edu if you have any issues using this code

clear
close all
restoredefaultpath;
addpath(genpath('./lib'));

%% SETUP
% Use GPU if possible (only for convolution)
GPU_ENABLED = logical(gpuDeviceCount);
fprintf('GPU_ENABLED: %d\n', GPU_ENABLED);

%% LOAD DATA
% Select an object from obj_list
obj_list = {'S', 'USAF', 'soap', 'bunny', 'numbers', 'TX', '2019', 'toy'};
obj_name = obj_list{7};
NLOSDATA = loadNLOSDATA(obj_name);

%% Filtered backprojection
tic
is_load_BP = false;

% 1D Laplacian filter
NLOSDATA.transient = convn(NLOSDATA.transient, LapGaussianFilter(0,0),'same');

[rho.FBP, time.FBP] = computeBP(NLOSDATA, is_load_BP); % alternative BP
% [rho.FBP, time.FBP] = computeFBP(rho.BP);
toc
%% Imaging
figure();imslice(rho.FBP);

%% Saving files
DIR_RESULT = sprintf('1D_results/%s', NLOSDATA.obj_name)

method = '1D_Lap';

mkdir(DIR_RESULT);
mkdir([DIR_RESULT '/albedos']);
mkdir([DIR_RESULT '/albedos_gray']);
mkdir([DIR_RESULT '/albedos_cmap']);

saveAlbedo(rho.FBP, method, [DIR_RESULT '/albedos']);
saveAlbedo_gray(rho.FBP, method, [DIR_RESULT '/albedos_gray']);
saveAlbedo_cmap(rho.FBP, method, [DIR_RESULT '/albedos_cmap']);

%% Otsu
tic
W1 = rho.FBP;
loop = 2;% define N here
for i=1:loop
    nW = W1;
    maxi = max(max(max(W1)));
    mini = min(min(min(W1)));
    nW = nW-mini;
    nW(nW<1e-4)=[];
    nW = nW/(maxi-mini);
    level = graythresh(nW)*(maxi-mini)+mini;
    W1(W1<level)=0;
end
toc
%% Otsu Imaging
figure();imslice(W1);

%% Otsu Saving
DIR_RESULT = sprintf('1D_results/%s', NLOSDATA.obj_name)

method = '1D_Lap_Otsu';

mkdir(DIR_RESULT);
mkdir([DIR_RESULT '/albedos']);
mkdir([DIR_RESULT '/albedos_gray']);
mkdir([DIR_RESULT '/albedos_cmap']);

saveAlbedo(W1, method, [DIR_RESULT '/albedos']);
saveAlbedo_gray(W1, method, [DIR_RESULT '/albedos_gray']);
saveAlbedo_cmap(W1, method, [DIR_RESULT '/albedos_cmap']);
