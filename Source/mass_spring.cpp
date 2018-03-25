/*
  ==============================================================================

    mass_spring.cpp
    Created: 25 Mar 2018 5:08:02pm
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/
#include <math.h>

class MassSpring
{
    public:
    double frequency = 0;

    void setSampleRate(double sampleRate)
    {
        T = 1.0f/sampleRate;
        alpha = 2.0f/T;
        frequency = 1/(2*M_PI) * sqrt(k/m);
        
        updateMatrices();
    }
    
    void init(double res, double stiffness, double mass, double initialState[2])
    {
        b = res;
        k = stiffness;
        m = mass;
        frequency = 1/(2*M_PI) * sqrt(k/m);
        
        x[0] = initialState[0];
        x[1] = initialState[1];
    }
    void setInput(double input)
    {
        u = input;
    }
    void run()
    {
        updateStates();
    }
    
    double getOutput()
    {
        return x[0];
    }
    
    
    private:

    void updateStates()
    {
        x[0] = Ad[0]*x[0] + Ad[1]*x[1] + Bd[0]*(u + du);
        x[1] = Ad[2]*x[0] + Ad[3]*x[1] + Bd[1]*(u + du);
        
        // update input to avoid inf if u is not set
        du = u;
        u = 0.0;
    }
    void updateMatrices()
    {
        A[0] = 0.0;
        A[1] = 1.0;
        A[2] = -k/m;
        A[3] = -b/m;
        
        B[0] = 0.0;
        B[1] = 1.0/m;
        
        invertMatrix();
        
        double temp[4];
        
        temp[0] = alpha+A[0];
        temp[1] = A[1];
        temp[2] = A[2];
        temp[3] = alpha+A[3];

        // make new matrices to make the updateStates easier
        Ad[0] = H[0]*temp[0] + H[1]*temp[2];
        Ad[1] = H[0]*temp[1] + H[1]*temp[3];
        
        Ad[2] = H[2]*temp[0] + H[3]*temp[2];
        Ad[3] = H[2]*temp[1] + H[3]*temp[3];
        
        Bd[0] = H[0]*B[0] + H[1]*B[1];
        Bd[1] = H[2]*B[0] + H[3]*B[1];
    }
    
    
    void invertMatrix()
    {
        double temp[4];
        
        temp[0] = alpha - A[0];
        temp[1] = A[1];
        temp[2] = A[2];
        temp[3] = alpha - A[3];

        // one over the determinant
        double det = 1.0/( temp[0]*temp[3] - temp[1]*temp[2]);
        
        // Matrix invertion
        H[0] = det*temp[3];
        H[1] = -det*temp[1];
        
        H[2] = -det*temp[2];
        H[3] = det*temp[0];
    }
    
    
    double u = 0.0;
    double du = 0.0; // delayed input
    
    // vectors and matrices
    double x[2] = {0.0,1.0};
    
    double A[4] = {0.0, 0.0, 0.0, 0.0};
    double B[2] = {0.0, 0.0};
    
    double Ad[4] = {0.0, 0.0, 0.0, 0.0};
    double Bd[2] = {0.0, 0.0};
    double H[4] = {0.0, 0.0, 0.0, 0.0};
    
    double T = 0.0;
    double alpha = 0.0;
    
    double b = 1.0;      // Resistance
    double k = 1000000;  // spring stiffness
    double m = 0.1;     // mass
};
