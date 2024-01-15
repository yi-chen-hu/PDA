clc;
clear all;
close all;
%=========================================================================%
%                           Global Declaration                            %
%=========================================================================%
inputFile = 'case1.txt';
outputFile = 'output.txt';

%=========================================================================%
%                                Read files                               %
%=========================================================================%
inputFileID = fopen(inputFile);
outputFileID = fopen(outputFile);
data = textscan(inputFileID, '%f %f', 1);
lowerBound = data{1,1};
upperBound = data{1,2};
[blockName, blockWidth, blockHeight] = textread(inputFile, '%s %d %d','headerlines', 1);
data = textscan(outputFileID, '%s %d %d %s', 'HeaderLines', 2);
blockX = data{2};
blockY = data{3};
R = data{4};

%=========================================================================%
%                                 Draw plot                               %
%=========================================================================%
chipWidth = 0;
chipHeight = 0;
for i = 1 : length(blockName)
    if R(i) == "R"
        temp = blockWidth(i);
        blockWidth(i) = blockHeight(i);
        blockHeight(i) = temp;
    end
    rectangle('Position', [blockX(i), blockY(i), blockWidth(i), blockHeight(i)], 'Linewidth', 2);
    text(blockX(i) + blockWidth(i) / 2, blockY(i) + blockHeight(i) / 2, string(blockName(i)), 'HorizontalAlignment', 'center', 'FontSize', 12);

    if (blockX(i) + blockWidth(i) > chipWidth)
        chipWidth = blockX(i) + blockWidth(i);
    end
    if (blockY(i) + blockHeight(i) > chipHeight)
        chipHeight = blockY(i) + blockHeight(i);
    end
end

%=========================================================================%
%                               Print info                                %
%=========================================================================%
disp("chipWidth = ");
disp(chipWidth);
disp("chipHeight = ");
disp(chipHeight);
disp("aspect ratio = ");
disp(double(chipWidth) / double(chipHeight));
