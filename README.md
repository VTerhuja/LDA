# LDA
**Topic Model using Gibbs Sampling for Latent Dirichlet Allocation:**

Introduction: http://blog.echen.me/2011/08/22/introduction-to-latent-dirichlet-allocation/<br />
Tutorial of Probabilistic Topic Model: http://www.cs.princeton.edu/~blei/papers/icml-2012-tutorial.pdf<br />

### Related papers:
1. http://www.ics.uci.edu/~newman/pubs/fastlda.pdf
2. http://psiexp.ss.uci.edu/research/papers/sciencetopics.pdf

### Description:
There are two versions:<br />
1. LDA_O (c++) : This implementation is according to the paper.<br />
2. LDA (c++ & Matlab) : Instead of computing p(topic t | document d), the joint probability p( word w, topic t | document d) is computed.<br />

This difference in #2 yields unique words (observed for the top 10 words) for each topic (observed up to 10 topics). 