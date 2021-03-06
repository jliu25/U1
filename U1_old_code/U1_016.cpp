#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <typeinfo>
#include <iostream>
#include <complex>
#include <vector>
#include <string>
using namespace std;


complex<double> unitarize(complex<double> comp)
{
    complex<double> uni = comp / sqrt(conj(comp)*comp);
    return uni;
}

int choose(int n, int k)
{
    if (k == 0)
    {
        return 1;
    }
    else
    {
        return (n * choose(n - 1, k - 1)) / k;
    }
}

// Returns a vector with the dimension lengths.
vector<int> user_dimensions(string L)
{
    string delim = "x";

    vector<int> dimension_lengths;
    int index = 0;

    int start = 0;
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
    vector< complex<double> > link;
};

// Initialize lattice.
void initialize_lattice(site lattice[], int lattice_size, vector<int> dimension_lengths, double init_angle)
{
    for (int s = 0; s < lattice_size; s++)
    {
        for (int l = 0; l < dimension_lengths.size(); l++)
        {
            lattice[s].link.push_back(complex<double> (cos(init_angle), sin(init_angle)));
        }
    }
}

// Print out the links of the lattice.
void print_lattice(site lattice[], int lattice_size, int number_of_dimensions)
{
    for (int i = 0; i < lattice_size; i++)
    {
        for (int l = 0; l < number_of_dimensions; l++)
        {
//            cout << "site: " << i << ", dir: " << l << ", complex: " << lattice[i].link[l] << endl;
            printf("site %i, dir: %i, real: %f, imag: %f\n", i, l, (lattice[i].link[l]).real(), (lattice[i].link[l]).imag());
            cout << "++++++++++++++++++++++++++++++" << endl;
        }
    }
}

int convert_index_to_single(vector<int> dimension_lengths, vector<int> indices)
{
    int d_len = 1;
    int single_index = indices[0];
    
    for (int i = 1; i < indices.size(); i++)
    {
        d_len *= dimension_lengths[i-1];

        single_index += d_len * indices[i];
    }
    
    return single_index;
}

vector<int> convert_index_to_multiple(vector<int> dimension_lengths, int index)
{   
    vector<int> indices;
    indices.reserve(dimension_lengths.size());
    
    int spacial_dimension = 1;
    int leng = 1;

    int m, mm;

    for (int d = 0; d < dimension_lengths.size(); d++)
    {
        m = index / leng;
        mm = m % dimension_lengths[d];

        indices.push_back(mm);

        leng *= dimension_lengths[d];
    }

    return indices;
}

// Go forward in one direction.
vector<int> looper_forward(vector<int> indices, vector<int> dimension_lengths, int direction)
{
    int change = indices[direction] + 1;
    if (change >= dimension_lengths[direction])
    {
        change = 0;
    }

    indices[direction] = change;
    
    return indices;
}

// Go backwards in one direction.
vector<int> looper_backward(vector<int> indices, vector<int> dimension_lengths, int direction)
{
    int change = indices[direction] - 1;
    if (change < 0)
    {
        change = dimension_lengths[direction] - 1;
    }

    indices[direction] = change;

    return indices;
}

// Collect the staples.
complex<double> get_staples(int index, vector<int> dimension_lengths, int first_direction, site lattice[])
{
    complex<double> staples(0.0, 0.0);

    for (int second_direction = 0; second_direction < dimension_lengths.size(); second_direction++)
    {
        if (second_direction != first_direction)
        {
            complex<double> b1 = conj(lattice[index].link[second_direction]);

            vector<int> indices = convert_index_to_multiple(dimension_lengths, index);
            vector<int> next_indices = looper_forward(indices, dimension_lengths, first_direction);
            int next_index = convert_index_to_single(dimension_lengths, next_indices);

            complex<double> c1 = lattice[next_index].link[second_direction];

            next_indices = looper_forward(indices, dimension_lengths, second_direction);
            next_index = convert_index_to_single(dimension_lengths, next_indices);

            complex<double> d1 = conj(lattice[next_index].link[first_direction]);

            next_indices = looper_backward(indices, dimension_lengths, second_direction);
            next_index = convert_index_to_single(dimension_lengths, next_indices);
            
            complex<double> b2 = lattice[next_index].link[second_direction];
            complex<double> c2 = conj(lattice[next_index].link[first_direction]);

            next_indices = looper_forward(next_indices, dimension_lengths, first_direction);
            next_index = convert_index_to_single(dimension_lengths, next_indices);

            complex<double> d2 = conj(lattice[next_index].link[second_direction]);

            staples += b1*c1*d1 + b2*c2*d2;
        }
    }

    return staples;
}

