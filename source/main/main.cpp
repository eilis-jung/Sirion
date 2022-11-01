
#include <GL/glut.h>
#include "core/core.h"
#include "main.h"
#include <filesystem>
#include <fstream>
#include <iostream>

#pragma region Callbacks
//static void MouseCallback(int button, int state, int x, int y) {
//	pApplication2D->mouseCallback(button, state, x, y);
//}
//
//static void MotionCallback(int x, int y) {
//	pApplication2D->motionCallback(x, y);
//}
//
//
//static void KeyboardCallback(unsigned char key, int x, int y) {
//	pApplication2D->keyboardCallback(key, x, y);
//}
//static void SpecialKeyboardCallback(int key, int x, int y) {
//	pApplication2D->specialKeyboardCallback(key, x, y);
//}
//
//static void SpecialKeyboardUpCallback(int key, int x, int y) {
//	pApplication2D->specialKeyboardUpCallback(key, x, y);
//}
//
//static void ExitCallback() {
//	pApplication2D->exitCallback();
//}
//
//static void ReshapeCallback(int width, int height) {
//	pApplication2D->reshapeCallback(width, height);
//}
//
//static void RenderCallback() {
//	pApplication2D->draw();
//	pApplication2D->update();
//}
#pragma endregion

namespace Sirion
{
    void metaExample()
    {
        Test1 test1_in;
        test1_in.m_int = 12;
        test1_in.m_char = 'g';
        int i = 1;
        test1_in.m_int_vector.emplace_back(&i);

        Test1 test1_out;
        // test on array
        Test2 test2_in;
        test2_in.m_test_base_array.emplace_back("Test1", &test1_in);
        Test1 Test2_temp;
        test2_in.m_test_base_array.emplace_back("Test1", &Test2_temp);

        // serializer & deserializer

        // write Test1_in (object) to Test1_json_in (json)
        auto test1_json_in = Serializer::write(test1_in);

        std::string test1_context = test1_json_in.dump();

        // read Test1_context (json) to Test1_out (object)
        std::string err;

        auto&& Test1_json = Json::parse(test1_context, err);
        Serializer::read(Test1_json, test1_out);


        auto        Test2_json_in = Serializer::write(test2_in);
        std::string test2_context = Test2_json_in.dump();

        std::fstream out_put("out.txt", std::ios::out);
        out_put << test2_context;
        out_put.flush();
        out_put.close();

        Test2  test2_out;
        auto&& test2_json = Json::parse(test2_context, err);
        Serializer::read(test2_json, test2_out);


        // reflection
        auto                       meta = TypeMetaDef(Test2, &test2_out);
        Reflection::FieldAccessor* fields;
        int                        fields_count = meta.m_meta.getFieldsList(fields);
        for (int i = 0; i < fields_count; ++i)
        {
            auto filed_accesser = fields[i];
            std::cout << filed_accesser.getFieldTypeName() << " " << filed_accesser.getFieldName() << " "
                << (char*)filed_accesser.get(meta.m_instance) << std::endl;
            if (filed_accesser.isArrayType())
            {
                Reflection::ArrayAccessor array_accesser;
                if (Reflection::TypeMeta::newArrayAccessorFromName(filed_accesser.getFieldTypeName(), array_accesser))
                {
                    void* field_instance = filed_accesser.get(meta.m_instance);
                    int   count = array_accesser.getSize(field_instance);
                    auto  typeMetaItem = Reflection::TypeMeta::newMetaFromName(array_accesser.getElementTypeName());
                    for (int index = 0; index < count; ++index)
                    {
                        std::cout << ":L:" << index << ":R:" << (int*)array_accesser.get(index, field_instance)
                            << std::endl;
                        ;
                    }
                }
            }
        }
    }
} // namespace Sirion


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

    Sirion::metaExample();
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