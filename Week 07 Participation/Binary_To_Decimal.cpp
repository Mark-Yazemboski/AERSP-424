#include <iostream> 
#include <string> 
#include<math.h>
using namespace std; 

int Binary_To_Decimal(string Binary){
    int Decimal_Number = 0;
    int Length = Binary.length();
    for (int i = 0;i<Length;i++){
        if (Binary[Length-i-1]=='1'){
            Decimal_Number += pow(2,i);
        }
    }
    return Decimal_Number;
}

int main(){
    string User_Binary;
    cout<<"Enter a Binary Number"<<endl;
    cin>>User_Binary;

    cout<<"Your Number In Decimal Is: " << Binary_To_Decimal(User_Binary)<<endl;

}