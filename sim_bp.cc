#include "sim_bp.h"

using namespace std;

// done for checking the debug runs in the code
//bool enable_debugging = false; 

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/

int main(int argc, char *argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file
    
    // creating an object for each of the branch predictor class
    gshare_branch_predictor *gshare_pred = nullptr;
    bimodal_branch_predictor *bimodal_pred = nullptr;
    hybrid_branch_predictor *hybrid_pred = nullptr;

    /*if (enable_debugging)
        printf("Program Started Sucessfully...\n");*/

    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc - 1);
        exit(EXIT_FAILURE);
    }

    params.bp_name = argv[1];

    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if (strcmp(params.bp_name, "bimodal") == 0) // Bimodal
    {
        if (argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc - 1);
            exit(EXIT_FAILURE);
        }
        params.M2 = strtoul(argv[2], NULL, 10);
        trace_file = argv[3];
        printf("COMMAND\n %s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);

        // executing the BIMODAL branch predictor 
        mode_of_prediction = type_of_prediction::bimodal_mode;
    }
    else if (strcmp(params.bp_name, "gshare") == 0) // Gshare
    {
        if (argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc - 1);
            exit(EXIT_FAILURE);
        }
        params.M1 = strtoul(argv[2], NULL, 10);
        params.N = strtoul(argv[3], NULL, 10);
        trace_file = argv[4];
        printf("COMMAND\n %s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

        // executing the GSHARE branch predictor when N!=0
        mode_of_prediction = type_of_prediction::gshare_mode;

        // if the N = 0 condition is there then the GSHARE predictor becomes BIMODAL
        if (params.N == 0)
        {
            params.M2 = params.M1;
            // executing the BIMODAL branch predictor
            mode_of_prediction = type_of_prediction::bimodal_mode;
        }
    }
    else if (strcmp(params.bp_name, "hybrid") == 0) // Hybrid
    {
        if (argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc - 1);
            exit(EXIT_FAILURE);
        }
        params.K = strtoul(argv[2], NULL, 10);
        params.M1 = strtoul(argv[3], NULL, 10);
        params.N = strtoul(argv[4], NULL, 10);
        params.M2 = strtoul(argv[5], NULL, 10);
        trace_file = argv[6];
        printf("COMMAND\n %s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

        // executing the HYBRID branch predictor
        mode_of_prediction = type_of_prediction::hybrid_mode;

        // if the N = 0 condition is there then the HYBRID predictor becomes BIMODAL
        if (params.N == 0)
        {
            strcpy(params.bp_name, "bimodal");
            // executing the BIMODAL branch predictor
            mode_of_prediction = type_of_prediction::bimodal_mode;
        }
    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }

    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if (FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }

    // analysing what branch predictor type is needed to start the prediction process
    if (mode_of_prediction == type_of_prediction::gshare_mode)
    {
        // executing GSHARE predictor
        gshare_pred = new gshare_branch_predictor(params);
    }
    else if (mode_of_prediction == type_of_prediction::bimodal_mode)
    {
        // executing BIMODAL predictor
        bimodal_pred = new bimodal_branch_predictor(params, params.M2);
    }
    else
    {
        // executing HYBRID predictor
        hybrid_pred = new hybrid_branch_predictor(params);
    }

    char str[2];
    while (fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        bool t = false;
        outcome = str[0];
        
        // determining what is the prediction type, whether it's taken target or not-taken target
        if (outcome == 't')
        {
            //if (enable_debugging)
                //printf("%lx %s\n", addr, "t"); // Print and test if file is read correctly

            t = true;
        }
        else if (outcome == 'n')
        {
            //if (enable_debugging)
                //printf("%lx %s\n", addr, "n"); // Print and test if file is read correctly

            t = false;
        }
        else
            continue;

        /*************************************
            Add branch predictor code here
        **************************************/

        if (mode_of_prediction == type_of_prediction::gshare_mode)
        {
            // performing the prediction process for GSHARE
            gshare_pred->predicting_process(addr, t);
        }
        else if (mode_of_prediction == type_of_prediction::bimodal_mode)
        {
            std::string str{outcome};
            // performing the prediction process for BIMODAL
            bimodal_pred->predicting_process(addr, str);
        }
        else
        {
            std::string str{outcome};
            // performing the prediction process for HYBRID
            hybrid_pred->predicting_process(addr, str);
        }
    }

    // implementing GSHARE prediction if the prediction type is gshare
    // after displaying the results, deleting the object created and assigning it to a null pointer
    if (mode_of_prediction == type_of_prediction::gshare_mode)
    {
        if (gshare_pred)
            gshare_pred->displaying_the_results();

        if (gshare_pred)
        {
            delete gshare_pred;
            gshare_pred = nullptr;
        }
    }

    // implementing BIMODAL prediction if the prediction type is bimodal
    // after displaying the results, deleting the object created and assigning it to a null pointer
    else if (mode_of_prediction == type_of_prediction::bimodal_mode)
    {
        if (bimodal_pred)
            bimodal_pred->displaying_the_results();

        if (bimodal_pred)
        {
            delete bimodal_pred;
            bimodal_pred = nullptr;
        }
    }

    // implementing HYBRID prediction if the prediction type is hybrid
    // after displaying the results, deleting the object created and assigning it to a null pointer
    else
    {
        if (hybrid_pred)
            hybrid_pred->displaying_the_results();

        if (hybrid_pred)
        {
            delete hybrid_pred;
            hybrid_pred = nullptr;
        }
    }

    return 0;
}

