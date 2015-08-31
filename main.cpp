#define _USE_MATH_DEFINES
#define NOMINMAX
#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

// ���O�v�Z�����}�b�v���g�p����Ȃ� 1
#define USEMAP 1

// �E�B���h�E�֘A�̏���
#include "Window.h"

namespace
{
  //
  // �O�p�`�������� Alias OBJ �`���̌`��f�[�^�t�@�C��
  //
  const char filename[] = "bunny.obj";

#if USEMAP
  //
  // ���ˏƓx�}�b�v
  //
  const char *const irrmaps[] =
  {
    "irrmap0.tga",
    "irrmap1.tga",
    "irrmap2.tga",
    "irrmap3.tga",
    "irrmap4.tga",
    "irrmap5.tga",
    "irrmap6.tga",
    "irrmap7.tga",
    "irrmap8.tga",
    "irrmap9.tga",
    "irrmapa.tga",
    "irrmapb.tga",
    "irrmapc.tga",
    "irrmapd.tga",
    "irrmape.tga",
    "irrmapf.tga",
    "irrmapg.tga",
    "irrmaph.tga"
  };

  //
  // ���}�b�v
  //
  const char *const envmaps[] =
  {
    "envmap0.tga",
    "envmap1.tga",
    "envmap2.tga",
    "envmap3.tga",
    "envmap4.tga",
    "envmap5.tga",
    "envmap6.tga",
    "envmap7.tga",
    "envmap8.tga",
    "envmap9.tga",
    "envmapa.tga",
    "envmapb.tga",
    "envmapc.tga",
    "envmapd.tga",
    "envmape.tga",
    "envmapf.tga",
    "envmapg.tga",
    "envmaph.tga"
  };

  //
  // ���ˏƓx�}�b�v�̐�
  //
  const size_t mapcount(sizeof irrmaps / sizeof irrmaps[0]);
#else
  //
  // �����ˉe�����̋��჌���Y�ŎB�e�����V��摜
  //
  const char *const skymaps[] =
  {
#if 0
    "skymap0.tga",
    "skymap1.tga",
    "skymap2.tga",
    "skymap3.tga",
    "skymap4.tga",
    "skymap5.tga",
    "skymap6.tga",
    "skymap7.tga",
    "skymap8.tga",
#endif
    "skymap9.tga"
  };

  //
  // �V��摜�̐�
  //
  const size_t mapcount(sizeof skymaps / sizeof skymaps[0]);

  //
  // ���������x
  //
  const GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

  //
  // �P���W��
  //
  const GLfloat shininess(60.0f);

  //
  // �V��摜���̓V��̈�̒��a�̍ő�l
  //
  const GLsizei skysize(1024);

  //
  // �쐬����e�N�X�`���̃T�C�Y
  //
  const GLsizei imapsize(256);
  const GLsizei emapsize(256);

  //
  // �t�B���^�̃T���v����
  //
  const unsigned int isamples(256);
  const unsigned int esamples(256);
#endif

  //
  // �����ʃ}�b�s���O�p�̃e�N�X�`���ϊ��s��
  //
  const GLfloat paraboloid[] =
  {
    -1.0f,  0.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  0.0f,  2.0f,
     0.0f, -1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  0.0f,  2.0f,
  };

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

    // ���̃|���S���̊g�U���ˌ��Ƌ��ʔ��ˌ������ʔ��ˌW���Ŕz������
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, floorspec);

    // ���̕`��
    for (int j = -size; j < size; ++j)
    {
      for (int i = -size; i < size; ++i)
      {
        // ���̃|���S���̊g�U���ˌW���� primary color (���_�F) �ɐݒ肷��
        glColor3fv(floordiff[(i + j) & 1]);

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
  void scene(GLuint ng, const GLuint (*group)[2], const GLfloat (*diff)[4], const GLfloat (*spec)[4],
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
      // �g�U���ˌ��Ƌ��ʔ��ˌ������ʔ��ˌW���Ŕz������
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, spec[g]);

      // �g�U���ˌW���� primary color (���_�F) �ɐݒ肷��
      glColor4fv(diff[g]);

      // �I�u�W�F�N�g��`�悷��
      glDrawArrays(GL_TRIANGLES, group[g][0], group[g][1]);
    }
  }

  //
  // �e�N�X�`���̍쐬
  //
  void createTexture(const GLubyte *buffer, GLsizei width, GLsizei height, GLenum format,
    const GLfloat *amb, GLuint tex)
  {
    // �e�N�X�`���I�u�W�F�N�g�Ƀe�N�X�`�������蓖�Ă�
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, buffer);

    // �e�N�X�`���͐��`��Ԃ���
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // ���E�F���g������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // �e�N�X�`���̋��E�F�ɑ�������ݒ肷��
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, amb);
  }

