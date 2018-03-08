/*
AlloGLV Example: App GUI

Description:
This demonstrates how to add a GUI to an al::App to control some properties of
a sphere.

Author:
Lance Putnam, Nov. 2013, putnam.lance@gmail.com
*/

#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <math.h>
#include "allocore/io/al_App.hpp"
#include "alloGLV/al_ControlGLV.hpp"
#include "GLV/glv.h"
#include "Cuttlebone/Cuttlebone.hpp"
#include "common.h"
using namespace al;
using namespace std;

template <typename Out>
void split(const string& s, char delim, Out result) {
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)) {
    *(result++) = item;
  }
}

vector<string> split(const string& s, char delim) {
  vector<string> elems;
  split(s, delim, back_inserter(elems));
  return elems;
}

struct Row {
  string country;
  float latitude;
  float longitude;
  vector<int> monthData;
  vector<int> colors;
  //vector<vector<int>> monthData;
};

struct Data {
  vector<Row> row;

  void load(string filePath) {
    printf("load\n");
    ifstream file(filePath);
    string line;

    if(file.is_open() == false){
      printf("error\n");
    } else printf("ok\n");

    while (getline(file, line)) {

      for(auto& s : split(line, '\r')) {
        vector<string> data = split(s, ',');

        Row r;
        r.country = data[0];
        r.latitude = stof(data[1]);
        r.longitude = stof(data[2]);
        r.monthData.resize(data.size() - 3);
        r.colors.resize(data.size() - 3);
        for (int i = 0; i < r.monthData.size(); ++i){
          r.monthData[i] = stoi(data[i + 3]);
          r.colors[i] = stoi(data[i + 3]);
          //cout << r.monthData[i] << " " << i << endl;
        }        
        for(int i = 0; i < data.size();i++) {
          //printf("%s\t", data[i].c_str());
        }
        row.push_back(r);
        //cout << endl;
      }
    }
  }
};

// enum Mode {
//  MANUAL,
//  SLIDES
// };

class MyApp : public App{
public:
  cuttlebone::Taker<State> taker;
  State* state = new State;

  Material material;
  Light light;
  Mesh mesh;
  //Mesh sphere;
  vector<Vec3f> pos;
  float worldradius = 1;
  //vector<Spheres> spheres;
  Data data;
  int radius = 0;
  //vector<int> radius;
  // vector<int> radius2;
  // vector<int> radius3;
  vector<vector<int>> size;
  vector<int> color;
  double time =0;
  double scale = .001;
  int radiusCounter = 0;
  //vector<vector<Color>> c;
  //double fine = 0, course = 0;
  double temptime;
  int inttime =0;
  int lastime = 0;



  GLVBinding gui;
  // glv::Slider slider;
  // glv::ColorPicker colorPicker;
  glv::Slider2D slider2d;
  glv::Slider slider;
  glv::Slider timeslide;
  glv::Table layout;
  float framenum = 0;

  // Mesh sphere;
  // float scaling;
  // Vec2f position;

  MyApp(){

      data.load("./ariella.gilmore/fp/justnumbers2_1.csv");

      addSphere(mesh);
      mesh.generateNormals();
    for (int i = 0; i < data.row.size(); i++) {
        Vec3f position;
        position.x  = -worldradius * cos((data.row[i].latitude)*(3.14/180))*cos((data.row[i].longitude)*(3.14/180));
        position.y  = worldradius * sin((data.row[i].latitude)*(3.14/180));
        position.z  = worldradius * cos((data.row[i].latitude)*(3.14/180))*sin((data.row[i].longitude)*(3.14/180));

        //c = HSV(255, 0, 1);
        
        //color.push_back(data.row[i].monthData[0]);
        // addSphere(sphere, amount[i]/100);
        // sphere.translate(position);
        //mesh.vertex(position);
        pos.push_back(position);
        for(int j = 0; j < 96; j++){
          //cout << i << " "<<  j<< " " << data.row[i].monthData[j] << " ";
        //outgoing = 0 + (10 - 0) * ((data.row[i].monthData[j] - 0) / (100 - 0));
        int outgoing = 5 + (data.row[i].monthData[j] - 0) * (100 - 5) / (100 - 0);
        int outgoingcolor = 0 + (data.row[i].colors[j] - 0) * (255 - 0) / (100 - 0);
        //cout << outgoing << endl;
        data.row[i].monthData[j] = outgoing;
        //cout << outgoing << endl;
        //data.row[i].monthData[j] = outgoing;
        //cout << data.row[i].monthData[j]*scale << endl;
        
        data.row[i].colors[j] = outgoingcolor;

        //cout << "here" << endl;
      }
    }

    nav().pos().set(0,0,4);
    initWindow();

    // Connect GUI to window
    gui.bindTo(window());

    // Configure GUI
    gui.style().color.set(glv::Color(0.7), 0.5);

    layout.arrangement(">p");

    slider.setValue(0);
    slider.interval(0,0.007);
    layout << slider;
    layout << new glv::Label("scale");

    timeslide.setValue(1);
    timeslide.interval(.1,8);
    layout << timeslide;
    layout << new glv::Label("time");

    layout.arrange();

    gui << layout;
  }
  double course;
  double tscale;
  double changetscale = 0;
  double change =0;
  double lastchange =0;
  double Ta = 0, Tb = 0;
  virtual void onAnimate(double dt){
    taker.get(*state);
    radius = state->radi;
    time = state->timer;
    tscale = state->secondslide;
    course = (state->firstslide)*-1;
    slider.setValue(course);
    timeslide.setValue(tscale);
    }
  virtual void onDraw(Graphics& g, const Viewpoint& v) {
    g.rotate(time*30*tscale,0,1,0);
    material();
    //light();
    for(int i = 0; i < data.row.size(); i++){
      g.pushMatrix();
      //g.color(255);
      for(int j = 0; j < 96; j++){
        //g.color(HSV(.6,0,1));
        g.color(RGB(.2 ,data.row[i].colors[j] / 255.0,data.row[i].colors[j]/ 255.0));
        //cout << data.row[i].colors[j]/255.0 << endl;;
      }
      g.translate(pos[i] + pos[i]*data.row[i].monthData[radius] * course);
      // if(data.row[i].monthData[radius] == 0){
      //  data.row[i].monthData[radius] = 5;
      // }
      g.scale(data.row[i].monthData[radius]*scale);
      g.draw(mesh);
      g.popMatrix();
      
    }
  }



};

int main(){
  MyApp app; app.taker.start(); app.start(); 
}
