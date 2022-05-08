//
//  OptionValueCalculator.hpp

#pragma once

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdio.h>



class OptionValueCalculator
{
protected:
    
    //strike price member variable
    double m_K;
    
public:
   
    //constructor
    OptionValueCalculator();
    
    //destructor
    virtual ~OptionValueCalculator();
    
    
    //value calc, vitrual
    virtual double Value(double S) const;
};