#if USEMAP
  //
  // ���ˏƓx�}�b�v�̓ǂݍ���
  //
  bool loadMap(const char *iname, const char *ename, GLuint imap, GLuint emap)
  {
    // �I���X�e�[�^�X
    bool status(true);

    // �ǂݍ��񂾉摜�̕��ƍ���, �t�H�[�}�b�g
    GLsizei width, height;
    GLenum format;

    // ���ˏƓx�}�b�v�̓ǂݍ���
    GLubyte const *const itexture(ggLoadTga(iname, &width, &height, &format));

    // �摜���ǂݍ��߂Ȃ���ΏI��
    if (!itexture) status = false;

    // �ǂݍ��񂾉摜�̍�����̉�f�̐F��������Ƃ���
    const GLfloat iamb[] = { itexture[2] / 255.0f, itexture[1] / 255.0f, itexture[0] / 255.0f };

    // �e�N�X�`���̍쐬
    createTexture(itexture, width, height, format, iamb, imap);

    // �ǂݍ��񂾃f�[�^�͂����g��Ȃ��̂Ń��������������
    delete[] itexture;

    // ���ˏƓx�}�b�v�̓ǂݍ���
    GLubyte const *const etexture(ggLoadTga(ename, &width, &height, &format));

    // �摜���ǂݍ��߂Ȃ���ΏI��
    if (!etexture) status = false;

    // �ǂݍ��񂾉摜�̍�����̉�f�̐F��������Ƃ���
    const GLfloat eamb[] = { etexture[2] / 255.0f, etexture[1] / 255.0f, etexture[0] / 255.0f };

    // �e�N�X�`���̍쐬
    createTexture(etexture, width, height, format, eamb, emap);

    // �ǂݍ��񂾃f�[�^�͂����g��Ȃ��̂Ń��������������
    delete[] etexture;

    // ���ׂēǂݍ��ݐ���
    return status;
  }
