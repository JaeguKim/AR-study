#include <iostream>
#include <fstream>
#include <string>
#include ".\include\GL\freeglut.h"
#include "NewObj.h"

using namespace std;

void NewObj::load1(char* filename)
{
	ifstream file;
	file.open(filename);

	string str;

	while (!file.eof()) //while we are still in the file
	{
		getline(file, str); //move one line down
		if (str[0] == 'v') break; //if we have a vertex line, stop
	}
	str += ' ';
	int v = 0;

	while (str.substr(0, 2) == "v ") {

		int i = 0;
		while (true)
		{
			Vertex temp;
			while (str[i] == ' ')
			{
				i++; //move a space over
			}
			i++;
			i++;
			i++;
			int j = i, k = i;
			while (str[i] != ' ') {
				i++;
				k = i;
			}
			temp.x = atof(str.substr(j, k - j).c_str());
			
			//moving on to the y coord

			while (str[i] == ' ') {
				i++;
			}

			int q = i, w = i;
			while (str[i] != ' ') {
				i++;
				w = i;
			}
			temp.y = atof(str.substr(q, w - q).c_str());

			while (str[i] == ' ') {
				i++;
			}

			int a = i, s = i;
			while (str[i] != ' ') {
				i++;
				s = i;
			}
			temp.z = atof(str.substr(a, s - a).c_str());
			vertex_vector.push_back(temp);
			break;
		}
		v++;

		getline(file, str);
		str += ' ';
	}
	
	while (true)
	{
		str += ' ';
		if (str[0] == 'f') {
			break;
		}
		getline(file, str);
	}

	int i = 0;
	int count = 1;

	while (str[0] == 'f')
	{

		printf("%d %s\n", count++, str.c_str());
		while (str[i] == 'f') i++;
		while (str[i] == ' ') i++;
		int j = i, k = i;

		while (str[i] != ' ' && str[i] != '/') {
			i++;
			k = i;
		}

		int one = atof(str.substr(j, k - j).c_str());

		while (str[i] != ' ') {
			i++;
		}

		i += 1;

		j = i;
		k = i;

		while (str[i] != ' ' && str[i] != '/') {
			i++;
			k = i;
		}

		int two = atof(str.substr(j, k - j).c_str());
		while (str[i] != ' ') {
			i++;
		}

		i += 1;

		j = i;
		k = i;

		while (str[i] != ' ' && str[i] != '/') {
			i++;
			k = i;
		}

		int three = atof(str.substr(j, k - j).c_str());
		while (str[i] != ' ') {
			i++;
		}

		i += 1;

		j = i;
		k = i;

		getline(file, str);
		str += ' ';
		i = 0;
		if (one < 0) one = -one;
		if (two < 0) two = -two;
		if (three < 0) three = -three;
		DrawInfo tdraw_info = { one,two,three };
		drawInfo_vector.push_back(tdraw_info);
	}
	//cout << "f is end" << endl;
	file.close();
}

void NewObj::load2(char* filename)
{
	ifstream file;
	file.open(filename);

	string str;

	while (!file.eof()) //while we are still in the file
	{
		getline(file, str); //move one line down
		if (str[0] == 'v') break; //if we have a vertex line, stop
	}
	str += ' ';
	int v = 0;

	while (str.substr(0, 2) == "v ") {

		int i = 0;
		//i++;
		while (true)
		{
			Vertex temp;
			while (str[i] == ' ')
			{
				i++; //move a space over
			}
			i++;
			i++;
			i++;
			int j = i, k = i;
			while (str[i] != ' ') {
				i++;
				k = i;
			}
			temp.x = (atof(str.substr(j, k - j).c_str()) - 10) / 3;
			
			//moving on to the y coord

			while (str[i] == ' ') {
				i++;
			}

			int q = i, w = i;
			while (str[i] != ' ') {
				i++;
				w = i;
			}
			temp.y = (atof(str.substr(q, w - q).c_str()) - 10) / 3;

			while (str[i] == ' ') {
				i++;
			}

			int a = i, s = i;
			while (str[i] != ' ') {
				i++;
				s = i;
			}
			temp.z = (atof(str.substr(a, s - a).c_str()) - 10) / 3;
			vertex_vector.push_back(temp);
			break;
		}
		v++;

		getline(file, str);
		str += ' ';
	}
	
	while (true)
	{
		str += ' ';
		if (str[0] == 'f') {
			break;
		}
		getline(file, str);
	}

	int i = 0;
	int count = 1;

	while (str[0] == 'f')
	{
		//printf("%d %s\n", count++, str.c_str());
		while (str[i] == 'f') i++;
		while (str[i] == ' ') i++;
		int j = i, k = i;

		while (str[i] != ' ' && str[i] != '/') {
			i++;
			k = i;
		}

		int one = atof(str.substr(j, k - j).c_str());

		while (str[i] != ' ') {
			i++;
		}

		i += 1;

		j = i;
		k = i;

		while (str[i] != ' ' && str[i] != '/') {
			i++;
			k = i;
		}

		int two = atof(str.substr(j, k - j).c_str());
		while (str[i] != ' ') {
			i++;
		}

		i += 1;

		j = i;
		k = i;

		while (str[i] != ' ' && str[i] != '/') {
			i++;
			k = i;
		}

		int three = atof(str.substr(j, k - j).c_str());
		while (str[i] != ' ') {
			i++;
		}

		i += 1;

		j = i;
		k = i;
	
		getline(file, str);
		str += ' ';
		i = 0;
		if (one < 0) one = -one;
		if (two < 0) two = -two;
		if (three < 0) three = -three;
		DrawInfo tdraw_info = { one,two,three };
		drawInfo_vector.push_back(tdraw_info);
	}
	//cout << "f is end" << endl;
	file.close();
}

void NewObj::draw1(char *filename)
{
	//cout << "Start Drawing" << endl;
	glColor3f(1.0f, 0.5f, 0.5f);
	for (int i = 0; i < drawInfo_vector.size(); i++) {
		glBegin(GL_TRIANGLES);
		glVertex3d(vertex_vector[drawInfo_vector.at(i).one - 1].x, vertex_vector[drawInfo_vector.at(i).one - 1].y, vertex_vector[drawInfo_vector.at(i).one - 1].z);
		glVertex3d(vertex_vector[drawInfo_vector.at(i).two - 1].x, vertex_vector[drawInfo_vector.at(i).two - 1].y, vertex_vector[drawInfo_vector.at(i).two - 1].z);
		glVertex3d(vertex_vector[drawInfo_vector.at(i).three - 1].x, vertex_vector[drawInfo_vector.at(i).three - 1].y, vertex_vector[drawInfo_vector.at(i).three - 1].z);
		glEnd();
	}
}


void NewObj::draw2(char *filename)
{
	//cout << "start draw" << endl;
	glColor3f(1.0f, 0.5f, 0.5f);
	for (int i = 0; i < drawInfo_vector.size(); i++) {
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3d(vertex_vector[drawInfo_vector.at(i).one - 1].x, vertex_vector[drawInfo_vector.at(i).one - 1].y, vertex_vector[drawInfo_vector.at(i).one - 1].z);
		glVertex3d(vertex_vector[drawInfo_vector.at(i).two - 1].x, vertex_vector[drawInfo_vector.at(i).two - 1].y, vertex_vector[drawInfo_vector.at(i).two - 1].z);
		glVertex3d(vertex_vector[drawInfo_vector.at(i).three - 1].x, vertex_vector[drawInfo_vector.at(i).three - 1].y, vertex_vector[drawInfo_vector.at(i).three - 1].z);
		glEnd();
	}
}