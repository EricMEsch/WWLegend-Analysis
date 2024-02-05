#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

//Storage for all Ge77 Events. Global variables cause im lazy and this is program consists of a singular file.
//One entry corresponds to one Ge77 Event
vector<vector<string>> AllIsotopes;
vector<vector<double>> AllNCaptureX;
vector<vector<double>> AllNCaptureY;
vector<vector<double>> AllNCaptureZ;
vector<vector<double>> AllNCaptureEnergy;
vector<vector<double>> AllNCaptureTime;

ifstream newfile;

int read_out(string filename)
{
    int counterGe = 0;
    newfile.open(filename);
    string delimiter = ";";
    string seperator = ",";
    if(newfile.is_open()){
        string s;
        //Skip first 12 useless lines
        for(int i = 0; i < 12; i++)
        {
            newfile.ignore(1000,'\n');
        }

        while(getline(newfile, s))
        {
            //Store all neutron information for one Ge77 Event
            vector<string> IsotopeNames;
            vector<double> NCaptureX;
            vector<double> NCaptureY;
            vector<double> NCaptureZ;
            vector<double> NCaptureEnergy;
            vector<double> NCaptureTime;

            //Get first entry array. Each array is seperated with seperator.
            size_t pos = 0;
            string token;
            string element;
            //First is nGe so just add to Ge counter.
            pos =s.find(seperator);
            token = s.substr(0, pos);
            counterGe = counterGe + stoi(token);
            s.erase(0,pos + delimiter.length());
            // Second is Amount which currently doesnt work so erase
            pos =s.find(seperator);
            s.erase(0,pos + delimiter.length());
            //Next is isotope names, mucho importante
            pos =s.find(seperator);
            token = s.substr(0, pos);
            s.erase(0,pos + delimiter.length());

            //Iterate through each entry of the array, seperated by delimiter. First Isotope names
            stringstream ss(token);
            while (getline(ss, element, ';')) 
            {
                // Add each value to the vector
                IsotopeNames.push_back(element);
            }

            //Second x Position
            pos =s.find(seperator);
            token = s.substr(0, pos);
            s.erase(0,pos + delimiter.length());
            ss.clear();
            ss.str(token);
            while (getline(ss, element, ';')) 
            {
                double value = stod(element);
                NCaptureX.push_back(value);
            }
            //Third y Position
            pos =s.find(seperator);
            token = s.substr(0, pos);
            s.erase(0,pos + delimiter.length());
            ss.clear();
            ss.str(token);
            while (getline(ss, element, ';')) 
            {
                double value = stod(element);
                NCaptureY.push_back(value);
            }
            //Fourth z Position
            pos =s.find(seperator);
            token = s.substr(0, pos);
            s.erase(0,pos + delimiter.length());
            ss.clear();
            ss.str(token);
            while (getline(ss, element, ';')) 
            {
                double value = stod(element);
                NCaptureZ.push_back(value);
            }
            //Fifth Energy in eV
            pos =s.find(seperator);
            token = s.substr(0, pos);
            s.erase(0,pos + delimiter.length());
            ss.clear();
            ss.str(token);
            while (getline(ss, element, ';')) 
            {
                double value = stod(element);
                NCaptureEnergy.push_back(value);
            }
            //Last time in ns
            pos =s.find(seperator);
            token = s.substr(0, pos);
            s.erase(0,pos + delimiter.length());
            ss.clear();
            ss.str(token);
            while (getline(ss, element, ';')) 
            {
                double value = stod(element);
                NCaptureTime.push_back(value);
            }
            //After one Ge77 Event is done add it to the whole entry as one array
            AllIsotopes.push_back(IsotopeNames);
            AllNCaptureX.push_back(NCaptureX);
            AllNCaptureY.push_back(NCaptureY);
            AllNCaptureZ.push_back(NCaptureZ);
            AllNCaptureEnergy.push_back(NCaptureEnergy);
            AllNCaptureTime.push_back(NCaptureTime);
        }
        //After every line is through each "All" entry is an array corresponding to one Ge77 event. 
        newfile.close();
    }
    else{
        cout << "Could not open file: " << filename << endl;
    }
    return counterGe;
}

