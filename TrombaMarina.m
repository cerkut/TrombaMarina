% Tromba Marina model
% Stefania bow model together with a mass spring model
clear; clc; close all;
Fs = 44100;
[body, fs] = audioread('aguitar.wav'); % random guitar body IR
body = body(:,1);
freq = 110;
% mass spring

T = 1/Fs;

b = 5;       % Resistance
k = 100000;  % spring stiffness
m = 1;        % mass

% calculate k to find a precise mass spring frequency
springFreq = freq*2; % could be dependent on the string frequency
k = (springFreq*(2*pi) * sqrt(m))^2;

% frequency of spring
f0 = 1/(2*pi) * sqrt(k/m)

% mass spring states
x = [0.,0]';

% input to mass spring
u = 0;
du = 0;

% state space matrices
A = [0, 1; -k/m, -b/m];

B = [0,1/m]';

C = [1,0];

D = 0;

a = 2/T;
I = eye(2);
H = inv(a*I - A);
Ad = H * (a*I + A);
Bd = H * B;

% Bow model

N = 4*Fs;

stringLength = floor(Fs/freq);
l = stringLength/2;
Pb = 0.1; % bowing point
Fb = 0.25;  % force (N)
Vb = 0.15;  % velocity (m/s)

nutLength = floor(stringLength*(1-Pb));
brigdeLength = floor(stringLength*Pb);
nutDelay = zeros(1,nutLength);
brigdeDelay = zeros(1,brigdeLength);

Z = 0.55;
mus = 0.8; % static coeffient of friction
zslope = 2*Z;

Von = 0;
Vob = 0;
Vin = 0;
Vib = 0;
Vh = 0;

%dVon = 0;
dVib = 0;
dVin = 0;
stick = 0;

output = zeros(N,1);
frictionOutput = zeros(N,1);
vOutput = zeros(N,1);

for i = 1:N
    % make input stop to hear how that sounds
    if i > 2*Fs
        Fb = 0;
    end
    
    Vin = nutDelay(nutLength);
    Vib = brigdeDelay(brigdeLength);
    
    LPVib = 0.5*(Vib + dVib);
    dVib = LPVib;
    
    LPVin = 0.5 * (Vin + dVin);
    dVin = LPVin;
    
    Vh = Vin + LPVib;
    
    % Bow friction calculations
    
    B2=-0.2*zslope-0.3*Fb-zslope*Vb-zslope*Vh;
    C2=0.06*Fb+zslope*Vh*Vb+0.2*zslope*Vh+0.3*Vb*Fb+0.1*Fb;
    delta2=B2*B2-4*zslope*C2;
    
    if Vh == Vb % MIDDLE
        
        v = Vb;
        stick = 1;
        
    elseif (delta2 < 0)
        
        v = Vb;    % the only possible solution is the stickone
        stick = 1;
        
    else
        
        if (stick==1)    % if we are in the stick condition, we keep it........
            
            f = zslope*(Vb-Vh);
            
            if ((f <= mus * Fb) & (f > 0))  %.....if we did not find a value above the maximum bow force.
                v = Vb;
            else
                
                v1 = (-B2+sqrt(B2*B2-4*zslope*C2))/(2*zslope);
                v2 = (-B2-sqrt(B2*B2-4*zslope*C2))/(2*zslope);
                
                vtemp = min(v1, v2);  % we choose the minimum solution, because it is NOT the one in the middle
                
                stick = 0;
                
                if(vtemp > Vb) % we do not want a solution v>Vb
                    v = Vb;
                    stick = 1;
                else
                    v = vtemp;
                end
            end
        else % we are in SLIP, so we stay in SLIP.....
            
            v1 = (-B2+sqrt(B2*B2-4*zslope*C2))/(2*zslope);
            v2 = (-B2-sqrt(B2*B2-4*zslope*C2))/(2*zslope);
            v = min(v1,v2);    % we choose the minimum solution, because it is NOT the one in the middle
            
            stick = 0;
            
            if(v > Vb) % on reste en SLIP sauf si v > Vb
                v = Vb;
                stick = 1;
            end
        end
    end
    
    % update outgoing velocities with estimated v
    f = zslope*(v-Vh);
    
    Von = -(LPVib + (f/(2*Z))); % new outgoing velocity to nut
    Vob = -(Vin +  (f/(2*Z)));  % new outgoing velocity to bridge
    
    % attach mass spring to bow
    
    
    % output of spring
    massSpringOutput = x(1)*50000;
    
    threshold = 0.5;
    if massSpringOutput > threshold 
        %massSpringOutput = 0.5; %(1/threshold * threshold) * ;
        %Vob = Vob + massSpringOutput;
        bodyImpulse = 1
       % x(1) = -x(1);
       % x(2) = -x(2);
    else 
        bodyImpulse = 0;
    end
    u = Vob;
    % update states
    x = Ad*x + Bd*(u + du);
    du = u;
    
    nutDelay = [Von + massSpringOutput*0.01, nutDelay(1:nutLength-1)];
    brigdeDelay = [Vob + massSpringOutput*0.01, brigdeDelay(1:brigdeLength-1)];
    
    
    if bodyImpulse == 1
        output(i) = Vob + 1;
    else
        output(i) = Vob;
    end
    
    frictionOutput(i) = f;
    vOutput(i) = v;
end
outWithBody = conv(output, body);
plot(outWithBody)

% figure
% plot(vOutput)

soundsc(outWithBody,Fs)

