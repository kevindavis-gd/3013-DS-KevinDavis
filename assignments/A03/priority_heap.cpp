/**
* Name: Kevin Davis
* Date: Sep 29 2019
* Course: CMPS 3013
* Program: A03
* Description: This program reads from a Json file and stores the information into an array.
The values of the array is then placed into different max heaps based on specific calculations.
The 5 highest priorities of each heap will be printed.
*/
#include "json_helper.cpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <cstdlib>

using namespace std;

const static double EarthRadiusKm = 6372.8;
inline double DegreeToRadian(double angle) {
	return M_PI * angle / 180.0;
}

class Coordinate {
public:
	Coordinate(double latitude, double longitude) : myLatitude(latitude), myLongitude(longitude) {}

	double Latitude() const {
		return myLatitude;
	}

	double Longitude() const {
		return myLongitude;
	}

private:
	double myLatitude;
	double myLongitude;
};

double HaversineDistance(const Coordinate &p1, const Coordinate &p2) {
	double latRad1 = DegreeToRadian(p1.Latitude());
	double latRad2 = DegreeToRadian(p2.Latitude());
	double lonRad1 = DegreeToRadian(p1.Longitude());
	double lonRad2 = DegreeToRadian(p2.Longitude());
	double diffLa = latRad2 - latRad1;
	double doffLo = lonRad2 - lonRad1;
	double computation = asin(sqrt(sin(diffLa / 2) * sin(diffLa / 2) + cos(latRad1) * cos(latRad2) * sin(doffLo / 2) * sin(doffLo / 2)));
	return 2 * EarthRadiusKm * computation;
}

struct Animal {
	string animal_name;
	long date;
	double latitude;
	double longitude;
	float adjuster;
	float priority;
	bool validated;
	string version;

	Animal() {
		animal_name = "";
		date = 0;
		latitude = 0.0;
		longitude = 0.0;
		adjuster = 0.0;
		validated = 0;
		version = "";
		priority = 0;
	}

	Animal(string name, long _date, double lat, double lon, float pri, bool val, string ver) {
		animal_name = name;
		date = _date;
		latitude = lat;
		longitude = lon;
		adjuster = pri;
		validated = val;
		version = ver;
	}

	Animal(json j) {
		animal_name = j["animal_name"];
		date = j["date"];
		latitude = j["latitude"];
		longitude = j["longitude"];
		adjuster = j["adjuster"];
		validated = j["validated"];
		version = j["version"];
		Coordinate D1(latitude, longitude);
		Coordinate D2(33.9137, -98.4934);

		//Finding the priority
		priority = (6372.8 - HaversineDistance(D1, D2)) * adjuster / animal_name.length();
		// invert if validated
		if (validated == true)
			priority *= -1;
	}
};

class AnimalHelper {
private:
	Animal * *Animals;
	JsonHelper *J;
	json obj;
	int size;

public:
	AnimalHelper(string filename) {
		J = new JsonHelper(filename);
		size = J->getSize();
		Animals = new Animal *[size];
		for (int i = 0; i < size; i++) {
			obj = J->getNext();
			Animals[i] = new Animal(obj);
		}
	}

	void PrintAnimals() {

		for (int i = 0; i < size; i++) {
			cout << Animals[i]->animal_name << " "
				<< Animals[i]->latitude     << " "
				<< Animals[i]->longitude   << " "
				<< Animals[i]->date         << " "
				<< Animals[i]->adjuster     << " "
				<< Animals[i]->validated    << " "
				<< Animals[i]->version      <<  " "
				<< Animals[i]->priority << endl;
		}

	}
	//pass in the iteration from a for loop
	int GetDate(int pos)
	{
		return Animals[pos]->date;
	}

	int GetSize()
	{
		return size;
	}

	Animal GetAnimal(int i)
	{
		Animal temp;

		temp.animal_name = Animals[i]->animal_name;
		temp.latitude = Animals[i]->latitude;
		temp.longitude = Animals[i]->longitude;
		temp.date = Animals[i]->date;
		temp.adjuster = Animals[i]->adjuster;
		temp.validated = Animals[i]->validated;
		temp.version = Animals[i]->version;
		temp.priority = Animals[i]->priority;

		return temp;
	}
};
//******************************************************

class Heap {
private:
	Animal * Anim; // Pointer to allocate dynamic array
	int Next;     // Next available location
	int MaxSize;  // Max size since were using array
	int HeapSize; // Actual number of items in the array.

