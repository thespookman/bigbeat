#ifndef BEAT_H
#define BEAT_H
#include <vector>

class Beat {
  private:
    std::vector<int> volumes;

  public:
    Beat ();
    explicit Beat (int number);
    explicit Beat (Beat* b);
    unsigned int length ();
    int          get (int index);
    void         add (Beat* b);
    ~Beat ();
};
#endif
