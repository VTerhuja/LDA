function [ nDocData ] = RandomInitialization( docData, K )
%RandomInitialization: Randomly assign topic to each word in the document
%   docData : Document
%   K       : # topics
    nIdx = 1;
    totalWords = sum(docData(:,3));
    nDocData = zeros(totalWords,2);
    for i=1:size(docData,1)
        nWords = docData(i,3);
        for w=1:nWords
            nDocData(nIdx,1) = docData(i,2);
            nDocData(nIdx,2) = randi(K);
            nIdx = nIdx + 1;
        end
    end
end

