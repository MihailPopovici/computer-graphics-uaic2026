/* The GLUT library (GL/glut.h) already includes
   GL/glu.h and GL/gl.h .
   Functions from each library are prefixed by: gl, glu, or glut.

   In Visual Studio: create an empty CPP project, then
   extract the glut library package (containing
   glut.h, glut.lib, glut.dll - or their 32 bit versions).
   Copy them so that the .h and .lib files are in the same directory
   as your .cpp file, and the .dll is together with your .exe .
   If you choose the 32-bit version of files, make sure you're
   also building a 32-bit version of the project.

   On most GNU/Linux distro, MacOS, Codeblocks, Visual Studio Code + mingw:
   GLUT is implelmented by freeglut.
   In the case of Debian / Ubuntu and variants, the packages are
   'freeglut3' and 'freeglut3-dev'.
   Manually building the binary can be done like so:
   g++ -o binary -lGL -lGLU -lglut source.cpp
   Nota bene: GL and GLU are ALLCAPS, glut is not.
   You can also use the enclosed Makefile.
*/

#include <cstdlib>
#include <cmath>
#include <iostream>
//#include "glut.h" //MSVC local library install
#include "glut.h" //system-wide install (or compiler default path)

float circle = atan(1) * 8; // 2 * PI = TAU
float halfCircle = atan(1) * 4; // PI

int defaultW = 1000, defaultH = 1000;

int numberOfCols = 30, numberOfRows = 30;

float offset = 0.1f;

float cellSize = (2.0f - (2 * offset) ) / numberOfCols; //assuming a square window, we can use either 
                                        //the number of cols or rows, since they are the equal.
                                        //also, its basically the metric for scaling to our grid
float startPoint = -1 + offset;



unsigned char prevKey;


float ToGLCoords(float x) {
	return startPoint + x * cellSize;
    
}


void DrawGrid(int numberOfCols, int numberOfRows) {
    	glColor3f(0.9, 0.9, 0.9);
        glBegin(GL_LINES); {
            for (int x = 0; x <= numberOfCols; x++) {
            float gridX = ToGLCoords(x);
			glVertex2f(gridX, startPoint);
			glVertex2f(gridX, -startPoint);
		}
            for (int y = 0; y <= numberOfRows; y++) {
			float gridY = ToGLCoords(y);
			glVertex2f(startPoint, gridY);
			glVertex2f(-startPoint, gridY);
		}
	}
	glEnd();
}

void drawDisc(int i, int j) {
    float cx = ToGLCoords(i);
    float cy = ToGLCoords(j);


    int discSegments = 30; //cu ata mai multe, cu atat mai rotund
    float cellFraction = 0.2f;

    float radius = cellSize * cellFraction;

    glBegin(GL_TRIANGLE_FAN); {
        glVertex2f(cx, cy);
        for (int i = 0; i <= discSegments; i++) {
            float angle = i * circle / discSegments;
            glVertex2f(cx + radius * cos(angle), cy + radius * sin(angle));
        }
    }
    glEnd();
}

void DrawThickDiscs(int i, int j, int thickness = 2) {
	int half = thickness / 2;
	for (int tx = -half; tx <= half; tx++)
		for (int ty = -half; ty <= half; ty++)
			if (tx * tx + ty * ty <= half * half) //if( x^2 + y^2 <= r^2 ) is the equation of a circle, so this is a circle of points around the center of the disc, to make it thicker
				drawDisc(i + tx, j + ty);
}

void RasterLine_Octant1(int x0, int y0, int x1, int y1, int thickness) {
    const int dx = x1 - x0;
    const int dy = y1 - y0;
    
    const int dM = 2 * dy - dx;
    const int dE = 2 * dy;
    const int dNE = 2 * (dy - dx);

    int d = dM;
    int x = x0, y = y0;
    
    while (x <= x1) {
        DrawThickDiscs(x, y, thickness);
        x++;
        if (d < 0) d += dE;
        else { d += dNE; y++; }
        
    }
}

void RasterLine_Octant2(int x0, int y0, int x1, int y1, int thickness) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    int d = 2 * dx - dy;
    int dN = 2 * dx;
    int dNE = 2 * (dx - dy);

    int x = x0, y = y0;
    
    while (y <= y1) {
        DrawThickDiscs(x, y, thickness);

        if (d < 0) {
            d += dN;
        }
        else {
            d += dNE;
            x++;
        }
        y++;
    }
}

