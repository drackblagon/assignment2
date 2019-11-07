#pragma once

#include <iostream>

namespace std {

class netlist {
public:
    vector<obj> inputs;
    vector<obj> outputs;
    vector<obj> registers;
    vector<obj> wires;
    vector<operation> operations;
    
    obj *getObj (string name) {
        for (auto& oTemp: inputs) {
            if (name.compare(oTemp.name) == 0) {
                return &oTemp;
            }
        }
        
        for (auto& oTemp: outputs) {
            if (name.compare(oTemp.name) == 0) {
                return &oTemp;
            }
        }
        
        for (auto& oTemp: registers) {
            if (name.compare(oTemp.name) == 0) {
                return &oTemp;
            }
        }
        
        for (auto& oTemp: wires) {
            if (name.compare(oTemp.name) == 0) {
                return &oTemp;
            }
        }
        
        return nullptr;
    }
    
    int addObject(obj::objTypeEnum objType, vector<string> tokens) {
        if (tokens.size() < 3) {
            cout << "ERROR: no more tokens" << endl;
            return FAILURE;
        }
        
        tokens.erase(tokens.begin());
        obj::variableTypeEnum vType = obj::getTypeFromString(tokens.at(0));
        if (vType == obj::UNKNOWN) {
            cout << "ERROR: Unknown variable type found: " << tokens.at(0) << endl;
            return FAILURE;
        }
        tokens.erase(tokens.begin());
        
        vector<obj> *vec;
        if (objType == obj::INPUT) {
            vec = &inputs;
        }
        else if (objType == obj::OUTPUT) {
            vec = &outputs;
        }
        else if (objType == obj::REGISTER) {
            vec = &registers;
        }
        else {
            vec = &wires;
        }
        
        for(auto& s: tokens) {
            size_t loc;
            if ((loc = s.find_first_of(",")) != string::npos) {
                s = s.substr(0, loc);
            }
            obj o;
            o.name = s;
            o.objType = objType;
            o.vType = vType;
            if (getObj(o.name) != nullptr) {
                cout << "ERROR: obj " << o.name << " is already defined" << endl;
                return FAILURE;
            }
            vec->push_back(o);
        }
        
        return SUCCESS;
    }
    
    int addOperation(vector<string> tokens, string op_line) {
        if (tokens.size() < 3) {
            cout << "ERROR: no more tokens" << endl;
            return FAILURE;
        }
        operation op;
        op.op_line = op_line;
        obj * o;
        if ((o = getObj(tokens.at(0))) == nullptr) {
            cout << "ERROR: could not find variable assigned to" << endl;
            return FAILURE;
        }
        op.output = o;
        
        if (tokens.at(1).compare("=") != 0) {
            cout << "ERROR: no assignment symbol (=) found" << endl;
            return FAILURE;
        }
        
        if ((o = getObj(tokens.at(2))) == nullptr) {
            cout << "ERROR: could not find first input variable" << endl;
            return FAILURE;
        }
        op.inputs.push_back(o);
        
        operation::operatorEnum oEnum;
        if (tokens.size() == 3) {
            oEnum = operation::REG;
        }
        else {
            oEnum = operation::getTypeFromString(tokens.at(3));
            if (oEnum == operation::UNKNOWN) {
                cout << "ERROR: unknown operation" << endl;
                return FAILURE;
            }
            
        }
        op.op = oEnum;
        
        
        switch (op.op) {
            case operation::REG:
                break;
            case operation::MUX2x1:
                if (tokens.size() != 7) {
                    cout << "ERROR: wrong number of arguments found" << endl;
                    return FAILURE;
                }
                
                if (tokens.at(5).compare(":") != 0) {
                    cout << "ERROR: no colon found in mux" << endl;
                    return FAILURE;
                }
                
                if ((o = getObj(tokens.at(4))) == nullptr) {
                    cout << "ERROR: could not find variable assigned to" << endl;
                    return FAILURE;
                }
                op.inputs.push_back(o);
                
                if ((o = getObj(tokens.at(6))) == nullptr) {
                    cout << "ERROR: could not find variable assigned to" << endl;
                    return FAILURE;
                }
                op.inputs.push_back(o);
                
                break;
                
            case operation::ADD:
                //Fall through
            case operation::SUB:
                if (tokens.size() != 5) {
                    cout << "ERROR: wrong number of arguments found" << endl;
                    return FAILURE;
                }
                if (tokens.at(4).compare("1") == 0) {
                    op.op = (op.op == operation::ADD)? operation::INC : operation::DEC;
                    break;
                }
                if ((o = getObj(tokens.at(4))) == nullptr) {
                    cout << "ERROR: could not find variable assigned to" << endl;
                    return FAILURE;
                }
                op.inputs.push_back(o);
                break;
                
                
            case operation::MUL:
                //Fall through
            case operation::COMP_GT:
                //Fall through
            case operation::COMP_LT:
                //Fall through
            case operation::COMP_EQ:
                //Fall through
            case operation::SHR:
                //Fall through
            case operation::SHL:
                //Fall through
            case operation::DIV:
                //Fall through
            case operation::MOD:
                if (tokens.size() != 5) {
                    cout << "ERROR: wrong number of arguments found" << endl;
                    return FAILURE;
                }
                if ((o = getObj(tokens.at(4))) == nullptr) {
                    cout << "ERROR: could not find variable assigned to" << endl;
                    return FAILURE;
                }
                
                //TODO: still need to figure out a way to get if it is a signed operation
                op.inputs.push_back(o);
                break;
            default:
                return FAILURE;
        }
        
        operations.push_back(op);
        return SUCCESS;
    }
    
    int getVerilog(stringstream &output, string outFileName) {
        string moduleName = outFileName;
        int pos;
        int lastSlashPos;
        if ((lastSlashPos = outFileName.find_last_of("/")) != string::npos) {
            if (lastSlashPos > 0) {
                moduleName = moduleName.substr(lastSlashPos + 1, moduleName.length());
            }
        }
        
        if ((pos = moduleName.find(".")) != string::npos) {
            if (pos > 0) {
                moduleName = moduleName.substr(0, pos);
            }
        }
        
        output << "module " << moduleName << " (";
        for(auto& o: inputs) {
            output << o.name;
            output << ", ";
        }
        
        output << "Clk, Rst, ";
        
        for(auto& o: outputs) {
            output << o.name;
            if (&o != &outputs.back()) {
                output << ", ";
            }
        }
        
        output << ");" << endl;
        
        
        for(auto& o: inputs) {
            o.getInstanceString("input", output);
        }
        output << "\tinput Clk;" << endl;
        output << "\tinput Rst;" << endl;
        for(auto& o: outputs) {
            o.getInstanceString("output", output);
        }
        
        output << endl;
        
        for(auto& o: registers) {
            o.getInstanceString("reg", output);
        }
        for(auto& o: wires) {
            o.getInstanceString("wire", output);
        }
        
        output << endl;
        
        for(auto& op: operations) {
            op.getOutputVerilog(output);
        }
        
        output << "endmodule" << endl;
        
        
        return SUCCESS;
    }

    void print() {
        cout << "netlist:" << endl;
        cout << "\tinputs:" << endl;
        for(auto& o: inputs) {
            o.print();
        }
        cout << "\toutputs:" << endl;
        for(auto& o: outputs) {
            o.print();
        }
        cout << "\tregisters:" << endl;
        for(auto& o: registers) {
            o.print();
        }
        cout << "\twires:" << endl;
        for(auto& o: wires) {
            o.print();
        }
        cout << "\toperations:" << endl;
        for(auto& ops: operations) {
            ops.print();
        }
    }
};

}
