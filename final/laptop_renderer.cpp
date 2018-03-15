#include "common.h"

struct MyApp : public App {
  vector<Vec3f> pos;
  Data data;

  MyApp() {
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

    initWindow();
  }

  cuttlebone::Taker<State> taker;
  State* state = new State;
  virtual void onAnimate(double dt) {
    taker.get(*state);
    nav().set(state->pose);
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
};

int main() {
  MyApp app;
  app.taker.start();
  app.start();
}
