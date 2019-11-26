/* backprojects */
#include <math.h>
#include <matrix.h>
#include <mex.h>


//Define Functions
inline double distance2(double* v1,double *v2)
{
  double f = sqrt((v1[0]-v2[0])*(v1[0]-v2[0])+(v1[1]-v2[1])*(v1[1]-v2[1])+(v1[2]-v2[2])*(v1[2]-v2[2]));
  return f;
}

inline double round(double x) { return floor(x+0.5); }


//Load Variables
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  #define xsI prhs[0]//bpinput.data
  #define xlaserpos prhs[1]//bpinput.laserPos
  #define xvoxels prhs[2]//bpinput.pts
  #define xcpos prhs[3]//bpinput.cameraPos
  #define xlcop prhs[4]//bpinput.laserOrigin
  #define xccop prhs[5]//bpinput.cameraOrigin
  #define xtpp prhs[6]//bpinput.deltat
  #define xshift prhs[7]//bpinput.t0
  #define xlasernorm prhs[8]//bpinput.laserNorm
  #define xcameranorm prhs[9]//bpinput.cameraNorm

  #define xoutput plhs[0]// output

  double *sI = mxGetPr(xsI);//data 30000*382
  double *laserpos = mxGetPr(xlaserpos);//laserPos 3*382
  double *voxels = mxGetPr(xvoxels);//pts 3*840000
  double *cpos = mxGetPr(xcpos);//cameraPos 3*1 z=0
  double *geo_laser_cop = mxGetPr(xlcop);//laserOrigin 3*1
  double *geo_camera_cop = mxGetPr(xccop);//cameraOrigin 3*1
  double *cameranormal = mxGetPr(xcameranorm);//cameraNorm 3*1 [1;1;1]
  double *lasernormal = mxGetPr(xlasernorm);//laserNorm 3*1 [1;1;1]

  double tpp = *((double *)mxGetData(xtpp)); //deltat
  double shift = *((double *)mxGetData(xshift));//t0

  int nlasers = mxGetN(xsI);//382 镭射点数目
  int nx = mxGetN(xcpos);//1 相机点位数目
  int nt = mxGetM(xsI)/nx;//30000/1，计算了总时间单位吧
  int nvoxels = mxGetN(xvoxels);//840000

  int x = 0,p=0;
  int tpos = 0;


  mexPrintf("nlasers: %d\n", nlasers);
  mexPrintf("nvoxels: %d\n", nvoxels);
  mexPrintf("nx: %d\n", nx);
  mexPrintf("nt: %d\n", nt);
  mexPrintf("tpp: %f\n", tpp);
  mexPrintf("shift: %f\n", shift);
  mexPrintf("First voxel: %f %f %f\n", voxels[0], voxels[1], voxels[2]);
  mexPrintf("Laser cop: %f %f %f\n", geo_laser_cop[0], geo_laser_cop[1], geo_laser_cop[2]);
  mexPrintf("cameranorm: %f %f %f\n", cameranormal[0], cameranormal[1], cameranormal[2]);
  
  //Start Backproject
  mxArray *out_array = xoutput = mxCreateDoubleMatrix(nvoxels,1,mxREAL);

  double *output = mxGetPr(out_array);
  for (int i=0; i<nvoxels; i++) output[i]=0;//提前把output填充满
  double* d1l=new double[nlasers];//长度为382的数组，创建一个double的数组对象，初始化长度为nlaser或者nx
  double* d4l=new double[nx];//长度为1的数组


  for( tpos = 0;tpos<nlasers;tpos++) {//对于每个镭射点
    d1l[tpos] = distance2(geo_laser_cop,&laserpos[tpos*3]);//从激光原点到激光反射点的距离
    //mexPrintf("laserpos [%f %f %f]\n", laserpos[0], laserpos[1], laserpos[2]);
  }


  for (x=0;x<nx;x++) {//对于每个相机位点
    d4l[x] = distance2(&cpos[x*3],geo_camera_cop);//计算从相机点位到相机位置的距离
  }


  for(p=0;p<nvoxels;p++)//对每个像素
  {
    if (p % (nvoxels / 10) == 0)
    {
        mexPrintf("%d percent done\n", (p / (nvoxels / 10)) * 10);
        // mexPrintf("%d %d\n",p, nvoxels);
        mexEvalString("pause(.001);"); // to dump string.
    }

    double thesum=0;
    double * voxel1 = &voxels[p*3];//像素坐标的x
    
  //
    // through each laser
    for( tpos = 0;tpos<nlasers;tpos++) {
        double * laserpos1 = &laserpos[tpos*3];
        double * lasernorm1 = &lasernormal[tpos*3];//除了第一个都是0，0，0（实在是没想通这里是干森么）
        
        double d1 = d1l[tpos];
        double d2 = distance2(laserpos1,voxel1);//计算了d2
  	  
        
        for (x=0;x<nx;x++) {//对每个相机位点，在这里其实就一个
            double * cpos1 = &cpos[x*3];
            double * cameranorm1 = &cameranormal[x*3];//除了第一个都是0，0，0（实在是没想通这里是干森么）
  		  //cameranorm1 = &cameranormal[x * 3];
            
            double d3 = distance2(voxel1,cpos1);
  		  
            double d4 = d4l[x];
            double d=d1+d2+d3+d4;//总距离
  		  
//             double vlv[3];
//             double vcv[3];
//             vlv[0]= (voxel1[0]-laserpos1[0])/d2;
//             vlv[1]= (voxel1[1]-laserpos1[1])/d2;
//             vlv[2]= (voxel1[2]-laserpos1[2])/d2;
//             double dotlv = vlv[0]*lasernorm1[0]+vlv[1]*lasernorm1[1]+vlv[2]*lasernorm1[2];
//             
//   		  
//             vcv[0]= (voxel1[0]-cpos1[0])/d3;
//             vcv[1]= (voxel1[1]-cpos1[1])/d3;
//             vcv[2]= (voxel1[2]-cpos1[2])/d3;
//             double dotcv = vcv[0]*cameranorm1[0]+vcv[1]*cameranorm1[1]+vcv[2]*cameranorm1[2];
  		
            
            int tindex = round((d-(shift))/tpp);//计算的是时间单位，只要这个时间单位在30000个以内就说明有映射
  		  
            int index = 0;
            double tol=0.3;
            if(voxel1[0]>-tol && voxel1[0]<tol && voxel1[1]>46-tol && voxel1[1]<46+tol && voxel1[2]>-40-tol && voxel1[2]<-40+tol) {
                  mexPrintf("---------------------------------------------------------\n");
                  mexPrintf("Voxel 4000: [%f %f %f]\n", voxel1[0], voxel1[1], voxel1[2]);
                  mexPrintf("cpos: [%f %f %f]\n", cpos1[0], cpos1[1], cpos1[2]);
                  mexPrintf("lpos: [%f %f %f]\n", laserpos1[0], laserpos1[1], laserpos1[2]);
                  mexPrintf("d1 %f, d2 %f, d3 %f, d4 %f\n", d1, d2, d3, d4);
                  mexPrintf("INDEX: %d\n", tindex);
                  mexPrintf("d %f\n", d);
                  }
                        
            if ((tindex>=0) && (tindex<nt))
            {                          
                index = tpos*nx*nt + tindex +x*nt;  
                //可以看出，时间点是竖着排列的对每个相机点位，每一列代表一个镭射点
                //那么tpos*nx*nt是为了跳过已经逝去的镭射点
                //x*nt是为了跳过已经逝去的相机点位
                //tindex是在当前镭射点和相机点位的空间中的索引

                if(index >= 0 && index <= nx*nlasers*nt)
                { 
  				        thesum = thesum + sI[index];//这个好理解了，把映射到同一像素上的激光大小加起来。
                }
                else
                {
                    mexPrintf("stupid mistake\n");//意味着有点位的计算不在划定的范围内
                }
                
            }      
        }
    }
    output[p] = thesum; 
    
  }
  
  mexPrintf("100 percent done\n");
  delete[] d1l;
  delete[] d4l;
}