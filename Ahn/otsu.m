%% Load Data
clc;clear;close all;
addpath('lib/util');addpath('lib/util/vol3d');addpath('lib/lib_convnlos');

% This project doesn't experiment the methods on "soap", "number" and "toy"
% Please reach out to the original datasets from Ahn et al. if need them
obj_list = {'S', 'USAF', 'soap', 'bunny', 'numbers', 'TX', '2019', 'toy'};
obj_name = obj_list{4};
load(['results/' obj_name '/results.mat']);

W1 = rho.FBP;
W2 = rho.Gram_noprior;

%% Otsu Thresholding
loop1 = 1;
loop2 = 1;
for i=1:loop1
    nW = W1;
    maxi = max(max(max(W1)));
    mini = min(min(min(W1)));
    nW = nW-mini;
    nW(nW<1e-4)=[];
    nW = nW/(maxi-mini);
    level = graythresh(nW)*(maxi-mini)+mini;
    W1(W1<level)=0;
end
for i=1:loop2
    nW = W2;
    maxi = max(max(max(W2)));
    mini = min(min(min(W2)));
    nW = nW-mini;
    nW(nW<1e-4)=[];
    nW = nW/(maxi-mini);
    level = graythresh(nW)*(maxi-mini)+mini;
    W2(W2<level)=0;
end

figure;imslice(W1);
figure;imslice(W2);

path = ['L_results/' obj_name];
method1 = ['FBP_L' num2str(loop1)];
method2 = ['noprior_L' num2str(loop2)];

mkdir('L_results')
mkdir(path)
mkdir([path '/albedos'])
mkdir([path '/albedos_gray'])
mkdir([path '/albedos_cmap'])

saveAlbedo(W1, method1, [path '/albedos']);
saveAlbedo_gray(W1, method1, [path '/albedos_gray']);
saveAlbedo_cmap(W1, method1, [path '/albedos_cmap']);
saveAlbedo(W2, method2, [path '/albedos']);
saveAlbedo_gray(W2, method2, [path '/albedos_gray']);
saveAlbedo_cmap(W2, method2, [path '/albedos_cmap']);