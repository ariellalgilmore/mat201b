// MAT201B
// Winter 2017
// Author(s): Karl Yerkes
//
// Shows how to:
// - Parse a file that's a .csv of floats
//
// This code does not...
// - handle bad (i.e., non-float) values
// - verify that all the rows are the same length
//

#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include "allocore/io/al_App.hpp"
using namespace al;
using namespace std;

string fullPathOrDie(string fileName, string whereToLook = ".") {
  SearchPaths searchPaths;
  searchPaths.addSearchPath(whereToLook);
  string filePath = searchPaths.find(fileName).filepath();
  assert(filePath != "");
  return filePath;
}

class AlloApp : public App {
public: 
  Mesh mesh;
  vector<Vec3f> pos;
  float worldradius = 1;

  AlloApp() {
    // make a vector of vectors of floats
    //
    vector<vector<float>> data;

    // find and open the data file and die unless we have it open
    //
    ifstream f(fullPathOrDie("othercopy.csv", ".."), ios::in);
    if (!f.is_open()) {
      cout << "file not open" << endl;
      exit(1);
    }
    cout << "here" << endl;
    // for each line in the file
    //
    string line;
    while (getline(f, line)) {
      // push an empty "row" on our vector of vectors
      //
      data.push_back(vector<float>());

      // for each item in the line
      //
      stringstream stream(line);
      float v;
      while (stream >> v) {
        // push each value onto the current row
        data.back().push_back(v);

        if (stream.peek() == ',') stream.ignore();
      }
       //cout << "here2" << endl;
    }

    // close the file - we're done
    //
    f.close();

    // print out the data using ' ' as the delimiter
    //
    // for (auto line : data) {
    //   for (auto item : line) {
    //     cout << item << ",";
    //   }
    //   cout << endl;
    // }

    // alternatively, we can use (row, column) indexes
    //
    for (int row = 0; row < 50; row++) {
      for (int column = 0; column < data[row].size(); column++) {
        //cout << data[row][0] << " ";
        Vec3f position;
        position.x  = -worldradius * cos((data[row][0])*(3.14/180))*cos((data[row][1])*(3.14/180));
        position.y  = worldradius * sin((data[row][0])*(3.14/180));
        position.z  = worldradius * cos((data[row][0])*(3.14/180))*sin((data[row][1])*(3.14/180));


        mesh.vertex(position);
        pos.push_back(position);
        mesh.color(abs(data[row][2])*10, 100, 100 );
        //cout << position << endl;
      }
      //cout << endl;
    }




    //cout << "at the end" << endl;
  }
    virtual void onAnimate(double dt){
      for(int i = 0; i < mesh.vertices().size(); i++){
        mesh.vertices()[i];
      }
    }
  virtual void onDraw(Graphics& g) {
    g.draw(mesh);
  }
};


int main() { 
  AlloApp app;
  app.initWindow();
  app.start(); }
