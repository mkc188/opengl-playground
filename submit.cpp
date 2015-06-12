/*****************************************************************************
          FILE : submit.cpp (csc3260 2013-2014 Assignment 1)
          NOTE : you have to implement functions in this file
*****************************************************************************/

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glut.h>
#include <stdlib.h>
#include "bitmap.h"

/* Define state of animation */
enum State {NONE, BLUE_L, BLUE_R, ORANGE_L, ORANGE_R, GREEN, GREEN_SCL, SPEED};
State animation = NONE;
GLdouble blueLeft[] = {90.0, 1};
GLdouble blueRight[] = {90.0, -1};
GLdouble orange = 0.0;
GLdouble green[] = {0.0, 1};
GLdouble green_scale[] = {1.0, 1};
GLdouble sky = 0, sun = 90.0, global_angle = 0;
GLfloat rotation_x = 0.0, rotation_y = 0.0, old_rotation_x = 0.0,
        old_rotation_y = 0.0, current_x = 0.0, current_y = 0.0,
        dir_x = 0.0, dir_y = 0.0, dir_z = 0.0;
int speed = 1, dir = 0, move = 0, swing = 1, global_rotation = 0, still = 0;
double offset;
long i;

/* Define objects */
GLUquadricObj *quadratic;
GLubyte *texworld, *texbricks, *texsand, *texgrass, *texsun, *texface;

/* Define dimensions */
int textworld_w, textworld_h, texbricks_w, texbricks_h, texsand_w, texsand_h,
    texgrass_w, texgrass_h, texsun_w, texsun_h, texface_w, texface_h;

/* I - Bitmap file to load */
GLubyte* TextureLoadBitmap(char *filename, int *w, int *h)
{
	BITMAPINFO *info;				/* Bitmap information */
	void *bits;						/* Bitmap pixel bits */
	GLubyte	*rgb;					/* Bitmap RGB pixels */
	GLubyte err = '0';

	/* Try loading the bitmap and converting it to RGB */
	bits = LoadDIBitmap(filename, &info);
	if(bits == NULL)
		return(NULL);
	rgb = ConvertRGB(info, bits);
	if (rgb == NULL)
	{
		free(info);
		free(bits);
	};

	printf("%s: %d %d\n", filename, info->bmiHeader.biWidth, info->bmiHeader.biHeight);
	printf("read %s successfully\n", filename);
	*w = info->bmiHeader.biWidth;
	*h = info->bmiHeader.biHeight;

	/* Free the bitmap and RGB images, then return 0 (no errors) */
	free(info);
	free(bits);
	return (rgb);
}

/* All Setup For OpenGL Goes Here */
void init(void)
{
    glClearColor(0,0,0,0);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	/* Light source 0 */
	static GLfloat light0pos[] = {300.0, 100.0, 600.0, 1.0};
	static GLfloat light0_mat1[] = {0.6, 0.6, 0.6, 0.1};
	static GLfloat light0_diff1[] = {0.1, 0.1, 0.1, 0.1};
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_mat1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diff1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
    // glEnable(GL_NORMALIZE);

	/* Texture mapping parameters for filter and repeatance */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* Read texture images to texture arrays, and retreive image information */
	texworld = TextureLoadBitmap("skyball.bmp", &textworld_w, &textworld_h);
	texbricks = TextureLoadBitmap("brick.bmp", &texbricks_w, &texbricks_h);
	texsand = TextureLoadBitmap("sand.bmp", &texsand_w, &texsand_h);
	texgrass = TextureLoadBitmap("grass.bmp", &texgrass_w, &texgrass_h);
	texsun = TextureLoadBitmap("sun.bmp", &texsun_w, &texsun_h);
    texface = TextureLoadBitmap("face.bmp", &texface_w, &texface_h);

    quadratic = gluNewQuadric();

	/* Enable automatic texture coordinate generation in Quadric objects */
	gluQuadricTexture(quadratic, GL_TRUE);
}

/* Cube drawing function, (x, y, z) is bottom left of front face */
void draw_cube(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h, GLfloat d)
{
    glBegin(GL_QUADS);
        /* Draw only the top */
        glTexCoord2f(0.0, 0.0);
        glVertex3f(x+w, y, z);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(x+w, y, z-d);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(x, y, z-d);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(x, y, z);

        for (i = 0; i < 2; i++) {
            glTexCoord2f(0.0, 0.0);
            glVertex3f(x+w, y, z-d*i);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(x, y, z-d*i);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(x, y-h, z-d*i);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(x+w, y-h, z-d*i);
        }

        for (i = 0; i < 2; i++) {
            glTexCoord2f(0.0, 0.0);
            glVertex3f(x+w*i, y, z);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(x+w*i, y-h, z);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(x+w*i, y-h, z-d);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(x+w*i, y, z-d);
        }
    glEnd();
}

