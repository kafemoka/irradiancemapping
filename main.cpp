#define _USE_MATH_DEFINES
#define NOMINMAX
#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

// 事前計算したマップを使用するなら 1
#define USEMAP 1

// ウィンドウ関連の処理
#include "Window.h"

namespace
{
  //
  // 三角形分割した Alias OBJ 形式の形状データファイル
  //
  const char filename[] = "bunny.obj";

#if USEMAP
  //
  // 放射照度マップ
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
  // 環境マップ
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
  // 放射照度マップの数
  //
  const size_t mapcount(sizeof irrmaps / sizeof irrmaps[0]);
#else
  //
  // 距離射影方式の魚眼レンズで撮影した天空画像
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
  // 天空画像の数
  //
  const size_t mapcount(sizeof skymaps / sizeof skymaps[0]);

  //
  // 大域環境光強度
  //
  const GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

  //
  // 輝き係数
  //
  const GLfloat shininess(60.0f);

  //
  // 天空画像中の天空領域の直径の最大値
  //
  const GLsizei skysize(1024);

  //
  // 作成するテクスチャのサイズ
  //
  const GLsizei mapsize(256);
#endif

  //
  // 放物面マッピング用のテクスチャ変換行列
  //
  const GLfloat paraboloid[] =
  {
    -1.0f,  0.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  0.0f,  2.0f,
     0.0f, -1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  0.0f,  2.0f,
  };

  //
  // 床の描画
  //
  void floor(int size, GLfloat height)
  {
    // タイル１枚の頂点データ
    static const GLfloat floorvert[] =
    {
      0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 0.0f
    };

    // タイルの拡散反射係数
    static const GLfloat floordiff[][4] =
    {
      { 0.6f, 0.6f, 0.6f, 1.0f },
      { 0.3f, 0.3f, 0.3f, 1.0f }
    };

    // タイルの鏡面反射係数
    static const GLfloat floorspec[] =
    {
      0.1f, 0.1f, 0.1f, 1.0f
    };

    // 床のポリゴンの頂点位置を頂点配列から取得する
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, floorvert);

    // 床のポリゴンの法線ベクトルは glNormal3f() で設定する
    glDisableClientState(GL_NORMAL_ARRAY);
    glNormal3f(0.0f, 1.0f, 0.0f);

    // 床のポリゴンの拡散反射光と鏡面反射光を鏡面反射係数で配分する
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, floorspec);

    // 床の描画
    for (int j = -size; j < size; ++j)
    {
      for (int i = -size; i < size; ++i)
      {
        // 床のポリゴンの拡散反射係数を primary color (頂点色) に設定する
        glColor3fv(floordiff[(i + j) & 1]);

        // 床のポリゴンを描画する
        glPushMatrix();
        glTranslatef(GLfloat(i), height, GLfloat(j));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();
      }
    }
  }

  //
  // シーンの描画
  //
  void scene(GLuint ng, const GLuint (*group)[2], const GLfloat (*diff)[4], const GLfloat (*spec)[4],
    GLuint nv, const GLfloat (*pos)[3], const GLfloat (*norm)[3])
  {
    // オブジェクトの頂点位置を頂点配列から取得する
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, pos);

    // オブジェクトの法線ベクトルを頂点配列から取得する
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, norm);

    // オブジェクトの描画
    for (unsigned int g = 0; g < ng; ++g)
    {
      // 拡散反射光と鏡面反射光を鏡面反射係数で配分する
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, spec[g]);

      // 拡散反射係数を primary color (頂点色) に設定する
      glColor4fv(diff[g]);

      // オブジェクトを描画する
      glDrawArrays(GL_TRIANGLES, group[g][0], group[g][1]);
    }
  }

  //
  // テクスチャの作成
  //
  void createTexture(const GLubyte *buffer, GLsizei width, GLsizei height, GLenum format,
    const GLfloat *amb, GLuint tex)
  {
    // テクスチャオブジェクトにテクスチャを割り当てる
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, buffer);

    // テクスチャは線形補間する
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // 境界色を拡張する
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // テクスチャの境界色に大域環境光を設定する
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, amb);
  }

