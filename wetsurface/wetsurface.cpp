#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <iostream>
#include <fstream>
#include <string>

/*
instruction:
This code is used to generate nastran wetted surface information
this version can directly read the .wet file and generate the final wet surface file. 
In order to use this code, a .wet file containing wetted surface information should be input in the command window.
The .wet file should be exported from MAESTRO and copied in this code's folder. Examples are in this source code folder
You may have to adjust the .wet file a little bit if the format is different from the examples. 
You need to change the variable called "element_start"" (the starting numbering of wet element should be the next number of the last numbering of the CQUADR element in the input file) 
the blank line in the final file may have to be removed before pasting it into Nastran input file.
You also need to specify the waterline (the upper Y coordinate of wet surface) to extract full or partial wetted surface 
**
*/

int main()
{
	std::string str("element");
	std::string filename;
	std::cout << "input the file name here: " << std::endl;
	std::getline(std::cin, filename, '\n');
	//filename = "FSP_metric6_surface_refine10.wet";
	std::ifstream theFile; 
	std::ofstream interim;
	interim.open("interim.txt");
	theFile.open(filename);
	if (!theFile) {
		std::cerr << "cannot open the file" << std::endl;
		//return 1;
	}
	int i, j, a, b, c, d, e, k, l, m;
	double f, g, h;
	std::string str1;
	int count = 0;
	const int MAXELEMENT = 100000;
	//double waterline = 0.229;
	//double waterline = 0.0;
	double waterline = 0.3048;
	const int nnpe = 4; //number of node per element 
	int** ELE;
	double**coord;
	ELE = new int*[MAXELEMENT];
	coord = new double*[MAXELEMENT];
	for (i = 0; i < MAXELEMENT; i++) {
		ELE[i] = new int[nnpe + 3];
		coord[i] = new double[3];
	}
	for (i = 0; i < MAXELEMENT; i++) {
		for (j = 0; j < nnpe + 3; j++) {
			ELE[i][j] = 0;
		}
		for (j = 0; j < 3; j++) {
			coord[i][j] = 0.0;
		}
	}

	char buffer[256];
	char delimeter('"');
	while (!theFile.eof())
	{	
		theFile.getline(buffer, 256, '\n');
		//theFile.getline(buffer, delimeter);
		if (buffer[0] == 'e'&&buffer[1] == 'l'&&buffer[2] == 'e') {
			for (i = 0; i < 257; i++) {
				if (buffer[i] != delimeter) {
					interim << buffer[i];
				}
				else {
					break;
				}
			}
			interim << std::endl;
		}
	}
	interim.close();

	std::ifstream interim_op;
	interim_op.open("interim.txt");
	if (!interim_op) {
		std::cout << "cannot open the file" << std::endl;
		//return 1;
	}
	while (interim_op >> str1 >> a >> b >> c >> d >> e >> f >> g >> h >> k >> l >> m)
	//while (interim_op >> str1 >> a >> b >> c >> d >> e >> k)
	{
		if (a < 0) {
			a = -a;
		}
		if (g < waterline) {
			std::cout << str1 << a << b << c << d << e << f << std::endl;
			//ELE[count][0] = a; //element numbering
			ELE[count][0] = b; //the first local node
			ELE[count][1] = c; //the second local node
			ELE[count][2] = d; //the third local node
			ELE[count][3] = e; //the fourth local node
			coord[count][0] = f; //x coordinate of element central point
			coord[count][1] = g; //y coordinate of element central point 
			coord[count][2] = h; //z coordinate of element central point
			count += 1;
		}
	}

	int element_num = count;
	std::ofstream myfile;
	myfile.open("wetsurface_formated.txt");
	int element_start = 5540;    //to be changed by the user
	int element = element_start; //the starting numbering of wsf element
	for (i = 0; i < element_num; i++) {
		if (element >= 1e2 && element < 1e3) { //if 3 digit number
			myfile << "WETELMG" << "      "<<element<< "   QUAD4" << "                                                +" << std::endl;
		}
		else if (element >= 1e3&&element<1e4) { //if 4 digit number
			myfile << "WETELMG" << "     " << element << "   QUAD4" << "                                                +" << std::endl;
		}
		else if (element >= 1e4&&element<1e5) { //if 4 digit number
			myfile << "WETELMG" << "    " << element << "   QUAD4" << "                                                +" << std::endl;
		}
		else if (element >= 1e5) {
			myfile << "WETELMG" << "   " << element << "   QUAD4" << "                                                +" << std::endl;
		}
		else {
			std::cerr << "more element than allowed" << std::endl;
			return 1;
		}
		myfile << "+       ";
		for (j = 0; j < nnpe; j++) {
			if (ELE[i][j]<1e1) {
				myfile << "       " << ELE[i][j];
			}
			else if (ELE[i][j] >= 1e1 && ELE[i][j] < 1e2) {
				myfile << "      " << ELE[i][j];
			}
			else if (ELE[i][j] >= 1e2 && ELE[i][j] < 1e3) {
				myfile << "     " << ELE[i][j];
			}
			else if (ELE[i][j] >= 1e3 && ELE[i][j] < 1e4) {
				myfile << "    " << ELE[i][j];
			}
			else if (ELE[i][j] >= 1e4 && ELE[i][j] < 1e5) {
				myfile << "   " << ELE[i][j];
			}
			else if (ELE[i][j] >= 1e5 && ELE[i][j] < 1e6) {
				myfile << "  " << ELE[i][j];
			}
			else {
				std::cerr << "more element than allowed" << std::endl;
				return 1;
			}
		}
		myfile << std::endl;
		element += 1;
	}

	element = element_start;
	//back surface 
	double location1 = -2.4384;
	myfile << "WETSURF        1    back                                                +" << std::endl;
	//myfile << "+       ";
	count = 0;
	for (i = 0; i < element_num; i++) {
		if (abs(coord[i][2]-location1)<1e-6) {
			if ((i + element) >= 1e1 && (i + element) < 1e2) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "      " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e2 && (i + element) < 1e3) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "     " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e3 && (i + element) < 1e4) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "    " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e4 && (i + element) < 1e5) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "   " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e5 && (i + element) < 1e6) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "  " << i + element;
				count += 1;
			}
			if (count % 8 == 0) {
				myfile << "+" << std::endl;
			}
		}
	}
	myfile << std::endl;

	//left surface 
	double location2 = -4.2672;
	myfile << "WETSURF        2    left                                                +" << std::endl;
	count = 0;
	for (i = 0; i < element_num; i++) {
		if (abs(coord[i][0] - location2)<1e-6) {
			if ((i + element) >= 1e2 && (i + element) < 1e3) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "     " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e3 && (i + element) < 1e4) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "    " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e4 && (i + element) < 1e5) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "   " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e5 && (i + element) < 1e6) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "  " << i + element;
				count += 1;
			}
			if (count % 8 == 0) {
				myfile << "+" << std::endl;
			}
		}
	}
	myfile << std::endl;

	//bottom surface
	double location3 = -0.9144;
	myfile << "WETSURF        3  bottom                                                +" << std::endl;
	count = 0;
	for (i = 0; i < element_num; i++) {
		if (abs(coord[i][1] - location3)<1e-6) {
			if ((i + element) >= 1e2 && (i + element) < 1e3) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "     " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e3 && (i + element) < 1e4) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "    " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e4 && (i + element) < 1e5) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "   " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e5 && (i + element) < 1e6) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "  " << i + element;
				count += 1;
			}
			if (count % 8 == 0) {
				myfile << "+" << std::endl;
			}
		}
	}
	myfile << std::endl;

	//front surface
	double location4 = 2.4384;
	myfile << "WETSURF        4   front                                                +" << std::endl;
	count = 0;
	for (i = 0; i < element_num; i++) {
		if (abs(coord[i][2] - location4) < 1e-6) {
			if ((i + element) >= 1e2 && (i + element) < 1e3) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "     " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e3 && (i + element) < 1e4) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "    " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e4 && (i + element) < 1e5) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "   " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e5 && (i + element) < 1e6) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "  " << i + element;
				count += 1;
			}
			if (count % 8 == 0) {
				myfile << "+" << std::endl;
			}
		}
	}
	myfile << std::endl;

	//right surface
	double location5 = 4.2672;
	myfile << "WETSURF        5   right                                                +" << std::endl;
	count = 0;
	for (i = 0; i < element_num; i++) {
		if (abs(coord[i][0] - location5)<1e-6) {
			if ((i + element) >= 1e2 && (i + element) < 1e3) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "     " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e3 && (i + element) < 1e4) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "    " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e4 && (i + element) < 1e5) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "   " << i + element;
				count += 1;
			}
			if ((i + element) >= 1e5 && (i + element) < 1e6) {
				if (count % 8 == 0) {
					myfile << "+       ";
				}
				myfile << "  " << i + element;
				count += 1;
			}
			if (count % 8 == 0) {
				myfile << "+" << std::endl;
			}
		}
	}
	myfile << std::endl;

    return 0;
}









