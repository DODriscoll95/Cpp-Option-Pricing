//
//  BinomialOptionPricer.cpp
// Created Using XCode on 2020 MacBook Air

#include "OptionValueCalculator.hpp"
#include "CallOptionValueCalculator.hpp"
#include "PutOptionValueCalculator.hpp"
#include "BinomialOptionPricer.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <chrono>

//constructor to define member variables for the binomialoptionpricer object
BinomialOptionPricer::BinomialOptionPricer(unsigned int max_depth, double growth_factor, double growth_probability)
    :
    m_max_depth(max_depth),
    m_growth_factor(growth_factor),
    m_growth_probability(growth_probability)

{   // PRINT STATEMENT TO CHECK OPTION PRICER WAS CREATED
    std::cout << "BINOMIAL OPTION PRICER INITIATED \n\n";
}


BinomialOptionPricer::~BinomialOptionPricer()
    {   // // PRINT STATEMENT TO CHECK OPTION PRICER WAS TERMINATED
        std::cout << "BINOMIAL OPTION PRICER TERMINATED\n\n";
    }

double BinomialOptionPricer::price_vanilla_option_european_recursion(unsigned int depth, double delta_t, double S0, double r, const OptionValueCalculator& value_calc)

{   // PRICE A EUROPEAN OPTION USING THE FORWARD-RECURSION METHOD
    
    if(depth > this->m_max_depth)
    {   // MAX DEPTH USAGE
        std::cout << "DEPTH EXCEEDS THE PRE-SET LIMIT (MAX_DEPTH), LOWERING IT TO THE MAX_DEPTH, WARNING PARAMETERS MAY BE INCORRECT\n\n";
        depth = this->m_max_depth;
    }
    
    double ABOVE_NODE, BELOW_NODE;
    // DEFINE BASE CASE AT END OF TREE
    if(depth == 0)
    {   // VALUE_CALC TAKES CARE OF THE PUT/CALL IDENTITY
        return value_calc.Value(S0);
    }
    else
        //if we havent reached the expiry time at the end of the tree execute the recursion algo again (we are referring to depth here as the depth remaining in the tree to calculate for if we start at timestep 0
    {
        // ABOVE NODE (depth remaining decreases by one, stock goes up by factor of u, everything else is the same)
        ABOVE_NODE = price_vanilla_option_european_recursion(depth-1, delta_t, S0*this->m_growth_factor, r, value_calc);
        
        // BELOW NODE (depth remaining decreases by one, stock decreases)
        BELOW_NODE = price_vanilla_option_european_recursion(depth-1, delta_t, S0*(1.0/(this-> m_growth_factor)), r, value_calc);
        
        // CURRENT VALUE IS DISCOUNTED EXPECTED VALUE
        return exp(-r*delta_t) * (this->m_growth_probability * ABOVE_NODE + (1.0 - this->m_growth_probability) * BELOW_NODE);
    }
}

double BinomialOptionPricer::price_vanilla_option_american_recursion(unsigned int depth, double delta_t, double S0, double r, const OptionValueCalculator& value_calc)
{   // PRICE A AMERICAN OPTION USING THE FORWARD-RECURSION METHOD
    if(depth > this->m_max_depth)
    {
        std::cout << "DEPTH EXCEEDS THE PRE-SET LIMIT (MAX_DEPTH), LOWERING IT TO THE MAX_DEPTH, WARNING PARAMETERS MAY BE INCORRECT\n\n";
        depth = this->m_max_depth;
    }
    double ABOVE_NODE, BELOW_NODE, CURR_VALUE;
    // DEFINE BASE CASE
    
    if(depth == 0)
    {   // VALUE_CALC TAKES CARE OF THE PUT/CALL IDENTITY
        return value_calc.Value(S0);
    }
    else
    {
        // ABOVE NODE (depth remaining decreases by one, stock goes up by factor of u, everything else is the same) //this recursion doesnt use the (S0*u^2j-i) but it does the same thing
        ABOVE_NODE = price_vanilla_option_american_recursion(depth-1, delta_t, S0*this->m_growth_factor, r, value_calc);
       
        // BELOW NODE (depth remaining decreases by one, stock decreases)
        BELOW_NODE = price_vanilla_option_american_recursion(depth-1, delta_t, S0*(1.0/(this-> m_growth_factor)), r, value_calc);
        
        // CURRENT VALUE IS DISCOUNTED EXPECTED VALUE
        CURR_VALUE = exp(-r*delta_t) * (this->m_growth_probability * ABOVE_NODE + (1.0 - this->m_growth_probability) * BELOW_NODE);
        //early exercise condition
        return std::max(CURR_VALUE, value_calc.Value(S0));
    }
}


