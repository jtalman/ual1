// Library       : SXF_TRACKER
// File          : examples/SXF_TRACKER/driftOrDefault.hh
// Copyright     : see Copyright file
// Author        : 
// C++ version   : J.Talman, N.Malitsky

#ifndef UAL_DRIFT_OR_DEFAULT_HH
#define UAL_DRIFT_OR_DEFAULT_HH

#include "SMF/PacLattElement.h"

#include "SMF/PacElemLength.h"
#include "SMF/PacElemBend.h"
#include "SMF/PacElemMultipole.h"
#include "SMF/PacElemOffset.h"
#include "SMF/PacElemRotation.h"
#include "SMF/PacElemAperture.h"
#include "SMF/PacElemComplexity.h"
#include "SMF/PacElemSolenoid.h"
#include "SMF/PacElemRfCavity.h"

namespace SXF_TRACKER {

  class driftOrDefault : UAL::PropagatorNode {

  public:

    /** Constructor */
    driftOrDefault();

   /** Copy constructor */
    driftOrDefault(const driftOrDefault& st);

    /** Destructor */
    ~driftOrDefault();

    const char* getType(){return "JDT_SXF_DriftOrDefault";}

    /** Returns false */
    bool isSequence() { return false; }

    /** Defines the lattice elemements (PropagatorNode method)
	Note: integers i0 and i1 will be replaced by AcceleratorNode's 
    */
    virtual void setLatticeElements(const UAL::AcceleratorNode& sequence, int i0, int i1, 
				    const UAL::AttributeSet& attSet);

UAL::AcceleratorNode& getFrontAcceleratorNode()
{
  return m_frontNode;
}

UAL::AcceleratorNode& getBackAcceleratorNode()
{
  return m_backNode;
}

    /** Propagates a bunch */
    void propagate(UAL::Probe& bunch);

    /** Returns a deep copy of this object (inherited from UAL::PropagatorNode) */
    UAL::PropagatorNode* clone();

  protected:

    // Element data

    void setElementData(const PacLattElement& e);

    std::string m_name;

    PacElemMultipole* p_entryMlt;
    PacElemMultipole* p_exitMlt;

    PacElemLength* p_length;           // 1: l
    PacElemBend* p_bend;               // 2: angle, fint
    PacElemMultipole* p_mlt;           // 3: kl, ktl
    PacElemOffset* p_offset;           // 4: dx, dy, ds
    PacElemRotation* p_rotation;       // 5: dphi, dtheta, tilt
    // PacElemAperture*p_aperture;     // 6: shape, xsize, ysize
    PacElemComplexity* p_complexity;   // 7: n
    // PacElemSolenoid* p_solenoid;    // 8: ks
    // PacElemRfCavity* p_rf;          // 9: volt, lag, harmon

  protected:

    // TEAPOT trackers

    void setConventionalTracker(const UAL::AcceleratorNode& sequence,
                                int is0, int is1,
                                const UAL::AttributeSet& attSet);

    /** Conventional tracker (body) */
    UAL::PropagatorNodePtr m_tracker;

  protected:

    void propagateSXF_Tracker(UAL::Probe& b);
    void propagateSXF_Tracker(PAC::BeamAttributes& ba, PAC::Particle& prt);

    double get_psp0(PAC::Position& p, double v0byc);

  private:

    void copy(const driftOrDefault& st);

  protected:

    /**  Front node */ 
    PacLattElement m_frontNode;

    /** Back node */ 
    PacLattElement m_backNode;

  };

  class driftOrDefaultRegister
  {
    public:

    driftOrDefaultRegister();
  };


}

#endif