/*
FILE* fhandle;
const int MAXELEMENT = 10000;
const int nnpe = 4; //number of node per element
int i, j, a, b, c, d, e;
int ret;
int line = 0;
//const char* filename = "FSP_metric6.wet";
const char* filename = "wetsurface.txt";
fhandle = fopen(filename, "r");
if (fhandle == 0) {
printf("Cannot open >>%s<<.", filename);
exit(99);
}
printf("Reading file >>%s<<...\n", filename);

int** ELE;
ELE = new int*[MAXELEMENT];
for (i = 0; i < MAXELEMENT; i++) {
ELE[i] = new int[nnpe + 1];
}
for (i = 0; i < MAXELEMENT; i++) {
for (j = 0; j < nnpe + 1; j++) {
ELE[i][j] = 0;
}
}

int count = 0;
while (!feof(fhandle)) {
//Checks whether the end-of-File indicator associated with stream is set, returning a value different from zero if it is.
//This indicator is generally set by a previous operation on the stream that attempted to read at or past the end-of-file
//std::cout << "line: " << line << std::endl;
//line += 1;
//std::cout << "feof(fhandle) " << feof(fhandle) << std::endl;
ret=fscanf(fhandle, "element %i %i %i %i %i\n", &a, &b, &c, &d, &e); //scan from the first line of module definition
//std::cout << "ret " << ret << std::endl;
if (ret != 0) {
ELE[count][0] = a; //element numbering
ELE[count][1] = b; //the first local node
ELE[count][2] = c; //the second local node
ELE[count][3] = d; //the third local node
ELE[count][4] = e; //the fourth local node
count += 1;
std::cout << a << b << c << d << e << std::endl;
}
std::cout << "do nothing" << std::endl;
}
std::cout << "do nothing" << std::endl;
*/