// constructor of branch predictor class
branch_predictor::branch_predictor(bp_params b_params) : params(b_params) 
{}

// destructor of branch predictor class
branch_predictor::~branch_predictor() 
{}

// printing out the final results that is expected
// this function is called in bimodal, gshare and hybrid class to display the final table of contetns in the respective branch predictor
void branch_predictor::printing_final_results(bimodal_branch_predictor *bimodal_predictor, gshare_branch_predictor *gshare_predictor)
{
    double miss_predctions_rate = (double)num_of_mis_predictions / total_num_of_addr * 100;
    cout << "OUTPUT\n";
    cout << " number of predictions:\t\t" << total_num_of_addr << "\n";
    cout << " number of mispredictions:\t" << num_of_mis_predictions << "\n";
    cout << " misprediction rate:\t\t" << std::fixed << std::setprecision(2)
              << miss_predctions_rate << "%\n";

    // printing BIMODAL contents
    if (std::string(params.bp_name) == "bimodal")
    {
        cout << "FINAL BIMODAL CONTENTS\n";
        for (size_t i = 0; i < table_of_bimodal_prediction.size(); ++i)
        {
            cout << i << "\t" << table_of_bimodal_prediction[i] << "\n";
        }
    }

    // printing GSHARE contents
    else if (std::string(params.bp_name) == "gshare")
    {

        cout << "FINAL GSHARE CONTENTS\n";

        for (size_t i = 0; i < table_of_gshare_prediction.size(); i++)
        {
            cout << " " << i << " " << table_of_gshare_prediction[i] << endl;
        }
    }
    
    // printing HYBRID contents
    else if (std::string(params.bp_name) == "hybrid")
    {
        //cout << "entered";
        cout << "FINAL CHOOSER CONTENTS\n";
        for (size_t i = 0; i < table_of_chooser.size(); ++i)
        {
            cout << i << "\t" << table_of_chooser[i] << "\n";
        }

        cout << "FINAL GSHARE CONTENTS\n";
        for (size_t i = 0; i < gshare_predictor->table_of_gshare_prediction.size(); i++)
        {
            cout << " " << i << " " << gshare_predictor->table_of_gshare_prediction[i] << endl;
        }

        cout << "FINAL BIMODAL CONTENTS\n";
        for (size_t i = 0; i < bimodal_predictor->table_of_bimodal_prediction.size(); ++i)
        {
            cout << i << "\t" << bimodal_predictor->table_of_bimodal_prediction[i] << "\n";
        }
    }
}

// initialising the variables
bimodal_branch_predictor::bimodal_branch_predictor(bp_params n_params, int bits_of_PC) : branch_predictor(n_params)
{
    params = n_params;
    n_PC_bits = bits_of_PC;
    table_of_bimodal_prediction_size = 1 << bits_of_PC;
    table_of_bimodal_prediction.resize(table_of_bimodal_prediction_size, 2);

    /*if (enable_debugging)
    {
        cout << "pcBits %d\n" << n_PC_bits << endl;
        cout << "bimodal_prediction_table_size  %d\n" << bimodal_prediction_table_size << endl;
    }*/
};

// destructor for BIMODAL class
bimodal_branch_predictor::~bimodal_branch_predictor() 
{};

// calculating the pc index for bimodal prediction
int bimodal_branch_predictor::fetching_index(unsigned long int &address)
{
    unsigned int index_of_bimodal = (address >> 2) & ((1 << n_PC_bits) - 1);
    return index_of_bimodal;
}