double BinomialOptionPricer::price_vanilla_option_european(unsigned int depth, double delta_t, double S0, double r, const OptionValueCalculator& value_calc)
{   // PRICE A EUROPEAN OPTION USING THE BACKWARD-INDUCTION METHOD
    
    if(depth > this->m_max_depth)
    {
        std::cout << "DEPTH EXCEEDS THE PRE-SET LIMIT (MAX_DEPTH), LOWERING IT TO THE MAX_DEPTH, WARNING PARAMETERS MAY BE INCORRECT\n\n";
        depth = this->m_max_depth;
    }

    //
    double forward = exp(r*(delta_t));  // FORWARD VALUE EXP(r*DT)

    std::vector<double> stock(depth+1);  // STOCK VALUE VECTOR
    std::vector<double> option(depth+1); // OPTION VALUE VECTOR (value_calc determines whether it's a put or a call)
    stock[0] = S0*pow((1.0/(this->m_growth_factor)), depth);   // Fill the end of the tree, starting with the lowest further down node

    for(int i = 1; i<=depth; ++i)
    {
        stock[i] = pow(this->m_growth_factor, 2)*stock[i-1];  // Price at node "above" lowest down by multiplying by u factor twice, continue this verically up the tree
    }
    
    for(int i = 0; i <= depth;++i)
    {   // HERE VALUE_CALC TAKES CARE OF SPECIFYING WHETHER IT'S A PUT OR A CALL OPTION
        option[i] = value_calc.Value(stock[i]);
    }
    //two layered loop
    for(int i=depth-1; i>=0; --i)
    {
        for(int j=0; j<=i; ++j)
        {
            option[j] = (this->m_growth_probability*option[j+1]+(1.0-this->m_growth_probability)*option[j])/forward;
        }
    }
    return option[0];
}



double BinomialOptionPricer::price_vanilla_option_american(unsigned int depth, double delta_t, double S0, double r, const OptionValueCalculator& value_calc)
{   // PRICE A AMERICAN OPTION USING THE BACKWARD-INDUCTION METHOD
    
    if(depth > this->m_max_depth)
    { 
        std::cout << "DEPTH EXCEEDS THE PRE-SET LIMIT (MAX_DEPTH), LOWERING IT TO THE MAX_DEPTH\n\n";
        depth = this->m_max_depth;
    }


    double forward = exp(r*(delta_t));  // FORWARD VALUE EXP(R*DT)
    std::vector<double> stock(depth+1);  // STOCK VALUE VECTOR
    std::vector<double> option(depth+1); // OPTION VALUE VECTOR (value_calc determines whether it's a put or a call)
    stock[0] = S0*pow((1/(this->m_growth_factor)), depth);   // Fill the end of the tree, starting with the lowest further down node

    for(int i = 1; i<=depth; ++i)
    {
        stock[i] = pow(this->m_growth_factor, 2)*stock[i-1];// Price at node "above" lowest down by multiplying by u factor twice
    }
    
    for(int i = 0; i <= depth;++i)
    {   // HERE VALUE_CALC TAKES CARE OF SPECIFYING WHETHER IT'S A PUT OR A CALL OPTION
        option[i] = value_calc.Value(stock[i]);
    }
    
    //two layerd loop
    for(int i=depth-1; i>=0; --i)
    {
        for(int j=0; j<=i; ++j)
        {
            
            option[j] = (this->m_growth_probability*option[j+1]+(1.0-this->m_growth_probability)*option[j])/forward;
            stock[j] = this->m_growth_factor*stock[j];
            //american option early exercise
            if (option[j] <= (value_calc.Value(stock[j])))
            {
                option[j] = value_calc.Value(stock[j]);
            }

        }
    }
    return option[0];
}

