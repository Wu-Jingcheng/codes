function plotScenario(voxels, dataset)

figure();
hold on; grid on;
plot3(0,0,0, 'x') % Origin
plot3(dataset.laserPos(:,1), dataset.laserPos(:,2), dataset.laserPos(:,3), 'x', 'Color', [0.6350 0.0780 0.1840])
plot3(dataset.cameraPos(:,1), dataset.cameraPos(:,2), dataset.cameraPos(:,3), '.', 'Color', [0.4660 0.6740 0.1880])
plot3(voxels(1:1:end,1), voxels(1:1:end,2), voxels(1:1:end,3), '.', 'Color', [0 0.4470 0.7410])
xlabel('x'); ylabel('y'); zlabel('z')
legend('Origin', 'laserPos', 'cameraPos', '3D voxel space', 'Location', 'NorthWest')
view([-30 60])