% Latent Dirichlet Allocation for Topic Model
clear
clc
fileName = 'docword.nips.txt';
vocab = GetVocabulary(fileName);
V = size(vocab,1);

% Read data set
[D, W, NNZ, data] = ReadDataSet(fileName);

% # Topics
K = 5;

% Priors
% alpha = per-document topic distribution
alpha = 1;

% beta =  per-topic word distribution
beta = 1;

fprintf('Random Initalization . . .');
% nData : All the words in all the documents.
% CKW  : Topic-Word count matrix
% Count of # times word w is assigned to k topic in all documents.
% CKD  : Topic-DocCount count matrix
% Count of # words assigned to k topic in document d.
[nData, CKW, CKD] = RandomInitializationAll(data, K, W, D);
fprintf('\nDone!');

T = 20;
fprintf('\nStart Burn-In for T=%d . . .',T);
for t=1:T
    fprintf('\nIteration = %d', t);
    for i=1:size(nData,1)
        w = nData(i,2);     % wordId
        d = nData(i,1);     % docId
        kId = nData(i,3);   % topicId
        
        p = zeros(1,K);
        for k=1:K
            % # times word w assigned topic k in all documents except
            % current word
            if(CKW(k,w) ~= 0)
                firstNum = CKW(k,w) - 1;
            end
            % # words assigned to topic k in document d except current word
            if(CKD(k,d) ~= 0)
                secondNum = CKD(k,d) - 1;
            end
            % Sum: # words assigned to topic k
            sumkwCol = sum(CKW(k,:)) + W;
            % Sum: # words in document d
            sumkdCol = sum(CKD(:,d)) + K;
            first = double(firstNum + 1) / double(sumkwCol); % divide by Sum of the row
            second = double(secondNum + 1) / double(sumkdCol); % divide by Sum of the row
            p(k) = first * second;
        end
        
        % Compute sum for normalization
        sump = sum(p);
        % Normalize the probability for each topic
        p = p/sump;
        % Sample new topic from the distribution
        newk = discreternd(p,1);
        % Assign new topic
        nData(i,3) = newk;
        % Reduce the count from topic-word count matrix
        CKW(kId,w) = CKW(kId,w)-1;
        % Increase the count in topic-word count matrix
        CKW(newk,w) = CKW(newk,w)+1;
        % Reduce the count from topic-document count matrix
        CKD(kId,d) = CKD(kId,d)-1;
        % Increase the count in topic-document count matrix
        CKD(newk,d) = CKD(newk,d)+1;        
    end    
end
fprintf('\nDone!');
fprintf('\nCompute psi(w,k)...');

% Compute psi(w,k), sort, display top 10 words in each topic
vocab = GetVocabulary('vocab.nips.txt');
topTenProbK = zeros(K,10);
topTenWordK = zeros(K,10);
beta = ones(K,W);
psiwk = CKW + beta;
fprintf('\nCompute psi(w,k)...');
for k=1:K
   sumRow = sum(CKW(k,:));
   sumRow = sumRow + double(W);
   psiwk(k,:) = psiwk(k,:)./sumRow;
   [prob, idx] = sort(psiwk(k,:),'descend');
   topTenProbK(k,:) = prob(1:10);
   topTenWordK(k,:) = idx(1:10);
end
fprintf('\nDone!');

fprintf('\nTop 10 words of each topic');
for k=1:K
    fprintf('\n\nK=%d:',k);
    for i=1:10
        idx = topTenWordK(k,i);
        s = vocab(idx);
        fprintf('\n%s', vocab{idx});
    end
end
fprintf('\nComplete!!\n');




