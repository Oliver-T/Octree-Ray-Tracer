#include <array>
#include <cmath>
#include <iostream> 
#include <algorithm>
#include <vector>
#include <array>
#include "stdio.h"
using namespace std;

class Octree {
	public:

	double layers;
	vector<double> currentBox;

	void setLayers(double layer) {
		layers = layer;
	}

	double getLayers() {
		return layers;
	}

};

class Ray {
	public:

	double x1, y1, z1, x2, y2, z2, t, x, y, z;

	void populate(double x3, double y3, double z3, double x4, double y4, double z4) {

		x1 = x3;
		y1 = y3;
		z1 = z3;
		x2 = x4;
		y2 = y4;
		z2 = z4;

	}

	double getSign() {
		double xSign = x1 - x2;
		double ySign = y1 - y2;
		double zSign = z1 - z2;
		if (xSign != 0 || ySign != 0 || zSign != 0) {
			if (xSign > 0) {
				if (ySign > 0) {
					if (zSign > 0) {
						return 0;
					}
					else {
						return 1;
					}
				}
				else if (zSign > 0) {
					return 2;
				}
				else
				{
					return 3;
				}
			}
		}
	}

	vector<double> getCoordAt(string axis, double value) {
		vector<double> xyz;

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

vector<double> findPath(double current, vector<double> path, Ray ray) {
	double x, y, z, box;
	vector<double> xyz;
	bool done = true;

	//x
	xyz = ray.getCoordAt("x", 0.5);

	if (xyz[0] <= 1 && xyz[0] >= 0 && xyz[1] <= 1 && xyz[1] >= 0) {
		z = floor(xyz[1] * 2);
		y = floor(xyz[0] * 2) * 2;
		box = z + y;

		if (current < 4)
		{
			if (!count(path.begin(), path.end(), current + 4) && current  == box)
			{
				current += 4;
				path.push_back(current);
				done = false;		
			}

		}
		else if (!count(path.begin(), path.end(), current - 4) && (current - 4) == box) 
		{
			current -= 4;
			path.push_back(current);
			done = false;
		}
		
	}
	
	//y
	xyz = ray.getCoordAt("y", 0.5);

	if (xyz[0] <= 1 && xyz[0] >= 0 && xyz[1] <= 1 && xyz[1] >= 0) {
		z = floor(xyz[1] * 2);
		x = floor(xyz[0] * 2) * 2;
		box = z + x;


		if (current == 0 || current == 1 || current == 4 || current == 5)
		{
			if (!count(path.begin(), path.end(), current + 2) && current == box)
			{
				current += 2;
				path.push_back(current);
				done = false;
			}
		}
		else if (!count(path.begin(), path.end(), current - 2) && (current - 2) == box)
		{
			current -= 2;
			path.push_back(current);
			done = false;
		}
	}

	//z
	xyz = ray.getCoordAt("z", 0.5);

	if (xyz[0] <= 1 && xyz[0] >= 0 && xyz[1] <= 1 && xyz[1] >= 0) {
		y = floor(xyz[1] * 2) * 2;
		x = floor(xyz[0] * 2) * 4;
		box = y + x;

		if (current == 0 || current == 2 || current == 4 || current == 6)
		{
			if (!count(path.begin(), path.end(), current + 1) && current == box)
			{
				current += 1;
				path.push_back(current);
				done = false;
			}
		}
		else if (!count(path.begin(), path.end(), current - 1) && (current - 1) == box)
		{
			current -= 1;
			path.push_back(current);
			done = false;
		}
	}

	if (!done)
	{
		return findPath(current, path, ray);
	}

	return path;

}

vector<double> getBox(double layers, vector<double> coord, string axis) {

	double total = 0;
	vector<double> aBox;
	vector<double> bBox;
	vector<double> box;

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

	if (axis == "x") {
		for (int i = 0; i < layers; i++)
		{
			aBox[i] = floor(aBox[i] * 2) * 2;
			box.push_back(aBox[i] + bBox[i]);
		}
		return box;
	}

	if (axis == "y") {
		for (int i = 0; i < layers; i++)
		{
			aBox[i] = floor(aBox[i] * 2) * 4;
			box.push_back(aBox[i] + bBox[i]);
		}
		return box;
	}

	if (axis == "z") {
		for (int i = 0; i < layers; i++)
		{
			aBox[i] = floor(aBox[i] * 2) * 4;
			bBox[i] = floor(bBox[i] * 2) * 2;
			box.push_back(aBox[i] + bBox[i]);
		}
		return box;
	}
}

vector<double> firstContact(Ray ray, Octree oct) {
	vector<double> coord;

	coord = ray.getCoordAt("x", 0);
	if (coord[0] >= 0 && coord[0] < 256 && coord[1] >= 0 && coord[1] < 256) {
		return getBox(oct.getLayers(), coord, "x");
	}

	coord = ray.getCoordAt("y", 0);
	if (coord[0] >= 0 && coord[0] < 256 && coord[1] >= 0 && coord[1] < 256) {
		return getBox(oct.getLayers(), coord, "y");
	}

	coord = ray.getCoordAt("z", 0);
	if (coord[0] >= 0 && coord[0] < 256 && coord[1] >= 0 && coord[1] < 256) {
		return getBox(oct.getLayers(), coord, "z");
	}

	vector<double> bad = {8};
	return bad;
}

double firstContact(Ray ray) {
	vector<double> coord;
	double box;

	coord = ray.getCoordAt("x", 0);
	if (coord[0] >= 0 && coord[0] <= 1 && coord[1] >= 0 && coord[1] <= 1) {
		coord[0] = floor(coord[0] * 2) * 2;
		coord[1] = floor(coord[1] * 2);
		box = coord[0] + coord[1];
		return box;
	}

	coord = ray.getCoordAt("y", 0);
	if (coord[0] >= 0 && coord[0] <= 1 && coord[1] >= 0 && coord[1] <= 1) {
		coord[0] = floor(coord[0] * 2) * 4;
		coord[1] = floor(coord[1] * 2);
		box = coord[0] + coord[1];
		return box;
	}

	coord = ray.getCoordAt("z", 0);
	if (coord[0] >= 0 && coord[0] <= 1 && coord[1] >= 0 && coord[1] <= 1) {
		coord[0] = floor(coord[0] * 2) * 4;
		coord[1] = floor(coord[1] * 2) * 2;
		box = coord[0] + coord[1];
		return box;
	}

	return 8;
}

int main()
{
	Octree oct;
	oct.setLayers(8);

	Ray ray;
	ray.populate(-.5, 0, 0, 256, 256, 256);

	vector<double> first = firstContact(ray, oct);

	for (int i = 0; i < first.size(); i++)
	{
		cout << first[i] << endl;
	}

	return 0;
		
}