void RasterLine_Octant8(int x0, int y0, int x1, int y1, int thickness) {
    int dx = x1 - x0;
    int dy = y0 - y1;

    int d = 2 * dy - dx;
    int dE = 2 * dy;
    int dSE = 2 * (dy - dx);

    int x = x0, y = y0;

    while (x <= x1) {
        DrawThickDiscs(x, y, thickness);

        if (d < 0) {
            d += dE;
        }
        else {
            d += dSE;
            y--;
        }
        x++;
    }
}

void RasterLine_Octant7(int x0, int y0, int x1, int y1, int thickness) {
    int dx = x1 - x0;
    int dy = y0 - y1;

    int d = 2 * dx - dy;
    int dS = 2 * dx;
    int dSE = 2 * (dx - dy);

    int x = x0, y = y0;

    while (y >= y1) {
        DrawThickDiscs(x, y, thickness);

        if (d < 0) {
            d += dS;
        }
        else {
            d += dSE;
            x++;
        }
        y--;
    }
}

void RasterLine(int x0, int y0, int x1, int y1, int thickness = 2) {
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    if (dy >= 0) {
        if (dy <= dx)
            RasterLine_Octant1(x0, y0, x1, y1, thickness);
        else
            RasterLine_Octant2(x0, y0, x1, y1, thickness);
    }
    else {
        if (-dy <= dx)
            RasterLine_Octant8(x0, y0, x1, y1, thickness);
        else
            RasterLine_Octant7(x0, y0, x1, y1, thickness);
    }
}


void DrawCircle(int cx, int cy, int r, int segments = 8) {
	//glColor3f(0.2f, 0.2f, 1.0f);
    glBegin(GL_LINE_LOOP); {
        for (int i = 0; i < segments; i++)
        {
			float angle = i * circle / float(segments);

			float x = cx + r * (cos(angle));
			float y = cy + r * (sin(angle));

            glVertex2f(ToGLCoords(x), ToGLCoords(y));
            
		}
	}
	glEnd();
}


void DrawLine(int x1, int y1, int x2, int y2) {
   // glColor3f(0.1f, 0.1f, 1.0f);
    glBegin(GL_LINES); {
		glVertex2f(ToGLCoords(x1), ToGLCoords(y1));
		glVertex2f(ToGLCoords(x2), ToGLCoords(y2));
	}
	glEnd();
}

void DrawVirtualCircle(int cx, int cy, int r, int segments = 8) {
    float gcx = ToGLCoords(cx);
    float gcy = ToGLCoords(cy);
    float gr = r * cellSize; //basically the metric for scaling to our grid

    glColor3f(0.2f, 0.2f, 1.0f);
    DrawCircle(cx, cy, r, segments);


    glColor3f(0.1f, 0.1f, 0.1f);
    for (int i = 0; i < segments; i++)
    {
        float angle1 = i * circle / float(segments);
        float angle2 = (i + 1) * circle / float(segments);

        int x1 = cx + r * (cos(angle1));
        int y1 = cy + r * (sin(angle1));


        int x2 = cx + r * (cos(angle2));
        int y2 = cy + r * (sin(angle2));

        RasterLine(x1, y1, x2, y2);

    }
}


/*void RasterCircle(int cx, int cy, int r, int thickness = 2) {
    int x = 0, y = r;
    int d = 1 - r;
    int de = 3;
    int dse = -2 * r + 5;

    int half = thickness / 2;

    auto drawThick = [&](int px, int py) {
        for (int tx = -half; tx <= half; tx++)
            for (int ty = -half; ty <= half; ty++)
                if (tx * tx + ty * ty <= half * half) //if( x^2 + y^2 <= r^2 ) is the equation of a circle, so this is a circle of points around the center of the disc, to make it thicker
                drawDisc(px + tx, py + ty);
        };

    auto plot8 = [&](int x, int y) {
        drawThick(cx + x, cy + y);  
        drawThick(cx - x, cy + y);  
        drawThick(cx + x, cy - y); 
        drawThick(cx - x, cy - y); 
        drawThick(cx + y, cy + x); 
        drawThick(cx - y, cy + x);
        drawThick(cx + y, cy - x);
        drawThick(cx - y, cy - x);
        };

    plot8(x, y);
    while (x < y) {
        x++;
        if (d < 0) {
            d += de; de += 2; dse += 2;
        }
        else {
            y--; d += dse; de += 2; dse += 4;
        }
        plot8(x, y);
    }
}*/

