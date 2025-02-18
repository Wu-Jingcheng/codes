function [ bpinput] = backprojectprep(dataset, pts)
%Prepares backprojection inputs to form needed by the cpp file
%   Detailed explanation goes here
% setParametersSimulatedFiona;
bpinput.data = zeros(dataset.t*size(dataset.cameraPos,1), size(dataset.laserPos,1));

for i = 1:size(dataset.data, 1)
    for k = 1:size(dataset.data,2)
        bpinput.data(((k-1)*size(dataset.data,3)+1):((k)*size(dataset.data,3)), i)= squeeze(dataset.data(i, k, :));
%         size(bpinput.data)
    end
end

% the params are length and sigma, noted that length=6*sigma+1
bpinput.data = conv2(bpinput.data,LapGaussianFilter(25,4)','same');

bpinput.pts = pts'; %form needed for c++, [x; y; z]
bpinput.laserPos = dataset.laserPos';
bpinput.laserNorm = dataset.laserNorm';
bpinput.cameraPos = dataset.cameraPos';
bpinput.cameraNorm = dataset.cameraNormal';
bpinput.laserOrigin = dataset.laserOrigin';
bpinput.cameraOrigin = dataset.cameraOrigin';

end

function h = LapGaussianFilter(len, sigma)
    center = (len+1)/2;
    h = conv2(gaussmf(1:len,[sigma center]),[-1 2 -1],'full');
end

