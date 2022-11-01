

#include <iostream>
#include <GL/glut.h>

void renderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_TRIANGLES);
	glVertex3f(-0.5, -0.5, 0.0);
	glVertex3f(0.5, 0.0, 0.0);
	glVertex3f(0.0, 0.5, 0.0);
	glEnd();

	glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("Lighthouse3D- GLUT Tutorial");

	// register callbacks
	glutDisplayFunc(renderScene);

	glutIdleFunc(IdleCallback);
	glutMainLoop();

	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(SpecialKeyboardCallback);
	glutSpecialUpFunc(SpecialKeyboardUpCallback);
	glutReshapeFunc(ReshapeCallback);
	atexit(ExitCallback);
	glutDisplayFunc(RenderCallback);


	return 0;
}