#if USEMAP
  //
  // 放射照度マップの読み込み
  //
  bool loadMap(const char *iname, const char *ename, GLuint imap, GLuint emap)
  {
    // 終了ステータス
    bool status(true);

    // 読み込んだ画像の幅と高さ, フォーマット
    GLsizei width, height;
    GLenum format;

    // 放射照度マップの読み込み
    GLubyte const *const itexture(ggLoadTga(iname, &width, &height, &format));

    // 画像が読み込めなければ終了
    if (!itexture) status = false;

    // 読み込んだ画像の左上隅の画素の色を大域環境光とする
    const GLfloat iamb[] = { itexture[2] / 255.0f, itexture[1] / 255.0f, itexture[0] / 255.0f };

    // テクスチャの作成
    createTexture(itexture, width, height, format, iamb, imap);

    // 読み込んだデータはもう使わないのでメモリを解放する
    delete[] itexture;

    // 放射照度マップの読み込み
    GLubyte const *const etexture(ggLoadTga(ename, &width, &height, &format));

    // 画像が読み込めなければ終了
    if (!etexture) status = false;

    // 読み込んだ画像の左上隅の画素の色を大域環境光とする
    const GLfloat eamb[] = { etexture[2] / 255.0f, etexture[1] / 255.0f, etexture[0] / 255.0f };

    // テクスチャの作成
    createTexture(etexture, width, height, format, eamb, emap);

    // 読み込んだデータはもう使わないのでメモリを解放する
    delete[] etexture;

    // すべて読み込み成功
    return status;
  }
