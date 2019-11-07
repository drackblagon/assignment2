#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

#include "convert.h"

using namespace std;

int main(int argc, char *argv[]) {
    int result = 1;
    string line;
    ifstream inFile;
    ofstream outFile;
    string outFileName(argv[2]);
    
    stringstream in, out;
    
    if (argc != 3)
    {
        cout << "ERROR: Wrong number of arguments!" << endl;
        goto END;
    }
    
    inFile.open(argv[1]);
    if (!inFile.is_open())
    {
        cout << "ERROR: Unable to open input file!" << endl;
        goto END;
    }
    
    outFile.open(argv[2]);
    if (!outFile.is_open())
    {
        cout << "ERROR: Unable to open output file!" << endl;
        goto END;
    }
    
    in << inFile.rdbuf();
    if (convertToVerilog(in, out, outFileName) == FAILURE)
    {
        cout << "ERROR: Failed to parse input file!" << endl;
        goto END;
    }
        
    outFile << out.str();
    
    result = 0;
    END:
    
    if (inFile.is_open()) {
        inFile.close();
    }
    if (outFile.is_open()) {
        outFile.close();
    }
    
	getchar();
    return result;
}
