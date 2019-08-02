#include <array>
#include <cmath>
#include <iostream> 
#include <algorithm>
#include <vector>
#include <array>
#include <fstream>
#include "stdio.h"
#include <sstream>
#include <cstdlib>
#include <ctime>
using namespace std;

/*

Octree Ray Tracer - by Oliver Trad - 2019

This program finds the path of a given vector defined by 2 points in 3 dimensions through an octree of a given layer size 1 through 8.
The path is written to text file path.txt
The octree is a cube with coordinates (0, 0, 0) to (255, 255, 255).

								(255, 255, 255)
			    ____________________
			   /  3	     /  7      /|
			  /_________/_________/ |
			 /  1	   /  5	     /| |
			/_________/_________/ |7|
			|         |         | | |
			|  1      |  5      |5|/|
			|         |         | | |
			|_________|_________|/|6|
			|         |         | | /
			|  0      |  4      |4|/
			|         |         | /
z  y    	|_________|_________|/
| /			
|/____x (0, 0, 0)
		


Each box is defined by a list of numbers from it's largest parent to it's smallest child.
eg. box at (0, 0, 3) in an octree of 7 layers is defined as {0, 0, 0, 0, 0, 0, 4}

*/

/*
Octrees are defined by it's number of layers: int layers

functions:
void setLayers(int)
int getLayers()
vector<double> getBoxCoord(vector<int>)
*/
class Octree {
	public:

	//default number of layers set to 1
	int layers = 1;

	//sets the Octrees layers to the given int layer
	void setLayers(int layer) {
		layers = layer;
	}

	//returns the Octrees set amount of layers
	int getLayers() {
		return layers;
	}

	//returns the coordinates of the given box as a vector<double>
		//the coordinates of a box are defined by the corner of the box with the smallest coordinates
		//eg. in an Octree with 1 layer, box {0} is defined by the coordinates {0, 0, 0}
	vector<double> getBoxCoord(vector<int> box) {
		//initialise (x, y, z) coordinates
		double xValue = 0;
		double yValue = 0;
		double zValue = 0;
		//loop through each child of the box from largest to smallest
		for (size_t i = 0; i < box.size(); i++)
		{
			//for each direction (x, y, z), check if the parent box is the further from (0, 0, 0)
				//if so, add the distance of the child's coordinates in comparison to it's parent's coordinates
			if (box[i] > 3) {
				xValue += 128 / (pow(2, i));
			}
			if (box[i] == 2 || box[i] == 3 || box[i] == 6 || box[i] == 7) {
				yValue += 128 / (pow(2, i));
			}
			if (box[i] == 1 || box[i] == 3 || box[i] == 5 || box[i] == 7) {
				zValue += 128 / (pow(2, i));
			}
		}
		//define and return the given box's coordinates as a vector<double>
		vector<double> boxCoord = {xValue, yValue, zValue};
		return boxCoord;
	}

};

/*
Ray is the vector which travels through the octree defined by two points (x1, y1, z1), (x2, y2, z2)

functions:
void populate(double, double, double, double, double, double)
vector<double> getCoordAt(string, double)
*/
class Ray {
	public:
	
	//initialise the rays 2 defining points (x1, y1, z1), (x2, y2, z2) and variable t, x, y and z values
	double x1, y1, z1, x2, y2, z2, t, x, y, z;

	//define the ray by it's two points
	void populate(double x3, double y3, double z3, double x4, double y4, double z4) {
		x1 = x3; y1 = y3; z1 = z3; x2 = x4; y2 = y4; z2 = z4;
	}

	//returns the coordinates of the ray at any given axis and value
	vector<double> getCoordAt(string axis, double value) {
		//initialise coordinates
		vector<double> xyz;

		//check which axis is given
			//calculate the coordinates of the other axis', add them to the xyz variable in
			//order of x, y, z and return
		if (axis == "x")
		{
			t = (value - x1) / (x2 - x1);
			z = z1 + t * (z2 - z1);
			y = y1 + t * (y2 - y1);
			xyz = { y, z };
		}else if (axis == "y")
		{
			t = (value - y1) / (y2 - y1);
			x = x1 + t * (x2 - x1);
			z = z1 + t * (z2 - z1);
			xyz = { x, z };
		}else if (axis == "z")
		{
			t = (value - z1) / (z2 - z1);
			x = x1 + t * (x2 - x1);
			y = y1 + t * (y2 - y1);
			xyz = { x, y };
		}
		return xyz;
	}
};

