// $File: HaplotypingEngine.hpp $
// $LastChangedDate:  $
// $Rev:  $
// Copyright (c) 2014, Gao Wang <ewanggao@gmail.com>
// GNU General Public License (http://www.gnu.org/licenses/gpl.html)
#ifndef _HPE_HPP_
#define _HPE_HPP_

#include <string>
#include <stdexcept>
#include "Exception.hpp"
#include "HaplotypingUtils.hpp"

namespace cstatgen {

class HaplotypingEngine
{
public:
	HaplotypingEngine(const int verbose = 0) :
		__verbose(verbose), __mendelianErrorCount(0) {};
	~HaplotypingEngine() {};
	HaplotypingEngine * clone() const { return new HaplotypingEngine(*this); }
	// input chrom must be 1 .. 22 and X
	// input samples follows PED convention, e.g., { "1", "1", "0", "0", "1", "21", "21", "21" }
	// first 5 cols are fid, sid, pid, mid and sex (0/1/2 coding), followed by genotypes
	// (1/2 coding, 0 for missing)
	// positionAdjustment: adjust physical distance to map distance, 1 / 100 million
	VecVecVecString Execute(const std::string & chrom, const VecString & marker_names,
	                    const VecInt & marker_positions, const VecVecString & samples,
	                    double positionAdjustment = 1E-8)
	{
		Pedigree ped;

		reset_ped(ped);

		try {
			DataLoader dl;
			dl.LoadVariants(ped, marker_names, marker_positions, chrom, positionAdjustment);
			dl.LoadSamples(ped, samples);
			MendelianErrorChecker mc;
			mc.Apply(ped);
			__mendelianErrorCount += mc.errorCount;
			GeneticHaplotyper gh(chrom);
			gh.Apply(ped);
			if (__verbose) gh.Print();
			return gh.data;
			// } catch (...) {
		} catch (std::exception e) {
			// std::clog << e.what() << std::endl;
			const VecVecVecString nulldata(0);
			return nulldata;
		}
	}


	int countMendelianErrors() { return __mendelianErrorCount; }

private:
	int __verbose;
	int __mendelianErrorCount;
};

}
#endif
