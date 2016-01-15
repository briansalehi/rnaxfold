/******************************************************************************
 *   Copyright (C) 2014  Juan Antonio Garcia Martin , Peter Clote, Ivan Dotu  *
 *                                                                            *
 *  This program is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation, either version 3 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
 ******************************************************************************/

#include "misc.h"
#include "strtree.h"
#include "blosum.h"
#include "helix_constraint.h"
#include "local_constraint.h"
#include "aaconstraint.h"
#include "strtreegroup.h"

//#include "strtree.h"

// OR-Tools libraries 
#include <vector>
#include <map>
#include "base/logging.h"
#include "constraint_solver/constraint_solver.h"

namespace operations_research {


// Prints the instance of the Frequency Assignment Problem
class IFold{
	public:
		~IFold();
		IFold(Solver* const _solver, int _n);
		IFold(Solver* const _solver, int _n, int dangles, std::string rnaLib, std::string energyModel);
		void InitDomains(char* sequence);
		void InitSequenceDomain(char* sequence);
		void AddAminoAcidConstraint(AAConstraint* aaCstr);
		void AddStructureConstraints(int* _str_int, int* _str_int_undet, int includeDangles, double foldTemp, int cutPoint, int MFEstructure, int showHelices, std::vector<HelixCstr> helixCstrs);
		void AddLocalStructureConstraints(double foldTemp, int cutPoint, std::vector<LocalCstr> localCstrs);
		void AddNucleotideRangeConstraints(double minGCcont, double maxGCcont, int minAU, int maxAU, int minGC, int maxGC, int minGU, int maxGU,std::vector<std::tuple<int,int,int>> listMinA,std::vector<std::tuple<int,int,int>> listMaxA,std::vector<std::tuple<int,int,int>> listMinC,std::vector<std::tuple<int,int,int>> listMaxC,std::vector<std::tuple<int,int,int>> listMinG,std::vector<std::tuple<int,int,int>> listMaxG,std::vector<std::tuple<int,int,int>> listMinU,std::vector<std::tuple<int,int,int>> listMaxU,std::vector<std::tuple<int,int,int>> listConsA,std::vector<std::tuple<int,int,int>> listConsC,std::vector<std::tuple<int,int,int>> listConsG,std::vector<std::tuple<int,int,int>> listConsU);
		void AddCompatibilityConstraint(int* comp_str_int);
		void AddIncompatibilityConstraint(vector<pair<int,int>> vIncompBP);

		void SetSearchHeuristic(int helixHeuristic, int varHeuristic, int randomAssignment, int upthreshold, int bpthreshold);
		void Search(int maxSolutions,int64 timeLimit, int minimizeMFE, int minimizeEnsDef, int LNS, int LNSunchangedRestarts, int LNSrestartTime, int showMeasures);

		vector<CPHelix* > getHelices();
		vector<IntVar* > getSearchVars();
		
	private:
		const std::vector<int64> golomb {3,7,1,12};
		
		const std::vector<int64> coeffs {1,4,16,64,256,1024,4096,16384};
		const std::vector<int64> bpdoms = {-6,6,-9,9,-11,11}; //GC=-6,CG=6, AU=-9, UA=9, GU=-11, UG=11
		const std::vector<int64> nobpdoms = {0,-4,4,2,-2,-5,5}; //XX=0, AC=-4,CA=4, AG=2, GA=-2, CU=-5, UC=5		
		
		const std::vector<int64> bpToUpO = {2,-1,0,-1,-1,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,-1,3,-1,3};
		const std::vector<int64> bpToUpC = {3,-1,3,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2,-1,-1,0,-1,2};

		const std::vector<int64> isGC {0,1,1,0};