/*
void RasterCircle_Octant2(int cx, int cy, int r, int thickness, int sign) {
	int x = 0, y = r;
	int d = 1 - r;
	int de = 3;
	int dse = -2 * r + 5;

    DrawThickDiscs(cx + x, cy + sign * y, thickness);
	
    while (x < y) {
       
		x++;
        if (d < 0) {
			d += de; de += 2; dse += 2;
		}
        else {
			y--; d += dse; de += 2; dse += 4;
		}
        
	}
    DrawThickDiscs(cx + x, cy + sign * y, thickness);
}

void RasterCircle_Octant1(int cx, int cy, int r, int thickness, int sign) {
	int x = 0, y = r;
	int d = 1 - r;
	int de = 3;
	int dse = -2 * r + 5;

    

    while (x < y) {
		DrawThickDiscs(cx + y, cy + x * sign, thickness);
		x++;
        if (d < 0) {
			d += de; de += 2; dse += 2;
		}
        else {
			y--; d += dse; de += 2; dse += 4;
		}
		
	}
}

void RasterCircle_Octant3(int cx, int cy, int r, int thickness, int sign) {
    int x = 0, y = r;
    int d = 1 - r;
    int de = 3;
    int dse = -2 * r + 5;

    while (x < y) {
        DrawThickDiscs(cx - x, cy + y * sign, thickness); 
        x++;

        if (d < 0) {
            d += de; de += 2; dse += 2;
        }
        else {
            y--; d += dse; de += 2; dse += 4;
        }
    }
}


void RasterCircle_Octant4(int cx, int cy, int r, int thickness, int sign) {
    int x = 0, y = r;
    int d = 1 - r;
    int de = 3;
    int dse = -2 * r + 5;

    while (x < y) {
        DrawThickDiscs(cx - y, cy + sign * x, thickness); 
        x++;

        if (d < 0) {
            d += de; de += 2; dse += 2;
        }
        else {
            y--; d += dse; de += 2; dse += 4;
        }
    }
}
*/

void RasterCircle_Octant2(int cx, int cy, int r, int thickness, int sign) {
    int x = 0, y = r;
    int d = 1 - r;
    int de = 3;
    int dse = -2 * r + 5;

    while (x <= y) {
        DrawThickDiscs(cx + x * sign, cy + y, thickness);

        x++;
        if (d < 0) {
            d += de;
            de += 2;
            dse += 2;
        }
        else {
            y--;
            d += dse;
            de += 2;
            dse += 4;
        }
    }
}

void RasterCircle_Octant1(int cx, int cy, int r, int thickness, int sign) {
    int x = 0, y = r;
    int d = 1 - r;
    int de = 3;
    int dse = -2 * r + 5;

    while (x <= y) {
        DrawThickDiscs(cx + sign *y, cy + x , thickness);

        x++;
        if (d < 0) {
            d += de;
            de += 2;
            dse += 2;
        }
        else {
            y--;
            d += dse;
            de += 2;
            dse += 4;
        }
    }
}

void RasterCircle_Octant7(int cx, int cy, int r, int thickness, int sign) {
    int x = 0, y = r;
    int d = 1 - r;
    int de = 3;
    int dse = -2 * r + 5;

    while (x <= y) {
        DrawThickDiscs(cx + y *sign, cy - x , thickness);

        x++;
        if (d < 0) {
            d += de;
            de += 2;
            dse += 2;
        }
        else {
            y--;
            d += dse;
            de += 2;
            dse += 4;
        }
    }
}

void RasterCircle_Octant8(int cx, int cy, int r, int thickness, int sign) {
    int x = 0, y = r;
    int d = 1 - r;
    int de = 3;
    int dse = -2 * r + 5;

    while (x <= y) {
        DrawThickDiscs(cx + x*sign, cy - y , thickness);

        x++;
        if (d < 0) {
            d += de;
            de += 2;
            dse += 2;
        }
        else {
            y--;
            d += dse;
            de += 2;
            dse += 4;
        }
    }
}

