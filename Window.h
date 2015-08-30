#pragma once

#include <cstdlib>
#include <iostream>

#if defined(_WIN32)
#  pragma comment(lib, "glu32.lib")
#elif defined(__APPLE__)
#  define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#  include <OpenGL/gl.h>
#  include <OpenGL/glext.h>
#endif
#define GLFW_INCLUDE_GLU
#include "gg.h"
using namespace gg;

//
// �E�B���h�E�֘A�̏���
//
class Window
{
  // �E�B���h�E�̎��ʎq
  GLFWwindow *const window;

  // �g���b�N�{�[������
  GgTrackball tb;

  // ��]���S�ɑ΂���J�����̈ʒu
  GLfloat position[3];

public:

  // �R���X�g���N�^
  Window(const char *title = "Game Graphics", int width = 640, int height = 480);

  // �f�X�g���N�^
  virtual ~Window();

  // �E�B���h�E�����ׂ����𔻒肷��
  int shouldClose() const
  {
    return glfwWindowShouldClose(window) | glfwGetKey(window, GLFW_KEY_ESCAPE);
  }

  // ��ʃN���A
  void clear() const
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
  void swapBuffers();

  // �L�[�{�[�h���^�C�v�������̏���
  static void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods);

  // �}�E�X�{�^���𑀍삵���Ƃ��̏���
  static void mouse(GLFWwindow *window, int button, int action, int mods);

  // �}�E�X�z�C�[�����쎞�̏���
  static void wheel(GLFWwindow *window, double x, double y);

  // �E�B���h�E�̃T�C�Y�ύX���̏���
  static void resize(GLFWwindow *window, int width, int height);

  // �g���b�N�{�[���ɂ���]�����o��
  const GLfloat *getTb() const
  {
    return tb.get();
  }

  // �g���b�N�{�[���ɂ���]�����o��
  const GLfloat *getTbTransposed() const
  {
    return tb.getMatrix().transpose().get();
  }

  // ��]���S�ɑ΂���J�����̈ʒu�����o��
  const GLfloat *getPosition() const
  {
    return position;
  }
};
