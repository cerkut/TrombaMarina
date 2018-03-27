/*
  ==============================================================================

    TrombaMarina.h
    Created: 25 Mar 2018 5:07:12pm
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/

#pragma once
#include "DelayLine.cpp"

//#include "mass_spring.cpp"
class TrombaMarina
{
public:
    void setSampleRate(double sampleRate);
    void setForce(float force);
    void setVelocity(float velocity);
    void setBowingPoint(float bp);
    float getOutput(float vb);
    
private:
    void calculateV(double& v, double vh, float vb);
    
    
    DelayLine nutDelay, brigdeDelay;
    //MassSpring spring;
    
    float fb = 0.1;
    
    float pb = 0.2;
    float freq = 220;
    
    
    
//    float dVon = 0;
//    float dVib = 0;
//    float dVin = 0;
    bool stick = 1;
    
    
    double stringLength = 0.0;
    
    float Z = 0.55;
    float mus = 0.8; // static coeffient of friction
    float zslope = 2*Z;
};