void RasterCircle(int cx, int cy, int r, int thickness) {
    
    glColor3f(0.1f, 0.1f, 1.0f);
    DrawCircle(cx, cy, r, 30);
    
    glColor3f(0.1f, 0.1f, 0.1f);
	RasterCircle_Octant1(cx, cy, r, thickness, 1);
	RasterCircle_Octant2(cx, cy, r, thickness, 1);
    RasterCircle_Octant7(cx, cy, r, thickness, 1);
    RasterCircle_Octant8(cx, cy, r, thickness, 1);

    RasterCircle_Octant1(cx, cy, r, thickness, -1);
    RasterCircle_Octant2(cx, cy, r, thickness, -1);
    RasterCircle_Octant7(cx, cy, r, thickness, -1);
    RasterCircle_Octant8(cx, cy, r, thickness, -1);

}

/*void FillCircle(int cx, int cy, int r)
{
    for(int y = -r; y <= r; y++)
		for (int x = -r; x <= r; x++)
			if (x * x + y * y <= r * r)
				DrawThickDiscs(cx + x, cy + y);
}
*/

void FillCircleLine(int cx, int cy, int x, int y) {
    for (int i = cx - x; i <= cx + x; i++) {
        drawDisc(i, cy + y);
        drawDisc(i, cy - y);
    }
}

void Display5() {

    DrawGrid(numberOfCols, numberOfRows);

    //glColor3f(0.1f, 0.1f, 0.1f);
    RasterCircle(15, 15, 8, 2);
   // for (int y = -8; y <= 8; y++) {
    //    int x = int(sqrt(64 - y * y));
    //    FillCircleLine(15, 15, x, y);
    //}

    //FillCircle(15, 15, 8);
    //DrawVirtualCircle(15, 15, 8, 9);

    //glColor3f(0.1f, 0.1f, 0.1f);
    //RasterLine(3, 3, 27, 20);


}

void Display1() {
    glColor3f(0.2, 0.15, 0.88); // blue
    glBegin(GL_LINES); { // we'll draw lines; don't forget the ';'
        glVertex2i(1, 1); // the coordinates of a vertex (2d, integer coordinates)
        glVertex2i(-1, -1);
    }
    glEnd();

    glColor3f(1, 0.1, 0.1); // red
    glBegin(GL_LINES); {
        glVertex2i(-1, 1);
        glVertex2i(1, -1);
    }
    glEnd();

    glBegin(GL_LINES); {
        glVertex2d(-0.5, 0);
        glVertex2d(0.5, 0);
    }
    glEnd();
}

void Display2() {
    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINES); { // We don't need a special Begin/End block for each line segment
        glVertex2f(1.0, 1.0); // OpenGL knows that a line segment is determined by two vertices.
        glVertex2f(0.9, 0.9);
        glVertex2f(0.8, 0.8);
        glVertex2f(0.7, 0.7);
        glVertex2f(0.6, 0.6);
        glVertex2f(0.5, 0.5);
        glVertex2f(-0.5, -0.5);
        glVertex2f(-1.0, -1.0);
    }
    glEnd();
}

void Display3() {
    glColor3f(0.9, 0.1, 0.1);
    // Search for glPointSize(4) in this file;
    // try to change it to 12.
    glBegin(GL_POINTS); {
        glVertex2f(0.9, 0.9);
        glVertex2f(0.1, 0.9);
        glVertex2f(0.1, 0.1);
        glVertex2f(0.9, 0.1);
    }
    glEnd();

    glColor3f(0.1, 0.9, 0.1);
    //an open loop:
    //  (v0,v1), (v1,v2), ..., (v_{n-2},v_{n-1})
    glBegin(GL_LINE_STRIP); {
        glVertex2f(-0.9, 0.9);
        glVertex2f(-0.1, 0.9);
        glVertex2f(-0.1, 0.1);
        glVertex2f(-0.9, 0.1);
    }
    glEnd();

    glColor3f(0.1, 0.1, 0.9);
    //a self-closing loop:
    //  (v0,v1), (v1,v2), ..., (v_{n-2},v_{n-1}), (v_{n-1},v0)
    glBegin(GL_LINE_LOOP); {
        glVertex2f(-0.9, -0.9);
        glVertex2f(-0.1, -0.9);
        glVertex2f(-0.1, -0.1);
        glVertex2f(-0.9, -0.1);
    }
    glEnd();

    //one or more rectangles, which can be filled:
    glBegin(GL_QUADS); {
        //we can change colours inside glBegin
        glColor3f(0.9, 0.5, 0.1);
        glVertex2f(0.9, -0.9);
        glVertex2f(0.1, -0.9);
        glVertex2f(0.1, -0.1);
        glVertex2f(0.9, -0.1);

        //Try to fill this!
        //(And understand why it wasn't filling in the first place!)
        glColor3f(0.9, 0.1, 0.1);
        glVertex2f(0.7, -0.3);
        glColor3f(0.1, 0.9, 0.1);
        glVertex2f(0.3, -0.3);
        glColor3f(0.1, 0.1, 0.9);
        glVertex2f(0.3, -0.7);
        glColor3f(0.9, 0.9, 0.9);
        glVertex2f(0.7, -0.7);

    }
    glEnd();
}

