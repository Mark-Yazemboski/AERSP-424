//Imports all of the libraries I will need
#include <iostream>
#include <fstream> 
#include <utility>
#include <cmath>
using namespace std;


//Sets the type so I can pass my ODE function into my RK4 as a pointer
typedef float* (*DiffEqFunc)(float t, const float* X, int size);

//Rk4 Function, It will take in a ODE function, time span, initial state, stepsize, and size of the state vector.
//It will output a time array and solution array.
std::pair<float*, float**> rk4(DiffEqFunc ODE, float* tspan, float* y0, float h, int Size) {

    //Figures out how many time steps we will need to go through the full tspan
    int Time_Size = static_cast<int>((tspan[1] - tspan[0]) / h) + 1;

    //Sets the time array
    float* Time = new float[Time_Size];

    //Sets the Values array for the collumns
    float** Values = new float*[Time_Size]; 
    
    //Sets the Values array with rows
    for (int i = 0; i < Time_Size; ++i) {
        Values[i] = new float[Size]; 
    }

    //Initializes the time array and populates every space with a time
    Time[0] = tspan[0];
    for (int i = 1; i < Time_Size; ++i) {
        Time[i] = Time[i - 1] + h; 
    }

    //Initializes Values[0] with y0
    for (int i = 0; i < Size; i++) {
        Values[0][i] = y0[i]; 
    }


    //Loops for each time step
    for (int i = 0; i < Time_Size-1; i++) {

        //Gets the current time
        float Current_Time = Time[i];

        //Initilizzes all of the Ks
        float k1[Size], k2[Size], k3[Size], k4[Size];


        // Computes k1, and calls the ODE function with the correct inputs for K1
        float* dydt1 = ODE(Time[i], Values[i], Size);
        //Multiplies each element in the output of the ODE function by h
        for (int j = 0; j < Size; ++j) {
            k1[j] = h*dydt1[j];
        }
        //Frees memory
        delete[] dydt1;





        

        //Makes a temp variable, because we need to add K1 to the current state and
        //Than pass it into the ODE function
        float* temp_for_K2 = new float[Size];
        for (int j = 0; j < Size; ++j) {
            temp_for_K2[j] = Values[i][j] + k1[j]/2; 
        }

        // Computes k2, and calls the ODE function with the correct inputs for K2
        float* dydt2 = ODE(Time[i] + h / 2, temp_for_K2, Size);
        //Multiplies each element in the output of the ODE function by h
        for (int j = 0; j < Size; ++j) {
            k2[j] = h*dydt2[j];
        }

        //Frees memory
        delete[] dydt2;
        delete[] temp_for_K2; 



        //Makes a temp variable, because we need to add K2 to the current state and
        //Than pass it into the ODE function
        float* temp_for_K3 = new float[Size];
        for (int j = 0; j < Size; ++j) {
            temp_for_K3[j] = Values[i][j] + k2[j]/2; 
        }

        // Computes k3, and calls the ODE function with the correct inputs for K3
        float* dydt3 = ODE(Time[i] + h / 2, temp_for_K3, Size);
        //Multiplies each element in the output of the ODE function by h
        for (int j = 0; j < Size; ++j) {
            k3[j] = h*dydt3[j];
        }

        //Frees memory
        delete[] dydt3; // Free memory allocated in ODE
        delete[] temp_for_K3; // Free memory allocated in ODE



        //Makes a temp variable, because we need to add K3 to the current state and
        //Than pass it into the ODE function
        float* temp_for_K4 = new float[Size];
        for (int j = 0; j < Size; ++j) {
            temp_for_K4[j] = Values[i][j] + k3[j]; // Use k1 to update values for k2
        }

        // Computes k4, and calls the ODE function with the correct inputs for K4
        float* dydt4 = ODE(Time[i] + h, temp_for_K4, Size);
        for (int j = 0; j < Size; ++j) {
            k4[j] = h*dydt4[j];
        }

        //Frees memory
        delete[] dydt4; // Free memory allocated in ODE
        delete[] temp_for_K4; // Free memory allocated in ODE






        //Performs the RK4 equation to each element in the current state vector.
        for (int j = 0; j < Size; ++j) {
            float Angle = Values[i][j] + (1.0 / 6.0) * (k1[j] + 2 * k2[j] + 2 * k3[j] + k4[j]);

            //Checks to see if the angle is over 2pi, and will chop it back down to zero if it is
            while (Angle>2*M_PI){
                Angle = Angle - 2*M_PI;
            }
            Values[i + 1][j] = Angle;
        }
    }

    //Returns the time and Value array as a pair
    return std::make_pair(Time, Values);
}



//This is the function that will do our ODE for us. We pass in a state vector, and a time and it will output a
//Derivative state vector
float* Diff_EQs(float t,const float* X,int size) {

    //Sets the dydt array
    float* dydt = new float[size];

    //Sets the values of the euler angles 
    float phi = X[0];
    float theta = X[1];
    float psi = X[2];

    //Sets the angular rates 
    float p = M_PI/6; 
    float q = cos(6/M_PI * t); 
    float r = 3 * sin(30/M_PI * t); 


    //Does the gimbal equation and sets the coresponding equautions to their respective location in dydt    
    dydt[0] = p + sin(phi) * tan(theta) * q + cos(phi) * tan(theta) * r;
    dydt[1] = cos(phi) * q - sin(phi) * r;
    dydt[2] = (sin(phi) / cos(theta)) * q + (cos(phi) / cos(theta)) * r;
    

    //Returns the array
    return dydt;
}


//This function will send our data from here to a CSV file to be analysed
//It takes in a file name, time array, values array, the time size, and size of the state vector
void writeToCSV(const std::string& filename, float* timeArray, float** valuesArray, int time_Size, int size) {

    //Opens the file
    std::ofstream outFile(filename);

    //This will write the header
    outFile << "Time";
    for (int j = 1; j <= size; j++) {
        outFile << ",Value_" << j; // Create column headers for each value
    }
    outFile << "\n";

    //This will write the time and value data in the correct spot in the CSV file
    for (int w = 0; w < time_Size; w++) {
        outFile << timeArray[w]; // Write time
        for (int j = 0; j < size; j++) {
            outFile << "," << valuesArray[w][j]; // Write each value
        }
        outFile << "\n"; // New line for the next time point
    }

    //Closes the file
    outFile.close();
}



int main() {
    //Sets the different dt values
    float Hs[4] = {0.2, 0.1, 0.025, 0.0125};

    //Sets the tspan from 0 to 60 seconds
    float Tspan[2] = {0, 60};

    //Sets the state vector size to 3
    int Size = 3;

    //Initilizes the initial state vector
    float Y0[Size] = {0, 0, 0};

    //runs through all of the h values
    for (int i = 0; i < 4; i++) {

        //Calls the RK4 function
        auto result = rk4(Diff_EQs, Tspan, Y0, Hs[i], 3);
    
        //Splits the pair from the RK4 function into the time and values arrays
        float* TimeArray = result.first;
        float** ValuesArray = result.second;

        //Sets the size of the time array
        int time_Size = (Tspan[1] - Tspan[0]) / Hs[i]+1;

        //Writes the Data to a CSV file
        writeToCSV("output_H_" + std::to_string(Hs[i]) + ".csv", TimeArray, ValuesArray, time_Size, Size);
        

        //Deletes some unused stuff
        delete[] TimeArray;
        for (int j = 0; j < Size + 1; j++) {
            delete[] ValuesArray[j]; 
        }
        delete[] ValuesArray; 
    }

    return 0;
}