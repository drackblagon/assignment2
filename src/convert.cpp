//
//  convert.cpp
//  dpgen
//
//  Created by Zach Halvorsen on 10/30/19.
//

#include <iostream>
#include <vector>
#include <string>
#include <iterator>

#include "convert.h"
#include "obj.h"
#include "operation.h"
#include "netlist.h"
#include "critical.h"


using namespace std;

int convertToVerilog(stringstream &in, stringstream &out, string outFileName) {
    int result = FAILURE;
    string line;
    netlist nl;
    
    while(getline(in, line)) {
        if (line.find_first_not_of(" \t\n\v\f\r") == string::npos) {
            //This line is only whitespace
            continue;
        }
        if ((line.length() >= 2) && (line[0] == '/') && (line[1] == '/')) {
            //This line is a comment
            continue;
        }
        
        istringstream buf(line);
        istream_iterator<string> beg(buf), end;
        
        vector<string> tokens(beg, end);
        
        // Ignore comments
        for(vector<string>::size_type i = 0; i < tokens.size(); ++i) {
            int pos;
            if ((pos = tokens.at(i).find("//")) != string::npos) {
                vector<string>::iterator nth = tokens.begin() + i;
                if (pos > 0) {
                    tokens.at(i) = tokens.at(i).substr(0, pos);
                    if (nth != tokens.end()) {
                        nth++;
                    }
                }
                tokens.erase(nth, tokens.end());
                break;
            }
        }
        
        if (tokens.at(0).compare("input") == 0) {
            if (nl.addObject(obj::INPUT, tokens) != SUCCESS) {
                cout << "ERROR: adding obj to inputs" << endl;
                return result;
            }
        }
        else if (tokens.at(0).compare("output") == 0) {
            if (nl.addObject(obj::OUTPUT, tokens) != SUCCESS) {
                cout << "ERROR: adding obj to outputs" << endl;
                return result;
            }
        }
        else if (tokens.at(0).compare("register") == 0) {
            if (nl.addObject(obj::REGISTER, tokens) != SUCCESS) {
                cout << "ERROR: adding obj to wires" << endl;
                return result;
            }
        }
        else if (tokens.at(0).compare("wire") == 0) {
            if (nl.addObject(obj::WIRE, tokens) != SUCCESS) {
                cout << "ERROR: adding obj to wires" << endl;
                return result;
            }
        }
        else {
            if (nl.addOperation(tokens, line) != SUCCESS) {
                cout << "ERROR: adding operation" << endl;
                return result;
            }
        }
    }

    nl.print();
    
    if (nl.getVerilog(out, outFileName) != SUCCESS) {
        cout << "ERROR: converting to verilog" << endl;
        return result;
    }

	getCritPath(nl);
    
    result = SUCCESS;
    
    return result;
}