/* Here's Where We Do All The Drawing */
void display(void)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLfloat material_spec[] = {1.0,1.0,1.0,1.0};
    GLfloat material_emiss[] = {0.0,0.0,0.0,1.0};
    GLfloat material_shine[] = {50.0};
    glMaterialfv(GL_FRONT,GL_SPECULAR,material_spec);
    glMaterialfv(GL_FRONT,GL_EMISSION,material_emiss);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_shine);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glTranslatef(0, 0, -500);
     glRotatef(global_angle, 0, 1, 0);

	/* The sky */
	glPushMatrix();
        glRotatef(rotation_y+current_y, 1.0, 0.0, 0.0);
        glRotatef(rotation_x+current_x, 0.0, 1.0, 0.0);
        glRotatef(sky, 0.0, 1.0, 0.0);
        glRotatef(90.0,1.0,0.0,0.0);

		glColor3f(1.0, 1.0, 1.0);

		glEnable(GL_TEXTURE_2D);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, textworld_w, textworld_h, 0, GL_RGB, GL_UNSIGNED_BYTE, texworld);
			gluSphere(quadratic, 500.0, 100, 100);
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();

    /* The sun */
    glEnable(GL_LIGHT0);
    glPushMatrix();
		glRotatef(rotation_y+current_y, 1.0, 0.0, 0.0);
		glRotatef(rotation_x+current_x, 0.0, 1.0, 0.0);
        glTranslatef(0,-100,0);
        glRotatef(sun,0,0,1);
        glTranslatef(300,0,0);
    	glEnable(GL_TEXTURE_2D);
    		glTexImage2D(GL_TEXTURE_2D, 0, 3, texsun_w, texsun_h, 0, GL_RGB, GL_UNSIGNED_BYTE, texsun);
    		gluSphere(quadratic, 8, 50, 50);
    	glDisable(GL_TEXTURE_2D);
    glPopMatrix();

	/* Objects */
	glPushMatrix();
        glTranslatef(0, 0, 500);
		glTranslatef(0.0, 0.0, -525.0);
		glRotatef(rotation_y+current_y, 1.0, 0.0, 0.0);
		glRotatef(rotation_x+current_x, 0.0, 1.0, 0.0);

		/* Fence */
		glPushMatrix();
			glEnable(GL_TEXTURE_2D);
				glTexImage2D(GL_TEXTURE_2D, 0, 3, texbricks_w, texbricks_h, 0, GL_RGB, GL_UNSIGNED_BYTE, texbricks);
				draw_cube(-150.0, -100.0, 150.0, 10.0, 20.0, 300.0);
                draw_cube(-150.0, -100.0, -150.0, 300.0, 20.0, 10.0);
                draw_cube(-150.0, -100.0, 150.0, 300.0, 20.0, 10.0);
                draw_cube(140.0, -100.0, 150.0, 10.0, 20.0, 300.0);
			glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		glPushMatrix();

            /* Floor and grass */
			glPushMatrix();
				glEnable(GL_TEXTURE_2D);
					glTexImage2D(GL_TEXTURE_2D, 0, 3, texsand_w, texsand_h, 0, GL_RGB, GL_UNSIGNED_BYTE, texsand);
                    draw_cube(-150, -120, 150, 300, 1, 300);
				glDisable(GL_TEXTURE_2D);

                glEnable(GL_TEXTURE_2D);
                    glTexImage2D(GL_TEXTURE_2D, 0, 3, texgrass_w, texgrass_h, 0, GL_RGB, GL_UNSIGNED_BYTE, texgrass);
                    draw_cube(-500, -120.5, 500, 1000, 1, 1000);
                glDisable(GL_TEXTURE_2D);
			glPopMatrix();

            offset = 0.0;

            /* Blue parts */
            for (i=0;i<2;i++) {
                glPushMatrix();
                    glColor3f(0.0,0.5,1.0);
                    glTranslatef(offset,10,-80);
                    glRotatef(90,1,0,0);
                    gluCylinder(quadratic,3.0,3.0,120.0,32,32);
                    offset += 90;
                glPopMatrix();
            }
            glPushMatrix();
                glTranslatef(-3.0,10,-80);
                glRotatef(90,0,1,0);
                gluCylinder(quadratic,3.0,3.0,96.0,32,32);
            glPopMatrix();

            /* Left blue */
            glPushMatrix();
                glTranslatef(30,10,-80);
                glRotatef(blueLeft[0],1,0,0);
                glPushMatrix();
                    glTranslatef(-15,0,0);
                    gluCylinder(quadratic,1.5,1.5,80.0,32,32);
                    glTranslatef(15,0,0);
                    gluCylinder(quadratic,1.5,1.5,80.0,32,32);
                glPopMatrix();
                glTranslatef(-7,0,80);
                glutSolidTorus(3, 7, 10, 20);
            glPopMatrix();

            /* Right blue */
            glPushMatrix();
                glTranslatef(75,10,-80);
                glRotatef(blueRight[0],1,0,0);
                glPushMatrix();
                    glTranslatef(-15,0,0);
                    gluCylinder(quadratic,1.5,1.5,80.0,32,32);
                    glTranslatef(15,0,0);
                    gluCylinder(quadratic,1.5,1.5,80.0,32,32);
                glPopMatrix();
                glTranslatef(-7,0,80);
                glutSolidTorus(3, 7, 10, 20);
            glPopMatrix();

            /* Orange parts */
            glPushMatrix();
                glColor3f(1.0,0.5,0.25);
                glTranslatef(30,-120,0);
                glRotatef(-90,1,0,0);
                glutSolidCone(5, 15, 32, 32);
                glPushMatrix();
                    glTranslatef(0,0,15);
                    glutSolidSphere(3, 32, 32);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(0,0,13);
                    gluCylinder(quadratic,35.0,35.0,2.5,32,32);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(0,0,15.5);
                    gluDisk(quadratic,3.0,35.0,32,32);
                glPopMatrix();

                glRotatef(orange,0,0,1);
                offset = 0.0;
                for (i=0;i<6;i++) {
                    glPushMatrix();
                        glRotatef(offset,0,0,1);
                        glTranslatef(7.5,0,15.5);
                        gluCylinder(quadratic,0.5,0.5,7.5,32,32);
                        glPushMatrix();
                            glTranslatef(-0.5,0,7.5);
                            glRotatef(83,0,1,0);
                            gluCylinder(quadratic,0.5,0.5,23.5,32,32);
                        glPopMatrix();
                        glTranslatef(22.5,0,0);
                        gluCylinder(quadratic,0.5,0.5,10.0,32,32);
                    glPopMatrix();
                    offset += 60;
                }
            glPopMatrix();

            /* Green parts */
            glPushMatrix();
                glColor3f(0.0,0.5,0.25);
                glTranslatef(-30,-120,90);
                glRotatef(-90,1,0,0);
                glutSolidCone(7, 20, 32, 32);
                glPushMatrix();
                    glTranslatef(0,0,20);
                    glutSolidSphere(4, 32, 32);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(0,0,19.5);
                    glRotatef(green[0],0,1,0);
                    glScalef(10*green_scale[0],1,0.25);
                    glutSolidCube(10);
                glPopMatrix();
            glPopMatrix();
		glPopMatrix();

        /* Human */
        glPushMatrix();
            glTranslatef(-50+dir_x, -110+dir_y, 0.0+dir_z);
            glRotatef(dir*90, 0, 1, 0);
            glColor3f(0.1, 0.1, 0.1);

            /* Body */
            glPushMatrix();
                glRotatef(-90, 1.f, 0.f, 0.f);
                gluCylinder(quadratic,2,2,20,32,32);
            glPopMatrix();

            /* Head */
            glPushMatrix();
                glTranslatef(0.f, 25.f, 0.f);

                glColor3f(1.0, 1.0, 1.0);
                glEnable(GL_TEXTURE_2D);
                    glTexImage2D(GL_TEXTURE_2D, 0, 3, texface_w, texface_h, 0, GL_RGB, GL_UNSIGNED_BYTE, texface);
                    glRotatef(-90, 1, 0, 0);
                    gluSphere(quadratic, 10, 32, 32);
                glDisable(GL_TEXTURE_2D);
                glColor3f(0.1, 0.1, 0.1);
            glPopMatrix();

            /* Left hand */
            glPushMatrix();
                glTranslatef(0, 15, 0);
                glRotatef(90+30*swing, 1.f, 0.f, 0.f);
                glRotatef(40, 0, 1, 0);
                gluCylinder(quadratic,2,2,12,32,32);
            glPopMatrix();

            /* Right hand */
            glPushMatrix();
                glTranslatef(0, 15, 0);
                glRotatef(90-30*swing, 1.f, 0.f, 0.f);
                glRotatef(-40, 0, 1, 0);
                gluCylinder(quadratic,2,2,12,32,32);
            glPopMatrix();

            /* Left leg */
            glPushMatrix();
                glTranslatef(0, 0, 0);
                glRotatef(90-30*swing, 1.f, 0.f, 0.f);
                glRotatef(40, 0, 1, 0);
                gluCylinder(quadratic,2,2,12,32,32);
            glPopMatrix();

            /* Right leg */
            glPushMatrix();
                glTranslatef(0, 0, 0);
                glRotatef(90+30*swing, 1.f, 0.f, 0.f);
                glRotatef(-40, 0, 1, 0);
                gluCylinder(quadratic,2,2,12,32,32);
            glPopMatrix();

        glPopMatrix();
	glPopMatrix();

	glutSwapBuffers();
}