		std::map<char, vector<int> > IUPAC;
		vector< vector<int64> > aaBlosum ={{-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 0,-7,-1,-7,-7, 4,-1, 0,-7,-1,-7,-7, 0,-1,-1,-1, 1,-1,-1,-7,-4,-7,-3,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 0,-7,-1,-7,-7, 4,-1, 0,-7,-1,-7,-7, 0,-1,-1,-1, 1,-1,-1,-7,-4,-7,-4,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 0,-7,-1,-7,-7, 4,-1, 0,-7,-2,-7,-7, 0, 1,-2,-2, 1,-1,-1,-7,-2,-7, 0,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 0,-7,-1,-7,-7, 4,-1, 0,-7,-2,-7,-7, 0, 1,-2,-2, 1,-1,-1,-7,-2,-7, 0,-2},	// A
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-1,-7,-7,-1,-2,-1,-7, 0,-7,-7,-2, 5, 2, 1,-1, 5,-2,-7,-4,-7,-3,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-1,-2,-1,-7, 0,-7,-7,-2, 5, 2, 1,-1, 5,-2,-7,-4,-7,-4,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-1,-2,-1,-7,-2,-7,-7,-2,-1, 0, 0,-1, 5,-2,-7,-2,-7,-3,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-1,-2,-1,-7,-2,-7,-7,-2,-1, 0, 0,-1, 5,-2,-7,-2,-7,-3,-3},	// R
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-2,-7,-7,-2,-3, 0,-7, 0,-7,-7, 0, 0, 0, 0, 1, 0,-2,-7,-4,-7,-4,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-3, 0,-7, 0,-7,-7, 0, 0, 0, 0, 1, 0,-2,-7,-4,-7,-4,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-3, 0,-7, 1,-7,-7, 0, 1, 6, 1, 1, 0,-2,-7,-2,-7,-3,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-3, 0,-7, 1,-7,-7, 0, 1, 6, 1, 1, 0,-2,-7,-2,-7,-3,-3},	// N
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-4,-1,-7, 2,-7,-7,-1,-2,-1, 0, 0,-2,-1,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-4,-1,-7, 2,-7,-7,-1,-2,-1, 0, 0,-2,-1,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-4,-1,-7, 6,-7,-7,-1, 0, 1,-1, 0,-2,-1,-7,-3,-7,-3,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-4,-1,-7, 6,-7,-7,-1, 0, 1,-1, 0,-2,-1,-7,-3,-7,-3,-3},	// D
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7, 0,-1,-1,-7,-4,-7,-7,-3,-3,-3,-3,-1,-3,-3,-7,-4,-7,-2,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7, 0,-1,-1,-7,-4,-7,-7,-3,-3,-3,-3,-1,-3,-3,-7,-4,-7,-4,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7, 0,-1,-1,-7,-3,-7,-7,-3,-1,-3,-3,-1,-3,-3,-7,-2,-7, 9,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7, 0,-1,-1,-7,-3,-7,-7,-3,-1,-3,-3,-1,-3,-3,-7,-2,-7, 9,-2},	// C
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7, 0,-7,-7,-1,-2,-1,-7, 2,-7,-7,-2, 1, 1, 5, 0, 1,-1,-7,-4,-7,-2,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-2,-1,-7, 2,-7,-7,-2, 1, 1, 5, 0, 1,-1,-7,-4,-7,-4,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-2,-1,-7, 0,-7,-7,-2, 0, 0, 0, 0, 1,-1,-7,-1,-7,-3,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-2,-1,-7, 0,-7,-7,-2, 0, 0, 0, 0, 1,-1,-7,-1,-7,-3,-3},	// Q
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-2,-7,-7,-1,-3,-1,-7, 5,-7,-7,-2, 0, 1, 2, 0, 0,-1,-7,-4,-7,-3,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-3,-1,-7, 5,-7,-7,-2, 0, 1, 2, 0, 0,-1,-7,-4,-7,-4,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-3,-1,-7, 2,-7,-7,-2, 0, 0, 0, 0, 0,-1,-7,-2,-7,-4,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-3,-1,-7, 2,-7,-7,-2, 0, 0, 0, 0, 0,-1,-7,-2,-7,-4,-3},	// E
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7, 0,-4,-2,-7,-2,-7,-7, 6,-2,-2,-2, 0,-2,-2,-7,-4,-7,-2,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-4,-7,-7, 0,-4,-2,-7,-2,-7,-7, 6,-2,-2,-2, 0,-2,-2,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-4,-7,-7, 0,-4,-2,-7,-1,-7,-7, 6, 0, 0,-2, 0,-2,-2,-7,-3,-7,-3,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-4,-7,-7, 0,-4,-2,-7,-1,-7,-7, 6, 0, 0,-2, 0,-2,-2,-7,-3,-7,-3,-3},	// G
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-2,-7,-7,-2,-3,-2,-7, 0,-7,-7,-2, 0,-1, 0,-1, 0,-2,-7,-4,-7,-2,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-3,-2,-7, 0,-7,-7,-2, 0,-1, 0,-1, 0,-2,-7,-4,-7,-4,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-3,-2,-7,-1,-7,-7,-2,-1, 1, 8,-1, 0,-2,-7, 2,-7,-3,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-3,-2,-7,-1,-7,-7,-2,-1, 1, 8,-1, 0,-2,-7, 2,-7,-3,-1},	// H
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 3,-7, 1,-7,-7,-1, 2,-1,-7,-3,-7,-7,-4,-3,-3,-3,-2,-3,-3,-7,-4,-7,-3, 2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 3,-7, 4,-7,-7,-1, 2,-1,-7,-3,-7,-7,-4,-3,-3,-3,-2,-3,-3,-7,-4,-7,-4, 2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 3,-7, 4,-7,-7,-1, 2,-1,-7,-3,-7,-7,-4,-2,-3,-3,-2,-3,-3,-7,-1,-7,-1, 0,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 3,-7, 4,-7,-7,-1, 2,-1,-7,-3,-7,-7,-4,-2,-3,-3,-2,-3,-3,-7,-1,-7,-1, 0},	// I
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 1,-7, 2,-7,-7,-1, 4,-1,-7,-3,-7,-7,-4,-2,-2,-2,-2,-2,-3,-7,-4,-7,-2, 4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 1,-7, 2,-7,-7,-1, 4,-1,-7,-3,-7,-7,-4,-2,-2,-2,-2,-2,-3,-7,-4,-7,-4, 4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 1,-7, 2,-7,-7,-1, 4,-1,-7,-4,-7,-7,-4,-2,-3,-3,-2,-2,-3,-7,-1,-7,-1, 0,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 1,-7, 2,-7,-7,-1, 4,-1,-7,-4,-7,-7,-4,-2,-3,-3,-2,-2,-3,-7,-1,-7,-1, 0},	// L
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-1,-7,-7,-1,-2,-1,-7, 1,-7,-7,-2, 2, 5, 1, 0, 2,-1,-7,-4,-7,-3,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-2,-1,-7, 1,-7,-7,-2, 2, 5, 1, 0, 2,-1,-7,-4,-7,-4,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-2,-1,-7,-1,-7,-7,-2, 0, 0,-1, 0, 2,-1,-7,-2,-7,-3,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-2,-1,-7,-1,-7,-7,-2, 0, 0,-1, 0, 2,-1,-7,-2,-7,-3,-3},	// K
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 1,-7, 5,-7,-7,-1, 2,-1,-7,-2,-7,-7,-3,-1,-1, 0,-1,-1,-2,-7,-4,-7,-1, 2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 1,-7, 1,-7,-7,-1, 2,-1,-7,-2,-7,-7,-3,-1,-1, 0,-1,-1,-2,-7,-4,-7,-4, 2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 1,-7, 1,-7,-7,-1, 2,-1,-7,-3,-7,-7,-3,-1,-2,-2,-1,-1,-2,-7,-1,-7,-1, 0,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 1,-7, 1,-7,-7,-1, 2,-1,-7,-3,-7,-7,-3,-1,-2,-2,-1,-1,-2,-7,-1,-7,-1, 0},	// M
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7, 0,-7,-7,-2, 0,-2,-7,-3,-7,-7,-3,-3,-3,-3,-2,-3,-4,-7,-4,-7, 1, 0,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7, 0,-7,-7,-2, 0,-2,-7,-3,-7,-7,-3,-3,-3,-3,-2,-3,-4,-7,-4,-7,-4, 0,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7, 0,-7,-7,-2, 0,-2,-7,-3,-7,-7,-3,-2,-3,-1,-2,-3,-4,-7, 3,-7,-2, 6,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7, 0,-7,-7,-2, 0,-2,-7,-3,-7,-7,-3,-2,-3,-1,-2,-3,-4,-7, 3,-7,-2, 6},	// F
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-2,-7,-7,-1,-3,-1,-7,-1,-7,-7,-2,-2,-1,-1,-1,-2, 7,-7,-4,-7,-4,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-3,-1,-7,-1,-7,-7,-2,-2,-1,-1,-1,-2, 7,-7,-4,-7,-4,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-3,-1,-7,-1,-7,-7,-2,-1,-2,-2,-1,-2, 7,-7,-3,-7,-3,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-3,-1,-7,-1,-7,-7,-2,-1,-2,-2,-1,-2, 7,-7,-3,-7,-3,-4},	// P
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-1,-7,-7, 1,-2, 1,-7, 0,-7,-7, 0,-1, 0, 0, 4,-1,-1,-7,-4,-7,-3,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-2,-7,-7, 1,-2, 1,-7, 0,-7,-7, 0,-1, 0, 0, 4,-1,-1,-7,-4,-7,-4,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-2,-7,-7, 1,-2, 1,-7, 0,-7,-7, 0, 4, 1,-1, 4,-1,-1,-7,-2,-7,-1,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-2,-7,-7, 1,-2, 1,-7, 0,-7,-7, 0, 4, 1,-1, 4,-1,-1,-7,-2,-7,-1,-2},	// S
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 0,-7,-1,-7,-7, 0,-1, 5,-7,-1,-7,-7,-2,-1,-1,-1, 1,-1,-1,-7,-4,-7,-2,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 0,-7,-1,-7,-7, 0,-1, 5,-7,-1,-7,-7,-2,-1,-1,-1, 1,-1,-1,-7,-4,-7,-4,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 0,-7,-1,-7,-7, 0,-1, 5,-7,-1,-7,-7,-2, 1, 0,-2, 1,-1,-1,-7,-2,-7,-1,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 0,-7,-1,-7,-7, 0,-1, 5,-7,-1,-7,-7,-2, 1, 0,-2, 1,-1,-1,-7,-2,-7,-1,-2},	// T
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-1,-7,-7,-3,-2,-2,-7,-3,-7,-7,-2,-3,-3,-2,-3,-3,-4,-7,-4,-7,11,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-3,-2,-2,-7,-3,-7,-7,-2,-3,-3,-2,-3,-3,-4,-7,-4,-7,-4,-2,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-3,-2,-2,-7,-4,-7,-7,-2,-3,-4,-2,-3,-3,-4,-7, 2,-7,-2, 1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-3,-2,-2,-7,-4,-7,-7,-2,-3,-4,-2,-3,-3,-4,-7, 2,-7,-2, 1},	// W
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7,-2,-1,-2,-7,-2,-7,-7,-3,-2,-2,-1,-2,-2,-3,-7,-4,-7, 2,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7,-2,-1,-2,-7,-2,-7,-7,-3,-2,-2,-1,-2,-2,-3,-7,-4,-7,-4,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7,-2,-1,-2,-7,-3,-7,-7,-3,-2,-2, 2,-2,-2,-3,-7, 7,-7,-2, 3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7,-2,-1,-2,-7,-3,-7,-7,-3,-2,-2, 2,-2,-2,-3,-7, 7,-7,-2, 3},	// Y
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 4,-7, 1,-7,-7, 0, 1, 0,-7,-2,-7,-7,-3,-3,-2,-2,-2,-3,-2,-7,-4,-7,-3, 1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 4,-7, 3,-7,-7, 0, 1, 0,-7,-2,-7,-7,-3,-3,-2,-2,-2,-3,-2,-7,-4,-7,-4, 1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 4,-7, 3,-7,-7, 0, 1, 0,-7,-3,-7,-7,-3,-2,-3,-3,-2,-3,-2,-7,-1,-7,-1,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7, 4,-7, 3,-7,-7, 0, 1, 0,-7,-3,-7,-7,-3,-2,-3,-3,-2,-3,-2,-7,-1,-7,-1,-1},	// V
		                                   //{-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-4,-1,-7, 1,-7,-7,-1,-1, 0, 0, 0,-1,-2,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-4,-1,-7, 1,-7,-7,-1,-1, 0, 0, 0,-1,-2,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-4,-1,-7, 4,-7,-7,-1, 0, 3, 0, 0,-1,-2,-7,-3,-7,-3,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-3,-7,-3,-7,-7,-2,-4,-1,-7, 4,-7,-7,-1, 0, 3, 0, 0,-1,-2,-7,-3,-7,-3,-3},	// B
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7, 4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7, 4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4},	// B
		                                   //{-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-1,-7,-7,-1,-3,-1,-7, 4,-7,-7,-2, 0, 1, 3, 0, 0,-1,-7,-4,-7,-3,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-3,-1,-7, 4,-7,-7,-2, 0, 1, 3, 0, 0,-1,-7,-4,-7,-4,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-3,-1,-7, 1,-7,-7,-2, 0, 0, 0, 0, 0,-1,-7,-2,-7,-3,-3,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-2,-7,-3,-7,-7,-1,-3,-1,-7, 1,-7,-7,-2, 0, 0, 0, 0, 0,-1,-7,-2,-7,-3,-3},	// Z
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7, 4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7, 4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4},	// Z
		                                   //{-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7, 0,-1, 0,-7,-1,-7,-7,-1,-1,-1,-1, 0,-1,-2,-7,-4,-7,-2,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7, 0,-1, 0,-7,-1,-7,-7,-1,-1,-1,-1, 0,-1,-2,-7,-4,-7,-1,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7, 0,-1, 0,-7,-1,-7,-7,-1, 0,-1,-1, 0,-1,-2,-7,-1,-7,-2,-1,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-1,-7,-1,-7,-7, 0,-1, 0,-7,-1,-7,-7,-1, 0,-1,-1, 0,-1,-2,-7,-1,-7,-2,-1},	// X
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7, 1,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7, 1,-7, 1,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4},	// X
		                                   {-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7, 4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-7,-4,-7,-4,-7,-7,-4,-4,-4,-7,-4,-7,-7,-4,-4,-4,-4,-4,-4,-4,-7,-4,-7,-4,-4}};	// *
		
		vector<int*> str_int;
		vector<int*> str_int_undet;		
		vector<double> trgFoldTemps;

		Solver* solver;
		int n;

		int upthreshold_;
		int bpthreshold_;
		int dangles_;
		std::string rnaLib_;
		std::string energyModel_;
		int cutPoint_;
		

		vector<IntVar *> vSeq;
		vector<IntVar *> vSeqGolomb;
		
		vector<vector<IntVar *> > vBPs;
		vector<vector<int> > BPO;
		vector<vector<int> > BPC;
		
		
		vector<IntVar *> vCodons;
		vector<pair<int,int>> codonPositions; // Starting position and length of each codon constraint
		
		vector<IntVar *> vAaSeqSimilarity;
		int maxBlosumValue=0;
		

		vector<StrTree*> str_tree;
		StrTreeGroup* tree_group;
		vector<IntVar*> vars;
		vector<int> var_types;
		vector<int> var_str;
		
		Blosum* blosumHelper;
		
		void makeBp(int str_index);


		bool isFixed(int i, int j);
		int64 GetTableValue(const std::vector<int64>& table, const std::vector<int64>& indexes);

};

}

