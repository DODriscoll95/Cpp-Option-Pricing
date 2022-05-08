//
//  PutOptionValueCalculator.hpp


#pragma once

#include "OptionValueCalculator.hpp"


#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdio.h>


class PutOptionValueCalculator : public OptionValueCalculator
{   // DERIVED PUT CLASS FROM OPTIONVALUECALCULATOR
private:
    //member variable
    double m_K;
public:
    //constructor
    PutOptionValueCalculator(double K);
    //destructor
    ~PutOptionValueCalculator();
    //value calc
    double Value(double S) const override;
    
};
