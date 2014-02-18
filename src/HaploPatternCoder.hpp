// $File: HaploPatternCoder.hpp $
// $LastChangedDate:  $
// $Rev:  $
// Copyright (c) 2014, Gao Wang <ewanggao@gmail.com>
// GNU General Public License (http://www.gnu.org/licenses/gpl.html)
#ifndef _CHP_HPP_
#define _CHP_HPP_

#include <string>
#include <stdexcept>
#include "Exception.hpp"
#include "HaplotypingUtils.hpp"

namespace cstatgen {

class CHP
{
public:
	CHP(const int wsize, const double padj = 0.01, const int verbose = 0) :
		__windowSize(wsize), __positionAdjustment(padj), __verbose(verbose) {};
	~CHP() {};
	CHP * clone() const { return new CHP(*this); }
	// input chrom must be 1 .. 22 and X
	// input samples follows PED convention, e.g., { "1", "1", "0", "0", "1", "21", "21", "21" }
	// first 5 cols are fid, sid, pid, mid and sex (0/1/2 coding), followed by genotypes
	// (1/2 coding, 0 for missing)
	VecVecString Apply(const std::string & chrom, const VecString & marker_names,
	                   const VecInt & marker_positions, const VecVecString & samples)
	{
		Pedigree ped;

		reset_ped(ped);

		try {
			DataLoader dl;
			dl.LoadVariants(ped, marker_names, marker_positions, chrom, __positionAdjustment);
			dl.LoadSamples(ped, samples);
			MendelianErrorChecker mc;
			mc.Apply(ped);
			__mendelianErrorCount = mc.errorCount;
			GeneticHaplotyper gh(chrom);
			gh.Apply(ped);
			if (__verbose) gh.Print();
			HaplotypeCoder hc(__windowSize);
			hc.Apply(gh.data);
			__recombCount = hc.recombCount;
			if (__verbose) hc.Print();
			return hc.data;
			// } catch (...) {
		} catch (std::exception e) {
			// std::clog << e.what() << std::endl;
			const VecVecString nulldata(0);
			return nulldata;
		}
	}


	int countMendelianErrors() { return __mendelianErrorCount; }
	int countRecombs() { return __recombCount; }

private:
	int __mendelianErrorCount;
	int __recombCount;
	int __windowSize;
	double __positionAdjustment;
	int __verbose;
};
}
#endif
