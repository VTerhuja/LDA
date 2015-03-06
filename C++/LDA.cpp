/****************************/
/* Author: Vikedo Terhuja	*/
/*		March 2015			*/
/****************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <omp.h>
using namespace std;

/* Constants */
const char SPACE_SYMBOL = ' ';
const char HASH_SYMBOL = '#';

#define MAX_K 10
#define MAX_DOCS 1500
#define MAX_WORDS 20000

/* Global Variables */
int D = 0;	// Number of documents
int W = 0;	// Number of words in vocabulary

/* # times word w is assigned to topic k over all the documents */
int CWK[MAX_WORDS][MAX_K] = { 0 };
/* # times word w is assigned to topic k in each document */ 
int CWDK[MAX_WORDS][MAX_DOCS][MAX_K] = { 0 };
/* # word w in each document */
int CWD[MAX_WORDS][MAX_DOCS] = { 0 };
/* # words assigned to topic k over all documents */
int CK[MAX_K] = { 0 };
/* psi(w,k) matrix */
double PSI[MAX_WORDS][MAX_K] = { 0 };

/** Note: Everything is zero indexed unlike Matlab **/
/* Just a reminder for myself */

int totalWords = 0;

struct data
{
	int docId;
	int wordId;
	int count;
};

struct word
{
	int docId;
	int wordId;
	int topicId;
};

vector<string> ReadVocabulary(string fileName)
{
	vector<string> vocabulary;
	ifstream fs(fileName);
	string rowString;
	if(fs.is_open())
	{
		while (getline(fs, rowString))
		{
			vocabulary.push_back(rowString);
		}
	}
	fs.close();
	return vocabulary;
}

data ExtractData(string rowString)
{
	data temp;
	int nextIndex = 0;
	int startIndex = 0;
	nextIndex = rowString.find_first_of(SPACE_SYMBOL, startIndex);
	string value = rowString.substr(startIndex, nextIndex-startIndex);
	temp.docId = atoi(value.c_str());
	startIndex = nextIndex + 1;
	nextIndex = rowString.find_first_of(SPACE_SYMBOL, startIndex);
	value = rowString.substr(startIndex, nextIndex-startIndex);
	temp.wordId = atoi(value.c_str());
	startIndex = nextIndex + 1;
	nextIndex = rowString.find_first_of(SPACE_SYMBOL, startIndex);
	value = rowString.substr(startIndex, nextIndex-startIndex);
	temp.count = atoi(value.c_str());
	return temp;
}

vector<data> ReadDataFile(string fileName)
{
	ifstream fs(fileName);
	string rowString;
	vector<data> dataSet;
	if(fs.is_open())
	{
		int pIndex = 0;
		int count = 0;
		while(!fs.eof())
		{
			getline(fs, rowString);
			count++;
			if(count > 3)
			{
				dataSet.push_back(ExtractData(rowString));
			}
			else
			{
				if(count == 1)
					D = atoi(rowString.c_str());
				if(count == 2)
					W = atoi(rowString.c_str());
			}
		}
	}
	else
	{
		cout<< "Error: File: "<< fileName << " not found\n";
	}
	fs.close();
	return dataSet;
}

vector<word> PreprocessData(vector<data> dataSet, int K)
{
	vector<word> words;
	for(int i=0; i<dataSet.size(); i++)
	{
		int nWords = dataSet[i].count;
		for(int j=0; j<nWords; j++)
		{
			word tempWord;
			/* Zero indexed */
			tempWord.docId = dataSet[i].docId-1;
			tempWord.wordId = dataSet[i].wordId-1;
			tempWord.topicId = rand() % K;
			words.push_back(tempWord);
		}
	}
	return words;
}

int GetDiscreteRandomNumber(vector<double> p)
{
	/* Cumulative sum */
	vector<double> cumSum;
	for(int i=0; i<p.size(); i++)
	{
		double val = p[i];
		if(i == 0)
			cumSum.push_back(val);
		else
			cumSum.push_back(val + cumSum[i-1]);			
	}

	double randVal = ((double) rand() / (RAND_MAX));
	for(int i=0; i<cumSum.size(); i++)
	{
		if(cumSum[i] >= randVal)
		{
			return i;
		}
	}
	return p.size()-1;
}

void NormalizeDistribution(vector<double> &p)
{
	double sump = 0.0;
	for(int i=0; i<p.size(); i++)
	{
		sump += p[i];
	}

	for(int i=0; i<p.size(); i++)
	{
		p[i] /= sump;
	}
}

