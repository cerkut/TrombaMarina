/*
 ==============================================================================
 
 TrombaMarina.cpp
 Created: 25 Mar 2018 5:07:12pm
 Author:  Nikolaj Schwab Andersson
 
 ==============================================================================
 */

#include "TrombaMarina.h"
#include <cmath>

void TrombaMarina::setSampleRate(double sampleRate)
{
   // stringLength = freq;
    
    brigdeDelay.initDelay(0.01, sampleRate);
    brigdeDelay.setFrequency(freq*pb);
    nutDelay.initDelay(0.01, sampleRate);
    nutDelay.setFrequency(freq*(1-pb));

    
}

void TrombaMarina::calculateV()
{
    float v1, v2, vtemp, fv;
    
    auto B2 = -0.2*zslope-0.3*fb-zslope*vb-zslope*vh;
    auto C2 = 0.06*fb+zslope*vh*vb+0.2*zslope*vh+0.3*vb*fb+0.1*fb;
    auto delta2 = B2*B2-4*zslope*C2;
    
    if (vh == vb) // MIDDLE
    {
        v = vb;
        stick = 1;
    } else if (delta2 < 0)
    {
        v = vb;    // the only possible solution is the stickone
        stick = 1;
    } else
    {
        if (stick==1)    //% if we are in the stick condition, we keep it........
        {
            fv = zslope*(vb-vh);
            
            if ((fv <= mus * fb) && (fv > 0)) // %.....if we did not find a value above the maximum bow force.
            {
                v = fb;
            } else
            {
                v1 = (-B2+sqrt(B2*B2-4*zslope*C2))/(2*zslope);
                v2 = (-B2-sqrt(B2*B2-4*zslope*C2))/(2*zslope);
                
                vtemp = fmin(v1, v2);  // we choose the minimum solution, because it is NOT the one in the middle
                
                stick = 0;
                
                if(vtemp > vb) //% we do not want a solution v>Vb
                {   v = vb;
                    stick = 1;
                } else
                {
                    v = vtemp;
                }
            }
        } else //% we are in SLIP, so we stay in SLIP.....
        {
            v1 = (-B2+sqrt(B2*B2-4*zslope*C2))/(2*zslope);
            v2 = (-B2-sqrt(B2*B2-4*zslope*C2))/(2*zslope);
            v = fmin(v1,v2);   // we choose the minimum solution, because it is NOT the one in the middle
            
            stick = 0;
            
            if(v > vb)
            {
                v = vb;
                stick = 1;
            }
        }
    }
    
    
}
float TrombaMarina::getOutput()
{
    float f;
    vin = -nutDelay.getOutput();
    vib = -brigdeDelay.getLPOutput();
    
    vh = vin + vib;
    
    calculateV();
    
    f = zslope*(v-vh);
    
    
    von = -(vib + (f/(2*Z)));  // new outgoing velocity to nut
    vob = -(vin +  (f/(2*Z))); //new outgoing velocity to bridge
    
    // update delay
    brigdeDelay.tick(vob);
    nutDelay.tick(von);
    
    return vob;
}

