// Imports all of the libraries I will need
#include <iostream>
#include <fstream> 
#include <utility>
#include <cmath>
#include <string> // Added for std::to_string
using namespace std;

// Define a struct to hold the RK4 results
struct RK4Result {
    float* Time;                // Array of time points
    float** Values;             // 2D array of solution values
    float** Rates_of_Change;    // 2D array of rates of change
    float** PQR;
};

// Sets the type so I can pass my ODE function into my RK4 as a pointer
typedef std::tuple<float*, float**,float*> (*DiffEqFunc)(float t, float* X,float** DCM, int size);

// Gimbal Equation
float* Gimbal_Equation(float* PQR, float* Euler_Angles){
    float* result = new float[3];
    result[0] = PQR[0] + sin(Euler_Angles[0]) * tan(Euler_Angles[1]) * PQR[1] + cos(Euler_Angles[0]) * tan(Euler_Angles[1]) * PQR[2];
    result[1] = cos(Euler_Angles[0]) * PQR[1] - sin(Euler_Angles[0]) * PQR[2];
    result[2] = (sin(Euler_Angles[0]) / cos(Euler_Angles[1])) * PQR[1] + (cos(Euler_Angles[0]) / cos(Euler_Angles[1])) * PQR[2];
    return result;
}


float** Strap_Down_Equation(float* PQR, float** C) {
    // Dynamically allocate result matrix (3x3)
    float** result = new float*[3];
    for (int i = 0; i < 3; ++i) {
        result[i] = new float[3];
    }

    // Extract P, Q, R from PQR array
    float P = PQR[0];
    float Q = PQR[1];
    float R = PQR[2];

    // Define Strap matrix using P, Q, R
    float Strap[3][3] = {
        {0, -R, Q},
        {R, 0, -P},
        {-Q, P, 0}
    };

    // Initialize result matrix to zero
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result[i][j] = 0;
        }
    }

    // Perform matrix multiplication (Strap * C)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                result[i][j] += Strap[i][k] * C[k][j];
            }
        }
    }

    return result;
}

float**  matrixMultiply(float** A, float** B) {
    // Dynamically allocate result matrix (3x3)
    float** result = new float*[3];
    for (int i = 0; i < 3; ++i) {
        result[i] = new float[3];
    }
    // Initialize result matrix to zero
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return result;
}

float** addMatrices(float** A, float** B) {
    // Dynamically allocate result matrix (3x3)
    float** result = new float*[3];
    for (int i = 0; i < 3; ++i) {
        result[i] = new float[3];
    }

    // Perform matrix addition
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result[i][j] = A[i][j] + B[i][j];
        }
    }

    return result;
}

float** multiplyMatrixByScalar(float** matrix, float scalar) {
    // Dynamically allocate result matrix (3x3)
    float** result = new float*[3];
    for (int i = 0; i < 3; ++i) {
        result[i] = new float[3];
    }

    // Perform scalar multiplication
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result[i][j] = matrix[i][j] * scalar;
        }
    }

    return result;
}


float* multiplyMatrixVector(float** dcm, float* velocity) {
    // Dynamically allocate result vector (3 elements)
    float* result = new float[3];

    // Perform matrix-vector multiplication
    for (int i = 0; i < 3; ++i) {
        result[i] = 0;  // Initialize result[i]
        for (int j = 0; j < 3; ++j) {
            result[i] += dcm[i][j] * velocity[j];  // Access DCM as a 2D pointer
        }
    }

    return result;
}

