nfw=W;
nfw(nfw>1)=1;
sz=size(W);dcfw=[];
while(sum(sum(sum(nfw)))>1)
    temp=zeros(sz);
    pp=find(nfw>0.5);
    [seeds(1,1),seeds(1,2),seeds(1,3)]=ind2sub(sz,pp(1));
    nfw(seeds(1),seeds(2),seeds(3))=0;
    temp(seeds(1),seeds(2),seeds(3))=1;
%     seeds=[x,y,z];
    while(size(seeds,1))
        seed=seeds(1,:);
        seeds(1,:)=[];
        var = 1;
        for x=-var:var
            for y=-var:var
                for z=-var:var
                    if(~(x==0&&y==0&&z==0)&&(0<(seed(1)+x)<(sz(1)+1))&&...
                        (0<(seed(2)+y)<(sz(2)+1))&&(0<(seed(3)+z)<(sz(3)+1))&&...
                        nfw(seed(1)+x,seed(2)+y,seed(3)+z)>0.5)
                        seeds(size(seeds,1)+1,:)=seed+[x y z];
                        nfw(seed(1)+x,seed(2)+y,seed(3)+z)=0;
                        temp(seed(1)+x,seed(2)+y,seed(3)+z)=1;
                    end
                end
            end
        end
    end
    dcfw(size(dcfw,1)+1,:,:,:)=temp;
end

for i=1:size(dcfw,1)
    nfw=dcfw(i,:,:,:);
    nW = W;
    nW(nfw<0.5)=0;
    image2Draw = squeeze(max(nW, [], 3));
    figure() 
    imagesc(y, z, image2Draw);
    title('Gandalf - Filtered backprojection')
    xlabel('y'); ylabel('z')
    axis xy

    saveas(gcf, ['results/' num2str(i) '.jpeg']);
end