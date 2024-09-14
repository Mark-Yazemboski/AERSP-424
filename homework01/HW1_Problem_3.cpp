#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <cstdio>
#include <map>
using namespace std;

class plane
{

    using Airport_Rout = std::pair<std::string, std::string>;
    using Flight_Distances = std::map<Airport_Rout, int>;

    public:
        string To;
        string From;

        plane(string from, string to)
        {
            origin = from;
            destination = to;
        }

        ~plane()
        {
            cout<<"Deleted Plane Object"<<endl;
        }

        void oporate(double dt)
        {
            
        }

        double get_pos()
        {
            return pos;
        }

        string get_origin()
        {
            return origin;
        }

        string get_destination(){
            return destination;
        }

        bool get_SCE()
        {
            return at_SCE;
        }

        double get_vel()
        {
            return vel;
        }

        void set_vel(double New_Velocity)
        {
            vel = New_Velocity;
        }
    private:
        double pos;
        double vel;
        double distance;
        bool at_SCE;
        string origin;
        string destination;
        Airport_Rout Plane_Rout = Airport_Rout(origin,destination);
    
};

int main()
{


    return 0;
}