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