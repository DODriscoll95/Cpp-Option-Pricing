//
//  OptionValueCalculator.cpp



#include "OptionValueCalculator.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>


//constructor
OptionValueCalculator::OptionValueCalculator()
    
{
    // Check Object is working
    std::cout << "BINOMIAL OPTION TREE CREATED\n\n";
}

//destructor
OptionValueCalculator::~OptionValueCalculator()
{   // Check deletion
    std::cout << "BINOMIAL OPTION DELETED\n\n";
}


double OptionValueCalculator::Value(double S) const {

    return 0.0;
}
