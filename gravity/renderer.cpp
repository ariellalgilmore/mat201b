//ariella gilmore
//MAT 201B
//sublime
//ariellalgilmore@gmail.com
//2/7/18
#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/io/al_App.hpp"
#include "common.hpp"
using namespace al;
using namespace std;

// some of these must be carefully balanced; i spent some time turning them.
// change them however you like, but make a note of these settings.
unsigned particleCount = 50;     // try 2, 5, 50, and 500
double maximumAcceleration = 30;  // prevents explosion, loss of particles
double initialRadius = 50;        // initial condition
double initialSpeed = 50;         // initial condition
double gravityFactor = 1e6;       // see Gravitational Constant
double timeStep = 0.0625;         // keys change this value for effect
double scaleFactor = 0.1;         // resizes the entire scene
double sphereRadius = 3;  // increase this to make collisions more frequent

Mesh sphere;  // global prototype; leave this alone


// helper function: makes a random vector
Vec3f r() { return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()); }

struct Particle {
  Vec3f position, velocity, acceleration;
  Color c;
  Particle() {
    position = r() * initialRadius;
    velocity =
        // this will tend to spin stuff around the y axis
        Vec3f(0, 1, 0).cross(position).normalize(initialSpeed);
    c = HSV(rnd::uniform(), 0.7, 1);
  }
  void draw(Graphics& g) {
    g.pushMatrix();
    g.translate(position);
    g.color(c);
    g.draw(sphere);
    g.popMatrix();
  }
};

struct Phasor {
  float phase = 0, increment = 0.001;
  void frequency(float hz, float sampleRate) { increment = hz / sampleRate; }
  float getNextSample() {
    float returnValue = phase;
    phase += increment;
    if (phase > 1) phase -= 1;
    return returnValue;
  }
  float operator()() { return getNextSample(); }
};

// (-1, 1)
struct Sawtooth : Phasor {
  float getNextSample() { return 2 * Phasor::getNextSample() - 1; }
  float operator()() { return getNextSample(); }
};

struct Reverb {
  float effectSample(float sample) { return 0; }
  float operator()(float sample) { return effectSample(sample); }
};

struct MyApp : App {
  cuttlebone::Taker<State> taker;
  State* state = new State;
  Material material;
  Light light;
  bool simulate = true;

  vector<Particle> particle;

  MyApp() {
    addSphere(sphere, sphereRadius);
    sphere.generateNormals();
    light.pos(0, 0, 0);              // place the light
    nav().pos(0, 0, 30);             // place the viewer
    lens().far(400);                 // set the far clipping plane
    particle.resize(particleCount);  // make all the particles
    background(Color(0.07));

    initWindow();
        initAudio(44100);
    saw.frequency(261, 44100);
    frequency.frequency(0.7, 44100);
  }

  void onAnimate(double dt) {
    taker.get(*state);
    // if (!simulate)
    //   // skip the rest of this function
    //   return;

    //
    //  Detect Collisions Here
    //
    // for (unsigned i = 0; i < particle.size(); ++i){
    //   Particle& a = particle[i];
    //   for (unsigned j = 1 + i; j < particle.size(); ++j) {
    //     Particle& b = particle[j];
    //     if(a.position-b.position < 6){
    //       a.velocity = -a.velocity;
    //       b.velocity = -b.velocity;
    //       //cout << "collide" << endl;
    //     }
    //   }
    // }

    // for (unsigned i = 0; i < particle.size(); ++i){
    //   for (unsigned j = 1 + i; j < particle.size(); ++j) {
    //     Particle& a = particle[i];
    //     Particle& b = particle[j];
    //     Vec3f difference = (b.position - a.position);
    //     double d = difference.mag();
    //     // F = ma where m=1
    //     Vec3f acceleration = difference / (d * d * d) * gravityFactor;
    //     // equal and opposite force (symmetrical)
    //     a.acceleration += acceleration;
    //     b.acceleration -= acceleration;
    //   }
    // }

    // Limit acceleration
    // for (auto& p : particle){
    //   if (p.acceleration.mag() > maximumAcceleration)
    //     p.acceleration.normalize(maximumAcceleration);
    // }

    // Euler's Method; Keep the time step small
    for (unsigned i = 0; i < particle.size(); i++){
      particle[i].position = state->pos[i];
    }
    // for (auto& p : particle) p.velocity += p.acceleration * timeStep;
    // for (auto& p : particle) p.acceleration.zero();
  }

  void onDraw(Graphics& g) {
    material();
    light();
    g.scale(scaleFactor);
    for (auto p : particle) p.draw(g);
  }

  Sawtooth saw;
  Phasor frequency;


  void onSound(AudioIOData& io) {
    while (io()) {
      saw.frequency(200 + 100 * frequency(), 44100);
      float s = saw() * 0.3;
      for (unsigned i = 0; i < particle.size(); ++i){
          Particle& a = particle[i];
          for (unsigned j = 1 + i; j < particle.size(); ++j) {
              Particle& b = particle[j];
              if(a.position-b.position < 6){
                  io.out(0) = s;
                  io.out(1) = s;
                  //cout << "collide" << endl;
              }     
          }
        }
      }
    }

  void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
    switch (k.key()) {
      default:
      case '1':
        // reverse time
        timeStep *= -1;
        break;
      case '2':
        // speed up time
        if (timeStep < 1) timeStep *= 2;
        break;
      case '3':
        // slow down time
        if (timeStep > 0.0005) timeStep /= 2;
        break;
      case '4':
        // pause the simulation
        simulate = !simulate;
        break;
    }
  }
};

int main() { MyApp app; app.taker.start(); app.start(); }
