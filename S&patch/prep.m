% Preparation code inlcudes:  construct the voxel space and visulization
% before backprojection
% input parameter: 
% 1 minimalpos: minimal corner position, unit m
% 2 maximalpos: maximam corner position, unit m
% 3 gridsize: reconstructed spatial grid size, unit m
function [pts, pixels] = prep(dataset,minimalpos, maximalpos, gridsize)
        [pts, pixels] = makegrid(minimalpos, maximalpos, gridsize);        
        % Plot forward projection 
        fwdproject( minimalpos, maximalpos, dataset );
end


function [pts, pixels] = makegrid( minimalpos, maximalpos, gridsize)
    pixels = ceil(abs(minimalpos-maximalpos)./gridsize); % Number of pixels per direction
    vx=[1 0 0].*gridsize; % Unit vector
    vy=[0 1 0].*gridsize; % Unit vector
    vz=[0 0 1].*gridsize; % Unit vector
    npixels=pixels(1)*pixels(2)*pixels(3); % Total number of pixels

    pts=zeros(npixels,3); % Pre-allocation, for speed

    j=1;

    for x=1:pixels(1)
        for y=1:pixels(2)
            for z=1:pixels(3)
                tmp =minimalpos+(x-1).*vx+(y-1)*vy+(z-1)*vz;
                pts(j,:) = tmp;
                j=j+1;
            end
        end
    end
end


function fwdproject( rightbottomback, lefttopfront, dataset )
    x = [rightbottomback(1) lefttopfront(1) lefttopfront(1) rightbottomback(1) rightbottomback(1) rightbottomback(1)...
         rightbottomback(1) rightbottomback(1) rightbottomback(1) rightbottomback(1) lefttopfront(1) lefttopfront(1)...
         lefttopfront(1) lefttopfront(1) lefttopfront(1) lefttopfront(1) lefttopfront(1)...
         lefttopfront(1) rightbottomback(1)]';
    y = [lefttopfront(2) lefttopfront(2) rightbottomback(2) rightbottomback(2) lefttopfront(2) lefttopfront(2)...
         rightbottomback(2) rightbottomback(2) lefttopfront(2) lefttopfront(2) lefttopfront(2) lefttopfront(2)...
         rightbottomback(2) rightbottomback(2) lefttopfront(2) lefttopfront(2) lefttopfront(2)...
         rightbottomback(2) rightbottomback(2)]';
    z = [rightbottomback(3) rightbottomback(3) rightbottomback(3) rightbottomback(3) rightbottomback(3) lefttopfront(3)...
         lefttopfront(3) rightbottomback(3) rightbottomback(3) lefttopfront(3) lefttopfront(3) rightbottomback(3)...
         rightbottomback(3) lefttopfront(3) lefttopfront(3) rightbottomback(3) lefttopfront(3)...
         lefttopfront(3) lefttopfront(3)]';

    %plot the scene
    figure; hold on;
    axis equal; 
    title ('Forward Projection')
    plot3(x, y, z, 'b-'); %Plot outline of projection area
    plot3(dataset.cameraPos(:,1), dataset.cameraPos(:,2), dataset.cameraPos(:,3), 'k.'); %camera positions
    plot3(dataset.laserPos(:,1), dataset.laserPos(:,2), dataset.laserPos(:,3), 'r.') %laser positions
end