//returns the box value of the ray entry point from any given octree layer value,
//2D coordinate and face of octree as a vector<int>. The face of octree is represented as 
//x- where x=0, x+ where x=255 and so on.
vector<int> getBoxAt(int layers, vector<double> coord, string axis) {
	//initialise total value to compare against the given coordinates
	int total = 0;
	//initialise vectors for the box
	vector<int> aBox, bBox, box;

	//for each of the two given coordinates (a, b), find their box values in 2 dimensions
	//(essentially a binary tree)
	for (int i = 0; i < layers; i++)
	{
		if (coord[0] > total + 128 / (pow(2, i)))
		{
			total += 128 / (pow(2, i));
			aBox.push_back(1);
		}
		else
		{
			aBox.push_back(0);
		}
	}
	total = 0;

	for (int i = 0; i < layers; i++)
	{
		if (coord[1] > total + 128 / (pow(2, i)))
		{
			total += 128 / (pow(2, i));
			bBox.push_back(1);
		}
		else
		{
			bBox.push_back(0);
		}
	}

	//check which side of the octree the first contact is, change the (a, b) values to 
	//match and return.
	if (axis == "x+" || axis == "x-") {
		for (int i = 0; i < layers; i++)
		{
			aBox[i] = aBox[i] * 2;
			if (axis == "x+") {
				box.push_back(aBox[i] + bBox[i]);
			}
			else {
				box.push_back(aBox[i] + bBox[i] + 4);
			}
			
		}
		return box;
	}else if (axis == "y+" || axis == "y-") {
		for (int i = 0; i < layers; i++)
		{
			aBox[i] = aBox[i] * 4;
			if (axis == "y+") {
				box.push_back(aBox[i] + bBox[i]);
			}
			else {
				box.push_back(aBox[i] + bBox[i] + 2);
			}
		}
		return box;
	}else {
		for (int i = 0; i < layers; i++)
		{
			aBox[i] = aBox[i] * 4;
			bBox[i] = bBox[i] * 2;
			box.push_back(aBox[i] + bBox[i]);
		}
		return box;
	}
}

//returns the box value of the given ray's first contact of the given octree 
vector<int> firstContact(Ray ray, Octree oct) {
	//initialise the coordinates of the ray's first contact of the octree
	vector<double> coord;

	//find the coordinates of the ray at each face of the octree
	//check whether the ray intersects the octree at each face
		//find the box value with getBoxAt() and return 
	coord = ray.getCoordAt("x", 0);
	if (coord[0] >= 0 && coord[0] < 256 && coord[1] >= 0 && coord[1] < 256) {
		return getBoxAt(oct.getLayers(), coord, "x+");
	}

	coord = ray.getCoordAt("x", 255);
	if (coord[0] >= 0 && coord[0] < 256 && coord[1] >= 0 && coord[1] < 256) {
		return getBoxAt(oct.getLayers(), coord, "x-");
	}

	coord = ray.getCoordAt("y", 0);
	if (coord[0] >= 0 && coord[0] < 256 && coord[1] >= 0 && coord[1] < 256) {
		return getBoxAt(oct.getLayers(), coord, "y+");
	}

	coord = ray.getCoordAt("y", 255);
	if (coord[0] >= 0 && coord[0] < 256 && coord[1] >= 0 && coord[1] < 256) {
		return getBoxAt(oct.getLayers(), coord, "y-");
	}

	coord = ray.getCoordAt("z", 0);
	if (coord[0] >= 0 && coord[0] < 256 && coord[1] >= 0 && coord[1] < 256) {
		return getBoxAt(oct.getLayers(), coord, "z");
	}

	vector<int> bad = { 8 };
	return bad;
}

//finds the direction of the ray to optimise the path finding algorithm
//returns as an int value from 0-7
int getSign(Ray ray, Octree oct, vector<int> first) {
	//set xyz to the coordinates of the first contact box
	vector<double> xyz = oct.getBoxCoord(first);
	//initialise vector ab to track coordinates 
	vector<double> ab;
	//initialise variables to track the direction the ray is travelling on each axis
	//by default they are set to negative
	int x = 4, y = 2, z = 1;

	//check which face of the octree the first contact is		
	if (xyz[0] == 0) {
		//set the direction of the axis the face lies on to positive if the face is at 0
		x = 0;
		//set ab to the coordinates of the other axis' further along the ray
		ab = ray.getCoordAt("x", 255);
		//check of the ray is travelling in a positive or negative direction for the axis in ab
		if (ab[0] >= xyz[1]) {
			//change x, y, z value to 0 if necessary
			y = 0;
		}
		if (ab[1] >= xyz[2]) {
			z = 0;
		}
	}else if (xyz[0] == 255) {
		ab = ray.getCoordAt("x", 0);
		if (ab[0] >= xyz[1]) {
			y = 0;
		}
		if (ab[1] >= xyz[2]) {
			z = 0;
		}
	}else if (xyz[1] == 0) {
		y = 0;
		ab = ray.getCoordAt("y", 255);
		if (ab[0] >= xyz[0]) {
			x = 0;
		}
		if (ab[1] >= xyz[2]) {
			z = 0;
		}
	}else if (xyz[1] == 255) {
		y = 0;
		ab = ray.getCoordAt("y", 0);
		if (ab[0] >= xyz[0]) {
			x = 0;
		}
		if (ab[1] >= xyz[2]) {
			z = 0;
		}
	}
	else if (xyz[2] == 0) {
		z = 0;
		ab = ray.getCoordAt("z", 255);
		if (ab[0] >= xyz[0]) {
			x = 0;
		}
		if (ab[1] >= xyz[1]) {
			y = 0;
		}
	}

	//add the values together and return 
	return x + y + z;

}