//void walker(int start, vector< vector<int> > path, site lattice[])
//{
//    for (int i = 0; i < path.size(); i++)
//    {
//        int direction = path[i][0];
//        int distance = path[i][1];
//        cout << "direction: " << direction << ", distance: " << distance << endl;
//    }
//}


    
complex<double> update(complex<double> U, complex<double> staples, double beta, double scale)
{
    double theta = scale * 2 * ( drand48()-.5 );
    
    complex<double> U_change(cos(theta), sin(theta));
    
    complex<double> new_U = unitarize(U * U_change);

    double old_action = beta * (U*staples).real();
    double new_action = beta * (new_U*staples).real();

    if (new_action > old_action)
    {
        U = new_U;
    }
    else
    {
        double r = drand48();

        if (r < exp(new_action - old_action))
        {
            U = new_U;
        }
    }
    
    return U;
}

double compute_real_plaquettes_at_site(int index, vector<int> dimension_lengths, int first_direction, site lattice[])
{
    double real_plaquettes_at_site = 0;

    for (int second_direction = first_direction + 1; second_direction < dimension_lengths.size(); second_direction++)
    {
        complex<double> a = lattice[index].link[first_direction];
        complex<double> b = conj(lattice[index].link[second_direction]);

        vector<int> indices = convert_index_to_multiple(dimension_lengths, index);

        vector<int> next_indices = looper_forward(indices, dimension_lengths, first_direction);

        int next_index = convert_index_to_single(dimension_lengths, next_indices);

        complex<double> c = lattice[next_index].link[second_direction];

        next_indices = looper_forward(indices, dimension_lengths, second_direction);
        next_index = convert_index_to_single(dimension_lengths, next_indices);

        complex<double> d = conj(lattice[next_index].link[first_direction]);

        real_plaquettes_at_site += (a*b*c*d).real();
    }

    return real_plaquettes_at_site;
}

void compute_avg_plaquette(vector<int> dimension_lengths, int lattice_size, site lattice[])
{
    double total_real_plaquettes = 0;

    int number_of_plaquettes = choose(dimension_lengths.size(), 2) * lattice_size;

    for (int direction = 0; direction < dimension_lengths.size(); direction++)
    {
        // Iterate over sites.
        for (int s = 0; s < lattice_size; s++)
        {
            // Iterate over directions.
            if (direction < dimension_lengths.size() - 1)
            {
                total_real_plaquettes += compute_real_plaquettes_at_site(s, dimension_lengths, direction, lattice);
            }
        }
    }

    double avg_real_plaquettes = total_real_plaquettes / number_of_plaquettes;

//    cout << "PLAQ   " << avg_real_plaquettes << "   ";
}


complex<double>* update_polyakov(vector<int> dimension_lengths, site lattice[], complex<double>* polyakov_list)
{
    int time_dimension_position = dimension_lengths.size() - 1;

    complex<double> total_polyakov(0.0, 0.0);

    int spacial_dimension = 1;

    for (int s = 0; s < dimension_lengths.size(); s++)
    {
        spacial_dimension *= dimension_lengths[s];
    }

    complex<double> polyakov;
    complex<double> U;

    for (int i = 0; i < spacial_dimension; i++)
    {
        polyakov = 1;
        int time_index = i;
        for (int t = 0; t < dimension_lengths[time_dimension_position]; t++)
        {
            polyakov *= lattice[time_index].link[time_dimension_position];
            vector<int> time_indices = convert_index_to_multiple(dimension_lengths, time_index);
            time_indices = looper_forward(time_indices, dimension_lengths, time_dimension_position);
            time_index = convert_index_to_single(dimension_lengths, time_indices);
        }

        polyakov_list[i] = polyakov;

        total_polyakov += polyakov;
    }

    double avg_polyakov_real = total_polyakov.real()/spacial_dimension;
    double avg_polyakov_imag = total_polyakov.imag()/spacial_dimension;

    double avg_polyakov = sqrt(total_polyakov.real()*total_polyakov.real() + total_polyakov.imag()*total_polyakov.imag()) / spacial_dimension;

//    cout << "POLY " << avg_polyakov << " " << avg_polyakov_real << " " << avg_polyakov_imag << endl;
    printf("POLY    %f      %f      %f\n", avg_polyakov, avg_polyakov_real, avg_polyakov_imag);

    return polyakov_list;

}