// This is the function that will do our ODE for us. It takes in a state vector, and a time and it will output
// Derivative state vector and PQR values
std::tuple<float*, float**,float*> Diff_EQs(float t, float* Euler_Angles,float** DCM, int size) {

    
    
    // Allocate memory for dydt
    float* dydt = new float[size];
    
    // Allocate memory for pqr
    float* pqr = new float[size];
    pqr[0] = M_PI / 6; 
    pqr[1] = cos(6 / M_PI * t); 
    pqr[2] = 3 * sin(30 / M_PI * t); 
    
    // Compute the derivative using the Gimbal Equation
    float* temp_dydt = Gimbal_Equation(pqr, Euler_Angles);
    
    // Copy the results to dydt
    for(int i = 0; i < size; ++i){
        dydt[i] = temp_dydt[i];
    }
    
    // Free temporary memory
    delete[] temp_dydt;

    float**  C_Dot = Strap_Down_Equation(pqr, DCM);

    // Get Velocity in NED
    //this is positiondot for position measured in NED
    float Velocity[3] = {Euler_Angles[3], Euler_Angles[4], Euler_Angles[5]};  // Assuming Euler_Angles array is defined
    float* Rotated_V = multiplyMatrixVector( DCM, Velocity);


    // Assemble the vector of state derivatives -- putting in zeros for the
    // derivative of velocity since it is a constant here


    float* extended_dydt = new float[size];

    extended_dydt[0] = dydt[0];
    extended_dydt[1] = dydt[1];
    extended_dydt[2] = dydt[2];
    extended_dydt[3] = 0;
    extended_dydt[4] = 0;
    extended_dydt[5] = 0;
    extended_dydt[6] = Rotated_V[0];
    extended_dydt[7] = Rotated_V[1];
    extended_dydt[8] = Rotated_V[2];

    
    

    // Return both dydt and pqr
    return std::make_tuple(extended_dydt,C_Dot, pqr);
}