// determining the outcome of the branch predictor whether it's taken or not taken
std::string bimodal_branch_predictor::predicting_the_outcome(int counter)
{
    string counter_result;
    counter_result = (counter >= 2) ? "t" : "n";
    return counter_result;
}

// deciding on the prediction for BIMODAL by fetching the index, then updating the counter
bool bimodal_branch_predictor::determining_the_prediction(unsigned long int &address)
{
    int index_of_bimodal = fetching_index(address);
    int bm_pred_counter = table_of_bimodal_prediction[index_of_bimodal];

    bool bm_result;
    bm_result = (bm_pred_counter >= 2) ? true : false;
    return bm_result;
}

// predicting bimodal
void bimodal_branch_predictor::predicting_process(unsigned long int &addr, std::string outcome)
{
    // fetching the index value
    int index_of_bimodal = fetching_index(addr);

    int bm_pred_counter = table_of_bimodal_prediction[index_of_bimodal];
    // cout << "bm_pred_counter = %d\n" << bm_pred_counter;

    std::string bm_pred_outcome = predicting_the_outcome(bm_pred_counter);

    /*if (enable_debugging)
    {
        if (outcome == "t")
            cout << "=" << std::dec << total_of_num_addr << "	" << std::hex << addr << " t" << endl;
        else
            cout << "=" << std::dec << total_of_num_addr << "	" << std::hex << addr << " n" << endl;
        cout << "	BP: " << std::dec << index << " " << std::dec << table_of_bimodal_prediction[index_of_bimodal] << endl;
    }*/

    // updating the misspredictions or predictions based on the outcome that is predicted
    if (outcome != bm_pred_outcome)
    {
        num_of_mis_predictions++;
    }
    else
    {
        num_of_predictions++;
    }

    total_num_of_addr++;

    // updating the counters
    updating_the_counters(outcome, index_of_bimodal);

    /*if (enable_debugging)
    {
        cout << "	BU: " << std::dec << index << " " << std::dec << table_of_bimodal_prediction[index_of_bimodal] << endl;
    }*/
}

// updating the counters based on the prediction made
void bimodal_branch_predictor::updating_the_counters(std::string outcome, int index)
{
    // incrementing the count if it's taken target and index < 3
    if (outcome == "t" && table_of_bimodal_prediction[index] < 3)
    {
        table_of_bimodal_prediction[index]++;
    }
    // decrementing the count if it's not-taken target and index > 0
    else if (outcome != "t" && table_of_bimodal_prediction[index] > 0)
    {
        table_of_bimodal_prediction[index]--;
    }
}

// displaying the results for BIMODAL contents
void bimodal_branch_predictor::displaying_the_results()
{
    // calling the branch predictor display function to print the final value of predictions and mispredictions along with the miss prediction rate
    // also prints the FINAL BIMODAL CONTENTS
    printing_final_results();
}

// constructor of GSHARE class
gshare_branch_predictor::gshare_branch_predictor(bp_params n_params) : branch_predictor(n_params)
{
    params = n_params;

    m = params.M1; // number of low-order PC bits
    n = params.N;  // number of Global Branch Histroy Predictor bits
    GBHR = 0; // Global Branch History Register

    unsigned int gs_size = 1 << m; // Gshare Tabel Size
    table_of_gshare_prediction.assign(gs_size, 2);

    /*if (enable_debugging)
    {
        cout << "number of low-order PC bits (m) : %d\n" << m << endl;
        cout << "number of Global Histroy bits (n) : %d\n"<< n << endl;
        cout << "GSHARE History Pattern Table size: %zd\n"<< table_of_gshare_prediction.size() << endl;
    }*/
}

// Destructor of GSHARE class
gshare_branch_predictor::~gshare_branch_predictor()
{}

// calculating the index value for gshare table calculation
int gshare_branch_predictor::fetching_index(unsigned int address)
{
    unsigned int num_of_pc_bits = (address >> 2) & ((1 << m) - 1); // discarding 2 LSBs
    unsigned int num_of_upper_pc_bits = num_of_pc_bits >> (m - n);
    unsigned int result_of_xor = num_of_upper_pc_bits ^ (GBHR & ((1 << n) - 1));
    unsigned int num_of_lower_pc_bits = num_of_pc_bits & ((1 << (m - n)) - 1);
    unsigned final_result = (result_of_xor << (m - n)) | num_of_lower_pc_bits;
    return final_result;
}

