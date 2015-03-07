function [ maxD ] = GetMaxWordPerDoc( data, D)
%GetMaxWordPerDoc: Get the maximum words in a document
    maxD = 0;
    for d=1:D
        temp = data(data(:,1) == d,:,:);
        total = sum(temp(:,3));
        if(maxD < total)
            maxD = total;
        end
    end
end

