#ifndef LocFromFile_hh
#define LocFromFile_hh 1

/*
 * =c
 * LocFromFile(filename)
 * =d
 * Pretends to be a LocationInfo element, but actually reads the
 * locations from a file. Each line of the file looks like
 *
 *   interval lat lon
 *
 * This means "spend interval seconds moving to lat/lon."
 *
 * =a
 * LocationInfo
 */

#include "element.hh"
#include "locationinfo.hh"

class LocFromFile : public LocationInfo {
public:
  LocFromFile();
  ~LocFromFile();

  const char *class_name() const { return "LocFromFile"; }

  LocFromFile *clone() const { return new LocFromFile; }
  int configure(const Vector<String> &, ErrorHandler *);
  bool can_live_reconfigure() const { return true; }

protected:
  virtual void choose_new_leg(double *, double *, double *);

private:
  struct delta {
    double interval;
    double lat;
    double lon;
  };
  Vector<delta> _deltas;
  int _next;
};

#endif