#else
  //
  // ��l�������� (Xorshift �@)
  //
  GLfloat xor128()
  {
    static unsigned int x(123456789);
    static unsigned int y(362436069);
    static unsigned int z(521288629);
    static unsigned int w(88675123);
    const unsigned int t(x ^ x << 11);

    // �f�[�^�̓���ւ�
    x = y;
    y = z;
    z = w;

    //
    return GLfloat(w ^= w >> 19 ^ t ^ t >> 8) * 2.3283064e-10f;
  }

  //
  // �T���v���[�̍쐬
  //
  void createSampler(unsigned int samples, GLfloat(*sample)[4], GLfloat n)
  {
    // e �� 1 / (n + 1)
    const GLfloat e(1.0f / (n + 1.0f));

    for (unsigned int i = 0; i < samples; ++i)
    {
      const GLfloat y(pow(1.0f - xor128(), e));
      const GLfloat r(sqrt(1.0f - y * y));
      const GLfloat t(6.2831853f * xor128());
      const GLfloat x(r * cos(t)), z(r * sin(t));

      (*sample)[0] = x;
      (*sample)[1] = y;
      (*sample)[2] = z;
      (*sample)[3] = y; // (0, 1, 0) �Ƃ̓���
      ++sample;
    }
  }

  //
  // �T���v���[�̉�]
  //
  void rotateSampler(unsigned int samples, const GLfloat (*sample)[4],
    const GLfloat x, const GLfloat y, const GLfloat z, GLfloat (*result)[4])
  {
    // a �� x^2 + z^2;
    const GLfloat a(x * x + z * z);

    // ��]����K�v������Ƃ�
    if (a > 0)
    {
      // l �� length(x, z);
      const GLfloat l(sqrt(a));

      // m �� [(x, y, z) x (0, 1, 0), (x, y, z), (x, y, z) x (0, 1, 0) x (x, y, z)]
      const GLfloat m00(-z / l), m10(0.0f), m20(x / l);
      const GLfloat m01(x), m11(y), m21(z);
      const GLfloat m02(-m20 * y), m12(l), m22(m00 * y);

      // ��]���ăR�s�[
      for (unsigned int i = 0; i < samples; ++i)
      {
        result[i][0] = m00 * sample[i][0] + m01 * sample[i][1] + m02 * sample[i][2];
        result[i][1] = m10 * sample[i][0] + m11 * sample[i][1] + m12 * sample[i][2];
        result[i][2] = m20 * sample[i][0] + m21 * sample[i][1] + m22 * sample[i][2];
        result[i][3] = sample[i][3];
      }

      return;
    }

    // ��]����K�v���Ȃ���΂��̂܂܃R�s�[
    for (unsigned int i = 0; i < samples; ++i)
    {
      result[i][0] = sample[i][0];
      result[i][1] = sample[i][1];
      result[i][2] = sample[i][2];
      result[i][3] = sample[i][3];
    }
  }

  //
  // ������
  //
  void smooth(const GLubyte *src, GLsizei width, GLsizei height, GLenum format,
    GLsizei cs, GLsizei ct, GLsizei rs, GLsizei rt, unsigned int samples,
    GLubyte *dst, GLsizei size, const GLfloat *amb, GLfloat shi)
  {
    // �T���v���[
    GLfloat (*const sampler)[4](new GLfloat[samples][4]);
    createSampler(samples, sampler, shi);

    // ��]�����T���v���[�̕ۑ���
    GLfloat (*const rsampler)[4](new GLfloat[samples][4]);

    // �`�����l����
    const int channels(format == GL_BGRA ? 4 : 3);

    // ���������x
    const GLfloat ramb(amb[0] * 255.0f), gamb(amb[1] * 255.0f), bamb(amb[2] * 255.0f);

    // ���ˏƓx�}�b�v�̊e��f�ɂ���
    for (int dt = 0; dt < size; ++dt)
    {
      std::cout << "Processing line: " << dt
        << " (" << std::fixed << std::setprecision(1) << float(dt) * 100.0f / float(size) << "%)"
        << std::endl;

      for (int ds = 0; ds < size; ++ds)
      {
        // ���̉�f�̕��ˏƓx�}�b�v�̔z�� dst �̃C���f�b�N�X
        const int id((dt * size + ds) * 3);

        // ���̉�f�̕��ˏƓx�}�b�v��̐��K�����ꂽ���W�l (-1 �� u, v �� 1)
        const float du(float(ds * 2) / float(size - 1) - 1.0f);
        const float dv(1.0f - float(dt * 2) / float(size - 1));

        // ���ˏƓx�}�b�v������ʃ}�b�v�Ƃ��ĎQ�Ƃ���Ƃ��̂��̉�f�̕����x�N�g�� q
        const float qx(du);
        const float qy(1.0f - du * du - dv * dv);
        const float qz(dv);

        // ���̉�f�����ˏƓx�}�b�v�̒P�ʉ~�O�ɂ���Ƃ�
        if (qy <= 0.0f)
        {
          // ��������ݒ肷��
          dst[id + 0] = GLubyte(ramb);
          dst[id + 1] = GLubyte(gamb);
          dst[id + 2] = GLubyte(bamb);
          continue;
        }

        // �T���v���[�����̃x�N�g���̕����Ɍ�����
        rotateSampler(samples, sampler, qx, qy, qz, rsampler);

        // ���̃x�N�g���̕�����V���Ƃ��锼�V������̕��ˏƓx�̑��a
        float rsum(0.0f), gsum(0.0f), bsum(0.0f);

        for (unsigned int i = 0; i < samples; ++i)
        {
          // �V��Ɍ������x�N�g��
          const GLfloat &px(rsampler[i][0]);
          const GLfloat &py(rsampler[i][1]);
          const GLfloat &pz(rsampler[i][2]);
          const GLfloat &pq(rsampler[i][3]);  // p �� q �̓���

          // ���̃x�N�g�� p ���V��摜�̗̈�̊O (����) ���w���Ă���Ƃ�
          if (py <= 0.0f)
          {
            // �����������Z����
            rsum += ramb;
            gsum += gamb;
            bsum += bamb;
            continue;
          }

          // ���̃x�N�g���� xz ���ʏ�̒����� 2 ��
          const GLfloat l(1.0f - py * py);

          // ���̃x�N�g���� xz ���ʏ�̒����ɑ΂���V���p���狁�߂��V��摜�̒��S����̐��K�����ꂽ�����̔�
          const GLfloat r(l > 0.0f ? acos(py) * 2.0f / (float(M_PI) * sqrt(l)) : 0.0f);

          // ���̃x�N�g���̌����Ă�������̓V��摜�ɂ����鐳�K�����ꂽ���W�l (-1 �� u, v �� 1)
          const GLfloat su(px * r);
          const GLfloat sv(pz * r);

          // ���̉�f�̓V��摜��̉�f�ʒu
          const int ss(int(round(float(rs) * su)) - cs);
          const int st(ct - int(round(float(rt) * sv)));

          // ���̉�f�̓V��摜�̔z�� src �̃C���f�b�N�X
          const int is((st * width + ss) * channels);

          // �V��摜 src �̉�f�l����ˏƓx�}�b�v dst �̉�f�ɉ��Z����
          rsum += float(src[is + 2]);
          gsum += float(src[is + 1]);
          bsum += float(src[is + 0]);
        }

        // ���ˏƓx�}�b�v�̉�f�l�̕��ς����߂�
        dst[id + 0] = GLubyte(round(rsum / float(samples)));
        dst[id + 1] = GLubyte(round(gsum / float(samples)));
        dst[id + 2] = GLubyte(round(bsum / float(samples)));
      }
    }

    // �T���v���Ɏg�������������J������
    delete[] sampler;
    delete[] rsampler;
  }

  //
  // ���ˏƓx�}�b�v�̍쐬
  //
  bool createMap(const char *name, GLsizei diameter,
    GLuint imap, GLsizei isize, unsigned int isamples,
    GLuint emap, GLsizei esize, unsigned int esamples,
    const GLfloat *amb, GLfloat shi)
  {
    // �쐬�����e�N�X�`���̐�
    static int count(0);

    // �ǂݍ��񂾉摜�̕��ƍ���, �t�H�[�}�b�g
    GLsizei width, height;
    GLenum format;

    // �V��摜�t�@�C���̓ǂݍ���
    GLubyte const *const texture(ggLoadTga(name, &width, &height, &format));

    // �摜���ǂݍ��߂Ȃ���ΏI��
    if (!texture) return false;

    // ���̉摜�̒��S�ʒu
    const GLsizei cx(width / 2), cy(height / 2);

    // diameter, width, height �̍ŏ��l�� 1 / 2 �� radius �ɂ���
    const GLsizei radius(std::min(diameter, std::min(width, height)) / 2);

    // �����������ˏƓx�}�b�v�̈ꎞ�ۑ���
    std::vector<GLubyte> itemp(isize * isize * 3);

    // ���ˏƓx�}�b�v�p�ɕ�������
    smooth(texture, width, height, format, cx, cy, radius, radius, isamples, &itemp[0], isize, amb, 1.0f);

    // ���ˏƓx�}�b�v�̃e�N�X�`�����쐬����
    createTexture(&itemp[0], isize, isize, GL_RGB, amb, imap);

    // �쐬�����e�N�X�`����ۑ�����
    std::stringstream imapname;
    imapname << "irr" << std::setfill('0') << std::setw(5) << std::right << count << ".tga";
    ggSaveTga(isize, isize, 3, &itemp[0], imapname.str().c_str());

    // �����������}�b�v�̈ꎞ�ۑ���
    std::vector<GLubyte> etemp(esize * esize * 3);

    // ���}�b�v�p�ɕ�������
    smooth(texture, width, height, format, cx, cy, radius, radius, esamples, &etemp[0], esize, amb, shi);

    // ���}�b�v�̃e�N�X�`�����쐬����
    createTexture(&etemp[0], esize, esize, GL_RGB, amb, emap);

    // �쐬�����e�N�X�`����ۑ�����
    std::stringstream emapname;
    emapname << "env" << std::setfill('0') << std::setw(5) << std::right << count << ".tga";
    ggSaveTga(esize, esize, 3, &etemp[0], emapname.str().c_str());

    // �ǂݍ��񂾃f�[�^�͂����g��Ȃ��̂Ń��������������
    delete[] texture;

    // �쐬�����e�N�X�`���̐��𐔂���
    ++count;

    return true;
  }