//finds and returns the path the ray takes through the octree as a vector<vector<int>>
vector<vector<int>> findPath(vector<int> current, vector<double> currentCoords, vector<vector<int>> path, Ray ray, int sign, int layers) {
	//initialise variable xyz to track coordinates
	vector<double> xyz;
	//initialise boolean variables to track if the task is complete
	bool xyzDone;
	bool done = true;

	//for each axis
	//check which direction the axis is travelling to dermine which neighbouring box along the axis to check
	if (sign < 5) {
		//check if there exists a neighbour in the current direction
		//x+
		if (currentCoords[0] + 256 / (pow(2, layers)) < 256) {
			//get the 2D coordinates of the ray at the intersection point of the current and neighbouring box
			xyz = ray.getCoordAt("x", currentCoords[0] + 256 / (pow(2, layers)));
			//check if the ray intersects the neighbour
			if (xyz[0] <= currentCoords[1] + (256 / pow(2, layers)) && xyz[0] >= currentCoords[1] &&
				xyz[1] <= currentCoords[2] + (256 / pow(2, layers)) && xyz[1] >= currentCoords[2]) {
				//change booleans to represent the tasks are incomplete
				xyzDone = false;
				done = false;
				//change the current box to the neighbouring box
				for (int i = layers - 1; i >= 0; i--)
				{
					if (!xyzDone) {
						if (current[i] < 4) {
							current[i] += 4;
							xyzDone = true;
						}
						else {
							current[i] -= 4;
						}
					}
				}
				//add the neighbouring box to the path 
				path.push_back(current);
				//change the current coordinates to the coordinates to the neighbour
				currentCoords[0] += 256 / (pow(2, layers));
			}
		}
	}
	else if (currentCoords[0] - 256 / (pow(2, layers)) >= 0) {
		//x-
		xyz = ray.getCoordAt("x", currentCoords[0]);
		if (xyz[0] <= currentCoords[1] + (256 / pow(2, layers)) && xyz[0] >= currentCoords[1] &&
			xyz[1] <= currentCoords[2] + (256 / pow(2, layers)) && xyz[1] >= currentCoords[2]) {
			xyzDone = false;
			done = false;
			for (int i = layers - 1; i >= 0; i--)
			{
				if (!xyzDone) {
					if (current[i] > 3) {
						current[i] -= 4;
						xyzDone = true;
					}
					else {
						current[i] += 4;
					}
				}
			}
			path.push_back(current);
			currentCoords[0] -= 256 / (pow(2, layers));
		}
	}

	if ((sign < 2 || sign == 4 || sign == 5)) {
		//y+
		if (currentCoords[1] + 256 / (pow(2, layers)) < 256) {
			xyz = ray.getCoordAt("y", currentCoords[1] + 256 / (pow(2, layers)));
			if (xyz[0] <= currentCoords[0] + (256 / pow(2, layers)) && xyz[0] >= currentCoords[0] &&
				xyz[1] <= currentCoords[2] + (256 / pow(2, layers)) && xyz[1] >= currentCoords[2]) {
				xyzDone = false;
				done = false;
				for (int i = layers - 1; i >= 0; i--)
				{
					if (!xyzDone) {
						if (current[i] < 2 || current[i] == 4 || current[i] == 5) {
							current[i] += 2;
							xyzDone = true;
						}
						else {
							current[i] -= 2;
						}
					}
				}
				path.push_back(current);
				currentCoords[1] += 256 / (pow(2, layers));
			}
		}
	}
	else if (currentCoords[1] - 256 / (pow(2, layers)) >= 0) {
		//y-
		xyz = ray.getCoordAt("y", currentCoords[1]);
		if (xyz[0] <= currentCoords[0] + (256 / pow(2, layers)) && xyz[0] >= currentCoords[0] &&
			xyz[1] <= currentCoords[2] + (256 / pow(2, layers)) && xyz[1] >= currentCoords[2]) {
			xyzDone = false;
			done = false;
			for (int i = layers - 1; i >= 0; i--)
			{
				if (!xyzDone) {
					if (current[i] > 5 || current[i] == 2 || current[i] == 3) {
						current[i] -= 2;
						xyzDone = true;
					}
					else {
						current[i] += 2;
					}
				}
			}
			path.push_back(current);
			currentCoords[1] -= 256 / (pow(2, layers));
		}
	}

	if ((sign == 0 || sign == 2 || sign == 4 || sign == 6)) {
		//z+
		if (currentCoords[2] + 256 / (pow(2, layers)) < 256) {
			xyz = ray.getCoordAt("z", currentCoords[2] + 256 / (pow(2, layers)));
			if (xyz[0] <= currentCoords[0] + (256 / pow(2, layers)) && xyz[0] >= currentCoords[0] &&
				xyz[1] <= currentCoords[1] + (256 / pow(2, layers)) && xyz[1] >= currentCoords[1]) {
				xyzDone = false;
				done = false;
				for (int i = layers - 1; i >= 0; i--)
				{
					if (!xyzDone) {
						if (current[i] == 0 || current[i] == 2 || current[i] == 4 || current[i] == 6) {
							current[i] += 1;
							xyzDone = true;
						}
						else {
							current[i] -= 1;
						}
					}
				}
				path.push_back(current);
				currentCoords[2] += 256 / (pow(2, layers));
			}
		}
	}
	else if (currentCoords[2] - 256 / (pow(2, layers)) >= 0) {
		//z-
		xyz = ray.getCoordAt("z", currentCoords[2]);
		if (xyz[0] <= currentCoords[0] + (256 / pow(2, layers)) && xyz[0] >= currentCoords[0] &&
			xyz[1] <= currentCoords[1] + (256 / pow(2, layers)) && xyz[1] >= currentCoords[1]) {
			xyzDone = false;
			done = false;
			for (int i = layers - 1; i >= 0; i--)
			{
				if (!xyzDone) {
					if (current[i] == 1 || current[i] == 3 || current[i] == 5 || current[i] == 7) {
						current[i] -= 1;
						xyzDone = true;
					}
					else {
						current[i] += 1;
					}
				}
			}
			path.push_back(current);
			currentCoords[2] -= 256 / (pow(2, layers));
		}
	}

	//if a new neighbour was found then the task is not finished
	if (!done)
	{
		//return the path after finding the next neighbour
		return findPath(current, currentCoords, path, ray, sign, layers);
	}
	//if no neighbour was found then the task is finished. return the complete path
	return path;
}

