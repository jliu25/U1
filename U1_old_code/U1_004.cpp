#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <typeinfo>
#include <iostream>
#include <complex>
#include <vector>
#include <string>
using namespace std;


int k = pow(7, 5);
int m = pow(2, 31) - 1;

double q = m/k;
int r = (m % k);
double r1;


// Returns a vector with the dimension lengths.
vector<int> user_dimensions(string L)
{
    string delim = "x";

    vector<int> dimension_lengths;
    int index = 0;

    int start = 0U;
    int end = L.find(delim);
    
    while (end != string::npos)
    {
        dimension_lengths.push_back(stoi(L.substr(start, end - start)));
        start = end + delim.length();
        end = L.find(delim, start);
        index++;
    }
    dimension_lengths.push_back(stoi(L.substr(start, end - start)));

    return dimension_lengths;
}

// Represents a site that contains links.
struct site
{
    vector<double> link;
};

// Initialize lattice.
void initialize_lattice(site lattice[], int lattice_size, int number_of_dimensions, double init_exponent)
{
//    int index = 1;
    for (int i = 0; i < lattice_size; i++)
    {
//        cout << "i: " << i << endl;
        for (int l = 0; l < number_of_dimensions; l++)
        {
//            lattice[i].link.push_back(index);
            lattice[i].link.push_back(init_exponent*l);
        }
//        index++;
    }
}

// Print out links of the lattice.
void print_lattice(site lattice[], int lattice_size, int number_of_dimensions)
{
    for (int i = 0; i < lattice_size; i++)
    {
        for (int l = 0; l < number_of_dimensions; l++)
        {
//            cout << "i: " << i << endl;
//            cout << "l: " << l << endl;
            cout << lattice[i].link[l] << endl;
            cout << "*********************************" << endl;
        }
    }
}

// Go forward in one direction.
int looper_forward(int index, vector<int> dimension_lengths, int direction)
{
    int divisor = 1;
    int multiple = 1;
    
    for (int i = 0; i <= direction; i++)
        {
            divisor = divisor * dimension_lengths[i];
        }
    for (int i = 0; i < direction; i++)
        {
            multiple = multiple * dimension_lengths[i];
        }
    
    if (direction == 0)
    {
        if ( index % dimension_lengths[direction] == 0 )
        {
            index = index - (dimension_lengths[direction] - 1);
        }
        else
        {
            index = index + 1;
        }
    }
    
    else
    {
        if ( (index % divisor > (dimension_lengths[direction] - 1 ) * multiple) || (index % divisor == 0) )
        {
            index = index - (dimension_lengths[direction] - 1) * multiple;
        }
        else
        {
            index = index + multiple;
        }
    }

    return index;
}

// Go backwards in one direction.
int looper_backward(int index, vector<int> dimension_lengths, int direction)
{
    int divisor = 1;
    int multiple = 1;
    
    for (int i = 0; i <= direction; i++)
        {
            divisor = divisor * dimension_lengths[i];
        }
    for (int i = 0; i < direction; i++)
        {
            multiple = multiple * dimension_lengths[i];
        }    
    
    if (direction == 0)
    {
        if ( ( (index - 1) % dimension_lengths[direction] ) == 0 )
        {
            index = index + (dimension_lengths[direction] - 1);
        }
        else
        {
            index = index - 1;
        }
    }
    
    else
    {
        if ( index % divisor <= multiple && index % divisor != 0 )
        {
            index = index + (dimension_lengths[direction] - 1) * multiple;
        }
        else
        {
            index = index - multiple;
        }
    }

    return index;
}

// Collect the exponents of the links in the plaquettes containing the link of interest.
vector< vector<double> > collect_exps(int index, vector<int> dimension_lengths, int direction, site lattice[])
{
    //cout << "index: " << index << endl;
    //cout << "direction: " << direction << endl;
    
    vector< vector<double> > exp_list;
    
    double a = lattice[index-1].link[direction];

    vector<double> common;
    common.push_back(a);

    exp_list.push_back(common);


    for (int dir = 0; dir < dimension_lengths.size(); dir++)
    {
        if (dir != direction)
        {
//            cout << "-------------------------------" << endl;
//            cout << "direction: " << dir << endl;
            // Top.
//            cout << "*********TOP*********" << endl;
//            cout << a << endl;
                
            vector<double> top_exp;
            
            int index_top = index;

            double b1 = -lattice[index_top-1].link[dir];
//            cout << b1 << endl;
            top_exp.push_back(b1);
    
            index_top = looper_forward(index_top, dimension_lengths, direction);
            double c1 = lattice[index_top-1].link[dir];
//            cout << c1 << endl;
            top_exp.push_back(c1);

            index_top = looper_backward(index_top, dimension_lengths, direction);
            index_top = looper_forward(index_top, dimension_lengths, dir);
            double d1 = -lattice[index_top-1].link[direction];
//            cout << d1 << endl;
            top_exp.push_back(d1);

    
            // Bottom.
//            cout << "*********BOT*********" << endl;
//            cout << a << endl;

            vector<double> bot_exp;
        
            int index_bot = index;
            index_bot = looper_backward(index_bot, dimension_lengths, dir);
            double b2 = lattice[index_bot-1].link[dir];
//            cout << b2 << endl;
            bot_exp.push_back(b2);
    
            double c2 = -lattice[index_bot-1].link[direction];
//            cout << c2 << endl;
            bot_exp.push_back(c2);
    
            index_bot = looper_forward(index_bot, dimension_lengths, direction);
            double d2 = -lattice[index_bot-1].link[dir];
//            cout << d2 << endl;
            bot_exp.push_back(d2);

            exp_list.push_back(top_exp);
            exp_list.push_back(bot_exp);

        }
    }
    
    return exp_list;
}

