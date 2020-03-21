/*
 * File: Sierpinski.cpp
 * --------------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Sierpinski problem
 * on Assignment #3.
 * [TODO: extend the documentation]
 */

#include <iostream>
#include "gwindow.h"
#include "gobjects.h"
#include "simpio.h"
#include <cmath>

using namespace std;

//Constants of window size
const double windowWidth = 600;
const double windowHeight = 600;

/*If level is 0, then it is a simple triangle and we draw it with polarlines and 60degree angles.
	Else, we choose 3 points of a triangle and draw 3 fractal triangles of level-1. If level-1 is not 0,
	process is repeated.
*/
void drawTriangle(GWindow &window, int level, double length, GPoint point) {
	if(level == 0) {
		int angle = 60;
		for (int i=0; i<3; i++) {
			point = window.drawPolarLine(point,length,angle);
			angle -= 120;
		}
		return;
	} 

	GPoint p1 = point;
	GPoint p2(p1.getX() + length/2,p1.getY());
	GPoint p3(p1.getX() + length/4,p1.getY() - (length*sqrt(3.0)/4));

	drawTriangle(window,level-1,length/2,p1);
	drawTriangle(window,level-1,length/2,p2);
	drawTriangle(window,level-1,length/2,p3);
}

/*Runs the program. x0 and y0 are points, where triangles lower-left corner is placed.
	x0 and y0 are generated depending on properties of an equilateral triangle.
*/
int main() {
	GWindow window(windowWidth,windowHeight);
	int n = getInteger("Enter fractal level (recommended [0-9] ): ");
	while(n<0) {
		n = getInteger("Error. Level must be not negative : ");
	}
	double length = getReal("Enter triangle length (recommended [200-600] ): ");
	while (length <=0) {
		length = getReal("Error. Length must be positive : ");
	}
	double x0 = window.getWidth()/2.0 - length/2;
	double y0 = window.getHeight()/2.0 + sqrt(3.0)*length/4;
	GPoint point(x0,y0);
	drawTriangle(window,n,length,point);
    return 0;
}
