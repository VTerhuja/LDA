function [D, W, NNZ, data ] = ReadDataSet( fileName )
%ReadDataSet: Extract information about dataset
try
   fid = fopen(fileName, 'rt');
   header = textscan(fid, '%d', 3, 'Delimiter', '\n');
   D = header{1}(1);
   W = header{1}(2);
   NNZ = header{1}(3);
   
   tempData = textscan(fid, '%d %d %d');
   fclose(fid);
catch
   error('Failed to read text file %s.', fileName);
end

    data = zeros(size(tempData{1},1), 3);
    data(:,1) = tempData{1};
    data(:,2) = tempData{2};
    data(:,3) = tempData{3};
end

