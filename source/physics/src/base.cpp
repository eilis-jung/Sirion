

#include <core/core.h>
#include <GL/glut.h>

#pragma region Callbacks
static void MouseCallback(int button, int state, int x, int y) {
	pApplication2D->mouseCallback(button, state, x, y);
}

static void MotionCallback(int x, int y) {
	pApplication2D->motionCallback(x, y);
}


static void KeyboardCallback(unsigned char key, int x, int y) {
	pApplication2D->keyboardCallback(key, x, y);
}
static void SpecialKeyboardCallback(int key, int x, int y) {
	pApplication2D->specialKeyboardCallback(key, x, y);
}

static void SpecialKeyboardUpCallback(int key, int x, int y) {
	pApplication2D->specialKeyboardUpCallback(key, x, y);
}

static void ExitCallback() {
	pApplication2D->exitCallback();
}

static void ReshapeCallback(int width, int height) {
	pApplication2D->reshapeCallback(width, height);
}

static void RenderCallback() {
	pApplication2D->draw();
	pApplication2D->update();
}
#pragma endregion

static void IdleCallback() {
	glutPostRedisplay();
}

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


	glutDisplayFunc(renderScene);
	//glutMouseFunc(MouseCallback);
	//glutMotionFunc(MotionCallback);
	//glutKeyboardFunc(KeyboardCallback);
	//glutSpecialFunc(SpecialKeyboardCallback);
	//glutSpecialUpFunc(SpecialKeyboardUpCallback);
	//glutReshapeFunc(ReshapeCallback);
	//atexit(ExitCallback);
	//glutDisplayFunc(RenderCallback);

	glutIdleFunc(IdleCallback);
	glutMainLoop();




	return 0;
}