// making a prediction 
bool gshare_branch_predictor::determining_the_prediction(unsigned long int &addr)
{
    int index_of_gshare = fetching_index(addr);
    return table_of_gshare_prediction[index_of_gshare] >= 2;
}

bool gshare_branch_predictor::predicting_process(unsigned long int &addr, bool t)
{
    bool prediction;

    unsigned int num_of_pc_bits = (addr >> 2) & ((1 << m) - 1); // discarding 2 LSBs

    if (n == 0)
    {
        // Bimodal predictor
        return num_of_pc_bits;
    }

    // fetching the index value
    unsigned int index_of_gshare = fetching_index(addr);

    // making a prediction
    /*if (enable_debugging)
    {
        if (index_of_gshare >= table_of_gshare_prediction.size())
            cout << "Error Index Outoff Bound. table_of_gshare_prediction.size() = %zd\n" << table_of_gshare_prediction.size() << endl;
    }*/

    prediction = table_of_gshare_prediction[index_of_gshare] >= 2;

    /*if (enable_debugging)
    {
        if (t)
            cout << "=" << std::dec << total_of_num_addr << "	" << std::hex << addr << " t" << endl;
        else
            cout << "=" << std::dec << total_of_num_addr << "	" << std::hex << addr << " n" << endl;
        cout << "	GP: " << std::dec << index << " " << std::dec << table_of_gshare_prediction[index_of_gshare] << endl;
    }*/

    // updating the number of predictions and number of mispredictions
    if (prediction == t)
    {
        num_of_predictions++;
    }
    else
    {
        num_of_mis_predictions++;
    }

    total_num_of_addr++;

    // if the prediction is true and count value is less than 3 then incrementing the result
    if (t && table_of_gshare_prediction[index_of_gshare] < 3)
    {
        table_of_gshare_prediction[index_of_gshare]++;
    }

    // if the prediction is not true and the count value is greater than 3 then decrementing the value
    else if (!t && table_of_gshare_prediction[index_of_gshare] > 0)
    {
        table_of_gshare_prediction[index_of_gshare]--;
    }

    // shifting the global branch history register to the right as mentioned in the pdf
    if (n > 0)
    {
        GBHR = (GBHR >> 1) | ((t ? 1 : 0) << (n - 1));
    }

    /*if (enable_debugging)
    {
        cout << "	GU: " << std::dec << index << " " << std::dec << table_of_gshare_prediction[index_of_gshare] << endl;
    }*/

    return prediction;
}

// updating the GSHARE table index based on the prediction
// incrementing the data is prediction is taken and lesser than 3
// decrementing the counter if the prediction is not taken and greater than 0
void gshare_branch_predictor::updating_the_counters(std::string outcome, int index)
{
    if (outcome == "t" && table_of_gshare_prediction[index] < 3)
    {
        table_of_gshare_prediction[index]++;
    }
    else if (outcome != "t" && table_of_gshare_prediction[index] > 0)
    {
        table_of_gshare_prediction[index]--;
    }
}

// displaying the results for GHSARE branch prediction
void gshare_branch_predictor::displaying_the_results(void)
{
    // accessing the branch_predictor class for displaying the final results of num_of_predictions and num_of_mis_predictions
    // also prints the FINAL GSHARE CONTENTS
    printing_final_results();
}

// creating a constructor for hybrid branch predictor
hybrid_branch_predictor::hybrid_branch_predictor(bp_params n_params) : branch_predictor(n_params)
{
    params = n_params;

    // creating an object for bimodal and gshare branch predictor
    bimodal_predictor = new bimodal_branch_predictor(params, params.M2);
    gshare_predictor = new gshare_branch_predictor(params);

    chooser_bits = params.K;

    table_of_chooser.resize(1 << chooser_bits, 1);
}

// creating a destructor for hybrid branch predictor
hybrid_branch_predictor::~hybrid_branch_predictor()
{
    // deleting gshare object in the destructor class
    if (gshare_predictor)
    {
        delete gshare_predictor;
        gshare_predictor = nullptr;
    }

    // deleting bimodal object in the destructor class
    if (bimodal_predictor)
    {
        delete bimodal_predictor;
        bimodal_predictor = nullptr;
    }
}

