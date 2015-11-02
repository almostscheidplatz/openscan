/*
 * geometry.hpp
 *
 *  Created on: Oct 31, 2015
 *      Author: Ryan Peach
 */

#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <geometry.hpp>
#include <capture.hpp>

using namespace std;
using namespace cv;

typedef vector<Point> cnt;

#define PI 3.14159265

bool allSameLength(cnt poly, int distTol);

//Test that all focus points are inside the poly
bool allInside(cnt poly, vector<Fp> fps);

template <typename F>
list<F> rotateLst(list<F> lst);
template <typename G>
vector<G> rotateVec(vector<G> vec);
cnt rotateCnt(cnt contour);

//Returns the center of a contour, or of a bunch of contours, or of a bunch of points
Point centroid(cnt contour);
Point centroid(vector<cnt> contours);
Point centroid(Cnts contours);
Point centroid(vector<Fp> fps);

double dist(Point a, Point b);

double angle(Point origin, Point a);

double angle(Point origin, Point c2, Point c3);

//Runs the polygon rules of this application, that all valid shapes are convex, all size 4 shapes have all right angles within tolerance, and optionally all sides are the same length
bool isPoly(cnt poly, int size, bool regular, double tol);
bool isRectangle(cnt poly, bool square, double tol);
bool isSquare(cnt poly, double tol);

//Finds if there is a rectangle within poly
bool hasRectangle(vector<Fp> poly, double tol);

//Returns a vector of angles for the polygon
vector<double> angles(cnt poly);

//Returns a vector of distances for the polygon
vector<double> dists(cnt poly);

//Other Functions
template <typename T>
bool contains(list lst, T item);
template <typename E>
bool contains(vector lst, E item);
template <typename G>
int index(list<G> vec, G item);