void Display4() {
    //Multiple triangles. Explain why one is filled, the other empty!
    glBegin(GL_TRIANGLES); {
        glColor3f(0.9, 0.1, 0.1);
        glVertex2f(0.9, 0.9);
        glVertex2f(0.2, 0.9);
        glVertex2f(0.9, 0.2);

        glColor3f(0.1, 0.1, 0.9);
        glVertex2f(0.1, 0.1);
        glVertex2f(0.1, 0.8);
        glVertex2f(0.8, 0.1);
    }
    glEnd();

    /*
      Draw a fan of triangles. We start from a root point,
      and need another to make the first line.
      Then, each point extends from that first point, and
      connects to the previous point in the line.
    */
    float rootX = -0.9, rootY = 0.9, radius = 0.9;
    int maxSteps = 30;
    glBegin(GL_TRIANGLE_FAN); {
        glColor3f(0.9, 0.1, 0.1);
        glVertex2f(rootX, rootX); //Root point.
        for (int ii = 0; ii < maxSteps; ++ii) {
            glColor3f(0.9 * ii / maxSteps, 0.1, 0.9);
            float angle = -ii / float(maxSteps) * circle / 4.0;
            glVertex2f(rootX + radius * cos(angle), rootY + radius * sin(angle));
        }
    }
    glEnd();

    rootX = 0.45;
    rootY = -0.45;
    radius = 0.45;
    maxSteps = 360;
    //A polygon, taking as many points as we give it.
    //In this case, glBegin draws a single geometric figure.
    glBegin(GL_POLYGON); {
        for (int ii = 0; ii < maxSteps; ++ii) {
            float normalisedPosition = ii / float(maxSteps);
            //Try removing the minus. And fixing the drawing.
            float angle = -normalisedPosition * circle;
            glColor3f(normalisedPosition, 0, 0);
            glVertex2f(rootX + radius * cos(angle), rootY + radius * sin(angle));
        }
    }
    glEnd();

}



void Display6() {
}

void Display7() {

}

void Display8() {

}

void init(void) {
    // The screen is cleared using the function glClear()
    // This sets the screen to the RGBA colour specified below.
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glLineWidth(3);
    glPointSize(4);
    /*
      void glPolygonMode (GLenum face, GLenum mode)
      face: the face of the polygon (according to the right-hand rule)
      GL_FRONT
      GL_BACK
      GL_FRONT_AND_BACK
      mode: GL_POINT (only draw the vertices as points)
      GL_LINE (only draw the edges)
      GL_FILL (also fill the inside pixels)
    */
    glPolygonMode(GL_FRONT, GL_LINE);
}

void Display(void) {
    std::cout << ("Call Display") << std::endl;
    // Clear the buffer. See init();
    glClear(GL_COLOR_BUFFER_BIT);

    switch (prevKey) {
    case '1':
        Display1();
        break;
    case '2':
        Display2();
        break;
    case '3':
        Display3();
        break;
    case '4':
        Display4();
        break;
    case '5':
        Display5();
        break;
    case '6':
        Display6();
        break;
    case '7':
        Display7();
        break;
    case '8':
        Display8();
        break;
    default:
        break;
    }

    //Flushes all buffers, and forces the image to be sent to the screen.
    glFlush();
}