void compute_polyakov_pairs(vector<int> dimension_lengths, complex<double>* polyakov_list)
{
    int spacial_dimension = 1;
    
    for (int s = 0; s < dimension_lengths.size()-1; s++)
    {
        spacial_dimension *= dimension_lengths[s];
    }
    
    complex<double> origin_poly = polyakov_list[0];
    
    for (int i = 0; i < spacial_dimension; i++)
    {
        int sum_of_squares = 0;
        int leng = 1;

        for (int d = 0; d < dimension_lengths.size()-1; d++)
        {
            int m = i / leng;
            int mm = m % dimension_lengths[d];

            sum_of_squares += mm*mm;

            leng *= dimension_lengths[d];
        }
        
        double r = sqrt(sum_of_squares);

        complex<double> other_poly = polyakov_list[i];

        complex<double> PP = origin_poly * conj(other_poly);

//        cout << "P0P* " << r << " " << PP << endl;
        printf("P0P*    %f      %f      %f\n", r, PP.real(), PP.imag());
    }
    
}




double* fillarr(double* arr)
{
    arr[0] = 2;
    return arr;
}


int main(int argc, char* argv[])
{
    if (argc < 6)
    {
        cout << "Wrong number of arguments." << endl;
        return 1;
    }

    string L = argv[1];
    double beta = atof(argv[2]);
    double scale = atof(argv[3]);
    int traj_length = atof(argv[4]);
    int meas = atof(argv[5]);

    double init_angle = 0;

    vector<int> dimension_lengths = user_dimensions(L);
    int number_of_dimensions = dimension_lengths.size();

    int lattice_size = 1;

    for (int i = 0; i < number_of_dimensions; i++)
    {
        lattice_size = lattice_size * dimension_lengths[i];
    }

    site lattice[lattice_size];
    initialize_lattice(lattice, lattice_size, dimension_lengths, init_angle);

    int spacial_dimension = 1;

    for (int s = 0; s < dimension_lengths.size(); s++)
    {
        spacial_dimension *= dimension_lengths[s];
    }

    printf("Lattice:    %s\n", L.c_str());
    printf("Beta:       %f\n", beta);
    printf("Scale:      %f\n", scale);
    printf("Trajecs:    %d\n", traj_length);
    printf("Meas:       %d\n", meas);

    compute_avg_plaquette(dimension_lengths, lattice_size, lattice);

    complex<double> polyakov_list[spacial_dimension];
    complex<double> *polyakov_list_pointer = update_polyakov(dimension_lengths, lattice, polyakov_list);

//    compute_polyakov_pairs(dimension_lengths, polyakov_list);

    // Iterate over number of frames.
    for (int traj = 1; traj <= traj_length; traj++)
    {
        double total_real_plaquettes = 0;

        // Iterate over sites.
        for (int s = 0; s < lattice_size; s++)
        {
            // Iterate over directions.
            for (int direction = 0; direction < number_of_dimensions; direction++)
            {
                complex<double> staples = get_staples(s, dimension_lengths, direction, lattice);
                complex<double> U = lattice[s].link[direction];
                U = update(U, staples, beta, scale);
                lattice[s].link[direction] = U;
            }
        }
        if (traj % meas == 0)
        {
            compute_avg_plaquette(dimension_lengths, lattice_size, lattice);
            polyakov_list_pointer = update_polyakov(dimension_lengths, lattice, polyakov_list);
//            compute_polyakov_pairs(dimension_lengths, polyakov_list);
        }
    }

//    vector< vector<int> > all_indices;
//    all_indices.reserve(lattice_size);
//
//    for (int l = 0; l < lattice_size; l++)
//    {
//        vector<int> temporary;
//        temporary.reserve(dimension_lengths.size());
//        all_indices.push_back(temporary);
//    }

//    for (int d = 0; d < dimension_lengths.size(); d++)
//    {
//        for (int x = 0; x < dimension_lengths[d]; x++)
//        {
//            int y = x;
//            while ( y < lattice_size)
//            {
//                cout << "y: " << y << endl;
//                all_indices[x].push_back(y);
//                y += dimension_lengths[d];
//            }
//        }
//    }
    
//    for (int a = 0; a < lattice_size; a++)
//    {
//        for (int d = 0; d < dimension_lengths.size(); d++)
//        {
//            cout << all_indices[a][d];
//        }
//        cout << endl;
//        cout << convert_index_to_single(dimension_lengths, all_indices[a]) << endl;
//    }
//    vector<int> temp;
//    temp.push_back(1);
//    temp.push_back(1);
//    temp.push_back(0);
//    
//    cout << convert_index_to_single(dimension_lengths, temp) << endl;

//     int index = 0;

//    for (int index = 0; index < lattice_size; index++)
//    {
//        convert_index_to_multiple(dimension_lengths, index);
////        cout << endl;
//    }

//    double y[10];
//    double *a = fillarr(y);
//    cout << y[0] << endl;

//    a = fillarr(y);
//    cout << y[0] << endl;

//    y = a;

//    int lx = 4;
//    int ly = 3;
//    int lz = 2;
//    int i = 19;
//    
//    cout << i/lx << endl;
//    cout << i/(lx*ly) << endl;

//    cout << i % lx << endl;

    return 0;
}