void reshape(int w, int h)
{
    if(h == 0) h = 1;
    GLfloat ratio = 1.0* w / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    gluPerspective(40.0, ratio, 100., 1000.);
}

void special(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP:
        if (dir != 2) dir = 2;
        else move = 1;
        break;
        case GLUT_KEY_DOWN:
        if (dir != 0) dir = 0;
        else move = 2;
        break;
        case GLUT_KEY_LEFT:
        if (dir != 3) dir = 3;
        else move = 3;
        break;
        case GLUT_KEY_RIGHT:
        if (dir != 1) dir = 1;
        else move = 4;
    }
    swing = (swing == 1) ? -1 : 1;
}

void keyboard(unsigned char key, int x, int y) // Handle the keyboard events here
{
    switch (key)
    {
        case '\033'://press 'esc' to quit
        exit(0);
        break;

        // TODO:
        // Add keyboard control here
        case '1': animation = GREEN; break;
        case '2': animation = BLUE_L; break;
        case '3': animation = BLUE_R; break;
        case '4': animation = ORANGE_L; break;
        case '5': animation = ORANGE_R; break;
        case '6': animation = GREEN_SCL; break;
        case '7': speed = (speed == 1) ? 4 : 1; break;
        case '0': animation = NONE;break;
        case 'a': global_rotation = (global_rotation == 1) ? 0 : 1; break;
        case 'r':
        global_angle = 0;
        global_rotation = 0;
        break;
        case 'd': still = (still == 0) ? 1 : 0; break;
    }
}

