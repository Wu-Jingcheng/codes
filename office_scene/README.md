# Readme

Here, we directly use the result of *officescene.mat*. After BP, we saved it as *os.mat*.

In this case, we directly load data from *os.mat*. Please access original codes and data if you would like to have more insight. We made a slight difference by setting the gridsize to 0.02 to speed up the computation in the original codes.

Run *otsu.m* to implement denoising. Afterwards, run *component.m* to realize component detection. In this case, the region growing with minimal point distance of 5, which means that the maximum norm-1 distance between 2 adjacent points inside a region can be up to 5 voxels. In other cases of this project, the distance is set to 1.

# Acknowledgments

Thank Liu et al. for providing their codes and data.

The link for original codes: https://springernature.figshare.com/articles/Datasets_and_reconstruction_code_for_a_virtual_wave_non-line-of-sight_imaging_approach/8084987

The link for original paper: https://biostat.wisc.edu/~compoptics/phasornlos19/phasor_nlos_19.html