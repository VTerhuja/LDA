function [nDocData, Cwk, Cwdk, Cwd, Ck ] = RandomInitializationAll( docData, K, W, D)
%RandomInitialization: Randomly assign topic to each word in the document
%and count
%   Parameters:
%       docData : # document
%       K       : # topics
%       W       : words in vocabulary
%   Output:
%       nDocData : all words in dataset with assigned topic
%       Cwk      : # times word w is assigned topic k over all documents
%       Cwdk     : # times word w is assigned topic k in document d
%       Cwd      : # w words in d document
%       Ck       : # words assigned to topic k
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
    
    Cwk = zeros(W,K);
    Cwdk = zeros(W,D,K);
    Cwd = zeros(W,D);
    Ck = zeros(K,1);
    
    % Count # each words assigned to each topic over all documents
    for i=1:totalWords
        d = nDocData(i:1);
        w = nDocData(i,2);
        k = nDocData(i,3);

        Cwk(w,k) = Cwk(w,k) + 1;
        Cwdk(w,d,k) = Cwdk(w,d,k) + 1;
        Cwd(w,d) = Cwd(w,d) + 1;
        Ck(k) = Ck(k) + 1;
    end
end

