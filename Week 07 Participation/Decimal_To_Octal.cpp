#include <iostream> 
#include <string> 
#include<math.h>
using namespace std; 

string Decimal_to_Octal(int Decimal){
    string Octal_Number;
    int Length_Base_8_Number = log(Decimal) / log(8);
    for (int i = Length_Base_8_Number;i>=0;i--){
        int Digit = Decimal / pow(8, i);
        Octal_Number += to_string(Digit);
        Decimal -= Digit*pow(8,i);
    }
    return Octal_Number;
}

int main(){
    int User_Dec;
    cout<<"Enter a Decimal Number"<<endl;
    cin>>User_Dec;


    cout<<"Your Number In Decimal Is: " << Decimal_to_Octal(User_Dec)<<endl;

}