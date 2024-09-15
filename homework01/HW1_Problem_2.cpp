//Imports all of the libraries
#include <iostream>
#include <map>
#include <string>
#include <utility>

//This makes a pair of 2 strings
using Airport_Rout = std::pair<std::string, std::string>;

//This sets a map of Airport_Rout's and a corisponding int
using Flight_Distances = std::map<Airport_Rout, int>;

int main() {
    //Creates the map
    Flight_Distances Routs;

    //adds all of the routs and distances 
    Routs[Airport_Rout("SCE", "PHL")] = 160;
    Routs[Airport_Rout("SCE", "ORD")] = 640;
    Routs[Airport_Rout("SCE", "EWR")] = 220;


    // Makes a forloop to run through all of the routs and print their to and from locations and distances
    for (const auto& entry : Routs) {
        const Airport_Rout& pair = entry.first;
        const int distance = entry.second;
        std::cout << "Distance from " << pair.first << " to " << pair.second << " is " << distance << " miles." << std::endl;
    }

}
