//
// Window �֘A�̏���
//
#include <cmath>
#include "Window.h"

// �J�����̉�p
const GLdouble fovy(40.0);

// �O���ʂ̈ʒu
const GLdouble zNear(1.0);

// ����ʂ̈ʒu
const GLdouble zFar(20.0);

// ��]���S�܂ł̋����̏����l
const GLfloat initialPosition[] = { 0.0f, 0.0f, 5.0f };

// �z�C�[���ɂ��O��ړ��̑��x
const GLfloat distanceStep(0.1f);

//
// �R���X�g���N�^
//
Window::Window(const char *title, int width, int height)
  : window(glfwCreateWindow(width, height, title, NULL, NULL))
{
  if (window == NULL)
  {
    // �E�B���h�E���쐬�ł��Ȃ�����
    std::cerr << "Can't create GLFW window." << std::endl;
    exit(1);
  }

  // ���݂̃E�B���h�E�������Ώۂɂ���
  glfwMakeContextCurrent(window);

  // �쐬�����E�B���h�E�ɑ΂���ݒ�
  glfwSwapInterval(1);

  // �Q�[���O���t�B�b�N�X���_�̓s���ɂ��ƂÂ�������
  ggInit();

  // ���̃C���X�^���X�� this �|�C���^���L�^���Ă���
  glfwSetWindowUserPointer(window, this);

  // �L�[�{�[�h�𑀍삵�����̏���
  glfwSetKeyCallback(window, keyboard);

  // �}�E�X�{�^���𑀍삵���Ƃ��̏���
  glfwSetMouseButtonCallback(window, mouse);

  // �}�E�X�z�C�[�����쎞�ɌĂяo������
  glfwSetScrollCallback(window, wheel);

  // �E�B���h�E�̃T�C�Y�ύX���ɌĂяo��������o�^����
  glfwSetFramebufferSizeCallback(window, resize);

  // �J�����̏����ʒu��ݒ肷��
  position[0] = -initialPosition[0];
  position[1] = -initialPosition[1];
  position[2] = -initialPosition[2];

  // �E�B���h�E�̐ݒ������������
  resize(window, width, height);
}

//
// �f�X�g���N�^
//
Window::~Window()
{
  glfwDestroyWindow(window);
}

//
// �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
//
void Window::swapBuffers()
{
  // �J���[�o�b�t�@�����ւ���
  glfwSwapBuffers(window);

  // OpenGL �̃G���[���`�F�b�N����
  ggError("SwapBuffers");

  // �C�x���g�����o��
  glfwPollEvents();

  // �}�E�X�̈ʒu�𒲂ׂ�
  double x, y;
  glfwGetCursorPos(window, &x, &y);

  // ���{�^���h���b�O
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
  {
    // �g���b�N�{�[����]
    tb.motion(static_cast<GLfloat>(x), static_cast<GLfloat>(y));
  }
}

//
// �L�[�{�[�h���^�C�v�������̏���
//
void Window::keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  // ���̃C���X�^���X�� this �|�C���^�𓾂�
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
        case GLFW_KEY_SPACE:
          break;
        case GLFW_KEY_BACKSPACE:
        case GLFW_KEY_DELETE:
          break;
        case GLFW_KEY_UP:
          break;
        case GLFW_KEY_DOWN:
          break;
        case GLFW_KEY_RIGHT:
          break;
        case GLFW_KEY_LEFT:
          break;
        default:
          break;
      }
    }
  }
}

//
// �}�E�X�{�^���𑀍삵���Ƃ��̏���
//
void Window::mouse(GLFWwindow *window, int button, int action, int mods)
{
  // ���̃C���X�^���X�� this �|�C���^�𓾂�
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    switch (button)
    {
      case GLFW_MOUSE_BUTTON_1:
        // �g���b�N�{�[������
        if (action)
        {
          // �g���b�N�{�[�������J�n
          instance->tb.start(static_cast<float>(x), static_cast<float>(y));
        }
        else
        {
          // �g���b�N�{�[�������I��
          instance->tb.stop(static_cast<float>(x), static_cast<float>(y));
        }
        break;
      case GLFW_MOUSE_BUTTON_2:
        break;
      case GLFW_MOUSE_BUTTON_3:
        break;
      default:
        break;
    }
  }
}

//
// �}�E�X�z�C�[�����쎞�̏���
//
void Window::wheel(GLFWwindow *window, double x, double y)
{
  // ���̃C���X�^���X�� this �|�C���^�𓾂�
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
    {
      // Control �L�[��������Ă���΍��E�ʒu�𒲐�����
      instance->position[0] += distanceStep * static_cast<GLfloat>(y);
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
    {
      // Shift �L�[��������Ă���΍����𒲐�����
      instance->position[1] += distanceStep * static_cast<GLfloat>(y);
    }
    else
    {
      // ����������Ă��Ȃ����
      if (fabs(x) > fabs(y))
      {
        // ���E�ʒu�𒲐�����
        instance->position[0] += distanceStep * static_cast<GLfloat>(x);
      }
      else
      {
        // �O��ʒu�𒲐�����
        instance->position[2] += distanceStep * static_cast<GLfloat>(y);
      }
    }
  }
}

//
// �E�B���h�E�̃T�C�Y�ύX���̏���
//
void Window::resize(GLFWwindow *window, int width, int height)
{
  // ���̃C���X�^���X�� this �|�C���^�𓾂�
  Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

  if (instance)
  {
    // �E�B���h�E�S�̂��r���[�|�[�g�ɂ���
    glViewport(0, 0, width, height);

    // ���e�ϊ��s�������������
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, GLdouble(width) / GLdouble(height), zNear, zFar);

    // �g���b�N�{�[�������͈̔͂�ݒ肷��
    instance->tb.region(width, height);
  }
}
