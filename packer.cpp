#include "packer.h"

Packer::Packer(){

}

Packer::~Packer(){
	
}

void Packer::setInputCharts(const std::vector<Chart>& _charts){
	charts_ = _charts;
}

void Packer::getCharts(std::vector<Chart>& _charts){
	_charts = charts_;
}

void pack(){

	
}