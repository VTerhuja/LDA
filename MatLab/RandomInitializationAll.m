function [nDocData, Cwkn, Ckdn ] = RandomInitializationAll( docData, K, W, D)
%RandomInitialization: Randomly assign topic to each word in the document
%   docData : Document
%   K       : # topics
    nIdx = 1;
    totalWords = sum(docData(:,3));
    nDocData = zeros(totalWords,3);
    for i=1:size(docData,1)
        nWords = docData(i,3);
        for w=1:nWords
            nDocData(nIdx,1) = docData(i,1);
            nDocData(nIdx,2) = docData(i,2);
            nDocData(nIdx,3) = randi(K);
            nIdx = nIdx + 1;
        end
    end
    
    Cwkn = zeros(K,W);
    Ckdn = zeros(K,D);
    
    % Count # each words assigned to each topic over all documents
    for w=1:W
        temp = nDocData((nDocData(:,2) == w), 3);
        for k=1:K
            Cwkn(k,w) = length(find(temp(:,1) == k));
        end
    end
    
    % Count # words assigned to each topic in document d
    for d=1:D
        tempD = nDocData((nDocData(:,1) == d), 3);
        for k=1:K
            Ckdn(k,d) = length(find(tempD(:,1) == k));
        end
    end    
end