//prepares the data to find the path
//runs findPath() with the necessary data and returns the path
vector<vector<int>> findPath(Ray ray, Octree oct) {
	//get the number of octree layers 
	int layers = oct.getLayers();
	//find the rays first contact with the octree
	vector<int> first = firstContact(ray, oct);
	//initialise the path with the first box
	vector<vector<int>> path = { {first} };
	//find the direction the ray is travelling
	int sign = getSign(ray, oct, first);
	//get the coordinates of the first box
	vector<double> currentCoords = oct.getBoxCoord(first);

	//find and return the path
	return findPath(first, currentCoords, path, ray, sign, layers);

}

//write the path to the text file path.txt
void writeFile(vector<vector<int>> path) {

	ofstream myfile;
	myfile.open("path.txt");

	for (size_t i = 0; i < path.size(); i++)
	{
		for (size_t j = 0; j < path[0].size(); j++)
		{
			myfile << path[i][j] << " ";
		}
		myfile << endl;
	}
}

int main()
{
	string r;
	int layer;
	double x1, x2, y1, y2, z1, z2;

	ifstream input;
	input.open("input.txt");
	if (input.is_open()) {
		input >> r >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;	
	}
	input.close();

	if (r == "r")
	{
		srand(time(NULL));
		layer = rand() % 8;
		x1 = rand() % 256; x2 = rand() % 256; y1 = rand() % 256;
		y2 = rand() % 256; z1 = rand() % 256; z2 = rand() % 256;
	}
	else
	{
		stringstream geek(r);
		geek >> layer;
	}

	Octree oct;
	oct.setLayers(layer);

	Ray ray;
	ray.populate(x1, y1, z1, x2, y2, z2);

	vector<vector<int>> path = findPath(ray, oct);
	vector<vector<int>> bad = { {8} };

	if (path == bad) {
		ofstream myfile;
		myfile.open("path.txt");
		myfile << "Vector does not intersect the Octree";
	}
	else
	{
		writeFile(path);
		cout << "Path is outputted to path.txt" << endl;
	}

	return 0;		
}