void CountWordsPerTopicinAllDocs(vector<word> words, int nTopics)
{
	/* Count number of times word w is assigned topic t over the entire document */
	for(int i=0; i<words.size(); i++)
	{
		int wId = words[i].wordId;
		int kId = words[i].topicId;
		int dId = words[i].docId;
		CWK[wId][kId]++;
		CWDK[wId][dId][kId]++;
		CWD[wId][dId]++;
		CK[kId]++;
	}
}

void NormalizeRow(int k, int sumk)
{
	for(int i=0; i<W; i++)
	{
		PSI[i][k] = (double)(CWK[i][k] + 1)/sumk;	
	}
}

typedef std::pair<double,int> wpPair;
bool comparator ( const wpPair& l, const wpPair& r)
   { return l.first < r.first; }

int main( int argc, char** argv )
{
	int K = 5;
	cout<<"\nTOPICS: "<<K<<endl;
	cout<<"Reading dataset and vocabulary..."<<endl;
	vector<string> vocab = ReadVocabulary("vocab.nips.txt");
	vector<data> dataSet = ReadDataFile("docword.nips.txt");
	vector<word> words = PreprocessData(dataSet, K);
	totalWords = words.size();

	CountWordsPerTopicinAllDocs(words, K);

	double start = omp_get_wtime();
	int trials = 500;
	for(int t=0; t<trials; t++)
	{
		cout<<"\nIteration: "<<t+1;
		for(int i=0; i<words.size(); i++)
		{
			int dId = words[i].docId;
			int wId = words[i].wordId;
			int kId = words[i].topicId;			

			vector<double> p;
			for(int k=0; k<K; k++)
			{
				/* # of times word w is assigned to topic k not including the current word */
				int fVal = CWK[wId][k];
				if(fVal > 0)	fVal--;

				/* # of times word w in document dId is assigned to topic k not including the current word */
				int sVal = CWDK[wId][dId][k];
				if(sVal > 0)	sVal--;

				/* # of words assigned to topic k not including current word */
				int sumF = CK[k] - 1 + W;
				/* Sum: # word w in document d */
				int sumS = CWD[wId][dId] - 1 + K;
				/* fTerm: P(W(n,d) | Z(n,d) = k, Z(-n,d), W(-n,d)) */
				double fTerm = (double)(fVal + 1) / (sumF);
				/* sTerm: P(Z(n,d) = k | Z(-n,d), alpha) */
				double sTerm = (double)(sVal + 1) / (sumS);
				/* first * second: P(Z(n,d) = k | Z(-n,d) * W(.,d), alpha, beta) */
				p.push_back(fTerm * sTerm);
			}

			/* Normalize the distribution */
			NormalizeDistribution(p);
			/* Sample new topic from the distribution */
			int newk = GetDiscreteRandomNumber(p);
			/* Assign new topic to current word */
			words[i].topicId = newk;
			/* Reduce the count for kId topic from word-topic count matrix */
			CWK[wId][kId]--;
			/* Increase the count for newK topic in word-topic count matrix */
			CWK[wId][newk]++;
			/* Reduce the count for kId topic from word-document-topic count matrix */
			CWDK[wId][dId][kId]--;
			/* Increase the count for newK topic from word-document-topic count matrix */
			CWDK[wId][dId][newk]++; 
			/* Reduce count in old k */
			CK[kId]--;
			/* Increase count in new k */
			CK[newk]++;
		}
	}
	cout<<"\nTrial Complete"<<endl;
	double end = omp_get_wtime();
	cout<<"\nTime elapsed"<<" :"<<end - start;

	ofstream outputFile;
	outputFile.open("nips_result_5_500.txt");
	cout<<"\nCompute psi(w,k) ..."<<endl;
	for(int k=0; k<K; k++)
	{
		vector<wpPair> p;	
		NormalizeRow(k, CK[k] + W);

		/* Create index and probability pair */
		for(int i=0; i<W; i++)
		{
			wpPair temp;
			temp.first = PSI[i][k];
			temp.second = i;
			p.push_back(temp);
		}
		/* Sort in Descending Order */
		std::sort(p.rbegin(),p.rend(),comparator);

		cout<<"\nTopic: #"<<k+1<<endl;
		outputFile<<"\nTopic: #"<<k+1<<endl;

		/* Output top 10 words in each topic */
		for(int i=0; i<10; i++)
		{
			cout<<vocab[p[i].second]<<" "<<p[i].first<<endl;
			outputFile<<vocab[p[i].second]<<" "<<p[i].first<<"\n";
		}
	}
	cout<<"\nComplete!!"<<endl;
	outputFile.close();
}