#else
  //
  // 平滑化
  //
  void smooth(const GLubyte *src, GLsizei width, GLsizei height, GLenum format, GLsizei area,
    GLubyte *dst, GLsizei size, const GLfloat *amb, GLfloat shi)
  {
    // チャンネル数
    const int channels(format == GL_BGRA ? 4 : 3);

    // src に対する dst の拡大率
    const float scale(float(area) / float(size));

    // dst に対する src の中心のオフセット
    const float ox(float(width - area) * 0.5f);
    const float oy(float(height - area) * 0.5f);

    // 大域環境光強度
    const GLfloat ramb(amb[0] * 255.0f), gamb(amb[1] * 255.0f), bamb(amb[2] * 255.0f);

    // dst の各画素について
    for (int dy = 0; dy < size; ++dy)
    {
      std::cout << "Processing line: " << dy
        << " (" << std::fixed << std::setprecision(1) << float(dy) * 100.0f / float(size) << "%)"
        << std::endl;

      for (int dx = 0; dx < size; ++dx)
      {
        // この画素の dst のインデックス
        const int id((dy * size + dx) * 3);

        // この画素の dst 上の正規化された座標値 (-1 ≦ x, y ≦ 1)
        const float x((float(dx * 2) / float(size - 1) - 1.0f));
        const float y((float(dy * 2) / float(size - 1) - 1.0f));

        // この画素の dst の中心からの距離の二乗
        const float r(x * x + y * y);

        // この画素が dst 上の単位円外にあるとき
        if (0 && r >= 1.0f)
        {
          // 単位円外は大域環境光とする
          dst[id + 0] = GLubyte(round(ramb));
          dst[id + 1] = GLubyte(round(gamb));
          dst[id + 2] = GLubyte(round(bamb));
          continue;
        }

        // dst の中心からの距離を天頂角とする方向ベクトルの Z 成分
        const float z(cos(sqrt(r) * M_PI * 0.5f));

        // dst におけるこの画素の方向ベクトルの x, y 成分の長さ
        const float d(sqrt((1.0f - z * z) / r));

        // dst におけるこの画素の方向単位ベクトル (nx, ny, nz)
        const float nx(x * d);
        const float ny(y * d);
        const float nz(z);

        // dst のこの画素の src における対応する画素の位置
        const int cx(int(round(scale * float(dx) + ox)));
        const int cy(int(round(scale * float(dy) + oy)));

        // このベクトルの方向を天頂とする半天球からの放射照度の総和
        float rsum(0.0f), gsum(0.0f), bsum(0.0f);

        // 半天球の重み付け立体角の総和
        float wtotal(0.0f);

        // src の (cx, cy) を中心とし area x area の範囲の各画素について
        for (int ry = 0; ry < area; ++ry)
        {
          for (int rx = 0; rx < area; ++rx)
          {
            // この画素の src 上の位置
            const int sx(cx + rx - area / 2);
            const int sy(cy + ry - area / 2);

            // この画素の src 上の正規化された座標値 (-1 ≦ x, y ≦ 1)
            const float x((float(sx * 2) / float(area - 1) - 1.0f));
            const float y((float(sy * 2) / float(area - 1) - 1.0f));

            // この画素の src の中心からの距離の二乗
            const float r(x * x + y * y);

            // src の中心からの距離を天頂角とする方向ベクトルの Z 成分
            const float z(cos(sqrt(r) * M_PI * 0.5f));

            // 方向ベクトルの x, y 成分の長さを求める
            const float d(sqrt((1.0f - z * z) / r));

            // src におけるこの画素の方向単位ベクトル (lx, ly, lz)
            const float lx(x * d);
            const float ly(y * d);
            const float lz(z);

            // n と l の内積
            const float nl(nx * lx + ny * ly + nz * lz);

            // この画素が (nx, ny, nz) 方向の裏側にあるとき
            if (nl <= 0.0f) continue;

            // この点の立体角
            const float theta(acos(nl));
            const float sr(theta > 0.0f ? sqrt(1.0f - nl * nl) / theta : 1.0f);

            // shininess を反映する
            const float ns(pow(nl, shi) * sr);

            // 重み付け立体角を積算する
            wtotal += ns;

            // この画素が src 上の単位円外にあるとき
            if (r >= 1.0f)
            {
              // 大域環境光を加算する
              rsum += ramb * ns;
              gsum += gamb * ns;
              bsum += bamb * ns;
              continue;
            }

            // この画素の src のインデックス
            const int is((sy * width + sx) * channels);

            // src の画素値を dst に加算する
            rsum += float(src[is + 2]) * ns;
            gsum += float(src[is + 1]) * ns;
            bsum += float(src[is + 0]) * ns;
          }
        }

        // 重み付け立体角の総和（天空の面積）で割る
        dst[id + 0] = GLubyte(round(rsum / wtotal));
        dst[id + 1] = GLubyte(round(gsum / wtotal));
        dst[id + 2] = GLubyte(round(bsum / wtotal));
      }
    }
  }

  //
  // 放射照度マップの作成
  //
  bool createMap(const char *name, GLsizei radius, GLuint imap, GLuint emap, GLsizei size,
    const GLfloat *amb, GLfloat shi)
  {
    // 作成したテクスチャの数
    static int count(0);

    // 読み込んだ画像の幅と高さ, フォーマット
    GLsizei width, height;
    GLenum format;

    // 天空画像ファイルの読み込み
    GLubyte const *const texture(ggLoadTga(name, &width, &height, &format));

    // 画像が読み込めなければ終了
    if (!texture) return false;

    // テクスチャの平滑結果の一時保存先
    std::vector<GLubyte> temp(size * size * 3);

    // radius, width, height の最小値を radius にする
    radius = std::min(radius, std::min(width, height));

    // 放射照度マップ用に平滑する
    smooth(texture, width, height, format, radius, &temp[0], size, amb, 1.0f);

    // 放射照度マップのテクスチャを作成する
    createTexture(&temp[0], width, height, GL_RGB, amb, imap);

    // 作成したテクスチャを保存する
    std::stringstream imapname;
    imapname << "irrmap" << std::setfill('0') << std::setw(5) << std::right << count << ".tga";
    ggSaveTga(size, size, 3, &temp[0], imapname.str().c_str());

    // 環境マップ用に平滑する
    smooth(texture, width, height, format, radius, &temp[0], size, amb, shi);

    // 環境マップのテクスチャを作成する
    createTexture(&temp[0], width, height, GL_RGB, amb, emap);

    // 作成したテクスチャを保存する
    std::stringstream emapname;
    emapname << "envmap" << std::setfill('0') << std::setw(5) << std::right << count << ".tga";
    ggSaveTga(size, size, 3, &temp[0], emapname.str().c_str());

    // 読み込んだデータはもう使わないのでメモリを解放する
    delete[] texture;

    // 作成したテクスチャの数を数える
    ++count;
  }
