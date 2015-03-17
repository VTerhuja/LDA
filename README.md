# LDA
Topic Model: Using Gibbs Sampling for Latent Dirichlet Allocation

Introduction: http://blog.echen.me/2011/08/22/introduction-to-latent-dirichlet-allocation/
Tutorial of Probabilistic Topic Model: http://www.cs.princeton.edu/~blei/papers/icml-2012-tutorial.pdf

# Related papers:
1. http://www.ics.uci.edu/~newman/pubs/fastlda.pdf
2. http://psiexp.ss.uci.edu/research/papers/sciencetopics.pdf

There are two versions:
1. LDA_O (c++) : This implementation is according to the paper.
2. LDA (c++ & Matlab) : Instead of computing p(topic t | document d), the joint probability p( word w, topic t | document d) is computed.
This difference in the implementation yields unique words (observed for the top 10 words) for each topic (observed upto 10 topics). 