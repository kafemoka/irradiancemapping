#define _USE_MATH_DEFINES
#define NOMINMAX
#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>

// �E�B���h�E�֘A�̏���
#include "Window.h"

namespace
{
  //
  // �O�p�`�������� Alias OBJ �`���̌`��f�[�^�t�@�C��
  //
  const char filename[] = "bunny.obj";

  //
  // �����̈ʒu
  //
  const GLfloat lightPos[][4] =
  {
    { 0.0f, 5.0f, 10.0f, 1.0f },
    { 10.0f, 2.0f, 1.0f, 1.0f }
  };

  //
  // �����̐F
  //
  const GLfloat lightCol[][4] =
  {
    { 0.8f, 0.8f, 0.8f, 1.0f },
    { 0.4f, 0.4f, 0.4f, 1.0f }
  };

  //
  // �����̐F
  //
  const GLfloat lightAmb[][4] =
  {
    { 0.1f, 0.1f, 0.1f, 1.0f },
    { 0.1f, 0.1f, 0.1f, 1.0f }
  };

  //
  // �����̐�
  //
  const size_t lightCount(sizeof lightPos / sizeof lightPos[0]);

  //
  // ���̕`��
  //
  void floor(int size, GLfloat height)
  {
    // �^�C���P���̒��_�f�[�^
    static const GLfloat floorvert[] =
    {
      0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 0.0f
    };

    // �^�C���̊g�U���ˌW��
    static const GLfloat floordiff[][4] =
    {
      { 0.6f, 0.6f, 0.6f, 1.0f },
      { 0.3f, 0.3f, 0.3f, 1.0f }
    };

    // �^�C���̋��ʔ��ˌW��
    static const GLfloat floorspec[] =
    {
      0.1f, 0.1f, 0.1f, 1.0f
    };

    // ���̃|���S���̒��_�ʒu�𒸓_�z�񂩂�擾����
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, floorvert);

    // ���̃|���S���̖@���x�N�g���� glNormal3f() �Őݒ肷��
    glDisableClientState(GL_NORMAL_ARRAY);
    glNormal3f(0.0f, 1.0f, 0.0f);

    // ���̃|���S���̋��ʔ��ˌW����ݒ肷��
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, floorspec);

    // ���̕`��
    for (int j = -size; j < size; ++j)
    {
      for (int i = -size; i < size; ++i)
      {
        // ���̃|���S���̊g�U���ˌW����ݒ肷��
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, floordiff[(i + j) & 1]);

        // ���̃|���S����`�悷��
        glPushMatrix();
        glTranslatef(GLfloat(i), height, GLfloat(j));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();
      }
    }
  }

  //
  // �V�[���̕`��
  //
  void scene(GLuint ng, const GLuint (*group)[2], const GLfloat (*diff)[4], const GLfloat (*spec)[4], const GLfloat *shi,
    GLuint nv, const GLfloat (*pos)[3], const GLfloat (*norm)[3])
  {
    // �I�u�W�F�N�g�̒��_�ʒu�𒸓_�z�񂩂�擾����
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, pos);

    // �I�u�W�F�N�g�̖@���x�N�g���𒸓_�z�񂩂�擾����
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, norm);

    // �I�u�W�F�N�g�̕`��
    for (unsigned int g = 0; g < ng; ++g)
    {
      // �g�U���ˌW����ݒ肷��
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diff[g]);

      // ���ʔ��ˌW���ƋP���W����ݒ肷��
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec[g]);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shi[g] > 128.0f ? 128.0f : shi[g]);

      // �I�u�W�F�N�g��`�悷��
      glDrawArrays(GL_TRIANGLES, group[g][0], group[g][1]);
    }
  }

  //
  // �v���O�����I�����̏���
  //
  void cleanup()
  {
    // GLFW �̏I������
    glfwTerminate();
  }
}

//
// ���C���v���O����
//
int main()
{
  // GLFW ������������
  if (glfwInit() == GL_FALSE)
  {
    // �������Ɏ��s����
    std::cerr << "Can't initialize GLFW" << std::endl;
    return 1;
  }

  // �v���O�����I�����̏�����o�^����
  atexit(cleanup);

  // �E�B���h�E���쐬����
  Window window("Irradiance Mapping", 960, 540);

  // OpenGL �̏����ݒ�
  glClearColor(0.3f, 0.5f, 0.8f, 0.0f);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);

  // �A�e�t����L���ɂ���
  glEnable(GL_LIGHTING);

  // OpenGL �̌����̐ݒ�
  for (size_t i = 0; i < lightCount; ++i)
  {
    const GLenum light(GL_LIGHT0 + i);
    glEnable(light);
    glLightfv(light, GL_POSITION, lightPos[i]);
    glLightfv(light, GL_DIFFUSE, lightCol[i]);
    glLightfv(light, GL_SPECULAR, lightCol[i]);
    glLightfv(light, GL_AMBIENT, lightAmb[i]);
  }

  // �ގ��f�[�^
  GLuint ng;
  GLuint (*group)[2];
  GLfloat (*amb)[4], (*diff)[4], (*spec)[4], *shi;

  // �`��f�[�^
  GLuint nv;
  GLfloat (*pos)[3], (*norm)[3];

  // �`��f�[�^�̓ǂݍ���
  ggLoadObj(filename, ng, group, amb, diff, spec, shi, nv, pos, norm, false);

  // �E�B���h�E���J���Ă���ԌJ��Ԃ�
  while (window.shouldClose() == GL_FALSE)
  {
    // �E�B���h�E����������
    window.clear();

    // ���f���r���[�ϊ��s��̐ݒ�
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ���_�̈ړ�
    glTranslatef(window.getPosition()[0], window.getPosition()[1], window.getPosition()[2]);

    // �g���b�N�{�[�������ɂ���]
    glMultMatrixf(window.getTb());

    // �V�[���̕`��
    scene(ng, group, diff, spec, shi, nv, pos, norm);

    // ���̕`��
    floor(5, -1.0f);

    // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
    window.swapBuffers();
  }
}
