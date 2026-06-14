#ifndef SIM_BP_H
#define SIM_BP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

typedef struct bp_params
{
    unsigned long int K;
    unsigned long int M1;
    unsigned long int M2;
    unsigned long int N;
    char *bp_name;
} bp_params;

// determining which prediction to be made
enum type_of_prediction
{
    bimodal_mode = 0,
    gshare_mode,
    hybrid_mode
} mode_of_prediction;

class bimodal_branch_predictor;
class gshare_branch_predictor;

// prints the output results of which branch predictor was made
class branch_predictor
{
    protected:
        bp_params params;

        unsigned int num_of_predictions = 0;
        unsigned int num_of_mis_predictions = 0;
        double mis_predctions_rate = 0.0;
        int total_num_of_addr = 0;

        std::vector<int> table_of_bimodal_prediction;
        std::vector<int> table_of_gshare_prediction;
        std::vector<int> table_of_chooser;

    public:
        int m;
        int n;
        unsigned int GBHR = 0;
        int n_PC_bits;
        int chooser_bits;

        branch_predictor(bp_params b_params);
        ~branch_predictor();

        void printing_final_results(bimodal_branch_predictor *bimodal_predictor = nullptr, gshare_branch_predictor *gshare_predictor = nullptr);
};

// bimodal branch predictor 
// inherits the properties from branch_predictor as it's the parent class
class bimodal_branch_predictor : public branch_predictor
{
    public:
        int table_of_bimodal_prediction_size;

        bimodal_branch_predictor(bp_params n_params, int bits_of_PC);
        ~bimodal_branch_predictor();

        int fetching_index(unsigned long int &address);
        std::string predicting_the_outcome(int counter);
        bool determining_the_prediction(unsigned long int &address);
        void predicting_process(unsigned long int &addr, std::string outcome);
        void updating_the_counters(std::string outcome, int index);
        void displaying_the_results();
};

// gshare branch predictor
// inherits the properties from branch_predictor as it's the parent class
class gshare_branch_predictor : public branch_predictor
{
    public:
        gshare_branch_predictor(bp_params n_params);
        ~gshare_branch_predictor();

        int fetching_index(unsigned int address);
        bool determining_the_prediction(unsigned long int &addr);
        bool predicting_process(unsigned long int &addr, bool t);
        void updating_the_counters(std::string outcome, int index);
        void displaying_the_results(void);
};

// hybrid branch predictor
// inherits the properties from branch_predictor as it's the parent class
class hybrid_branch_predictor : public branch_predictor
{
    private:
        bimodal_branch_predictor *bimodal_predictor;
        gshare_branch_predictor *gshare_predictor;

        int chooser_bits;

    public:
        hybrid_branch_predictor(bp_params n_params);
        ~hybrid_branch_predictor();

        void predicting_process(unsigned long int &addr, std::string outcome);
        void displaying_the_results();
};

#endif
