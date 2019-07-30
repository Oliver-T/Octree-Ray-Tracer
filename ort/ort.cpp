#include <array>
#include <math.h>
#include <iostream> 
#include <algorithm>
#include <vector>
#include <array>
#include "stdio.h"
using namespace std;

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

double isIntersect(double t, double a1, double a2, double b1, double b2) {
	double a = a1 + t * (a2 - a1);
	double b = b1 + t * (b2 - b1);

	if (a >= 0 && a <= 1 && b >= 0 && b <= 1) {
		a = floor(a * 2);
		b = floor(b * 2) * 2;
		double box = a + b;
		return box;
	}

	return 5;
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

double firstContact(double x1, double y1, double z1, double x2, double y2, double z2) {
	double contact;
	double t;

	// when x = 0
	t = (0 - x1) / (x2 - x1);
	contact = isIntersect(t, z1, z2, y1, y2);
	if (contact != 5)
	{
		return contact;
	}

	// when x = 1
	t = (1 - x1) / (x2 - x1);
	contact = isIntersect(t, z1, z2, y1, y2);
	if (contact != 5)
	{
		return contact + 4;
	}

	// when y = 0
	t = (0 - y1) / (y2 - y1);
	contact = isIntersect(t, z1, z2, x1, x2);
	if (contact != 5)
	{
		if (contact > 1)
		{
			return contact + 2;
		}
		else
		{
			return contact;
		}

	}

	// when y = 1
	t = (1 - x1) / (x2 - x1);
	contact = isIntersect(t, z1, z2, x1, x2);
	if (contact != 5)
	{
		if (contact > 1)
		{
			return contact + 4;
		}
		else
		{
			return contact + 2;
		}
	}

	// when z = 0
	t = (0 - z1) / (z2 - z1);
	contact = isIntersect(t, y1, y2, x1, x2);
	if (contact != 5)
	{
		return contact * 2;
	}

	// when z = 1
	t = (1 - z1) / (z2 - z1);
	contact = isIntersect(t, y1, y2, x1, x2);
	if (contact != 5)
	{
		contact = contact * 2 + 1;
	}

	cout << "no intersection" << endl;
	return 9;
}

int main()
{
	Ray ray;
	ray.populate(.25, .25, 0, 1, .25, .75);

	double first = firstContact(ray);
	if (first == 8)
	{
		return 0;
	}

	vector<double> path = { first };
	path = findPath(first, path, ray);
	
	for (int i = 0; i < path.size(); i++)
	{
		cout << path[i] << endl;
	}

	return 0;
		
}