#endif

  //
  // ���ˏƓx�}�b�v�Ɏg���e�N�X�`�����j�b�g�̐ݒ�
  //
  void irradiance()
  {
    // �e�N�X�`�����W�ɖ@���x�N�g�����g��
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);

    // ���ˏƓx�}�b�v�̒l�������グ���� Ce �� Cb + Ct
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);          // ���Z
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_CONSTANT);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);   // Cb �� GL_TEXTURE_ENV_COLOR �� RGB �l
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);   // Ct �� ���ˏƓx�}�b�v�̒l

    // �e�N�X�`�����W�̕ϊ��s��ɕ����ʃ}�b�s���O�p�̕ϊ��s���ݒ肷��
    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf(paraboloid);
  }

  //
  // �g�U���ˌ����x�̎Z�o�ɂ����e�N�X�`�����j�b�g�̐ݒ�
  //
  void diffuse()
  {
    // ���̂̐F�i���_�F�̕�Ԓl�j�ɑO���C���ŋ��߂����ˌ����x�������� Cd �� Cv * Ce
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);     // ��Z
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PRIMARY_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);   // Cv �� ���_�F�̕�Ԓl
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);   // Ce �� �����グ�������ˏƓx (�O���C��)
  }

  //
  // ���}�b�v�̉��Z�Ɏg���e�N�X�`�����j�b�g�̐ݒ�
  //
  void reflection()
  {
    // �e�N�X�`�����W�ɔ��˃x�N�g�����g��
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);

    // ���}�b�v�̒l�ƑO���C���ŋ��߂��g�U���ˌ����x�����ʔ��ˌW���Ŕ��z������ C �� Ct * Cs + Cd * (1 - Cs)
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);  // ���
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);   // Ct �� ���}�b�v�̒l
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);   // Cd �� �g�U���ˌ����x (�O���C��)
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);   // Cs �� ���ʔ��ˌW��

    // �e�N�X�`�����W�̕ϊ��s��ɕ����ʃ}�b�s���O�p�̕ϊ��s���ݒ肷��
    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf(paraboloid);
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

  // �A�e�t���𖳌��ɂ���
  glDisable(GL_LIGHTING);

  // �e�N�X�`��
  GLuint imap[mapcount], emap[mapcount];
  glGenTextures(mapcount, imap);
  glGenTextures(mapcount, emap);

  // �e�N�X�`���̓ǂݍ���
  for (size_t i = 0; i < mapcount; ++i)
  {
#if USEMAP
    loadMap(irrmaps[i], envmaps[i], imap[i], emap[i]);
#else
    createMap(skymaps[i], skysize, imap[i], imapsize, isamples, emap[i], emapsize, esamples, ambient, shininess);
#endif
  }

  // ���ˏƓx�}�b�v�̂����グ�Ɏg���e�N�X�`�����j�b�g�̐ݒ�
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  irradiance();

  // ���ˏƓx�}�b�v�̂����グ�Ɏg���e�N�X�`�����j�b�g�̐ݒ�
  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  diffuse();

  // ���}�b�v�̉��Z�Ɏg���e�N�X�`�����j�b�g�̐ݒ�
  glActiveTexture(GL_TEXTURE2);
  glEnable(GL_TEXTURE_2D);
  reflection();

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

    // �e�N�X�`���̑I��
    const int select(window.getSelection() % mapcount);

    // ���邳
    GLfloat brightness[4];
    window.getBrightness(brightness);

    // ���ˏƓx�}�b�v�̂����グ
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imap[select]);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, brightness);

    // �g�U���ˌ����x�̎Z�o
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, imap[select]);

    // ���}�b�s���O
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emap[select]);

    // ���f���r���[�ϊ��s��̐ݒ�
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ���_�̈ړ�
    glTranslatef(window.getPosition()[0], window.getPosition()[1], window.getPosition()[2]);

    // �g���b�N�{�[�������ɂ���]
    glMultMatrixf(window.getTb());

    // �V�[���̕`��
    scene(ng, group, diff, spec, nv, pos, norm);

    // ���̕`��
    floor(5, -1.0f);

    // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
    window.swapBuffers();
  }
}
