#ifndef _DATA_H
#define _DATA_H

//------ DEFINITION OF TYPES OF RKO --------

/***********************************************************************************
 Struct: TSol
 Description: struct to represent a solution problem
************************************************************************************/
struct TSol
{
    std::vector <double> rk;                // random-key vector
    double ofv;                             // objetive function value
    int label;                              // defines a community solution with a number
    int similar;                            // indicates if a solution is similar to other (0 no, 1 yes)
    int flag;                               // indicates if a local search has already been performed on this solution (0 no, 1 yes)
    int promising;                          // indicates if a solution is promising to apply local search
};


#endif