// Rk4 Function, It will take in a ODE function, time span, initial state, stepsize, and size of the state vector.
// It will output a time array, solution array, and Rates_of_Change array encapsulated in RK4Result.
RK4Result rk4(DiffEqFunc ODE, float* tspan, float* y0, float h, int Size) {


    float** DCM = new float*[3];
    float** R_phi = new float*[3];
    float** R_theta = new float*[3];
    float** R_psi = new float*[3];
    for (int i = 0; i < 3; ++i) {
        DCM[i] = new float[3];
        R_phi[i] = new float[3];
        R_theta[i] = new float[3];
        R_psi[i] = new float[3];
    }

    // Identity matrix (DCM initialized as identity)
    DCM[0][0] = 1.0f; DCM[0][1] = 0.0f; DCM[0][2] = 0.0f;
    DCM[1][0] = 0.0f; DCM[1][1] = 1.0f; DCM[1][2] = 0.0f;
    DCM[2][0] = 0.0f; DCM[2][1] = 0.0f; DCM[2][2] = 1.0f;

    // Rotation matrix for phi (around x-axis)
    R_phi[0][0] = 1; R_phi[0][1] = 0;                        R_phi[0][2] = 0;
    R_phi[1][0] = 0; R_phi[1][1] = cos(y0[0]);      R_phi[1][2] = -sin(y0[0]);
    R_phi[2][0] = 0; R_phi[2][1] = sin(y0[0]);      R_phi[2][2] = cos(y0[0]);

    // Rotation matrix for theta (around y-axis)
    R_theta[0][0] = cos(y0[1]); R_theta[0][1] = 0; R_theta[0][2] = sin(y0[1]);
    R_theta[1][0] = 0;                    R_theta[1][1] = 1; R_theta[1][2] = 0;
    R_theta[2][0] = -sin(y0[1]);R_theta[2][1] = 0; R_theta[2][2] = cos(y0[1]);

    // Rotation matrix for psi (around z-axis)
    R_psi[0][0] = cos(y0[2]); R_psi[0][1] = -sin(y0[2]); R_psi[0][2] = 0;
    R_psi[1][0] = sin(y0[2]); R_psi[1][1] = cos(y0[2]);  R_psi[1][2] = 0;
    R_psi[2][0] = 0;                    R_psi[2][1] = 0;                    R_psi[2][2] = 1;

    DCM = matrixMultiply(R_phi, DCM);
    
    DCM = matrixMultiply(R_theta, DCM);
    
    DCM = matrixMultiply(R_psi, DCM);
    
    
    RK4Result result; // Initialize the struct to hold results

    // Calculate the number of time steps
    int Time_Size = static_cast<int>((tspan[1] - tspan[0]) / h) + 1;

    // Allocate memory for Time, Values, and Rates_of_Change
    result.Time = new float[Time_Size];
    result.Values = new float*[Time_Size];
    result.Rates_of_Change = new float*[Time_Size];
    result.PQR = new float*[Time_Size];
    
    for (int i = 0; i < Time_Size; ++i) {
        result.Values[i] = new float[Size];
        result.Rates_of_Change[i] = new float[Size];
        result.PQR[i] = new float[Size];
    }

    // Initialize the time array
    result.Time[0] = tspan[0];
    for (int i = 1; i < Time_Size; ++i) {
        result.Time[i] = result.Time[i - 1] + h; 
    }

    // Initialize the Values array with y0
    for (int i = 0; i < Size; i++) {
        result.Values[0][i] = y0[i]; 
    }

    // Loop over each time step
    for (int i = 0; i < Time_Size - 1; i++) {

        // Current time
        float Current_Time = result.Time[i];

        // Initialize k arrays
        float k1[Size], k2[Size], k3[Size], k4[Size];

        // Compute k1
        auto res_k1 = ODE(Current_Time, result.Values[i],DCM, Size);
        auto [dydt1, Cdot1, pqr1] = res_k1;

        // Store Rates_of_Change for k1
        for (int j = 0; j < Size; ++j) {
            
            result.PQR[i][j] = pqr1[j];
        }

        // Calculate k1
        for (int j = 0; j < Size; ++j) {
            k1[j] = dydt1[j];
        }

        // Free memory allocated by ODE
        delete[] dydt1;

        // Compute k2
        float* temp_for_K2 = new float[Size];
        for (int j = 0; j < Size; ++j) {
            temp_for_K2[j] = result.Values[i][j] + k1[j] / 2.0f * h; 
        }

        float** K2_DCM_Input = addMatrices(DCM, multiplyMatrixByScalar(Cdot1, h/2.0));

        auto res_k2 = ODE(Current_Time + h / 2.0f, temp_for_K2,K2_DCM_Input, Size);
        auto [dydt2, Cdot2, pqr2] = res_k2;

        for (int j = 0; j < Size; ++j) {
            k2[j] = dydt2[j];
        }

        delete[] dydt2;
        delete[] pqr2;
        delete[] temp_for_K2; 

        // Compute k3
        float* temp_for_K3 = new float[Size];
        for (int j = 0; j < Size; ++j) {
            temp_for_K3[j] = result.Values[i][j] + k2[j] / 2.0f * h; 
        }

        float** K3_DCM_Input = addMatrices(DCM, multiplyMatrixByScalar(Cdot2, h/2.0));

        auto res_k3 = ODE(Current_Time + h / 2.0f, temp_for_K3,K3_DCM_Input, Size);
        auto [dydt3, Cdot3, pqr3] = res_k3;

        for (int j = 0; j < Size; ++j) {
            k3[j] = dydt3[j];
        }

        delete[] dydt3;
        delete[] pqr3;
        delete[] temp_for_K3;

        // Compute k4
        float* temp_for_K4 = new float[Size];
        for (int j = 0; j < Size; ++j) {
            temp_for_K4[j] = result.Values[i][j] + k3[j]*h; 
        }

        float** K4_DCM_Input = addMatrices(DCM, multiplyMatrixByScalar(Cdot3, h));

        auto res_k4 = ODE(Current_Time + h, temp_for_K4,K4_DCM_Input, Size);
        auto [dydt4, Cdot4, pqr4] = res_k4;

        for (int j = 0; j < Size; ++j) {
            k4[j] = dydt4[j];
        }

        delete[] dydt4;
        delete[] pqr4;
        delete[] temp_for_K4;

        float** CD2_Mult = multiplyMatrixByScalar(Cdot2,2);
        float** CD3_Mult = multiplyMatrixByScalar(Cdot3,2);
        float** totalCdot = multiplyMatrixByScalar(addMatrices(addMatrices(addMatrices(Cdot1,CD2_Mult),CD3_Mult),Cdot4),(1.0/6.0));
        

        for (int l = 0; l < Size; ++l) {
            result.Rates_of_Change[i][l] = (1.0f / 6.0f) * (k1[l] + 2.0f * k2[l] + 2.0f * k3[l] + k4[l]);
        }

        DCM = addMatrices(DCM,multiplyMatrixByScalar(totalCdot,h));
        // Update the next Values using RK4 formula
        for (int j = 0; j < Size; ++j) {
            float Value = result.Values[i][j] + result.Rates_of_Change[i][j]*h;
            if (j <3){
            // Normalize the angle to [0, 2π]
            while (Value > 2 * M_PI){
                Value -= 2 * M_PI;
            }
            while (Value < -2*M_PI){
                Value += 2 * M_PI;
            }
            }

            result.Values[i + 1][j] = Value;
        }
    }

    // Return the RK4Result struct
    return result;
}

