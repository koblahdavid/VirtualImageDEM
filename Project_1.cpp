	//Name:David Selasi Koblah
	//Course: ECE 1305 - 002, Spring 2016
	//Project 1
	//The aim of this project is to create a virtual image as viewed from a satellite looking directly down on the DEM. The image
	//will be illuminated based on the direction and elevation of the sun.
	// I used a series of functions to ensure that each part of the program was taken care of.
	//The image produces is grayscale

	#define _USE_MATH_DEFINES
	#include <iostream>
	#include <cmath>
	#include <iomanip>
	#include <string>
	#include <fstream>
	// This line increases the stack size of the array
	#pragma comment(linker, "/STACK:16777216") 
	using namespace std;
	void readDEM(string Pe, string name, int &cl, int &rw, float &min, float &max, float &xas, float &yas, float pixels[]);
	void conver(float azi, float elev, float &x, float &y, float &z);
	void vectormath(float pixels[], int cols, int rws, float minv, float maxv, float xas, float yas, float azi, float elev, int brit[]);
	void cross(float ax, float ay, float az, float bx, float by, float bz, float &cx, float &cy, float &cz);
	void normalize(float xin, float yin, float zin, float &xnorm, float &ynorm, float &znorm);
	float dot(float x1, float y1, float z1, float x2, float y2, float z2);
	int bright(float proxi);
	void createPGM(string P2, string outp, int cols, int rows, int maxval, int pixels[]);
	int main(void)
	{
		// This declares the integer values for the header info of the .egm 
		int cols, rows; 
		float xspa, yspa, maxval, minval, azi, elev;
		float cupi[999999] = { 0 };
		 int ear[999999]={0};

		 // Pe will store E1 part of the header and inny will store the name of the file entered by the user
		string outty, inny, Pe, act; 

		cout << "This Program Will Convert DEM Files Into Digital Images" << endl;
		cout << "\n";

		//These lines will accept the file names and verify their readability 
		cout << "Enter the name of the file you want to access: ";
		cin >> inny;
		cout << "\n";
		ifstream ifs(inny+".egm");  
		if (!ifs)
		{
			cout << "Error:  can't open " << inny<< " for reading " << endl;
			return 1;  
		}
		ifs.close();

		cout << "Enter the name of the file you want to convert it into:";
		cin >> outty;
		ofstream ofs(outty+".pgm"); 
		if (!ofs)
		{
			return false; 
		}
		ofs.close();  


		cout << "Please insert azimuth and elevation (all in degrees): " << endl;
		cin >> azi >> elev;
		
		//Function call for DEM reader
		readDEM(Pe, inny, cols, rows, minval, maxval, xspa, yspa, cupi); 
		cout << "\n";

		// These lines print out the header info of the DEM 
		cout<<"Number Of Columns: "<<cols<<endl;
		cout<<"Number Of Rows: "<<rows<<endl;
		cout<<"Minimum Value: "<<minval<<"   "<<"Maximum Value: "<<maxval<<endl;
		cout<<"X-SCALING: "<<xspa<<"  "<<"Y-SCALING: "<<yspa<<endl;

		//This function contains other funtions which do the math required
		vectormath(cupi,cols,rows, minval, maxval,xspa,yspa,azi,elev,ear); 

		// The values from the vectormath function are used to generated the illuminated PGM.
		createPGM("P2",outty,cols,rows,255,ear);
		cout<<"Illumination Process Complete!"<<endl;
	}

	// This is the File Reader Function
	// Input:DEM File
	// Output:Header info("E2",cl,rw,min,max,xas,yas,elevation values)
	void readDEM(string Pe, string name, int &cl, int &rw, float &min, float &max, float &xas, float &yas, float pixels[])
	{
		ifstream reader(name+".egm");
		reader>>Pe>>cl>>rw>>min>>max>>xas>>yas;

		for (int i = 0;i < (rw*cl);i++)
		{
			reader>>pixels[i];
		}


	}

	//This is the Vector Math Function
	//Input: elevation values in an array(pixels[]), No. of columns, No. of rows, Minimum elevation value, Max elevation value
	//x-scaling, y-scaling, azimuth and elevation
	//Output: Brightness in array brit[]
	void vectormath(float pixels[], int cols, int rws, float minv, float maxv, float xas, float yas, float azi, float elev, int brit[])
	{
		
		float xs, ys,xn,yn,z1,z2,x3,y3,z3, x,y,z, xz,yz,zz, prod;
		float zx={0}; 
		float zy={0};
		
		for (int cnt = 0;cnt < (cols*rws);cnt++)
		{	// This if condition caters for the last column of the DEM
			if((((cnt+1)%cols)==0)&&(cnt!=((rws*cols)-1)))
			{zx= pixels[cnt]-pixels[cnt-1];
			zy=((pixels[cnt+cols]) )-((pixels[cnt]) );
			normalize(-1, 0, zx/xas, xn, yn, z1);
			normalize(0, yas / yas, zy / yas, xs, ys, z2);
			}
		//This if condition caters for the last row values
			else if ((cnt>=(cols*(rws-1)))&&(cnt!=((rws*cols)-1)))
			{	
				zx=((pixels[cnt + 1]) ) - ((pixels[cnt]) );
				zy=pixels[cnt]-pixels[cnt-cols];
				normalize(xas / xas, 0, zx / xas, xn, yn, z1);
				normalize(0, -1, zy/yas, xs, ys, z2);
			}	
			//This if condition caters for the very last element in the DEM file
			else if(cnt==((rws*cols)-1))
			{
			zx= pixels[cnt] - pixels[cnt - 1];
			zy= pixels[cnt] - pixels[cnt - cols];
			normalize(-1, 0, zx/xas, xn, yn, z1);
			normalize(0, -1, zy / yas, xs, ys, z2);
			}
		// All other vectors are calculated here
			else
			{
			zx = ((pixels[cnt + 1])  ) - ((pixels[cnt])  );
			zy = ((pixels[cnt + cols])  ) - ((pixels[cnt])  );
			normalize(xas / xas, 0, zx / xas, xn, yn, z1);
			normalize(0, yas / yas, zy / yas, xs, ys, z2);
			}
		
		//The cross product is calculated, and dotted with the normalized azimuth and elevation x-y-z values
			cross(xn,yn,z1,xs,ys,z2,x3,y3,z3);
			conver(azi,elev,x,y,z);
			normalize(x,y,z,xz,yz,zz);
			prod=dot(xz,yz,zz,x3,y3,z3);
			brit[cnt]=bright(prod);
		}
	}

	// Cross Product function
	//Inputs: x1, y1,z1, x2, y2, z2
	//Outputs: x3, y3, z3
	void cross(float x1, float y1, float z1, float x2, float y2, float z2, float &x3, float &y3, float &z3)
	{
		x3 = ((y1*z2) - (y2*z1));
		y3 = -1*((x1*z2) - (x2*z1));
		z3 = ((x1*y2) - (x2*y1));

	}

	//Normalize Function
	//Inputs: xin, yin, zin
	//Outputs: xnorm, ynorm, znorm
	void normalize(float xin, float yin, float zin, float &xnorm, float &ynorm, float &znorm)
	{	
		float mag=sqrt((xin*xin)+(yin*yin)+(zin*zin));
		xnorm=xin/mag;
		ynorm=yin/mag;
		znorm=zin/mag;
	}
	//Azimuth & Elevation To x-y-z Converter
	//Inputs: azi, elev
	//Output: x, y, z
	void conver(float azi, float elev, float &x, float &y, float &z)
	{	float eli,aziz;
		eli = (elev)*(M_PI/180);
		aziz = (azi)*(M_PI/180);
		x = 1 * cos(eli) * cos(-aziz);
		y= 1 * cos(eli) * sin(-aziz);
		z= 1 * sin(eli);
	}

	// Dot Product Function
	// Inputs: x1,y1, z1, x2, y2, z2
	// Output: returns the value stored in dit
	float dot(float x1, float y1, float z1, float x2, float y2, float z2) //Dot product function
	{
		float dit;
		dit = (x1*x2) + (y1*y2) + (z1*z2);
		return dit;
	}

	// Illumination Function
	//Input: proxi
	// Output: pixi
	int bright(float proxi)
	{	int pixi;
		pixi=(255 * ((proxi*0.9) + 0.1));
		return pixi;
	}

	// PGM Creator
	// Input: P2, outp, cols, rows, maxval, Array pixels[]
	// Output: PGM File is written to the directory
	void createPGM(string P2, string outp, int cols, int rows, int maxval, int pixels[])
	{
		using std::ofstream; //ofstream allows program to write to external file.
		using std::cout;
		using std::ios;

		ofstream retro(outp+".pgm");
		retro<<"P2"<<setw(5)<<cols<< setw(5)<<rows<< setw(5)<<maxval<<endl;
		for (int i = 0;i < (rows*cols);i++)
		{	
			if(pixels[i]<0)
			retro<<sqrt(pixels[i]*pixels[i])<<" ";
			else
			retro<<pixels[i]<<" ";

			if((i+1)%cols==0)
			retro<<endl;
		}
		
		
		retro.close();
	}