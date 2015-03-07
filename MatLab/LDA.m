% Latent Dirichlet Allocation for Topic Model
fileName = 'docword.kos.txt';
vocab = GetVocabulary(fileName);
V = size(vocab,1);

% Read data set
[D, W, NNZ, data] = ReadDataSet(fileName);

% # Topics
K = 2;

% Priors
% alpha = per-document topic distribution
alpha = 1;

% beta =  per-topic word distribution
beta = 1;

% Intial topic assignment
% Random assignment of topic for every word in the vocabulary.
% nIdx = 1;
% nData = zeros(1,3);

% Takes too long
% for i=1:size(data,1)
%     nWords = data(i,3);
%     for j=1:nWords
%         nData(nIdx,1) = data(i,1);
%         nData(nIdx,2) = data(i,2);
%         nData(nIdx,3) = randi(K);
%         nIdx = nIdx + 1;
%     end
% end    

% Probability of W(n,d)=w conditional on Z(n,d)=k
P_W_Z = zeros(K,W);

% Probability of Z(n,d)=k conditional on others
P_Z_K = zeros(K,D);

Psi_W_K = zeros(K,W);

T = 100;
for t=1:T
    % T = 1 (Random Initialization) 
    for d=1:D
        docData = data(data(:,1) == d,:,:);
        % Assign topic randomly
        if(t==1)
            nDocData = RandomInitialization(docData, K);
        else
            % Assign topic according to the distribution
        end
        
        % Count # words in document d assigned to topic k
        for k=1:K
            P_Z_K(k,d) = length(nDocData(:,2) == k) + P_Z_K(k,d);
            % Sum and normalize after iterating through all documents
        end
        
        % Start Counting
        % Count # times word w is assigned to topic k not include current
        Wndk = zeros(K, size(nDocData,1));
        Zndk = zeros(K, size(nDocData,1));
        
        for wIdx=1:size(nDocData,1)
            w = nDocData(wIdx,1);
            temp = nDocData((nDocData(:,1) == w), 2);
            for k=1:K
                cwk_n = length(find(temp(:,1)==k));
                if(cwk_n ~= 0)
                    Wndk(k,wIdx) = cwk_n - 1;
                end
                
                
            end
        end
    end    
end