int main(){
    
    // Open output file in corresponding directories
    ofstream outfile{ "../Output/NeutronTagger/NeutronTemp/run.txt" };
    // Read out each file for each thread into corresponding global arrays.
    int counterGe = read_out("../Output/NeutronTagger/NeutronTemp/session_nt_Score_t0.csv");
    counterGe = counterGe + read_out("../Output/NeutronTagger/NeutronTemp/session_nt_Score_t1.csv");
    counterGe = counterGe + read_out("../Output/NeutronTagger/NeutronTemp/session_nt_Score_t2.csv");
    counterGe = counterGe + read_out("../Output/NeutronTagger/NeutronTemp/session_nt_Score_t3.csv");

    int counterW = 0;
    int counterElse = 0;
    int counterModerator = 0;
    int counterSteel = 0;
    int counterArg = 0;
    int counterGd = 0;
    
    double radius = 3500; //radius of cryostat in mm
    //Now todo: Print output and count captures

    for (size_t i = 0; i < AllIsotopes.size(); ++i)
    {
        const auto& innerVector = AllIsotopes[i];
        for (size_t j = 0; j < innerVector.size(); ++j)
        {
            const auto& token = innerVector[j];
            double xPosition = AllNCaptureX[i][j];
            double yPosition = AllNCaptureY[i][j];
            double zPosition = AllNCaptureZ[i][j];
            double Energy = AllNCaptureEnergy[i][j];
            double time = AllNCaptureTime[i][j];
            if(token == "H1" || token == "O16")
            {
                outfile << "1,";
                // Check if capture is outside of cryostat so it was water. Otherwise could be moderator
                double nradiussquare = AllNCaptureX[i][j] * AllNCaptureX[i][j] + AllNCaptureY[i][j] * AllNCaptureY[i][j];
                if(nradiussquare > (radius * radius) || AllNCaptureZ[i][j] > 2500 || AllNCaptureZ[i][j] < -4500)
                {
                    counterW = counterW + 1;
                }
                else
                {
                    counterModerator = counterModerator + 1;
                }
                
            }
            else if((token == "Gd154" || token == "Gd155") || (token == "Gd156" || token == "Gd158") || (token == "Gd160"))
            {
                outfile << "0,";
                counterGd = counterGd + 1;
            }
            else if(token == "Gd157")
            {
                outfile << "3,";
                counterGd = counterGd + 1;
            }
            else if((token.find("Ar") != string::npos))
            {
                outfile << "4,";
                counterArg = counterArg + 1;
            }
            else if((token.find("Fe") != string::npos)  || (token.find("Cr") != string::npos) || (token.find("Ni") != string::npos) )
            {
                outfile << "4,";
                counterSteel = counterSteel + 1;
            }
            else 
            {
                outfile << "4,";
                counterElse = counterElse + 1;
            }
            outfile << xPosition << "," << yPosition << "," << zPosition << endl;
        }
    }
    outfile.close();
    cout << "Amount of Ge Captures: " << counterGe << endl;
    cout << "Amount of Water Captures: " << counterW << endl;
    cout << "Amount of Gd Captures: " << counterGd << endl;
    cout << "Amount of Argon Captures: " << counterArg << endl;
    cout << "Amount of Moderator Captures: " << counterModerator << endl;
    cout << "Amount of Steel Captures: " << counterSteel << endl;
    cout << "Amount of Else Captures: " << counterElse << endl;
    //ofstream outfile{ "run.mac" };
    //outfile << "/run/beamOn " << counterW + counterGd +counterArg + counterSteel + counterElse << endl;
    //outfile.close();

    return 0;
}