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

	vector<double> getBoxCoord(vector<double> box) {
		double xValue = 0;
		double yValue = 0;
		double zValue = 0;
		for (int i = 0; i < box.size(); i++)
		{
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
		vector<double> boxCoord = {xValue, yValue, zValue};
		return boxCoord;
	}

};


class Ray {
	public:

	double x1, y1, z1, x2, y2, z2, t, x, y, z;

	void populate(double x3, double y3, double z3, double x4, double y4, double z4) {
		x1 = x3; y1 = y3; z1 = z3; x2 = x4; y2 = y4; z2 = z4;
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

double getSign(Ray ray, Octree oct, vector<double> first) {
	vector<double> xyz = oct.getBoxCoord(first);
	vector<double> ab;
	double x = 4, y = 2, z = 1;

	if (xyz[0] == 0) {
		x = 0;
		ab = ray.getCoordAt("x", 256);
		if (ab[0] >= xyz[1]) {
			y = 0;
		}
		if (ab[1] >= xyz[2]) {
			z = 0;
		}
	}else if (xyz[1] == 0) {
		y = 0;
		ab = ray.getCoordAt("y", 256);
		if (ab[0] >= xyz[0]) {
			x = 0;
		}
		if (ab[1] >= xyz[2]) {
			z = 0;
		}
	}else if (xyz[2] == 0) {
		z = 0;
		ab = ray.getCoordAt("z", 256);
		if (ab[0] >= xyz[0]) {
			x = 0;
		}
		if (ab[1] >= xyz[1]) {
			y = 0;
		}
	}

	return x + y + z;

	
}

vector<vector<double>> findPath(vector<double> current, vector<double> currentCoords, vector<vector<double>> path, Ray ray, Octree oct) {
	vector<double> xyz;
	double sign = getSign(ray, oct, path[0]);
	bool xyzDone;
	bool done = true;
	double layers = oct.getLayers();
	
	if (sign < 5) {
		//x+
		if (currentCoords[0] + 256 / (pow(2, layers)) < 256) {
			xyz = ray.getCoordAt("x", currentCoords[0] + 256 / (pow(2, layers)));
			if (xyz[0] <= currentCoords[1] + (256 / pow(2, layers)) && xyz[0] >= currentCoords[1] &&
				xyz[1] <= currentCoords[2] + (256 / pow(2, layers)) && xyz[1] >= currentCoords[2]) {
				xyzDone = false;
				done = false;
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
				path.push_back(current);
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
		if (xyz[0] <= currentCoords[0] + (256 / pow(2, oct.layers)) && xyz[0] >= currentCoords[0] &&
			xyz[1] <= currentCoords[2] + (256 / pow(2, oct.layers)) && xyz[1] >= currentCoords[2]) {
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

	if (!done)
	{
		return findPath(current, currentCoords, path, ray, oct);
	}

	return path;
	   
}

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

vector<double> getBoxAt(double layers, vector<double> coord, string axis) {

	double total = 0;
	vector<double> aBox, bBox, box;

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
			aBox[i] = aBox[i] * 2;
			box.push_back(aBox[i] + bBox[i]);
		}
		return box;
	}

	if (axis == "y") {
		for (int i = 0; i < layers; i++)
		{
			aBox[i] = aBox[i] * 4;
			box.push_back(aBox[i] + bBox[i]);
		}
		return box;
	}

	if (axis == "z") {
		for (int i = 0; i < layers; i++)
		{
			aBox[i] = aBox[i] * 4;
			bBox[i] = bBox[i] * 2;
			box.push_back(aBox[i] + bBox[i]);
		}
		return box;
	}
}

vector<double> firstContact(Ray ray, Octree oct) {
	vector<double> coord;

	coord = ray.getCoordAt("x", 0);
	if (coord[0] >= 0 && coord[0] < 256 && coord[1] >= 0 && coord[1] < 256) {
		return getBoxAt(oct.getLayers(), coord, "x");
	}

	coord = ray.getCoordAt("y", 0);
	if (coord[0] >= 0 && coord[0] < 256 && coord[1] >= 0 && coord[1] < 256) {
		return getBoxAt(oct.getLayers(), coord, "y");
	}

	coord = ray.getCoordAt("z", 0);
	if (coord[0] >= 0 && coord[0] < 256 && coord[1] >= 0 && coord[1] < 256) {
		return getBoxAt(oct.getLayers(), coord, "z");
	}

	vector<double> bad = {8};
	return bad;
}

int main()
{
	Octree oct;
	oct.setLayers(8);

	Ray ray;
	ray.populate(0, 0, 0, 256, 256, 256);

	vector<double> first = firstContact(ray, oct);

	vector<vector<double>> path = { {first} };

	path = findPath(first, oct.getBoxCoord(first), path, ray, oct);

	for (int i = 0; i < path.size(); i++)
	{
		for (int j = 0; j < oct.getLayers(); j++)
		{
			cout << path[i][j] << " ";
		}
		cout << endl;
	}

	return 0;
		
}