// This function will send our data from here to a CSV file to be analysed
// It takes in a file name, time array, values array, the time size, and size of the state vector
void writeToCSV(const std::string& filename, float* timeArray, float** valuesArray, int time_Size, int size) {

    // Opens the file
    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // This will write the header
    outFile << "Time";
    for (int j = 1; j <= size; j++) {
        outFile << ",Value_" << j; // Create column headers for each value
    }
    outFile << "\n";

    // This will write the time and value data in the correct spot in the CSV file
    for (int w = 0; w < time_Size; w++) {
        outFile << timeArray[w]; // Write time
        for (int j = 0; j < size; j++) {
            outFile << "," << valuesArray[w][j]; // Write each value
        }
        outFile << "\n"; // New line for the next time point
    }

    // Closes the file
    outFile.close();
}

int main() {
    // Sets the different dt values
    float Hs[4] = {0.2f, 0.1f, 0.025f, 0.0125f};

    // Sets the tspan from 0 to 60 seconds
    float Tspan[2] = {0.0f, 60.0f};

    // Sets the state vector size to 3
    int Size = 9;

    // Initializes the initial state vector
    float Y0[Size] = {0.0f, 0.0f, 0.0f,101.269f,0.0f,0.0f,0.0f,0.0f,0.0f};

    // Runs through all of the h values
    for (int i = 0; i < 4; i++) {

        // Calls the RK4 function
        RK4Result result = rk4(Diff_EQs, Tspan, Y0, Hs[i], Size);
    
        // Assign arrays from the result struct
        float* TimeArray = result.Time;
        float** ValuesArray = result.Values;
        float** RatesArray = result.Rates_of_Change;
        float** PQRArray = result.PQR;

        // Sets the size of the time array
        int time_Size = static_cast<int>((Tspan[1] - Tspan[0]) / Hs[i] + 1);

        // Writes the Data to a CSV file
        writeToCSV("output_H_" + std::to_string(Hs[i]) + ".csv", TimeArray, ValuesArray, time_Size, Size);
        
        // Optionally, write Rates_of_Change to a separate CSV
        writeToCSV("rates_H_" + std::to_string(Hs[i]) + ".csv", TimeArray, RatesArray, time_Size, Size);

        writeToCSV("PQR_H_" + std::to_string(Hs[i]) + ".csv", TimeArray, PQRArray, time_Size, Size);

        // Deletes allocated memory
        delete[] TimeArray;
        for (int j = 0; j < time_Size; j++) { // Corrected loop condition
            delete[] ValuesArray[j]; 
            delete[] RatesArray[j]; // Also delete Rates_of_Change
        }
        delete[] ValuesArray; 
        delete[] RatesArray; 
    }

    return 0;
}
