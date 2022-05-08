//
//  CallOptionValueCalculator.hpp
//  Test

#pragma once

#include "OptionValueCalculator.hpp"


#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdio.h>


class CallOptionValueCalculator : public OptionValueCalculator
{
    // DERIVED CALL CLASS FROM OPTIONVALUECALCULATOR
private:
    double m_K;

public:
    
//constructor
    CallOptionValueCalculator(double K);
    
        //destructor
    ~CallOptionValueCalculator();
    
    //value calc
    double Value(double S) const override;
    
};
