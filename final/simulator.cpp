#include "common.h"

#include "GLV/glv.h"
#include "alloGLV/al_ControlGLV.hpp"
#include "alloutil/al_AlloSphereAudioSpatializer.hpp"
#include "alloutil/al_Simulator.hpp"

struct MyApp : App, AlloSphereAudioSpatializer, InterfaceServerClient {
  vector<Vec3f> pos;
  Data data;

  GLVBinding gui;
  glv::Slider2D slider2d;
  glv::Slider scaleSlider;
  glv::Slider rateSlider;
  glv::Slider month;
  glv::Slider year;
  glv::Label monthlabel;
  glv::Label yearlabel;
  glv::Table layout;
  glv::Button labels;
  glv::Label labelslabel;
  glv::Button rotation;
  glv::Label rotationLabel;

  SoundSource aSoundSource;

  Texture texture[FILE_LIST_N];

  MyApp()
      : maker(Simulator::defaultBroadcastIP()),
        InterfaceServerClient(Simulator::defaultInterfaceServerIP()) {
    memset(state, 0, sizeof(State));

    for (int i = 0; i < FILE_LIST_N; i++) {
      Image image;
      string s = fullPathOrDie(fileList[i]);
      cout << s << endl;
      if (!image.load(s)) {
        cerr << "failed to load " << fileList[i] << endl;
        exit(1);
      }
      texture[i].allocate(image.array());
    }

    Image background;

    if (!background.load(fullPathOrDie("possiblebg.png"))) {
      fprintf(stderr, "FAIL\n");
      exit(1);
    }
    backTexture.allocate(background.array());

    addSphereWithTexcoords(backMesh, 1.3);
    backMesh.generateNormals();

    lens().far(1000);

    data.load(fullPathOrDie("finaltennisdata.csv"));

    addSphere(sphere);
    sphere.generateNormals();
    float worldradius = 1;
    for (int i = 0; i < data.row.size(); i++) {
      Vec3f position;
      position.x = -worldradius * cos((data.row[i].latitude) * (3.14 / 180)) *
                   cos((data.row[i].longitude) * (3.14 / 180));
      position.y = worldradius * sin((data.row[i].latitude) * (3.14 / 180));
      position.z = worldradius * cos((data.row[i].latitude) * (3.14 / 180)) *
                   sin((data.row[i].longitude) * (3.14 / 180));

      pos.push_back(position);
      for (int j = 0; j < data.row[0].monthData.size(); j++) {
        int outgoing =
            5 + (data.row[i].monthData[j] - 0) * (100 - 5) / (100 - 0);
        int outgoingcolor =
            0 + (data.row[i].colors[j] - 0) * (255 - 0) / (100 - 0);
        data.row[i].monthData[j] = outgoing;
        data.row[i].colors[j] = outgoingcolor;
      }
    }

    nav().pos().set(0, 0, 4);
    // nav().quat(Quatd(0.96, 0.00, 0.29, 0.00));
    nav().quat(Quatd(0, 0.00, 0, 0.00));
    initWindow();

    App::background(Color(0.2, 1.0));

    gui.bindTo(window());
    gui.style().color.set(glv::Color(0.7), 0.5);
    layout.arrangement(">p");

    scaleSlider.setValue(0);
    scaleSlider.interval(0, 0.007);
    layout << scaleSlider;
    layout << new glv::Label("scale");

    rateSlider.setValue(.01);
    rateSlider.interval(.01, .03);
    layout << rateSlider;
    layout << new glv::Label("rate");

    month.setValue(0);
    month.interval(0, 11);
    layout << month;
    layout << monthlabel.setValue(months[0]);

    year.setValue(0);
    year.interval(0, 9);
    layout << year;
    layout << yearlabel.setValue(years[0]);

    layout << labels;
    layout << labelslabel.setValue("Labels");

    layout << rotation;
    layout << rotationLabel.setValue("rotate");

    layout.arrange();
    gui << layout;

    // audio
    AlloSphereAudioSpatializer::initAudio();
    AlloSphereAudioSpatializer::initSpatialization();
    // if gamma
    // gam::Sync::master().spu(AlloSphereAudioSpatializer::audioIO().fps());
    scene()->addSource(aSoundSource);
    scene()->usePerSampleProcessing(false);
  }

