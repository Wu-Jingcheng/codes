clc;
clear;
close all;
load os

fw = nfbp(fw,'log',[3 3 3]); 

% v_max_FBP

%% otsu
loop = 1;
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

%% Imaging
v_max_FBP