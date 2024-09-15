//Imports all of the nessesary libraries
#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <cstdio>
#include <map>
using namespace std;

//Plane class
class Plane
{

    //Sets the containers for the to and from and distance data
    using Airport_Rout = std::pair<std::string, std::string>;
    using Flight_Distances = std::map<Airport_Rout, int>;

    //Public section
    public:


        //This is the section where when you make a plane object, this stuff happens
        Plane(string from, string to)
        {
            //Sets the to and from to the origion and destination
            origin = from;
            destination = to;
        }

        //This is the deconstructor
        ~Plane()
        {
            cout<<"Deleted Plane Object"<<endl;
        }

        //The oporate function
        void oporate(double dt)
        {
            
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
            vel = New_Velocity;
        }

    //Private Section
    private:

        //Sets all of the variables and makes a new Airport rout
        double pos;
        double vel;
        double distance;
        bool at_SCE;
        string origin;
        string destination;
        Airport_Rout Plane_Rout = Airport_Rout(origin,destination);
    
};

//Main
int main()
{


    return 0;
}