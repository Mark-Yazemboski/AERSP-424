// Imports all of the nessesary libraries
#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <cstdio>
using namespace std;

//Function that takes in a string and will turn it into an array of integers
void splitInputToArray(const std::string& input, int* arr, int size) {
    //Sets up the string to be split
    std::stringstream ss(input);
    std::string temp;

    // Loops through the string and will split at the camma and assign the number a space in the array
    for (int i = 0; i < size; ++i) {
        std::getline(ss, temp, ',');  
        arr[i] = std::stoi(temp);     
    }
}

//Takes an array of ints and adds them together
int Sum_Arrays(int* arr, int size){
    int Sum=0;
    //Runs through eat index of the array and adds it to a sum variable
    for (int i = 0; i <size; i++){
        Sum += arr[i];
    }
    return Sum;
}

int main() {
    //Sets all of the variables and all of the limits
    float Empty_Weight, Empty_Weight_Momment, Front_Momment_Arm, Rear_Momment_Arm, Fuel, Fuel_Weight_Per_Gal, Fuel_Tank_Momment_Arm, Baggage_Weight, Baggage_Momment_Arm;
    int Front_Seat_Passengers, Rear_Seat_Passengers;
    int Weight_Limit = 2950;
    float CG_Limit_Forward = 82.1, CG_Limit_Rear = 84.7;

    //Asks the user for the aircraft empty weight
    std::cout << "Enter the aircraft empty weight (lbs): ";
    std::cin >> Empty_Weight;

    //Asks the user for the aircraft empty weight moment
    std::cout << "Enter the aircraft empty weight moment (pounds-inches): ";
    std::cin >> Empty_Weight_Momment;

    //Asks the user for the number of front seat passengers
    std::cout << "Enter the number of front seat passengers: ";
    std::cin >> Front_Seat_Passengers;
    //Sets an array for the number of passengers in the front
    int* Front_Seat_Array = new int[Front_Seat_Passengers];

    //Asks the user for the weights of the passengers, then will split it and save it as a list of integers
    std::cout << "Enter the weight of each front seat occupant (comma-separated, e.g., 180,150): ";
    std::string Front_Seat_Passenger_Weight;
    std::cin.ignore();  
    std::getline(std::cin, Front_Seat_Passenger_Weight);
    splitInputToArray(Front_Seat_Passenger_Weight, Front_Seat_Array, Front_Seat_Passengers);

    //Asks the user for the front seat moment arm
    std::cout << "Enter the front seat moment arm (inches): ";
    std::cin >> Front_Momment_Arm;

    //Asks the user for the number of rear seat passengers
    std::cout << "Enter the number of rear seat passengers: ";
    std::cin >> Rear_Seat_Passengers;
    //Sets an array for the number of passengers in the rear
    int* Rear_Seat_Array = new int[Rear_Seat_Passengers];

    //Asks the user for the weights of the passengers, then will split it and save it as a list of integers
    std::cout << "Enter the weight of each rear seat occupant (comma-separated, e.g., 160,170): ";
    std::string Rear_Seat_Passenger_Weight;
    std::cin.ignore();
    std::getline(std::cin, Rear_Seat_Passenger_Weight);
    splitInputToArray(Rear_Seat_Passenger_Weight, Rear_Seat_Array, Rear_Seat_Passengers);

    //Asks the user for the rear seat moment arm
    std::cout << "Enter the rear seat moment arm (inches): ";
    std::cin >> Rear_Momment_Arm;

    //Asks the user for the number of gallons of usable fuel
    std::cout << "Enter the number of gallons of usable fuel: ";
    std::cin >> Fuel;

    //Asks the user for the weight per gallon of usable fuel
    std::cout << "Enter the weight per gallon of usable fuel (lbs): ";
    std::cin >> Fuel_Weight_Per_Gal;

    //Asks the user for the fuel tank moment arm
    std::cout << "Enter the fuel tank moment arm (inches): ";
    std::cin >> Fuel_Tank_Momment_Arm;

    //Asks the user for the baggage weight
    std::cout << "Enter the baggage weight (lbs): ";
    std::cin >> Baggage_Weight;

    //Asks the user for the baggage moment arm 
    std::cout << "Enter the baggage moment arm (inches): ";
    std::cin >> Baggage_Momment_Arm;


    //Adds up the weights of the passengers
    int Total_Front_Seat_Weight = Sum_Arrays(Front_Seat_Array, Front_Seat_Passengers);
    int Total_Rear_Seat_Weight = Sum_Arrays(Rear_Seat_Array, Rear_Seat_Passengers);

    //Calculates the total weight of the fuel
    float Fuel_Weight = Fuel*Fuel_Weight_Per_Gal;

    //Creates a variable to keep track of the weight of the fuel before being modified
    float Old_Fuel = Fuel_Weight;

    //Adds up all of the weights
    float Total_Weight = Empty_Weight+Total_Front_Seat_Weight+Total_Rear_Seat_Weight+Fuel_Weight+Baggage_Weight;

    //Creates all of the momments
    float Momment = Empty_Weight_Momment + Total_Front_Seat_Weight*Front_Momment_Arm + Total_Rear_Seat_Weight*Rear_Momment_Arm + Fuel*Fuel_Weight_Per_Gal*Fuel_Tank_Momment_Arm + Baggage_Weight*Baggage_Momment_Arm;
    
    //Finds the planes CG from the tip of the nose
    float CG = Momment/Total_Weight;

    //Checks to see if the weight and CG are in the correct limits
    if (Total_Weight > Weight_Limit || CG < CG_Limit_Forward || CG > CG_Limit_Rear) {
        fprintf(stdout, "\nThe aircraft is outside the weight or CG limits.\n");
        
        //If the weight and CG are not in the correct limit, this while loop will run untill they are
        while (Total_Weight > Weight_Limit || CG < CG_Limit_Forward || CG > CG_Limit_Rear) {

            //This if statment will adjust the fuel in the nessesary direction by 0.01 lbs depending in the condition that failed.
            if (Total_Weight > Weight_Limit) {
                Fuel_Weight -= 0.01;  
            } else if (CG < CG_Limit_Forward) {
                Fuel_Weight += 0.01;  
            } else if (CG > CG_Limit_Rear) {
                Fuel_Weight -= 0.01;  
            }

            //Recalculates the weight moment and CG values to be rechecked
            Total_Weight = Empty_Weight+Total_Front_Seat_Weight+Total_Rear_Seat_Weight+Fuel_Weight+Baggage_Weight;
            Momment = Empty_Weight_Momment + Total_Front_Seat_Weight*Front_Momment_Arm + Total_Rear_Seat_Weight*Rear_Momment_Arm + Fuel*Fuel_Weight_Per_Gal*Fuel_Tank_Momment_Arm + Baggage_Weight*Baggage_Momment_Arm;
            CG = Momment/Total_Weight;
        }

        //Once the fuel weight correction is done, this will see if fuel was added or taken and print the coresponding message
        float Delta_Fuel = Old_Fuel-Fuel_Weight;
        if (Delta_Fuel>0){
            fprintf(stdout, "\n%g lbs of fuel was drained from the fuel tanks.\n", Delta_Fuel);
        }else{
            fprintf(stdout, "%g lbs of fuel was added to the fuel tanks.\n", -1*Delta_Fuel);
        }

        //Prints out the final weight and CG location
        fprintf(stdout, "The new total weight is: %g lbs.\n", Total_Weight);
        fprintf(stdout, "The new CG location is: %g inches from the nose.\n", CG);
    }else{
        //If everything was within spec, this will print out the weight and the CG location
        fprintf(stdout, "The aircraft is inside the weight or CG limits.\n");
        fprintf(stdout, "The total weight is: %g lbs.\n", Total_Weight);
        fprintf(stdout, "The CG location is: %g inches from the nose.\n", CG);
    }

    

}
