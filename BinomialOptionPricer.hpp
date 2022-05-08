//
//  BinomialOptionPricer.hpp

#pragma once

#include "OptionValueCalculator.hpp"
#include "CallOptionValueCalculator.hpp"
#include "PutOptionValueCalculator.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdio.h>


class BinomialOptionPricer
{
    
private:
    
    //member variables
    unsigned int m_max_depth;
    double m_growth_factor;
    double m_growth_probability;
    
public:
    
    //constructor
    BinomialOptionPricer(unsigned int max_depth, double growth_factor, double growth_probability);
    
    //destructor
    ~BinomialOptionPricer();
    
    
    //algorithm functions
    double price_vanilla_option_european_recursion(unsigned int depth, double delta_t, double S0, double r, const OptionValueCalculator& value_calc);
        
    double price_vanilla_option_american_recursion(unsigned int depth, double delta_t, double S0, double r, const OptionValueCalculator& value_calc);
        
    double price_vanilla_option_european(unsigned int depth, double delta_t, double S0, double r, const OptionValueCalculator& value_calc);
        
    double price_vanilla_option_american(unsigned int depth, double delta_t, double S0, double r, const OptionValueCalculator& value_calc);
};

//Black Scholes

double Black_Scholes(double t, double growth_factor, double S0, double r, double delta_t, double K);