void Mouse(int button, int state, int x, int y)
{
    if(state) {
        current_x += x - old_rotation_x;
        current_y += y - old_rotation_y;

        rotation_x = 0;
        rotation_y = 0;
    }
    else {
        old_rotation_x = x;
        old_rotation_y = y;
    }
}

void MotionMouse(int x, int y)
{
    rotation_x = x - old_rotation_x;
    rotation_y = y - old_rotation_y;

    glutPostRedisplay();
}

void idle()
{
	if (sky >= 360) sky = 0;
    if (!still) {
        sky += 0.1;
        sun -= 0.2;
    }

    if (global_rotation >= 360) global_rotation = 0;
    if (global_rotation) global_angle-=1;

    switch (animation) {
        case GREEN:
        if (green[0] < -20 || green[0] > 20)
            green[1] *= -1;
        green[0] += green[1] * 0.2 * speed;
        break;
        case GREEN_SCL:
        if (green_scale[0] < 0.8 || green_scale[0] > 1.2) {
            green_scale[1] *= -1;
            animation = NONE;
        }
        else if (green_scale[0] == 1.0)
            animation = NONE;
        green_scale[0] += green_scale[1] * 0.01f;
        break;
        case BLUE_L:
        if (blueLeft[0] < 60 || blueLeft[0] > 120)
            blueLeft[1] *= -1;
        blueLeft[0] += blueLeft[1] * 0.2 * speed;
        break;
        case BLUE_R:
        if (blueRight[0] < 60 || blueRight[0] > 120)
            blueRight[1] *= -1;
        blueRight[0] += blueRight[1] * 0.2 * speed;
        break;
        case ORANGE_L: orange += 0.2 * speed; break;
        case ORANGE_R: orange -= 0.2 * speed; break;
        default: break;
    }

    /* Movements */
    switch (move) {
        case 1: if (dir_z >= -130) dir_z-=2; break;
        case 2: if (dir_z <= 130) dir_z+=2; break;
        case 3: if (dir_x >= -75) dir_x-=2; break;
        case 4: if (dir_x <= 170) dir_x+=2;
    }
    move = 0;

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
	/* Initialization of GLUT Library */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);

	/* Create a window with title specified */
	glutInitWindowSize(800, 640);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Assignment 2");

	init();	/* Not GLUT call, initialize several parameters */

	/*
	 * Register different CALLBACK function for GLUT to response
	 * with different events, e.g. window sizing, mouse click or
	 * keyboard stroke
	 */
	glutReshapeFunc(reshape);		/* Resize */
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);		/* Keyboard event */
	glutSpecialFunc(special);
	glutIdleFunc(idle);
	glutMouseFunc(Mouse);
	glutMotionFunc(MotionMouse);

	/*
	 * Enter the GLUT event processing loop which never returns.
	 * it will call different registered CALLBACK according
	 * to different events.
	 */
	glutMainLoop();	/* Go into forever looping */

    return 0;
}
