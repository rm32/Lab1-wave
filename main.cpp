//*******************************************************************
//                           Lab1-wave
//             create this waveform using an MBED system. 
//
// Description
//    Create 14 digital pulses with increasing width with a space of 900us. 
//    There should be a space of 1.5uS between each block of pulses
//
// Method
//    Set digital outputs high and low to create digital pulses
//
// Version
//    Roshenac Mitchell  January 2016
//*******************************************************************   


#include "mbed.h"
#include "MCP23017.h" // include 16-bit parallel I/O header file

//M   13    
//I   9     
//T   7    
//C   3  

#define A   1300    // minimum pulse width (uS)
#define B   900     // width of space between pulses (uS)
#define C   14      // number of pulses in a block
#define D   1500    // space between pulse blocks (uS)

//  mode 1: Remove 3 pulses from pulse block 
//          (i.e. c-3 pulses in block) until
//          switch 2 set back to 0.        
//  mode 2: generate inverted form of complete 
//          Sig A waveform until switch set back to 0
//  mode 3: insert an extra 3 pulses into pulse block 
//          (i.e. c+3 pulses in block) until switch set back to 0
//  mode 4: half d and b time until switch set back to 0
#define MODE    1 

// bounds for the for loop
#define LOWER_BOUND     0   
#define HIGHER_BOUND    C

#define SWITCH_1        8       // switch 1 output
#define SWITCH_2        9       //  switch 2 output  

// set the output of the two signals
DigitalOut sigA(p21);
DigitalOut sigB(p22);

MCP23017 *par_port; // pointer to 16-bit parallel I/O object

// variables to hold current values
int pulseWidth; 
int pulseNo;
int loLoop; 
int highLoop;
int pulseSpace;
int blockSpace;

// resets the pulse and variables back to default
// block space is decreased by 1000 uS due to additional
// time taken to go through code
void defaultPulse()
{
    pulseWidth =    A;
    pulseNo =       C;
    loLoop =        LOWER_BOUND ;
    highLoop =      HIGHER_BOUND ;  
    pulseSpace =    B;
    blockSpace =    D - 1000;
}

// gets the pulse width depending on what 
// pulse number (i) you are on 
// 2nd pulse = a + 50μS 
// 3rd pulse = a + 100μS 
// 4th pulse = a + 150μS 
// etc.
int getPulseWidth(int i)
{
    return A + (50 * abs(i));
}

// decrease the number of pulses by 3 
// Note. you need to set back to default before
// you decrease so you dont decrease indefinitly 
void decreasePulses()
{
    pulseNo =   C - 3;
    highLoop =  pulseNo;  
}

// increase the number of pulses by 3
void increasePulses()
{
     pulseNo =   C + 3;
     highLoop =  pulseNo;  
}

// start from largest pulse and get smaller
void invertPulses()
{
    loLoop =    -(C) +1 ;
    highLoop =  1;
}

// half the amount of time between the pulses and 
// half the amount of time between the blocks 
void halfTime()
{
    pulseSpace =   B/2;
    blockSpace =   D/2;
}


// send a trigger pulse on signal B
void trigger()
{
    sigB = 1;       //signal B high
    wait_us(10);
    sigB = 0;       //signal B low
}


int main() {
    
    //initialise the port 
    par_port = new MCP23017(p9, p10, 0x40);
    // configure the input and outputs
    par_port->config(0x0F00, 0x0F00, 0x0F00);
    
    // set the pulse with default variables 
    defaultPulse();
    
    // infinite loop 
    while(1) {
        
        // check switch 1 
        int switch1 = par_port->read_bit(SWITCH_1);

        // if switch 1 = 0 -> enable stream of pulses 
        if( switch1 == 0)
        { 
            // start the trigger
            trigger();
        
            // create the given number of pulses with varying widths.
            // default: loLoop = 0; highLoop = C (number of pulses)
            for(int i = loLoop; i< highLoop; i++)
            {
                // get the calculated with for pulse i 
                int width = getPulseWidth(i);
                
                sigA = 1;       // signal A high
                wait_us(width);
                sigA = 0;       // signal A low
                
                // wait pulse space 
                wait_us(pulseSpace); 
            }
    
            // wait block space 
            wait_us(blockSpace);  // THIS DOESNT SEEM TO BE RIGHT!!!
            
            // read switch 2
            int switch2 = par_port->read_bit(SWITCH_2);
         
            // if switch2 = 1 -> run with modified waveform 
            // the modified waveform depending on what 
            // mode it is in (see top of page for modes)
            if(switch2 == 1)
            {
                switch(MODE){
                    case 1: 
                        decreasePulses();
                        break;
                    case 2:
                        invertPulses();
                        break;
                    case 3:
                        increasePulses();
                        break;
                    case 4:
                        halfTime();
                        break;
                  default:
                        defaultPulse();
            }
            }else{
                // when swith 2 is off, run as normal mode
                defaultPulse();
            }  
        }
    }
    return 0; 
}