//Imports all of the nessesary libraries
#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <cstdio>
#include <map>
#include <utility>
using namespace std;

//Plane class
class Plane
{


    //Public section
    public:

        //Initializes the container stuff in here so I can set a master dictionary outside of the class that it can reference.
        using Airport_Rout = std::pair<std::string, std::string>;
        using Flight_Distances = std::map<Airport_Rout, int>;
        static Flight_Distances Routs;

        //This is the section where when you make a plane object, this stuff happens
        Plane(string from, string to)
        {
            //Sets the to and from to the origion and destination
            origin = from;
            destination = to;

            //Makes a variable called plane rout that stores the to and from destination and can be used to find the distance within the dictionary
            Airport_Rout Plane_Rout = Airport_Rout(origin,destination);

            //Checks to see if the plane rout is in the Routs dictionary and if it is it will set the distance to the value in the dict and if not it will get set to -1
            if (Routs.find(Plane_Rout) != Routs.end()) {
                distance = Routs[Airport_Rout(origin, destination)];
            } else {
                distance = -1; 
            }

            //Sets the pos vel and atSCE to zero per the instructions
            pos = 0;
            vel = 0;
            at_SCE = 0;

            //Prints out that a plane was created and where in memory its stored
            std::cout << "Plane Created at " << this << std::endl;

            //Prints where the plane is going and a little silly message
            cout << "Thank you for flying with us, this plane will be going from "<< origin << " to " << destination <<"."<<endl;
            cout << "Lucky for you this plane is not a Boeing... Unlucky for you, this plane has no pilot and is 100 lines of code." << endl;
        }

        //This is the deconstructor
        ~Plane()
        {
            cout<<"New door created.... Plane Destroyed."<<endl;
        }

        //The oporate function
        void oporate(double dt)
        {
            //Checks to make sure the dt is positive
            if (dt<0){
                dt = 0;
            }

            //Checks to see if the position is less than the distance. If it is it will add the vel*tim to the pos and make sure the at_SCE variable is at zero
            if (pos < distance)
            {
                pos += vel*dt/3600;
                at_SCE = 0;

            // If the position is greater or equal to the distance it will check if it was going to SCE, if so it will change at_SCE to 1
            //If it wasn't going to SCE it will switch the destination and origion. both posibilities sets the position to zero    
            }else{
                cout << "Plane has arrived at " << destination <<" from " <<  origin << "."<<endl;
                if (destination == "SCE")
                {
                    cout << "Welcome to state college... WE ARE!!!"<<endl;
                    at_SCE = 1;
                }
                string placeholder = origin;
                origin = destination;
                destination = placeholder;
                cout << "This flight will now be going from " << origin << " to " << destination << "." << endl;
                pos = 0;
            }
        }

        //Gets the position
        double get_pos()
        {
            return pos;
        }

        //Gets the origin
        string get_origin()
        {
            return origin;
        }

        //Gets the destination
        string get_destination(){
            return destination;
        }

        //Gets if we are at SCE
        bool get_SCE()
        {
            return at_SCE;
        }

        //Gets the velocity
        double get_vel()
        {
            return vel;
        }

        //Sets the velocity
        void set_vel(double New_Velocity)
        {
            if (New_Velocity < 0){
                vel = 0;
            }else{
                vel = New_Velocity;
            }
            
        }

        //Gets the distance
        double get_dist()
        {
            return distance;
        }

    //Private Section
    private:

        //Sets all of the variables and makes a new Airport rout
        double pos ;
        double vel;
        double distance;
        bool at_SCE;
        string origin;
        string destination;
        
    
};

//This is the rout map/dictionary that contains all of the routs and their corisponding distances.
Plane::Flight_Distances Plane::Routs = {
    {Plane::Airport_Rout("SCE", "PHL"), 160},
    {Plane::Airport_Rout("SCE", "ORD"), 640},
    {Plane::Airport_Rout("SCE", "EWR"), 220}
};

//Main
int main()
{
    //Makes a plane going from SCE to ORD
    Plane Plane1("SCE", "ORD");

    //Sets the plane velocity to 450 mph
    Plane1.set_vel(450);

    //Sets the time step to 15 seconds
    int timestep = 15;

    //Sets the max iterations to 1500
    int Max_Iterations = 1500;

    //Runs a for loop from 0 to the max iterations and calls the oporate function and prints the time and position
    for (int i = 0; i<Max_Iterations; i++){
        Plane1.oporate(timestep);
        cout << "Time " << timestep*i << " seconds. Position: " << Plane1.get_pos() <<" miles." << endl;
    }
    return 0;
}