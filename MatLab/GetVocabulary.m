function [ vocab ] = GetVocabulary( fileName )
%GetVocabulary: Get all the words in the vocabulary from the file
%   fileName: name of the file
%   vocab: the vocabulary

try
  % vocab = textread(fileName, '%s', 'delimiter', '\n');
   fid = fopen(fileName, 'rt');
   temp = textscan(fid, '%s', 'delimiter', '\n');
   fclose(fid);
catch
   error('Failed to read text file %s.', fileName);
end

    vocab = temp{1};
end