// prediction of hybrid branch predictor
void hybrid_branch_predictor::predicting_process(unsigned long int &addr, std::string outcome)
{

    bool b_outcome = (outcome == "t") ? true : false;

    // STEP 1 : fetching and calculating the index of gshare and bimodal
    int index_of_gshare = gshare_predictor->fetching_index(addr);
    int index_of_bimodal = bimodal_predictor->fetching_index(addr);

    // determining the prediction for gshare and bimodal
    bool prediction_of_gshare = gshare_predictor->determining_the_prediction(addr);
    bool prediction_of_bimodal = bimodal_predictor->determining_the_prediction(addr);

    // STEP 2 : determining the branch's index and discarding the last 2 bits of PC
    int index_of_chooser = (addr >> 2) & ((1 << chooser_bits) - 1);

    // STEP 3 : making an overall prediction
    int chooser_counter = table_of_chooser[index_of_chooser];

    /*if (enable_debugging)
    {
        if (outcome == "t")
            cout << "=" << std::dec << total_of_num_addr << "	" << std::hex << addr << " t" << endl;
        else
            cout << "=" << std::dec << total_of_num_addr << "	" << std::hex << addr << " n" << endl;
        cout << "	GP: " << std::dec << index_of_gshare << " " << std::dec << gshare_predictor->table_of_gshare_prediction[index_of_gshare] << endl;
        cout << "	BP: " << std::dec << index_of_bimodal << " " << std::dec << bimodal_predictor->bimodal_prediction_table[index_of_bimodal] << endl;
        cout << "	CP: " << std::dec << index_of_chooser << " " << std::dec << table_of_chooser[index_of_chooser] << endl;
    }*/

    // STEP 3 : Selecting predictor based on chooser counter
    bool final_prediction;
    bool used_gshare;

    // if the chooser counter value is weakly taken(10) or strongly taken(11) then predicting gshare
    if (chooser_counter >= 2)
    {
        final_prediction = prediction_of_gshare;
        used_gshare = true;
    }

    // if the chooser counter value is weakly not taken(01) or strongly not taken then predicting bimodal
    else
    {
        final_prediction = prediction_of_bimodal;
        used_gshare = false;
    }

    // STEP 4 : updatng the selected branch predictior based on the branch's actual outcome
    // if it's using gshare then updating the counters of gshare
    // else it's usin bimodal then updating the counters of bimodal
    if (used_gshare)
    {
        gshare_predictor->updating_the_counters(outcome, index_of_gshare);
    }
    else
    {
        bimodal_predictor->updating_the_counters(outcome, index_of_bimodal);
    }

    // STEP 5 : updating gshare global branch history register
    gshare_predictor->GBHR = (gshare_predictor->GBHR >> 1) | ((b_outcome ? 1 : 0) << (gshare_predictor->n - 1));

    // Counting the number of misprediction
    // comparing if the final prediction is not equal to outcome of the branch predictor 
    if (final_prediction != b_outcome)
    {
        num_of_mis_predictions++;
    }

    /*if (enable_debugging)
    {
        cout << "	GU: " << std::dec << index_of_gshare << " " << std::dec << gshare_predictor->table_of_gshare_prediction[index_of_gshare] << endl;
        cout << "	BU: " << std::dec << index_of_bimodal << " " << std::dec << bimodal_predictor->table_of_bimodal_prediction[index_of_bimodal] << endl;
    }*/

    // STEP 6 : updating the branch choosen counter
    if (prediction_of_gshare != prediction_of_bimodal)
    {
        // passing the index_of_chooser to gshare
        if (prediction_of_gshare == b_outcome)
        {
            if (table_of_chooser[index_of_chooser] < 3)
            {
                table_of_chooser[index_of_chooser]++;
                //cout << "	CU: " << std::dec << index_of_chooser << " " << std::dec << gshare_predictor->table_of_chooser[index_of_chooser] << endl;
            }
        }
        // passing the index_of_chooser to bimodal
        else if (prediction_of_bimodal == b_outcome)
        {
            if (table_of_chooser[index_of_chooser] > 0)
            {
                table_of_chooser[index_of_chooser]--;
                //cout << "	CU: " << std::dec << index_of_chooser << " " << std::dec << bimodal_predictor->table_of_chooser[index_of_chooser] << endl;
            }
        }
    }

    total_num_of_addr++;
}

// displaying the final results of hybrid branch predictor
void hybrid_branch_predictor::displaying_the_results()
{
    // accessing the branch_predictor class for displaying the final results of num_of_predictions and num_of_mis_predictions
    // also prints the final contents of the branch predictor that was selected
    printing_final_results(bimodal_predictor, gshare_predictor);
}