vector<double> random_theta(double x2, double scale)
{
    vector<double> random_theta_result;
    
    x2 = k*fmod(x2, q) - (r*x2)/q;
    
    double theta = scale * (x2/m) * (M_PI);

    random_theta_result.push_back(x2);
    random_theta_result.push_back(theta);

    return random_theta_result;

}

vector<double> accept_reject(double x1, double x2, double beta, double scale, double old_action, double new_action, double common, double theta)
{
    vector<double> accept_reject_results;
    
    x1 = k*fmod(x1, q) - (r*x1)/q;
        
    if (x1 < 0)
    {
        x1 += m;
    }
    
    r1 = x1/m;
    
//    cout << "new action: " << new_action << endl;

    if ( new_action < -log(r1)/beta + old_action )
    {
        common = fmod(common + scale*theta, 2*M_PI );
        
//        cout << "accept random" << endl;
//        cout << "accept random: n_" << new_action << " o_" << old_action << endl;
    }
    else
    {
//        cout << "reject random" << endl;
    }

    accept_reject_results.push_back(x1);
    accept_reject_results.push_back(common);
    accept_reject_results.push_back(x2);

    return accept_reject_results;
}

vector<double> update(vector< vector<double> > exp_list, double beta, double scale, double x1, double x2)
{    
    vector<double> accept_reject_results;
    
    double old_action = 0;
    double new_action = 0;
    
    double common = exp_list[0][0];
    
    vector<double> random_theta_results = random_theta(x2, scale);
    double theta = random_theta_results[1];
    x2 = random_theta_results[0];
    
    double new_common = fmod(common + theta, 2*M_PI);
    double exp_sum;
    
    for (int i = 1; i < exp_list.size(); i++)
    {
        exp_sum = 0;
        for (int j = 0; j < exp_list[i].size(); j++)
        {
            exp_sum += exp_list[i][j];
        }
        old_action += cos(common + exp_sum);
        new_action += cos(new_common + exp_sum);
    }
        
    if (new_action < old_action)
    {
        accept_reject_results.push_back(x1);
        accept_reject_results.push_back(new_common);
        accept_reject_results.push_back(x2);

//        cout << "accpet new action" << endl;
//        cout << "accept new action smaller: n_" << new_action << " o_" << old_action << endl;
    }
    else
    {
        accept_reject_results = accept_reject(x1, x2, beta, scale, old_action, new_action, common, theta);
    }

//    cout << "x1: " << accept_reject_results[0] << endl;
//    cout << "common: " << accept_reject_results[1] << endl;
//    cout << "x2: " << accept_reject_results[2] << endl;

    return accept_reject_results;

}

// Warning: this method double counts when there are only two dimensions!
double compute_plaquettes_at_site(int index, vector<int> dimension_lengths, int direction, site lattice[])
{
    int second_direction;
    
    if (direction + 1 > (dimension_lengths.size() - 1) )
    {
        second_direction = 0;
    }
    else
    {
        second_direction = direction + 1;
    }

//    cout << "site: " << index << endl;
//    cout << "direcion: " << direction << endl;
//    cout << "second direction: " << second_direction << endl;

    double a = lattice[index-1].link[direction];
    double b = -lattice[index-1].link[second_direction]; // minus
    
    index = looper_forward(index, dimension_lengths, direction);
//    cout << "next site: " << index << endl;
    double c = lattice[index-1].link[second_direction];
    
    index = looper_backward(index, dimension_lengths, direction);
//    cout << "next site: " << index << endl;
    
    index = looper_forward(index, dimension_lengths, second_direction);
//    cout << "next site: " << index << endl;
    double d = -lattice[index-1].link[direction]; // minus
    
//    double real_plaquette = cos(a + b + c + d);
//    double imag_plaquette = sin(a + b + c + d);
    
//    vector<double> plaquette;
//    plaquette.push_back(real_plaquette);
//    plaquette.push_back(imag_plaquette);

//    cout << "a: " << a << ", b: " << b << ", c: " << c << ", d: " << d << endl;
//    cout << "exponential: " << a + b + c + d << endl;
    double real_plaquettes_at_site = cos(a+b+c+d);

//    cout << "real plaquette: " << real_plaquette << endl;

    return real_plaquettes_at_site;
}

