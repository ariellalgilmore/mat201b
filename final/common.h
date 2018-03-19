#ifndef __ARI_COMMON__
#define __ARI_COMMON__

#include <math.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Cuttlebone/Cuttlebone.hpp"
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
};

struct Data {
  vector<Row> row;

  void load(string filePath) {
    printf("load\n");
    ifstream file(filePath);
    string line;

    if (file.is_open() == false) {
      printf("error\n");
    } else
      printf("ok\n");

    while (getline(file, line)) {
      for (auto& s : split(line, '\r')) {
        vector<string> data = split(s, ',');

        Row r;
        r.country = data[0];
        r.latitude = stof(data[1]);
        r.longitude = stof(data[2]);
        r.monthData.resize(data.size() - 3);
        r.colors.resize(data.size() - 3);
        for (int i = 0; i < r.monthData.size(); ++i) {
          r.monthData[i] = stoi(data[i + 3]);
          r.colors[i] = stoi(data[i + 3]);
        }
        row.push_back(r);
      }
    }
  }
};

Mesh backMesh;
Texture backTexture;

string months[] = {"January",   "February", "March",    "April",
                   "May",       "June",     "July",     "August",
                   "September", "October",  "November", "December"};
string years[] = {"2008", "2009", "2010", "2011", "2012",
                  "2013", "2014", "2015", "2016", "2017"};

struct State {
  Pose pose;
  double angle;
  double course;
  int indexOfDataSet;
  int turnOnLabels;
  int rotation;
};

const char* fileList[] = {"Australia.png",
                          "Cyprus.png",
                          "Serbia.png",
                          "New Zealand.png",
                          "Bosnia & Herzegovina.png",
                          "Argentina.png",
                          "Switzerland.png",
                          "Singapore.png",
                          "Hong Kong.png",
                          "Chile.png",
                          "Pakistan.png",
                          "Dominican Republic.png",
                          "United Arab Emirates.png",
                          "France.png",
                          "Malaysia.png",
                          "Croatia.png",
                          "Israel.png",
                          "South Africa.png",
                          "Belgium.png",
                          "Slovakia.png",
                          "Slovenia.png",
                          "Japan.png",
                          "Netherlands.png",
                          "Philippines.png",
                          "Colombia.png",
                          "Czechia.png",
                          "United Kingdom.png",
                          "United States.png",
                          "Bulgaria.png",
                          "Romania.png",
                          "Canada.png",
                          "India.png",
                          "Ireland.png",
                          "Spain.png",
                          "Thailand.png",
                          "Austria.png",
                          "Peru.png",
                          "Italy.png",
                          "Sweden.png",
                          "Greece.png",
                          "Mexico.png",
                          "Poland.png",
                          "Hungary.png",
                          "Taiwan.png",
                          "Germany.png",
                          "Finland.png",
                          "Denmark.png",
                          "Portugal.png",
                          "Vietnam.png",
                          "Venezuela.png",
                          "Indonesia.png",
                          "Morocco.png",
                          "Egypt.png",
                          "Russia.png",
                          "Norway.png",
                          "Ukraine.png",
                          "Brazil.png",
                          "Lithuania.png",
                          "Turkey.png",
                          "China.png",
                          "Saudi Arabia.png",
                          "Qatar.png",
                          "Uruguay.png",
                          "Costa Rica.png",
                          "Puerto Rico.png",
                          "South Korea.png",
                          "Ecuador.png",
                          "Iran.png",
                          "Latvia.png",
                          "Luxembourg.png",
                          "Tunisia.png",
                          "Estonia.png",
                          "Algeria.png",
                          "Kuwait.png",
                          "Kazakhstan.png",
                          "Montenegro.png",
                          "Macedonia.png",
                          "Belarus.png",
                          "Guatemala.png",
                          "Bolivia.png",
                          "Iraq.png",
                          "Nigeria.png",
                          "Honduras.png",
                          "Kenya.png",
                          "St. Helena.png",
                          "Paraguay.png"};

#define FILE_LIST_N (sizeof(fileList) / sizeof(fileList[0]))

#endif
