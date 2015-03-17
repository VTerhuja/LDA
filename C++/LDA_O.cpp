/****************************/
/* Author: Vikedo Terhuja	*/
/*		March 2015			*/
/****************************/

/* Note: The difference in LDA.cpp and LDA_O.cpp is the numerator of the second term */
/* LDA.cpp: # of word w is assigned to topic k in document d */
/* LDA_O.cpp: # of words assigned to topic k in document d - TThis is according to the introduction and tutorial */
/* This difference yields unique words (top 10 words) for each topic (observed in upto 10 topics) */

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
#define MAX_DOCS 10000
#define MAX_WORDS 200000

/* Global Variables */
int D = 0;	// Number of documents
int W = 0;	// Number of words in vocabulary

int CKW[MAX_K][MAX_WORDS] = { 0 };
int CKD[MAX_K][MAX_DOCS] = { 0 };
int CD[MAX_DOCS] = { 0 };
int CK[MAX_K] = { 0 };
double PSI[MAX_K][MAX_WORDS] = { 0 };

/** Note: Everything is zero indexed unlike Matlab **/

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

vector<word> PreprocessData(vector<data> dataSet, int nTopics)
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
			tempWord.topicId = rand()%nTopics;
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
		if(cumSum[i] > randVal)
		{
			return i;
		}
	}
	return 0;
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
		int tId = words[i].topicId;
		int dId = words[i].docId;
		CKW[tId][wId]++;
		CKD[tId][dId]++;
		CD[dId]++;
		CK[tId]++;
	}
}

int SumColumn(int row)
{
	int total = 0;
	for(int i=0; i<W; i++)
	{
		total += CKW[row][i];
	}
	return total;
}

void NormalizeRow(int k, int sumk)
{
	for(int i=0; i<W; i++)
	{
		PSI[k][i] = (double)(CKW[k][i] + 1)/sumk;	
	}
}

typedef std::pair<double,int> mypair;
bool comparator ( const mypair& l, const mypair& r)
   { return l.first < r.first; }

int main( int argc, char** argv )
{
	vector<mypair> a;
	for(int i=0;i<5;i++)
	{
		mypair temp;
		temp.first = rand()%5;
		temp.second = i;
		a.push_back(temp);
	}

	std::sort(a.rbegin(),a.rend(),comparator);

	int nTopics = 2;
	cout<<"\nTOPICS: "<<nTopics<<endl;
	cout<<"Reading dataset and vocabulary..."<<endl;
	vector<string> vocab = ReadVocabulary("vocab.kos.txt");
	vector<data> dataSet = ReadDataFile("docword.kos.txt");
	vector<word> words = PreprocessData(dataSet, nTopics);
	totalWords = words.size();

	CountWordsPerTopicinAllDocs(words, nTopics);

	double start = omp_get_wtime();
	int trials = 50;
	for(int t=0; t<trials; t++)
	{
		cout<<"\nIteration: "<<t+1;
		for(int i =0; i<words.size(); i++)
		{
			int dId = words[i].docId;
			int wId = words[i].wordId;
			int kId = words[i].topicId;			
			int K = nTopics;

			vector<double> p;
			for(int k=0; k<nTopics; k++)
			{
				int fVal = CKW[k][wId];
				if(fVal > 0 && k == kId)
				{
					fVal -= 1;
				}

				int sVal = CKD[k][dId];
				if(sVal > 0 && k == kId)
				{
					sVal -= 1;
				}

				/* Sum: # words assigned to topic k => Sum all the columns of the kth row */
				int sumkwCol = CK[k] + W;
				/* Sum: # words in document d => Sum all the rows of the dth column */
				int sumkdCol = CD[dId] + K;
				double first = (double)(fVal + 1) / (sumkwCol);
				double second = (double)(sVal + 1) / (sumkdCol);
				p.push_back(first * second);
			}

			/* Normalize the distribution */
			NormalizeDistribution(p);
			/* Sample new topic from the distribution */
			int newk = GetDiscreteRandomNumber(p);
			/* Assign new topic to current word */
			words[i].topicId = newk;
			/* Reduce the count from topic-word count matrix */
			CKW[kId][wId]--;
			/* Increase the count in topic-word count matrix */
			CKW[newk][wId]++;
			/* Reduce the count from topic-document count matrix */
			CKD[kId][dId]--;
			/* Increase the count in topic-document count matrix */
			CKD[newk][dId]++; 
			/* Reduce count in old k */
			CK[kId]--;
			CK[newk]++;
		}
	}
	cout<<"\nTrial Complete"<<endl;
	double end = omp_get_wtime();
	cout<<"\nTime elapsed"<<" :"<<end - start;

	ofstream outputFile;
	outputFile.open("kos_result_k_2.txt");
	cout<<"\nCompute psi(w,k) ..."<<endl;
	for(int k=0; k<nTopics; k++)
	{
		vector<mypair> p;
		int sumRow = SumColumn(k) + W;
		NormalizeRow(k, sumRow);

		/* Create index and probability pair */
		for(int i=0; i<W; i++)
		{
			mypair temp;
			temp.first = PSI[k][i];
			temp.second = i;
			p.push_back(temp);
		}
		/* Sort in Descending Order */
		std::sort(p.rbegin(),p.rend(),comparator);

		cout<<"\nTopic: #"<<k+1<<endl;
		outputFile<<"\nTopic: #"<<k+1<<endl;

		for(int i=0; i<10; i++)
		{
			int idx = p[i].second;
			cout<<vocab[idx]<<" "<<p[i].first<<endl;
			outputFile<<vocab[idx]<<" "<<p[i].first<<"\n";
		}
	}
	cout<<"\nComplete!!"<<endl;
	outputFile.close();
}