#pragma once
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>
#include <iterator>
#include <stdbool.h>
#include "netlist.h"
#include "operation.h"
#include "obj.h"


std::obj processInput(std::netlist net, std::obj input) {
	std::obj newInput;

	for (auto& ops : net.operations) {
		std::cout << ops.getType() << ": " << std::endl;
		for (auto& ins : ops.inputs) {
			std::cout << ins->name << std::endl;
			if (input.name.compare(ins->name) == 0) {
				newInput = *ops.output;
				std::cout << "NEW INPUT: " << newInput.name << std::endl;
				return newInput;
			}
		}
	}

}

double getCritPath(std::netlist net) {
	bool done = false;
	std::obj newInput;

	for (auto& o : net.inputs) {
		std::cout << "INPUT NAME: " << o.name << std::endl;
		done = false;
		newInput = o;
		while (done != true) {
			newInput = processInput(net, newInput);
			for (auto& outs : net.outputs) {
				if (outs.name.compare(newInput.name) == 0) {
					std::cout << "WE FINISHED, Output: " << newInput.name << std::endl;
					done = true;
				}
			}
			
		}
	}



	return 0.0;
}