void compute_polyakov_loop(site lattice[], vector<int> dimension_lengths, int lattice_size)
{
    int direction = 0;
    int time_length = dimension_lengths[0];
    int number_of_loops = 1;
    
    for (int d = 1; d < dimension_lengths.size(); d++)
    {
        number_of_loops *= dimension_lengths[d];
    }
    
//    cout << "number of loops: " << number_of_loops << endl;
    
    double sum_cos = 0;
    double sum_sin = 0;
    
    double sum_exp;
    int s;
    
    for (int n = 0; n < number_of_loops; n++)
    {
        s = (n*time_length) + 1;
//        cout << "s: " << s << endl;
        sum_exp = 0;
        for (int t = 1; t <= time_length; t++)
        {
            sum_exp += lattice[s].link[0];
            s = looper_forward(s, dimension_lengths, direction);
        }
        sum_cos += cos(sum_exp);
        sum_sin += sin(sum_exp);
    }

    double avg_real_polyakov = sum_cos/number_of_loops;
    double avg_imag_polyakov = sum_sin/number_of_loops;
    
//    cout << "avg real Polyakov: " << avg_real_polyakov << endl;
//    cout << "avg imag Polyakov: " << avg_imag_polyakov << endl;
}


int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        cout << "Wrong number of arguments." << endl;
        return 1;
    }

    string L = argv[1];
    double beta = atof(argv[2]);
    double scale = atof(argv[3]);
    int traj_length = atof(argv[4]);
    
    double common;
    double x1 = 19940910;
    double x2 = 1904991;
    vector<double> accept_reject_results;

    double init_exponent = .1;

    vector<int> dimension_lengths = user_dimensions(L);
    int number_of_dimensions = dimension_lengths.size();    
    
    int lattice_size = 1;
    
    for (int i = 0; i < number_of_dimensions; i ++)
    {
        lattice_size = lattice_size * dimension_lengths[i];
    }

    site lattice[lattice_size];
    initialize_lattice(lattice, lattice_size, number_of_dimensions, init_exponent);

    double number_of_plaquettes = number_of_dimensions * lattice_size;

    double total_real_plaquettes;
    double avg_real_plaquettes;

    vector< vector<double> > exp_list;
    double real_plaquettes_at_site;

//    print_lattice(lattice, lattice_size, number_of_dimensions);

//    for (int traj = 0; traj < traj_length; traj++)
//    {
//        for (int direction = 0; direction < number_of_dimensions; direction++)
//        {
//            for (int s = 1; s <= lattice_size; s++)
//            {
//                cout << "direction: " << direction << ", s: " << s << endl;
//                cout << lattice[s-1].link[direction] << endl;
//                real_plaquettes_at_site = compute_plaquettes_at_site(s, dimension_lengths, direction, lattice);
//                cout << "plaquette: " << real_plaquettes_at_site << endl;
//            }   
//        }
//    }
    // Iterate over number of frames.
    for (int traj = 0; traj < traj_length; traj++)
    {
//        cout << "traj: " << traj << endl;

        total_real_plaquettes = 0;
        avg_real_plaquettes;

        // Iterate over directions.
        for (int direction = 0; direction < number_of_dimensions; direction++)
        {
            // Iterate over sites.
            for (int s = 1; s <= lattice_size; s++)
            {
                exp_list = collect_exps(s, dimension_lengths, direction, lattice);
                accept_reject_results = update(exp_list, beta, scale, x1, x2);
            
                x1 = accept_reject_results[0];
                x2 = accept_reject_results[2];
                common = accept_reject_results[1];
//                cout << "common: " << common << endl;
                lattice[s-1].link[direction] = accept_reject_results[1]; // common
                
                real_plaquettes_at_site = compute_plaquettes_at_site(s, dimension_lengths, direction, lattice);

                total_real_plaquettes += real_plaquettes_at_site;

//                cout << "*************************************" << endl << endl;
            }
        }
        
//        cout << "total real plaquettes: " << total_real_plaquettes << endl;
        avg_real_plaquettes = total_real_plaquettes / number_of_plaquettes;
        cout << "avg real plaquettes: " << avg_real_plaquettes << endl;

//        compute_polyakov_loop(lattice, dimension_lengths, lattice_size);

    }
    
//    print_lattice(lattice, lattice_size, number_of_dimensions);
//
//    for (int i = 0; i < 100; i++)
//    {
//        vector<double> random_theta_results = random_theta(x2, scale);
//        x2 = random_theta_results[0];
//        cout << random_theta_results[1] << endl;
//        
//    }

    
    return 0;
}