double Black_Scholes(double t, double growth_factor, double S0, double r, double delta_t, double K)
{
    double sigma = (log(growth_factor))/sqrt(delta_t);
    double d1 = (log(S0/K) + (r+(sigma*sigma/2))*t)/(sigma*sqrt(t));
    double d2 = d1 - sigma*sqrt(t);
    double BSCall = S0*(0.5*erfc(-(d1/sqrt(2)))) - K*exp(-r*t)*(0.5*erfc(-d2/sqrt(2)));
    double BSPut = -S0*(0.5*erfc((d1/sqrt(2)))) + K*exp(-r*t)*(0.5*erfc(d2/sqrt(2)));
    std::cout << "sigma = " << sigma << "\n";
    std::cout << "d1 = " << d1 << "\n";
    std::cout << "d2 = " << d2 << "\n";
    std::cout << "Value of the Black-Scholes European Call is = " << BSCall << "\n";
    std::cout << "Value of the Black-Scholes European Put is = " << BSPut << "\n\n";
    return 0;
}

/* ******************
 
 
 
 
        TESTER




   *****************
 */


int main(){
    // A FEW TESTS FOR OPTIONVALUECALCULATOR
    // TESTING THE OPTION GETS CREATED AND CHECKING THE VALUE FUNCTION WORKS
    
    double K = 100; //SET STRIKE
    
    //CREATE THE Call and Put OPTION WITH STRIKE K
    const OptionValueCalculator *call = new CallOptionValueCalculator(K);
    const OptionValueCalculator *put = new PutOptionValueCalculator(K);
    
    //SHowing the value calc works
    std::cout << "Call value at expiry with S = 80, K = " << K << "\t" << "=" << call->Value(80.0) << "\n";
    std::cout << "Call value at expiry with S = 100, K = " << K << "\t" <<"=" <<  call->Value(100.0) << "\n";
    std::cout << "Call value at expiry with S = 120, K = " << K << "\t" <<"=" <<  call->Value(120.0) << "\n";
    
    std::cout << "Put value at expiry with S = 80, K = " << K << "\t" <<"=" <<  put->Value(80.0) << "\n";
    std::cout << "Put value at expiry with S = 100, K = " << K << "\t" << "=" << put->Value(100.0) << "\n";
    std::cout << "Put value at expiry with S = 120, K = " << K << "\t" <<"=" <<  put->Value(120.0) << "\n";
    
    
    // Parameters for testing
    unsigned int max_depth = 10000; //Max Depth
    double S0  = 90; //Current Price of underlying
    double r = 0.05; //Interest Rate (Annual)
    double sigma = 0.45; //Vol
    double t = 1.0; //maturity time in years
    unsigned int depth = 10; // how many time steps   (setting depth > 30 for forward recursion takes a few minutes to run.)
    double delta_t = t/depth; //Time elapsed between steps in tree, only use this if not specifying t
    double growth_factor = exp(sigma*sqrt(delta_t)); //growth_factor from CRR, can be changed to set speicific value rather than calculating it, here we assume the growth factor is > 1 i.e if the growth is 6% then set growth_factor = 1.06
    double decay_factor = 1.0/growth_factor;
    double q = 0.00; //dividend not sure if we need to implement dividends or not (assuming its always 0 as the detials for the algorithms dont mention divdends, algorithms would be different if we had to implement dividends).
    double growth_probability = (exp(delta_t*r) - decay_factor)/(growth_factor - decay_factor); //growth probability from CRR, can be changed to set specific value
    
    //create an object callled BOP
    
    BinomialOptionPricer BOP(max_depth, growth_factor, growth_probability);
    
    //arbitrage checks d < r < u
    if (1+r < decay_factor || 1+r > growth_factor) {
        std::cout << "WARNING POTENTIAL ARBITRAGE OPPORTUNITES, CHECK PARAMETERS" << std::endl;
    }
    
    
    // PRINT OPTION DATA
    std::cout << "OPTION DATA\n----------------------\n";
    std::cout << "max_depth: " << max_depth << "\ngrowth_factor: " << growth_factor << "\n growth_probability: " << growth_probability <<"\n";
    std::cout << "depth: " << depth << "\ndelta_t: " << delta_t << "\nS0: " << S0 << "\nr: " << r <<"\n\n";
    
    // EUROPEAN RECURSION
    std::cout << "\nEUROPEAN OPTION FORWARD RECURSION METHOD:\n----------------------\n";
    std::cout << "CALL PRICE: " << BOP.price_vanilla_option_european_recursion(depth, delta_t, S0,r, *call) <<"\n";
    std::cout << "PUT PRICE: " << BOP.price_vanilla_option_european_recursion(depth, delta_t, S0,r, *put)<<"\n";
    
    // AMERICAN RECURSION
    std::cout << "\nAMERICAN OPTION FORWARD RECURSION METHOD:\n----------------------\n";
    std::cout << "CALL PRICE: " << BOP.price_vanilla_option_american_recursion(depth, delta_t, S0, r,*call) <<"\n";
    std::cout << "PUT PRICE: " << BOP.price_vanilla_option_american_recursion(depth, delta_t, S0,r,*put) << "\n";
    
    
    // EUROPEAN BACKWARDS
    std::cout << "\nEUROPEAN OPTION BACKWARDS INDUCTION METHOD:\n----------------------\n";
    std::cout << "CALL PRICE: " << BOP.price_vanilla_option_european(depth, delta_t, S0, r, *call) << "\n";
    std::cout << "PUT PRICE: " << BOP.price_vanilla_option_european(depth, delta_t, S0, r, *put) << "\n";
    
    // AMERICAN BACKWARDS
    std::cout << "\nAMERICAN OPTON BACKWARDS INDUCTION METHOD:\n----------------------\n";
    std::cout << "CALL PRICE: " << BOP.price_vanilla_option_american(depth, delta_t, S0, r, *call)<< "\n";
    std::cout << "PUT PRICE: " <<BOP.price_vanilla_option_american(depth, delta_t, S0, r, *put)<< "\n";
    
    std::cout << "\n----------------------\nBLACK SCHOLES:\n----------------------\n"; //testing
    
    Black_Scholes(t, growth_factor, S0, r, delta_t, K); //blackscholes price
    
    // DELETE POINTERS SINCE WE USED new
    delete call;
    delete put;
    
    
    /*
    //TESTING FOR TIME AND BLACKSCHOLES COMPARISON graphs in report (Not important to run this just used it to get the runtimes and values for the graphs and tables in the report / the forward recustion method takes a few minutes to run for the largest depths which is why its commented out, its not elegant or the most compact, just wanted it to work to get the desired figures / tables)
     
    double N[] = {1,2,3,4,5,6,7,8,9,10,15,20,25,30,35,40,45,50,55,60,65,70,75,100,125,150,175,200,250,300,400,500,650,800,1000,1250,1500,2000,2500,3000,4000,5000,7500,9999};
    int size = 44;
    double bwd_eur_calls[44] = {};
    double bwd_am_calls[44] = {};
    double bwd_eur_puts[44] = {};
    double bwd_am_puts[44] = {};
    double bwd_eur_calltime[44] = {};
    double bwd_am_calltime[44] = {};
    double bwd_eur_puttime[44] = {};
    double bwd_am_puttime[44] = {};

    //BACKWARD EUROPEAN
    for(int i  = 0; i < size; i++)
    {
        unsigned int max_depth = 10000;
        double depth = N[i];

        double delta_t = t/depth;
        double growth_factor = exp(sigma*sqrt(delta_t));
        double decay_factor = 1.0/growth_factor;
        double growth_probability = (exp(delta_t*r) - decay_factor)/(growth_factor - decay_factor);
        
        Black_Scholes(t, growth_factor, S0, r, delta_t, K);
     
        BinomialOptionPricer BOP1(max_depth, growth_factor, growth_probability);
        BinomialOptionPricer BOP2(max_depth, growth_factor, growth_probability);
        
        const OptionValueCalculator *call = new CallOptionValueCalculator(K);
        const OptionValueCalculator *put = new PutOptionValueCalculator(K);
        
        // EUROPEAN BACKWARDS
     
        std::cout << "\n BACKWARDS METHOD:\n----------------------\n";

        using clock_t = std::chrono::high_resolution_clock;
        auto start = clock_t::now();
        bwd_eur_calls[i]= {BOP1.price_vanilla_option_european(depth, delta_t, S0, r, *call)};
       // std::cout << bwd_eur_calls[i];
        auto end = clock_t::now();
        auto elapsed = end - start;
        double timer = std::chrono::duration<double>(elapsed).count();
        bwd_eur_calltime[i] = timer;
        
        
        
        using clock_tp = std::chrono::high_resolution_clock;
        auto startp = clock_t::now();
        bwd_eur_puts[i] = {BOP1.price_vanilla_option_european(depth, delta_t, S0, r, *put)};
        //std::cout << bwd_eur_puts[i];
        auto endp = clock_tp::now();
        auto elapsedp = endp - startp;
        double timerp = std::chrono::duration<double>(elapsedp).count();
        bwd_eur_puttime[i] = timerp;

        using aclock_t = std::chrono::high_resolution_clock;
        auto amcstart = aclock_t::now();
        bwd_am_calls[i]= {BOP2.price_vanilla_option_american(depth, delta_t, S0, r, *call)};
        //std::cout << bwd_am_calls[i];
        auto amcend = aclock_t::now();
        auto amcelapsed = amcend - amcstart;
        double amctimer = std::chrono::duration<double>(amcelapsed).count();
        bwd_am_calltime[i] = amctimer;
        
        
        using aclock_tp = std::chrono::high_resolution_clock;
        auto ampstartp = aclock_t::now();
        bwd_am_puts[i] = {BOP2.price_vanilla_option_american(depth, delta_t, S0, r, *put)};
        //std::cout << bwd_am_puts[i];
        auto ampendp = aclock_tp::now();
        auto ampelapsedp = ampendp - ampstartp;
        double amptimerp = std::chrono::duration<double>(ampelapsedp).count();
        bwd_am_puttime[i] = amptimerp;
        
        delete call;
        delete put;
    }
    
    std::cout << "\n \n CALL PRICES \n \n";
    for(int i=0;i<44;i++)
    {
        std::cout <<bwd_eur_calls[i] << std::endl;
    }
    
    std::cout << "\n \n CALL TIMES \n \n";
    for(int i=0;i<44;i++)
    {
        std::cout <<bwd_eur_calltime[i] << std::endl;
    }
    std::cout << "\n \n PUT PRICES \n \n";
    
    for(int i=0;i<44;i++)
    {
        std::cout <<bwd_eur_puts[i] << std::endl;
    }

    std::cout << "\n \n BWD PUT TIMES \n \n";
    for(int i=0;i<44;i++)
    {
        std::cout <<bwd_eur_puttime[i] << std::endl;
    }
    
    std::cout << "\n \n BWD AM CALL PRICES \n \n";
    for(int i=0;i<44;i++)
    {
        std::cout <<bwd_am_calls[i] << std::endl;
    }
    
    std::cout << "\n \n BWD AM CALL TIMES \n \n";
    for(int i=0;i<44;i++)
    {
        std::cout <<bwd_am_calltime[i] << std::endl;
    }
    std::cout << "\n \n BWD AM PUT PRICES \n \n";
    
    for(int i=0;i<44;i++)
    {
        std::cout <<bwd_am_puts[i] << std::endl;
    }

    std::cout << "\n \n BWD AM PUT TIMES \n \n";
    for(int i=0;i<44;i++)
    {
        std::cout <<bwd_am_puttime[i] << std::endl;
    }
    
    double M[] = {1,2,3,4,5,6,7,8,9,10,15,20,25,26,27,28,29,30,31,32};
    int fwdsize = 20;
    double fwd_eur_calls[20] = {};
    double fwd_am_calls[20] = {};
    double fwd_eur_puts[20] = {};
    double fwd_am_puts[20] = {};
    double fwd_eur_calltime[20] = {};
    double fwd_am_calltime[20] = {};
    double fwd_eur_puttime[20] = {};
    double fwd_am_puttime[20] = {};

    //BACKWARD EUROPEAN
    for(int i  = 0; i < fwdsize; i++)
    {
        unsigned int max_depth = 10000;
        double depth = M[i];

        double delta_t = t/depth;
        double growth_factor = exp(sigma*sqrt(delta_t));
        double decay_factor = 1.0/growth_factor;
        double growth_probability = (exp(delta_t*r) - decay_factor)/(growth_factor - decay_factor);
        
        Black_Scholes(t, growth_factor, S0, r, delta_t, K);
     
        BinomialOptionPricer BOP1(max_depth, growth_factor, growth_probability);
        BinomialOptionPricer BOP2(max_depth, growth_factor, growth_probability);
        
        const OptionValueCalculator *call = new CallOptionValueCalculator(K);
        const OptionValueCalculator *put = new PutOptionValueCalculator(K);
        
        // EUROPEAN BACKWARDS
     
        std::cout << "\n FORWARD RECURSION METHOD:\n----------------------\n";

        using clock_t = std::chrono::high_resolution_clock;
        auto start = clock_t::now();
        fwd_eur_calls[i]= {BOP1.price_vanilla_option_european_recursion(depth, delta_t, S0, r, *call)};
       // std::cout << fwd_eur_calls[i];
        auto end = clock_t::now();
        auto elapsed = end - start;
        double timer = std::chrono::duration<double>(elapsed).count();
        fwd_eur_calltime[i] = timer;
        
        
        
        using clock_tp = std::chrono::high_resolution_clock;
        auto startp = clock_t::now();
        fwd_eur_puts[i] = {BOP1.price_vanilla_option_european_recursion(depth, delta_t, S0, r, *put)};
        //std::cout << fwd_eur_puts[i];
        auto endp = clock_tp::now();
        auto elapsedp = endp - startp;
        double timerp = std::chrono::duration<double>(elapsedp).count();
        fwd_eur_puttime[i] = timerp;

        using aclock_t = std::chrono::high_resolution_clock;
        auto amcstart = aclock_t::now();
        fwd_am_calls[i]= {BOP2.price_vanilla_option_american_recursion(depth, delta_t, S0, r, *call)};
        //std::cout << fwd_am_calls[i];
        auto amcend = aclock_t::now();
        auto amcelapsed = amcend - amcstart;
        double amctimer = std::chrono::duration<double>(amcelapsed).count();
        fwd_am_calltime[i] = amctimer;
        
        
        using aclock_tp = std::chrono::high_resolution_clock;
        auto ampstartp = aclock_t::now();
        fwd_am_puts[i] = {BOP2.price_vanilla_option_american_recursion(depth, delta_t, S0, r, *put)};
        //std::cout << fwd_am_puts[i];
        auto ampendp = aclock_tp::now();
        auto ampelapsedp = ampendp - ampstartp;
        double amptimerp = std::chrono::duration<double>(ampelapsedp).count();
        fwd_am_puttime[i] = amptimerp;
        
        delete call;
        delete put;
    }
    
    std::cout << "\n \n CALL PRICES \n \n";
    for(int i=0;i<fwdsize;i++)
    {
        std::cout <<fwd_eur_calls[i] << std::endl;
    }
    
    std::cout << "\n \n CALL TIMES \n \n";
    for(int i=0;i<fwdsize;i++)
    {
        std::cout <<fwd_eur_calltime[i] << std::endl;
    }
    std::cout << "\n \n PUT PRICES \n \n";
    
    for(int i=0;i<fwdsize;i++)
    {
        std::cout <<fwd_eur_puts[i] << std::endl;
    }

    std::cout << "\n \n fwd PUT TIMES \n \n";
    for(int i=0;i<fwdsize;i++)
    {
        std::cout <<fwd_eur_puttime[i] << std::endl;
    }
    
    std::cout << "\n \n fwd AM CALL PRICES \n \n";
    for(int i=0;i<fwdsize;i++)
    {
        std::cout <<fwd_am_calls[i] << std::endl;
    }
    
    std::cout << "\n \n fwd AM CALL TIMES \n \n";
    for(int i=0;i<fwdsize;i++)
    {
        std::cout <<fwd_am_calltime[i] << std::endl;
    }
    std::cout << "\n \n fwd AM PUT PRICES \n \n";
    
    for(int i=0;i<fwdsize;i++)
    {
        std::cout <<fwd_am_puts[i] << std::endl;
    }

    std::cout << "\n \n fwd AM PUT TIMES \n \n";
    for(int i=0;i<fwdsize;i++)
    {
        std::cout <<fwd_am_puttime[i] << std::endl;
    }
    */
}