  cuttlebone::Maker<State> maker;
  State* state = new State;
  int monthCounter = 0;
  int yearCounter = 0;
  virtual void onAnimate(double dt) {
    while (InterfaceServerClient::oscRecv().recv())
      ;  // XXX

    state->rotation = rotation.getValue();
    if(rotation.getValue() == 1){
      nav().pos(0,0,0);
      nav().spinU(rateSlider.getValue());
      state->angle = rateSlider.getValue();
    }
    else{
      nav().spinU(0);
    }
    //state->angle += rateSlider.getValue();
    state->pose = nav();

    static double timer = 0;
    timer += dt;
    if (timer > 3) {
      timer -= 3;
      month.setValue(monthCounter);
      monthlabel.setValue(months[monthCounter]);
      year.setValue(yearCounter);
      yearlabel.setValue(years[yearCounter]);
      monthCounter++;
      if (monthCounter == 12) {
        yearCounter++;
        monthCounter = 0;
      }
      state->indexOfDataSet++;
      if (state->indexOfDataSet >= data.row[0].monthData.size())
        state->indexOfDataSet = 0;
    }

    state->course = -scaleSlider.getValue();
    state->turnOnLabels = labels.getValue();
    state->pose = nav();
    maker.set(*state);
  }

  Material material;
  Light light;
  Mesh sphere;
  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    // cout << labels.getValue() << endl;

    if (false) {
      backTexture.bind();
      g.draw(backMesh);
      backTexture.unbind();
    }

    g.depthMask(true);
    g.depthTesting(true);
    g.blending(false);

    material();
    light();

    for (int i = 0; i < data.row.size(); i++) {
      g.pushMatrix();

      // g.rotate(state->angle, 0, 1, 0);

      for (int j = 0; j < data.row[0].monthData.size(); j++) {
        g.color(HSV(data.row[i].colors[j] / 255.0, .4, .5));
      }
      g.translate(pos[i] + pos[i] *
                               data.row[i].monthData[state->indexOfDataSet] *
                               state->course);
      Vec3f src = pos[i] + pos[i] *
                               data.row[i].monthData[state->indexOfDataSet] *
                               state->course;
      double scale = .001;
      g.scale(data.row[i].monthData[state->indexOfDataSet] * scale);
      g.draw(sphere);
      g.popMatrix();
    }

    // g.clear(Graphics::COLOR_BUFFER_BIT);
    g.depthMask(false);
    g.depthTesting(false);
    g.blending(true);
    // g.blendModeAdd();
    g.blendModeTrans();
    if (labels.getValue() == 1) {
      for (int i = 0; i < data.row.size(); i++) {
        Vec3f src = pos[i] + pos[i] *
                                 data.row[i].monthData[state->indexOfDataSet] *
                                 state->course;
        g.pushMatrix();
        // g.translate(.9, 0, .9);
        g.translate(src);
        Vec3d forward = Vec3d(nav().pos() - src).normalize();
        // Vec3d forward = Vec3d(Vec3f(0, 0, 0) - src).normalize();
        Quatd rot = Quatd::getBillboardRotation(forward, nav().uu());
        g.rotate(rot);
        g.scale(0.07);
        texture[i].quad(g);
        g.popMatrix();
      }
    }
  }

  virtual void onSound(AudioIOData& io) {
    aSoundSource.pose(nav());
    while (io()) {
      aSoundSource.writeSample(0);
    }
    listener()->pose(nav());
    scene()->render(io);
  }
};

int main() {
  MyApp app;
  app.AlloSphereAudioSpatializer::audioIO().start();
  app.InterfaceServerClient::connect();
  app.maker.start();
  app.start();
}
