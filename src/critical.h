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


std::obj processOp(std::netlist net, std::operation op) {
	std::obj newOp;
	//add critical path op


	newOp = *op.output;
	std::cout << "NEW INPUT: " << newOp.name << std::endl;
	return newOp;


}

double getCritPath(std::netlist net) {
	bool done = false;
	std::obj newOp;

	for (auto& in : net.inputs) {
		std::cout << "INPUT NAME: " << in.name << std::endl;
		for (auto& ops : net.operations) {
			std::cout << ops.getType() << ": " << std::endl;
			for (auto& opIn : ops.inputs) {
				if (opIn->name.compare(in.name))
				{
					done = false;
					while (done != true) {
						newOp = processOp(net, newOp);
						for (auto& outs : net.outputs) {
							if (outs.name.compare(newOp.output->name) == 0) {
								std::cout << "WE FINISHED, Output: " << newOp.output->name << std::endl;
								done = true;
							}
						}

					}
				}
			}
		}
	}



	return 0.0;
}

