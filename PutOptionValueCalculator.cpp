//
//  PutOptionValueCalculator.cpp


#include "OptionValueCalculator.hpp"
#include "PutOptionValueCalculator.hpp"


#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>


PutOptionValueCalculator::PutOptionValueCalculator(double K)
    :
    m_K(K)
{  // PRINT STATEMENT TO CHECK OPTION WAS CREATED
    std::cout << "PUT OPTION CREATED WITH STRIKE " << m_K << "\n\n";
}

PutOptionValueCalculator::~PutOptionValueCalculator()
{    // PRINT STATEMENT TO CHECK OPTION WAS DELETED
    std::cout << "PUT OPTION WITH STRIKE " << m_K << " DELETED\n\n";
}

double PutOptionValueCalculator::Value(double S) const
{
    // PUT OPTION VALUE (OVERRIDES THE BASE CLASS)
    return std::max((this->m_K-S), 0.0);
}
