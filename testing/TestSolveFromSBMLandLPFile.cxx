#include <stdio.h>
#include <assert.h> 		
#include <math.h>
#include <iostream>
#include "fbcFBAProblem.hxx" 	
 	
/** \brief Test for solving LP formatted models firstly initilaized by a sbml
 *         file. 	
 * Loads a simple SBML file, saves the model in a LP model file, opens this LP
 * model file and solves the problem using lp_solve library. 	
 */ 	
int main (int argc, char *argv[]) 	
{ 	
   fbc::FBAProblem pbm;
   pbm.initFromSBMLFile(argv[1]);
   pbm.writeProblem("TestLPthroughSBML.xml");
   pbm.initFromLPFile("TestLPthroughSBML.xml");
   pbm.solveProblem(); 	
   assert(floor(100 * pbm.getSolution()->getObjectiveValue()) == 38);	
   return 0; 	
}
