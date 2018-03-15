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
  glv::Table layout;

  SoundSource aSoundSource;

  MyApp()
      : maker(Simulator::defaultBroadcastIP()),
        InterfaceServerClient(Simulator::defaultInterfaceServerIP()) {
    data.load(fullPathOrDie("justnumbers2_1.csv"));

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
    initWindow();

    gui.bindTo(window());
    gui.style().color.set(glv::Color(0.7), 0.5);
    layout.arrangement(">p");

    scaleSlider.setValue(0);
    scaleSlider.interval(0, 0.007);
    layout << scaleSlider;
    layout << new glv::Label("scale");

    rateSlider.setValue(1);
    rateSlider.interval(.1, 8);
    layout << rateSlider;
    layout << new glv::Label("rate");

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
  virtual void onAnimate(double dt) {
    while (InterfaceServerClient::oscRecv().recv())
      ;  // XXX

    state->angle += rateSlider.getValue();
    state->pose = nav();

    static double timer = 0;
    timer += dt;
    if (timer > 3) {
      timer -= 3;
      state->indexOfDataSet++;
      if (state->indexOfDataSet >= data.row[0].monthData.size())
        state->indexOfDataSet = 0;
    }

    state->course = -scaleSlider.getValue();

    state->pose = nav();
    maker.set(*state);
  }

  Material material;
  Light light;
  Mesh sphere;
  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    g.rotate(state->angle, 0, 1, 0);
    material();
    light();

    for (int i = 0; i < data.row.size(); i++) {
      g.pushMatrix();
      for (int j = 0; j < data.row[0].monthData.size(); j++) {
        g.color(RGB(.2, data.row[i].colors[j] / 255.0,
                    data.row[i].colors[j] / 255.0));
      }
      g.translate(pos[i] + pos[i] *
                               data.row[i].monthData[state->indexOfDataSet] *
                               state->course);
      double scale = .001;
      g.scale(data.row[i].monthData[state->indexOfDataSet] * scale);
      g.draw(sphere);
      g.popMatrix();
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
