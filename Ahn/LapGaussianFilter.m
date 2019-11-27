function h = LapGaussianFilter(len, sigma)
    if(sigma==0)
        h=[-1 2 -1];
        h = reshape(h,[1 1 3]);
    else
        center = (len+1)/2;
        h = conv2(gaussmf(1:len,[sigma center]),[-1 2 -1],'full');
        h = reshape(h, [1 1 length(h)]);
    end

end