	void BubbleUp(int i) {
		int p = Parent(i);
		while (p > 0 && Anim[i].priority > Anim[p].priority) {
			Swap(i, p);
			i = p;
			p = Parent(i);
		}
	}

	void BubbleDown(int i) {
		int c = PickChild(i);

		while (c > 0) {
			if (Anim[i].priority < Anim[c].priority) {
				Swap(i, c);
				i = c;
				c = PickChild(i);
			}
			else {
				c = -1;
			}
		}
	}

	void Swap(int p, int i) {
		Animal temp = Anim[p];
		Anim[p] = Anim[i];
		Anim[i] = temp;
	}

	int Parent(int i) {
		return int(i / 2);
	}

	int LeftChild(int i) {
		return i * 2;
	}

	int RightChild(int i) {
		return i * 2 + 1;
	}

	int PickChild(int i) {
		if (RightChild(i) >= Next) {    //No right child
			if (LeftChild(i) >= Next) { //No left child
				return -1;
			}
			else { //you have a left no right
				return LeftChild(i);
			}
		}
		else {
			//right child exists
			if (Anim[RightChild(i)].priority > Anim[LeftChild(i)].priority) {
				return RightChild(i);
			}
			else {
				return LeftChild(i);
			}
		}
	}

public:
	Heap(int size) {
		Anim = new Animal[size];
		Next = 1;
		MaxSize = size;
		HeapSize = 0;
	}

	void Insert(Animal x) {
		Anim[Next] = x;
		BubbleUp(Next);
		Next++;
		HeapSize++;
	}

	Animal Extract() {

		if (Empty()) {
			cout << "ERROR Heap Empty!";
			exit(0);
		}

		Animal retval = Anim[1];
		Anim[1] = Anim[--Next];
		HeapSize--;

		if (HeapSize > 1) {
			BubbleDown(1);
		}

		return retval;
	}

	void PrintHeap() {
		for (int i = 1; i < Next; i++) {
			cout << Anim[i].animal_name << " ";
			cout << Anim[i].latitude << " ";
			cout << Anim[i].longitude << " ";
			cout << Anim[i].date << " ";
			cout << Anim[i].adjuster << " ";
			cout << Anim[i].validated << " ";
			cout << Anim[i].version << " ";
			cout << Anim[i].priority << " ";
			cout << endl;
		}
		cout << endl;
	}

	int Size() {
		return Next - 1;
	}

	bool Empty() {
		return Next == 1;
	}

	void Heapify(int *A, int size) {
		int i = size / 2;
		// H = A;
		// Next = size;
		// HeapSize = size - 1;

		for (int j = i; j >= 1; j--) {
			BubbleDown(j);
		}
	}
};


//*************************************** MAIN *******************************.

int main(int argc, char *argv[]) 
{

	// create a vector that can hold Heaps(vectors) that can hold animals
	vector <Heap>  holder;
	AnimalHelper AH("animals.json");
	Animal get;
	ofstream output;
	output.open("Buckets.txt");

	int number_of_buckets;

	if (argc >= 2)
	{
		//convert char from argv[1] to int
		number_of_buckets = atoi(argv[1]);
		cout << "Command line reads: " << number_of_buckets  << endl;
	}
	else
	{
		number_of_buckets = 3;
		cout << "Buckets not set, Default to 3\n";
		system("pause");
	}

	//create certain amount of heaps
	for (int x = 0; x < number_of_buckets; x++)
	{
		// create a heap that can hold 1005 elements
		//needs this many if 1 bucket is used
		Heap H(1005);
		//push that heap onto the holder vector
		holder.push_back(H);
	}

	//loop through the entire array
	for (int x = 0; x < AH.GetSize(); x++)
	{	//calculate which bucket the data will be entered into
		int Bucket = abs(AH.GetDate(x)) % number_of_buckets;
		get = AH.GetAnimal(x);
		//call the insert method from the specified bucket
		holder[Bucket].Insert(get);
	}

	//loop through each bucket
	for (int x = 0; x < number_of_buckets; x++)
	{	
		output << "Heap " << x << endl;
		output << "======\n";
		// loop through each item in the bucket
		for (int y = 0; y < 5; y++)
		{
			//extract the animal and save it in val
			Animal val = holder[x].Extract();
			output << val.animal_name << " ";
			output << val.priority << " ";
			output << endl;
		}
		output << "==============================\n";
		output << endl << endl;
	}
	output.close();

	cout << "Buckets Complete\n";
	system("pause");
	return 0;
}