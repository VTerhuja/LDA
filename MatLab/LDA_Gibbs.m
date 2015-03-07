% Latent Dirichlet Allocation for Topic Model
clear
clc
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

fprintf('Random Initalization . . .');
% nDocData : All the words in all the documents.
% Cwk      : # times word w is assigned topic k over all documents
% Cwdk     : # times word w is assigned topic k in document d
% Cwd      : # w words in d document
% Ck       : # words assigned to topic k
[nData, Cwk, Cwdk, Cwd, Ck ] = RandomInitializationAll(data, K, W, D);
fprintf('\nDone!');

T = 10;
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
            firstNum = 0;
            if(Cwk(w,k) > 0)
                firstNum = Cwk(w,k) - 1;
            end
            % # words assigned to topic k in document d except current word
            secondNum = 0;
            if(Cwdk(w,d,k) > 0)
                secondNum = Cwdk(w,d,k) - 1;
            end
            % Sum: # words assigned to topic k
            sumkwCol = Ck(k) - 1 + W;
            % Sum: # words in document d
            sumkdCol = Cwd(w,d) - 1 + K;
            first = double(firstNum + 1) / double(sumkwCol); % divide by Sum of the row
            second = double(secondNum + 1) / double(sumkdCol); % divide by Sum of the row
            p(k) = first * second;
        end
        
        % Compute sum for normalization
        sump = sum(p);
        % Normalize the probability for each topic
        p = p/sump;
        % Sample new topic from the distribution
        newk = discreternd(p,1,i);
        % Assign new topic
        nData(i,3) = newk;
        % Reduce the count for kId topic from word-topic count matrix
        Cwk(w,kId) = Cwk(w,kId)-1;
        % Increase the count for newK topic in word-topic count matrix
        Cwk(w,newk) = Cwk(w,newk)+1;
        % Reduce the count for kId topic from word-document-topic count matrix
        Cwdk(w,d,kId) = Cwdk(w,d,kId)-1;
        % Increase the count for newK topic from word-document-topic count matrix 
        Cwdk(w,d,newk) = Cwdk(w,d,newk)+1;    
        % Reduce count in old k */
        Ck(kId) = Ck(kId) - 1;
		% Increase count in new k */
		Ck(newk) = Ck(newk) + 1;
    end    
end
fprintf('\nDone!');
fprintf('\nCompute psi(w,k)...');

% Compute psi(w,k), sort, display top 10 words in each topic
vocab = GetVocabulary('vocab.kos.txt');
topTenProbK = zeros(K,10);
topTenWordK = zeros(K,10);
beta = ones(W,K);
psiwk = Cwk + beta;
fprintf('\nCompute psi(w,k)...');
for k=1:K
   sumRow = Ck(k) + double(W);
   psiwk(:,k) = psiwk(:,k)./sumRow;
   [prob, idx] = sort(psiwk(:,k),'descend');
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




