///////////////////////////////////////////////////////////////////////////////
//
// Date:             28 October 2019
// Title:            Program 3 - Graphs Part 1
// Semester:         Fall 2019
// Course:           CMPS 3013 
// Assignment:       A05
// Author:           (Kevin Davis)
// Email:            (KevinDavis.gd@gmail.com)
// Files:            (Json.hpp, HsonFacade.hpp,cities.json,Geo.hpp)
// Description:
//       This program calculates the distance between a list of cities. 
//		 It then goes through every city and gets the Max_Degree closest cities per city.
//		 It does this by using a min heap for every city.
//		 It goes through the listin order and prints the first and last
//		 ten cities in the file and its Max-Degree closest cities.
		
//
/////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include "Heap.hpp"
#include "JsonFacade.hpp"
#include "json.hpp"
#include "Geo.hpp"
#include "map_parts.hpp"

using namespace std;

using json = nlohmann::json;

struct City {
	float longitude, latitude;
	long double Priority;
	string Name;
	City() {
		Priority = 0;
		Name = "";
	}
	City(json obj) {
		Name = obj["city"];
		latitude = obj["latitude"];
		longitude = obj["longitude"];
		Priority = 0;
	}
};
struct path {

	int type;
	double distance;

	path() {
		type = 0;
		distance = 0.0;
	}
	path(double d) {
		distance = d;
	}
};

int main(int argc, char *argv[]) {
	json obj;
	City** Cities;
	City* temp;
	string filename = "cities.json";
	JsonFacade J(filename);
	int max_degree, count = 0, inner_count=0;
	Heap <City> H(1000, false);

	ofstream out;
		out.open("output.txt");

	
	// if no input from command line default to 5
	if (argc >= 2)
	{
		//convert char from argv[1] to int
		max_degree = atoi(argv[1]);
		cout << "Command line reads: " << max_degree << endl << endl;
		system("pause");
		cout << endl << endl;
	}
	else
	{
		max_degree = 5;
		cout << "Degrees not set, Default to 5\n\n";
		system("pause");
		cout << endl << endl;
	}




	int size = J.getSize();
	Cities = new City *[size];
	// loads an array of cities with the json data called cities
	for (int i = 0; i < size; i++) {
		obj = J.getNext();
		Cities[i] = new City(obj);
	}

	//create for loop to go through every city,
	//then calculate the distance between itself and every other city
	//storing it within a min heap
	//then getting x amount of nearest cities to that city

	for (int i = 0; i < size; i++)
	{
		count++;
		if (i < 10 || i >= size - 10)
		{
			out << setfill('0') << setw(4) << count << ":" << Cities[i]->Name << endl;


			for (int j = 0; j < size; j++)
			{
				//convert lat andlong into coordinates
				Coordinate p1(Cities[i]->latitude, Cities[i]->longitude);
				Coordinate p2(Cities[j]->latitude, Cities[j]->longitude);
				//set each city's priority
				Cities[j]->Priority = HaversineDistance(p1, p2);
				H.Insert(Cities[j]);
			}
			//Empty Heap
			for (int p = 0; p < size; p++)
			{
				City* temp = H.Extract();
				//dont  print distance from self to self 
				if (p > 0 && p < max_degree + 1)
				{
					inner_count++;
					out << setfill(' ') << setw(8) << " " << inner_count << " \)"<< temp->Name << " " << temp->Priority << endl;
				}	
			}
			inner_count = 0;

			out << endl << endl;
		}
	}
	//system("pause");
	return 0;
}