#endif

  //
  // 放射照度マップに使うテクスチャうニットの設定
  //
  void irradiance()
  {
    // テクスチャ座標に法線ベクトルを使う
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);

    // 放射照度マップの値をかさ上げする Ce ← Cb + Ct
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);            // 加算
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_CONSTANT);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);     // Cb ← GL_TEXTURE_ENV_COLOR の RGB 値
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);     // Ct ← 放射照度マップの値

    // テクスチャ座標の変換行列に放物面マッピング用の変換行列を設定する
    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf(paraboloid);
  }

  //
  // 拡散反射光強度の算出につかうテクスチャユニットの設定
  //
  void diffuse()
  {
    // 物体の色（頂点色の補間値）に前レイヤで求めた入射光強度をかける Cd ← Cv * Ce
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);     // 乗算
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PRIMARY_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);   // Cv ← 頂点色の補間値
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);   // Ce ← かさ上げした放射照度 (前レイヤ)
  }

  //
  // 環境マップの加算に使うテクスチャユニットの設定
  //
  void reflection()
  {
    // テクスチャ座標に反射ベクトルを使う
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);

    // 環境マップの値と前レイヤで求めた拡散反射光強度を鏡面反射係数で比例配分する C ← Ct * Cs + Cd * (1 - Cs)
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);  // 補間
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);   // Ct ← 環境マップの値
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);   // Cd ← 拡散反射光強度 (前レイヤ)
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);   // Cs ← 鏡面反射係数

    // テクスチャ座標の変換行列に放物面マッピング用の変換行列を設定する
    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf(paraboloid);
  }

  //
  // プログラム終了時の処理
  //
  void cleanup()
  {
    // GLFW の終了処理
    glfwTerminate();
  }
}

//
// メインプログラム
//
int main()
{
  // GLFW を初期化する
  if (glfwInit() == GL_FALSE)
  {
    // 初期化に失敗した
    std::cerr << "Can't initialize GLFW" << std::endl;
    return 1;
  }

  // プログラム終了時の処理を登録する
  atexit(cleanup);

  // ウィンドウを作成する
  Window window("Irradiance Mapping", 960, 540);

  // OpenGL の初期設定
  glClearColor(0.3f, 0.5f, 0.8f, 0.0f);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);

  // 陰影付けを無効にする
  glDisable(GL_LIGHTING);

  // テクスチャ
  GLuint imap[mapcount], emap[mapcount];
  glGenTextures(mapcount, imap);
  glGenTextures(mapcount, emap);

  // テクスチャの読み込み
  for (size_t i = 0; i < mapcount; ++i)
  {
#if USEMAP
    loadMap(irrmaps[i], envmaps[i], imap[i], emap[i]);
#else
    createMap(skymaps[i], skysize, imap[i], emap[i], mapsize, ambient, shininess);
#endif
  }

  // 放射照度マップのかさ上げに使うテクスチャユニットの設定
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  irradiance();

  // 放射照度マップのかさ上げに使うテクスチャユニットの設定
  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  diffuse();

  // 環境マップの加算に使うテクスチャユニットの設定
  glActiveTexture(GL_TEXTURE2);
  glEnable(GL_TEXTURE_2D);
  reflection();

  // 材質データ
  GLuint ng;
  GLuint (*group)[2];
  GLfloat (*amb)[4], (*diff)[4], (*spec)[4], *shi;

  // 形状データ
  GLuint nv;
  GLfloat (*pos)[3], (*norm)[3];

  // 形状データの読み込み
  ggLoadObj(filename, ng, group, amb, diff, spec, shi, nv, pos, norm, false);

  // ウィンドウが開いている間繰り返す
  while (window.shouldClose() == GL_FALSE)
  {
    // ウィンドウを消去する
    window.clear();

    // テクスチャの選択
    const int select(window.getSelection() % mapcount);

    // 明るさ
    GLfloat brightness[4];
    window.getBrightness(brightness);

    // 放射照度マップのかさ上げ
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imap[select]);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, brightness);

    // 拡散反射光強度の算出
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, imap[select]);

    // 環境マッピング
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emap[select]);

    // モデルビュー変換行列の設定
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 視点の移動
    glTranslatef(window.getPosition()[0], window.getPosition()[1], window.getPosition()[2]);

    // トラックボール処理による回転
    glMultMatrixf(window.getTb());

    // シーンの描画
    scene(ng, group, diff, spec, nv, pos, norm);

    // 床の描画
    floor(5, -1.0f);

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }
}
