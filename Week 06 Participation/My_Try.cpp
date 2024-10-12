//Imports the nessesary libraries
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

//Sets Pi, because M_Pi looks weird
const double PI = M_PI;

//Creates a structure to hold the angular rates
struct AngularRates {
    double p;
    double q;
    double r;
};

//Creates a structure to hold the Derivatives
struct Derivatives {
    // [phi_dot, theta_dot, psi_dot, VN_dot, VE_dot, VD_dot, PN_dot, PE_dot, PD_dot]
    double xdot[9]; 
    // Direction Cosine Matrix derivative
    double Cdot[3][3]; 
};

//Adjusts an angle proveded to a angle between -pi and pi
double Adjust_Angle(double Angle) {
    while (Angle > PI) {
        Angle -= 2.0 * PI;
    }
    while (Angle < -PI) {
        Angle += 2.0 * PI;
    }
    return Angle;
}

//This function takes in a time t and will figure out the angular rates and store them in a 
//the angular rates structure 
AngularRates Find_Angular_Rates(double t) {
    AngularRates Rates;
    Rates.p = PI / 6.0; 
    Rates.q = std::cos(6.0 / PI * t);
    Rates.r = 3.0 * std::sin(30.0 / PI * t);
    return Rates;
}

//This function will take in 2 matricies and a output matrix and multiply the 2 input matricies and stor them in the output
//Since the output matrix is massed in like a pointer, I dont need to return anythign.
void multiplyMatrices(double A[3][3], double B[3][3], double result[3][3]) {
    //Matrix multiplication requires 3 for loops, and is O(n^3) for this basic method
    for(int i=0;i<3;i++) {
        for(int j=0;j<3;j++) {
            //Initilizes the result matrix with a zero because below im doing += 
            //to simplify the code, and the results matrix could initially be full of random values
            result[i][j] = 0.0;
            for(int k=0;k<3;k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

//This will add 2 matrixes together, also a output matrix is provied to no return is nessesary.
void addMatrices(const double A[3][3], const double B[3][3], double result[3][3]) {
    for(int i=0;i<3;i++) {
        for(int j=0;j<3;j++) {
            result[i][j] = A[i][j] + B[i][j];
        }
    }
}

//This function will multiply a matrix by a vector, its also given a output matrix so no return
//Is nessesary
void multiplyMatrixVector(const double A[3][3], const double v[3], double result[3]) {
    for(int i=0;i<3;i++) {
        //Initilizes the result matrix with a zero because below im doing += 
        //to simplify the code, and the results matrix could initially be full of random values
        result[i] = 0.0;
        for(int j=0;j<3;j++) {
            result[i] += A[i][j] * v[j];
        }
    }
}



// Function to compute derivatives
Derivatives Diff_EQs(double x[9],double dcm[3][3],double vbody[3], double t) {
    Derivatives Derivative_Output;

    //Gets the euler angles from the state vector provided
    double phi = x[0];
    double theta = x[1];
    double psi = x[2];

    //calculates the angular rates from the time provided and stores them in p,q,r
    AngularRates Rates = Find_Angular_Rates(t);
    double p = Rates.p;
    double q = Rates.q;
    double r = Rates.r;

    //This is the gimbal equation
    double G[3][3] = {
        {1, std::tan(theta) * std::sin(phi), std::tan(theta) * std::cos(phi)},
        {0, std::cos(phi), -std::sin(phi)},
        {0, std::sin(phi) / std::cos(theta), std::cos(phi) / std::cos(theta)}
    };

    //Makes the angular rate vector and the Euler angle derivative vector
    double Angular_Rate_Vector[3] = {p, q, r};
    double Euler_Derivative[3] = {0.0, 0.0, 0.0};

    //Finds the Euler Derivate vector by multiplying the angular rates by the gimbal matrix
    multiplyMatrixVector(G, Angular_Rate_Vector, Euler_Derivative); 
    

    //Makes the strapdown matrix
    double Strapdown_Matrix[3][3] = {
        {0, -r, q},
        {r, 0, -p},
        {-q, p, 0}
    };

    //Finds Cdot by multiplying the dcm by the strapdown matrix
    multiplyMatrices(dcm, Strapdown_Matrix, Derivative_Output.Cdot);

    //Sets the velocity vector for the NED frame
    double VNED[3] = {0.0, 0.0, 0.0};

    //Finds the new velocity vector in the NED frame by multiplying the dcm by the velocity vector of the plane
    multiplyMatrixVector(dcm, vbody, VNED);

    //Assembles the state derivative
    //[phi_dot, theta_dot, psi_dot, VN_dot, VE_dot, VD_dot, PN_dot, PE_dot, PD_dot]
    Derivative_Output.xdot[0] = Euler_Derivative[0];
    Derivative_Output.xdot[1] = Euler_Derivative[1];
    Derivative_Output.xdot[2] = Euler_Derivative[2];
    Derivative_Output.xdot[3] = 0.0;
    Derivative_Output.xdot[4] = 0.0;
    Derivative_Output.xdot[5] = 0.0;
    Derivative_Output.xdot[6] = VNED[0];
    Derivative_Output.xdot[7] = VNED[1];
    Derivative_Output.xdot[8] = VNED[2];

    //returns the state derivative
    return Derivative_Output;
}


//Main
int main() {
    //Sets the dts and time of simutation
    double dts[4] = {0.2f, 0.1f, 0.025f, 0.0125f};
    double tmax = 60.0;

    //Initial State Vector [phi, theta, psi, VN, VE, VD, PN, PE, PD]
    double Initial_State_Vector[9] = {0.0, 0.0, 0.0, 60 * 0.514444, 0.0, 0.0, 0.0, 0.0, 0.0};

    //Initial Derivative State Vector [phi_dot, theta_dot, psi_dot, VN_dot, VE_dot, VD_dot, PN_dot, PE_dot, PD_dot]
    double Initial_Derivative_State_Vector[9] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    //Initial PQR
    double Initial_PQR[9] = {0.0, 0.0, 0.0};            

    //Body frame velocity in m/s
    double V_body[3] = {60 * 0.514444, 0.0, 0.0};


    //This for loop will run through all of the dt's we want to simulate
    for (int k = 0; k<4;k++){

        //Sets the current dt
        double dt = dts[k];

        //Calculates the number of time speps it will take to go from 0 to 60 with the current step size
        int steps = static_cast<int>(tmax / dt);


        //Makes a vecotr object for the state vector
        std::vector<std::vector<double>> state_vectors;

        //Makes a vecotr object for the Derivative state vector
        std::vector<std::vector<double>> Derivative_state_vectors;

        //Makes a vecotr object for the PQR
        std::vector<std::vector<double>> PQR;

        //Basically appends the initial state vector to the state vector variable that will keep track of all of the states at every time
        state_vectors.emplace_back(std::vector<double>(Initial_State_Vector, Initial_State_Vector + 9));

        //Basically appends the initial Derivative state vector to the Derivative state vector variable that will keep track of all of the states at every time
        Derivative_state_vectors.emplace_back(std::vector<double>(Initial_Derivative_State_Vector, Initial_Derivative_State_Vector + 9));

        //Basically appends the PQR to the PQR variable that will keep track of all of the PQRs at every time
        PQR.emplace_back(std::vector<double>(Initial_PQR, Initial_PQR + 3));

        //creates the time vector
        std::vector<double> time_arr;

        //Sets  the first time as 0
        time_arr.push_back(0.0);

        //Initilizes the phi,theta, and psi values from the initial state vector
        double phi = Initial_State_Vector[0];
        double theta = Initial_State_Vector[1];
        double psi = Initial_State_Vector[2];

        //Creates the DCM matrix variable and starts it out as the identity matrix
        double DCM[3][3] = {
            {1.0, 0.0, 0.0},
            {0.0, 1.0, 0.0},
            {0.0, 0.0, 1.0}
        };

        //R1 rotation using phi
        double R_x[3][3] = {
            {1,          0,           0},
            {0, std::cos(phi), -std::sin(phi)},
            {0, std::sin(phi),  std::cos(phi)}
        };


        //Sets a temp variable to store the intermediate DCM
        double tempDCM1[3][3];

        //Multiplies the R1 and current DCM and stores it in the temp variable
        multiplyMatrices(R_x, DCM, tempDCM1);

        //Copies the temp value back to the DCM
        for(int i=0;i<3;i++) {
            for(int j=0;j<3;j++) {
                DCM[i][j] = tempDCM1[i][j];
            }
        }

        //R2 rotation using theta
        double R_y[3][3] = {
            {std::cos(theta), 0, std::sin(theta)},
            {0,               1,              0},
            {-std::sin(theta), 0, std::cos(theta)}
        };

        //Sets a temp variable to store the intermediate DCM
        double tempDCM2[3][3];

        //Multiplies the R2 and current DCM and stores it in the temp variable
        multiplyMatrices(R_y, DCM, tempDCM2);

        //Copies the temp value back to the DCM
        for(int i=0;i<3;i++) {
            for(int j=0;j<3;j++) {
                DCM[i][j] = tempDCM2[i][j];
            }
        }

        //R1 rotation using psi
        double R_z[3][3] = {
            {std::cos(psi), -std::sin(psi), 0},
            {std::sin(psi),  std::cos(psi), 0},
            {0,               0,            1}
        };


        //Sets a temp variable to store the intermediate DCM
        double tempDCM3[3][3];

        //Multiplies the R3 and current DCM and stores it in the temp variable
        multiplyMatrices(R_z, DCM, tempDCM3);
        
        //Copies the temp value back to the DCM
        for(int i=0;i<3;i++) {
            for(int j=0;j<3;j++) {
                DCM[i][j] = tempDCM3[i][j];
            }
        }

        

        //This will run though all of the time steps
        for(int n = 0; n < steps; ++n){

            //Sets the current time
            double time = n * dt;


            //Finds the angular rates at the current time
            AngularRates Rates = Find_Angular_Rates(time);

            //Saves the angular rates
            double Current_PQR[3] = {Rates.p,Rates.q,Rates.r};     
            PQR.emplace_back(std::vector<double>(Current_PQR, Current_PQR + 3));
                

            //Obtains the current state vector
            std::vector<double>& xn = state_vectors[n];

            //Finds K1 which contains xdot and Cdot
            Derivatives k1 = Diff_EQs(xn.data(), DCM, V_body, time);

            //Preps the x_k1 vector to be used to find k2
            double x_k1[9];
            for(int i=0;i<9;i++) {
                x_k1[i] = xn[i] + k1.xdot[i] * dt / 2.0;
            }

            //Preps the DCM_k1 vector to be used to find k2
            double DCM_k1[3][3];
            for(int i=0;i<3;i++) {
                for(int j=0;j<3;j++) {
                    DCM_k1[i][j] = DCM[i][j] + k1.Cdot[i][j] * dt / 2.0;
                }
            }

            //Finds K2 which contains xdot and Cdot
            Derivatives k2 = Diff_EQs(x_k1, DCM_k1, V_body, time + dt / 2.0);

            //Preps the x_k2 vector to be used to find k3
            double x_k2[9];
            for(int i=0;i<9;i++) {
                x_k2[i] = xn[i] + k2.xdot[i] * dt / 2.0;
            }

            //Preps the DCM_k2 vector to be used to find k3
            double DCM_k2[3][3];
            for(int i=0;i<3;i++) {
                for(int j=0;j<3;j++) {
                    DCM_k2[i][j] = DCM[i][j] + k2.Cdot[i][j] * dt / 2.0;
                }
            }

            //Finds K3 which contains xdot and Cdot
            Derivatives k3 = Diff_EQs(x_k2, DCM_k2, V_body, time + dt / 2.0);

        //Preps the x_k3 vector to be used to find k4
            double x_k3[9];
            for(int i=0;i<9;i++) {
                x_k3[i] = xn[i] + k3.xdot[i] * dt;
            }

            //Preps the DCM_k3 vector to be used to find k4
            double DCM_k3[3][3];
            for(int i=0;i<3;i++) {
                for(int j=0;j<3;j++) {
                    DCM_k3[i][j] = DCM[i][j] + k3.Cdot[i][j] * dt;
                }
            }

            //Finds K4 which contains xdot and Cdot
            Derivatives k4 = Diff_EQs(x_k3, DCM_k3, V_body, time + dt);

            //Combines K1 xdot,K2 xdot,K3 xdot,K4 xdot to find the xdot
            double xdot[9];
            for(int i=0;i<9;i++) {
                xdot[i] = (k1.xdot[i] + 2.0 * k2.xdot[i] + 2.0 * k3.xdot[i] + k4.xdot[i]) / 6.0;
            }

            //Saves the Derivative_state_vectors
            Derivative_state_vectors.emplace_back(std::vector<double>(xdot, xdot + 9));
            

            //Combines K1 Cdot,K2 Cdot,K3 Cdot,K4 Cdot to find the Cdot
            double Cdot[3][3];
            for(int i=0;i<3;i++) {
                for(int j=0;j<3;j++) {
                    Cdot[i][j] = (k1.Cdot[i][j] + 2.0 * k2.Cdot[i][j] + 2.0 * k3.Cdot[i][j] + k4.Cdot[i][j]) / 6.0;
                }
            }

            //This will use Cdot to update the DCM
            for(int i=0;i<3;i++) {
                for(int j=0;j<3;j++) {
                    DCM[i][j] += Cdot[i][j] * dt;
                }
            }

            //This will use xdot to find the new state vector
            double x_new[9];
            for(int i=0;i<9;i++) {
                x_new[i] = xn[i] + xdot[i] * dt;
                //This will look at the euler angles and set them between -pi and pi
                if(i < 3) {
                    x_new[i] = Adjust_Angle(x_new[i]);
                }
            }

            
            //Adds the new state vector to the current position in the state vector 
            state_vectors.emplace_back(std::vector<double>(x_new, x_new + 9));

            //sets the time for the state vector we just predicted
            time_arr.push_back(time + dt);
        }

        //Gonna be honest, Dr.GPT helped me with storing this stuff in excel

        //Sets the file names
        std::ofstream angular_rates_file("angular_rates_H_" + std::to_string(dt) + ".csv");
        std::ofstream euler_rates_file("euler_rates_H_" + std::to_string(dt) + ".csv");
        std::ofstream euler_angles_file("euler_angles_H_" + std::to_string(dt) + ".csv");
        std::ofstream velocity_file("velocity_H_" + std::to_string(dt) + ".csv");
        std::ofstream position_file("position_H_" + std::to_string(dt) + ".csv");

        //Sets the headers for the excel files
        angular_rates_file << "time,p_deg_per_sec,q_deg_per_sec,r_deg_per_sec\n";
        euler_rates_file << "time,phi_dot_deg_per_sec,theta_dot_deg_per_sec,psi_dot_deg_per_sec\n";
        euler_angles_file << "time,phi_deg,theta_deg,psi_deg\n";
        velocity_file << "time,VN_m_per_sec,VE_m_per_sec,VD_m_per_sec,V_m_per_sec\n";
        position_file << "time,PN_m,PE_m,PD_m,P_m\n";

        //This will write the data to a CSV file
        for(int n = 0; n <= steps; ++n){

            //Gets the time
            double time = time_arr[n];

            //Gets the state at this time
            std::vector<double> xn = state_vectors[n];

            //Gets the state derivative at this time
            std::vector<double> dxn = Derivative_state_vectors[n];

            //Gets the pqr at this time
            std::vector<double> c_pqr = PQR[n];

            //Saves the angular rate to a csv file
            angular_rates_file << std::fixed << std::setprecision(6)
                            << time << ","
                            << c_pqr[0] * 180.0 / PI << ","
                            << c_pqr[1] * 180.0 / PI << ","
                            << c_pqr[2] * 180.0 / PI << "\n";      
            
            //Saves the euler_rates to a csv file
            euler_rates_file << std::fixed << std::setprecision(6)
                        << time << ","
                        << dxn[0] << ","
                        << dxn[1] << ","
                        << dxn[2] << "\n";

            //Saves the euler_angles rate to a csv file
            euler_angles_file << std::fixed << std::setprecision(6)
                            << time << ","
                            << xn[0] * 180.0 / PI << ","
                            << xn[1] * 180.0 / PI << ","
                            << xn[2] * 180.0 / PI << "\n";

                    
            //Finds the magnitude of velocity
            double V_mag = std::sqrt(dxn[6]*dxn[6] + dxn[7]*dxn[7] + dxn[8] *dxn[8] );
            
            //Saves the velocity to a csv file
            velocity_file << std::fixed << std::setprecision(6)
                        << time << ","
                        << dxn[6] << ","
                        << dxn[7] << ","
                        << dxn[8] << ","
                        << V_mag << "\n";

            //Saves the position to a csv file
            double P_mag = std::sqrt(xn[6]*xn[6] + xn[7]*xn[7] + xn[8]*xn[8]);
            position_file << std::fixed << std::setprecision(6)
                        << time << ","
                        << xn[6] << ","
                        << xn[7] << ","
                        << xn[8] << ","
                        << P_mag << "\n";
        }

        //Closes the files
        angular_rates_file.close();
        euler_rates_file.close();
        euler_angles_file.close();
        velocity_file.close();
        position_file.close();

        //Prints when done with the current dt
        std::cout << "Done!, Data writen to the CSV Files \n";

    }

    return 0;
}
