//
//  CallOptionValueCalculator.cpp



#include "OptionValueCalculator.hpp"
#include "CallOptionValueCalculator.hpp"


#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

//initialise constructor with the strike price K, store in member variable m_K
CallOptionValueCalculator::CallOptionValueCalculator(double K)
    :
    m_K(K)
{
    // PRINT STATEMENT TO CHECK OPTION WAS CREATED NOT ESSNETIAL BUT SHOWS US WHATS GOING ON
    std::cout << "CALL OPTION CREATED WITH STRIKE " << m_K << "\n\n";
}

CallOptionValueCalculator::~CallOptionValueCalculator()
{
    // PRINT STATEMENT TO CHECK OPTION WAS DELETED
    std::cout << "CALL OPTION WITH STRIKE " << m_K << " DELETED\n\n";
}

double CallOptionValueCalculator::Value(double S) const
{
    // CALL OPTION VALUE (OVERRIDES THE BASE CLASS)
    return std::max((S - (this->m_K)), 0.0);
}
