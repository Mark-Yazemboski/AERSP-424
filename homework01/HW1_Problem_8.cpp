//Imports all of the necessary libraries
#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <cstdio>
#include <map>
#include <memory> 
#include <utility>
using namespace std;

// This is a way to tell C++ that the cockpit class is in the program, and allows the other classes to get compiled if they use methods from cockpit
class Cockpit;

// Makes the plane Class
class Plane {
    //The public section
    public:
        //Sets the nessesary stuff for the containers
        using Airport_Rout = std::pair<std::string, std::string>;
        using Flight_Distances = std::map<Airport_Rout, int>;
        static Flight_Distances Routs;

        //This is where plane objects are initilized, with a to and from stirng
        Plane(string from, string to) {
            //Sets the to and from string to the destination and origin
            origin = from;
            destination = to;

            //Makes a plane rout variable
            Airport_Rout Plane_Rout = Airport_Rout(origin, destination);

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
            cout << "Plane Created with a tail number " << this << "." << endl;
        }

        //This is the deconstructor
        ~Plane() {
            cout<<"New door created.... Plane Destroyed."<<endl;
        }

        //The oporate function
        void oporate(double dt) {

            //Checks to make sure the dt is positive
            if (dt<0){
                dt = 0;
            }

            //Checks to see if the position is less than the distance. If it is it will add the vel*tim to the pos and make sure the at_SCE variable is at zero
            if (pos < distance) {
                pos += vel * dt / 3600;
                at_SCE = 0;

            // If the position is greater or equal to the distance it will check if it was going to SCE, if so it will change at_SCE to 1
            //If it wasn't going to SCE it will switch the destination and origion. both posibilities sets the position to zero    
            } else {
                cout << "Plane has arrived at " << destination <<" from " << origin << ". On to the next leg." <<endl;
                if (destination == "SCE") {
                    cout<<"The plane " << this << " is at SCE."<<endl;
                    cout << "The Pilots will now switch." << endl;
                    at_SCE = 1;
                }
                string placeholder = origin;
                origin = destination;
                destination = placeholder;
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
        string get_destination() 
        { 
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
            vel = (New_Velocity < 0) ? 0 : New_Velocity; 
        }

        //Gets the distance
        double get_dist() 
        { 
            return distance; 
        }

    //Sets the private variables
    private:
        double pos;
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

// Creates the Pilot class
class Pilot {
    public:
        // Using smart pointer instead of raw pointer
        shared_ptr<Plane> my_plane;

        //This  initilization  is only for when you only give a pilots name. I think its better for this problem
        Pilot(string pilot_name) {
            name = pilot_name;
            cout<<"Pilot " << name << " with certification number " << this << " is at the gate and ready to board the plane" << endl;
        }

        //This is what runs when the pilot class is deleted
        ~Pilot() {
            cout<<"Pilot " << name << " is out of the plane." << endl;
        }

        //Gets the name of the pilot
        string get_name() 
        { 
            return name; 
        }

        //This will set the plane that the pilot is flying. if the plane is empty is will print a different message
        void Set_Plane(shared_ptr<Plane> Pilots_Plane) {
            my_plane = Pilots_Plane;
            if (!my_plane) {
                cout<<"Pilot " << name << " with certification number " << this << " is taking a break." << endl;
            } else {
                cout<<"Pilot " << name << " with certification number " << this << " is in control of plane "<< my_plane.get() <<"." << endl;
            }
        }

        //This will remove a plane that the pilot is flying and print a message
        void Remove_Plane() {
            my_plane = nullptr;
            cout<<"Pilot " << name << " with certification number " << this << " is taking a break."<< endl;
        }
    
    //Sets some private variables
    private:
        string name;
};

//This is the cockpit class and is my solution to efficiently dealing with the switching pilots
class Cockpit {
    public:

        //This will initilize a cockpit object with 2 pilots, a captain and a copilot, and an airplane
        Cockpit(shared_ptr<Pilot> Captiani, shared_ptr<Pilot> CoPiloti, shared_ptr<Plane> Cockpits_Planei) {
            Captain = Captiani;
            CoPilot = CoPiloti;
            Cockpits_Plane = Cockpits_Planei;
        }

        //This is if you wanted to chenge the pilots in the cockpit
        void Add_Pilots(shared_ptr<Pilot> Captiani, shared_ptr<Pilot> CoPiloti) {
            Captain = Captiani;
            CoPilot = CoPiloti;
        }

        //This is where the pilots will switch. first the copilot gets control of the plane, and the captain gets removed, them the copilot becomes the capain and the captain becomes the co pilot
        void Switch_Pilots() {
            CoPilot->Set_Plane(Cockpits_Plane);
            Captain->Remove_Plane();
            std::swap(Captain, CoPilot);
        }

    //Private section with some pilot and plane variables
    private:
        shared_ptr<Pilot> Captain;
        shared_ptr<Pilot> CoPilot;
        shared_ptr<Plane> Cockpits_Plane;
};



int main() {
    //sets the new pointers for the 2 pilots and plane1
    shared_ptr<Pilot> Fred = make_shared<Pilot>("Fred");
    shared_ptr<Pilot> Ted = make_shared<Pilot>("Ted");
    shared_ptr<Plane> Plane1 = make_shared<Plane>("SCE", "PHL");

    //Sets the pilots planes, with Fred flying first
    Fred->Set_Plane(Plane1);
    Ted->Set_Plane(nullptr);

    //Makes a new cockpit pointer with fred ted and plane1
    shared_ptr<Cockpit> Plane_1_Cockpit = make_shared<Cockpit>(Fred, Ted, Plane1);

    //Sets the planes velocity
    Plane1->set_vel(450);

    //Sets the time step and the max iterations
    int timestep = 15;
    int Max_Iterations = 1500;

    cout << endl;

    //Runs a for loop from 0 to the max iterations and will check where the plane is and switch pilots when nessesary
    for (int i = 0; i < Max_Iterations; i++) {
        //Calls the oporate function for the plane to update its position
        Plane1->oporate(timestep);

        //Checks to see if the plane is at SCE, if so the pilots will switch
        if (Plane1->get_SCE()) {
            Plane_1_Cockpit->Switch_Pilots();
            cout << endl;
        }
    }
    return 0;
}