/*
  Callback upon window reshape.
*/
void Reshape(int w, int h) {
    std::cout << "Window reshape. Width: " << w << " height: " << h << std::endl;
    /* void glViewport (GLint x, GLint y,
                        GLsizei width, GLsizei height)
       reshapes the surface onto which we draw so that it matches the inside
       of the on-desktop window. (x, y) define the lower-left pixel.
       Use (0, 0, w, h) for a full-window drawing surface.
     */

    //glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    int size = (w < h) ? w : h;
    int offX = (w - size) / 2;
    int offY = (h - size) / 2; //am scos offseturile desi mie mi se pare ca ar trebui
                                //e ceva legat de OpenGL coordinates, dar viewport lucreaza cu pixeli

    glViewport(offX, offY, size, size);
    
}

/*
  Callback upon key press. We receive the keycode, and the
  mouse cursor position.
*/
void KeyboardFunc(unsigned char key, int x, int y) {
    std::cout << "Key: " << key << " has been pressed at coordinates: ";
    std::cout << x << " x " << y << std::endl;
    //Save the last-pressed key in the global variable, so we can switch
    //display functions.
    prevKey = key;
    if (key == 27) // escape
        exit(0);
    //The proper way to ask glut to redraw the window.
    glutPostRedisplay();
}

/*
  Callback upon mouse press or release.
  The button can be:
   GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
   (and further for mousewheel and other mouse buttons)
  The state can be either GLUT_DOWN or  GLUT_UP, for
  a pressed or released button.
  (x, y) are the coordinates of the mouse.
*/
void MouseFunc(int button, int state, int x, int y) {
    std::cout << "Mouse button ";
    std::cout << ((button == GLUT_LEFT_BUTTON) ? "left" : ((button == GLUT_RIGHT_BUTTON) ? "right" : "middle")) << " ";
    std::cout << ((state == GLUT_DOWN) ? "pressed" : "released");
    std::cout << " at coordinates: " << x << " x " << y << std::endl;
}

int main(int argc, char** argv) {
    /*Initialise the GLUT library.
      void glutInit(int *argcp, char **argv)
      GLUT may read and change/remove arguments from the command line.
      You should not have any glut* function call before this one.
    */
    glutInit(&argc, argv);

    //Set window size, in pixels.
    glutInitWindowSize(defaultW, defaultH);

    /* Set window position, in pixels from the top-left corner of the deskop
       to the top-left corner of the window.
       (-1, -1) means let the window manager decide.
     */
    glutInitWindowPosition(-1, -1);

    /*
      void glutInitDisplayMode (unsigned int mode)
      Request a specific display mode, with specific
      capabilities. Capability flags are bitmasks.
      GLUT_SINGLE or  GLUT_DOUBLE : single buffering or
        double-buffering (for animations).
      GLUT_RGB or GLUT_RGBA : Alpha: enable transparency
      Multisampling and other advanced modes can be enabled here.
     */
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    /*
    int glutCreateWindow (char *name)
    Attempt to create a window with the provided name.
    */
    glutCreateWindow(argv[0]);


    //Our custom on-initialisation operations.
    init();

    /*Callback functions: we can write our own behaviour in
      functions, then register those function pointers in GLUT,
      so that the GLUT event loop calls our code when appropriate.
     */

     // void glutReshapeFunc (void (*Reshape)(int width, int height))
    glutReshapeFunc(Reshape);

    // void glutKeyboardFunc (void (*KeyboardFunc)(unsigned char,int,int))
    glutKeyboardFunc(KeyboardFunc);

    // void glutMouseFunc (void (*MouseFunc)(int,int,int,int))
    glutMouseFunc(MouseFunc);

    /* void glutDisplayFunc (void (*Display)(void))
     Display is also a callback. GLUT calls it whenever it needs to
     redraw pixels - window resize, window dragged from off-screen,
     unminimised, or if another window partially covered it,
     and now it (partially) uncovered it.
    */
    glutDisplayFunc(Display);

    /*
      void glutIdleFunc (void (*IdleFunc) (void))
      Whenever GLUT has some spare time, it calls this function.
      This is an easy way to display animations at a fluid framerate.
     */
     //glutIdleFunc(Display);

     /* After the previous initialisation, the glut main loop starts. This
       event loop will process exdternal events, and call our assigned callback
       functions as appropriate. We surrender control of the program flow to it.
     */
    glutMainLoop();

    return 0;
}
