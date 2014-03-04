// $File: HaplotypingUtils.cpp $
// $LastChangedDate:  $
// $Rev:  $
// Copyright (c) 2014, Gao Wang <ewanggao@gmail.com>
// GNU General Public License (http://www.gnu.org/licenses/gpl.html)
#include "HaplotypingUtils.hpp"

#include <iostream>
#include <cstdlib>
#include <cctype>
#include <set>
#include <algorithm>

namespace cstatgen {

void reset_ped(Pedigree & ped)
{

	// FIXME: It does not make sense to me but I have to reset ped data object manually,
	// otherwise in a Python program that calls CHP::Apply multiple times in a loop,
	// ped.GetMarkerInfo(i)->freq.dim will not equal 0 after a few runs
	// complaining the same marker name has been previously used.
	// cannot yet figure out why as this is suppose to be a brand new ped object here!
	// UPDATE:
	// It might due to using it from Python. The swig generated wrapper did not delete it after use
	// anyways let me just manually clean up everything instead of wrestling with swig
	// UPDATE 2:
	// seems it's just the markerInfo part gives problems; ped.count and ped.familyCount are always 0

	// delete old pointer
	for (int i = 0; i < ped.markerInfoCount; i++)
		delete ped.markerInfo[i];
	delete [] ped.markerInfo;
	delete [] ped.markerInfoByInteger;
	// FIXME: Only Clear() is not going to delete the char * buffer pointer which unfortunately is protected ...
	// so there will still be memory leak here
	ped.markerNames.Clear();
	ped.markerLookup.Clear();
	ped.markerInfoByName.Clear();
	ped.markerCount = ped.markerInfoCount = ped.markerInfoSize = 0;
	// reset pointer
	ped.GrowMarkerInfo();
	// delete old pointer
	for (int i = 0; i < ped.count; i++)
		delete ped.persons[i];

	for (int i = 0; i < ped.familyCount; i++)
		delete ped.families[i];

	delete [] ped.families;
	delete [] ped.persons;
	ped.size = 10000;
	ped.count = ped.familyCount = ped.haveTwins = 0;
	// reset pointer
	ped.persons = new Person *[ped.size];
	ped.families = new Family * [1];
}


void DataLoader::LoadVariants(Pedigree & ped,
                              const VecString & names,
                              const VecInt & positions,
                              const std::string & chrom,
                              double positionAdjustment)
{
	VecInt vs(names.size());

	for (unsigned i = 0; i < names.size(); ++i) {
		ped.pd.columnHash.Push(ped.GetMarkerID(names[i].c_str()));
		ped.pd.columns.Push(1);
		ped.pd.columnCount++;
		MarkerInfo * info = ped.GetMarkerInfo(i);
		info->chromosome = (chrom == "X" || chrom == "x") ? 999 : atoi(chrom.c_str());
		// adjust input position to make sure every position is unique
		int position = positions[i];
		while (std::find(vs.begin(), vs.end(), position) != vs.end()) position++;
		vs[i] = position;
		info->positionFemale = info->positionMale = info->position = position * positionAdjustment;
		// std::clog << info->freq.dim << std::endl;
	}
}


void DataLoader::LoadSamples(Pedigree & ped, const VecVecString & samples)
{

	for (unsigned i = 0; i < samples.size(); ++i) {
		VecString fam_info(samples[i].begin(), samples[i].begin() + 5);
		VecString genotypes(samples[i].begin() + 5, samples[i].end());
		__AddPerson(ped, fam_info, genotypes);
	}

	ped.Sort();
	SortFamilies(ped);

}


void DataLoader::__AddPerson(Pedigree & ped, VecString & fam_info, VecString & genotypes)
{
	// add person info
	bool sex_failure = false;

	ped.AddPerson(fam_info[0].c_str(), fam_info[1].c_str(),
		fam_info[2].c_str(), fam_info[3].c_str(),
		ped.TranslateSexCode(fam_info[4].c_str(), sex_failure));
	// add person genotypes
	for (unsigned i = 0; i < genotypes.size(); ++i) {
		// convert char's face value to int
		ped.FindPerson(fam_info[0].c_str(), fam_info[1].c_str())->markers[i].one = (int)genotypes[i][0] - 48;
		ped.FindPerson(fam_info[0].c_str(), fam_info[1].c_str())->markers[i].two = (int)genotypes[i][1] - 48;
		// String c1(genotypes[i][0]);
		// String c2(genotypes[i][1]);
		// Alleles new_genotype;
		// new_genotype[0] = ped.LoadAllele(ped.GetMarkerInfo(i), c1);
		// new_genotype[1] = ped.LoadAllele(ped.GetMarkerInfo(i), c2);
		// if (new_genotype.isKnown())
        // ped.FindPerson(fam_info[0].c_str(), fam_info[1].c_str())->markers[i] = new_genotype;
	}
}


void GeneticHaplotyper::Apply(Pedigree & ped)
{
	data.resize(0);

	String chrom = __chrom.c_str();
	if (chrom == "X") ped.chromosomeX = true;
	//
	ped.EstimateFrequencies(0, true);
	// recode alleles so more frequent alleles have lower allele numbers internally
	ped.LumpAlleles(0.0);
	// remove uninformative family or individuals
	// !! Do not trim here, because if a family is uninformative we can report as is
	// ped.Trim(true);
	FamilyAnalysis engine(ped);
	// activate haplotyping options
	engine.bestHaplotype = true;
	engine.zeroRecombination = false;
	engine.SetupGlobals();
	engine.SetupMap(chrom);
	for (int i = 0; i < ped.familyCount; i++) {
		if (engine.SelectFamily(ped.families[i])) {
			engine.Analyse();
			data.push_back(engine.hapOutput);
		}
	}
	engine.CleanupGlobals();
}


void GeneticHaplotyper::Print()
{
	for (unsigned f = 0; f < data.size(); f++) {
		for (unsigned p = 0; p < data[f].size(); p++) {
			for (unsigned i = 0; i < data[f][p].size(); i++) {
				std::clog << data[f][p][i] << "\t";
			}
			std::clog << std::endl;
		}
		std::clog << std::endl;
	}
	std::clog << std::endl;
}


void MendelianErrorChecker::Apply(Pedigree & ped)
{
	// check mendelian error for everyone's every marker in input ped object
	for (int i = 0; i < ped.count; i++) {
		// skip founder
		if (ped[i].isFounder()) continue;
		// identify founders for this person
		Person * mom = ped[i].mother;
		Person * dad = ped[i].father;
		for (int m = 0; m < ped.markerCount; m++) {
			//
			// genotype data missing for both founders
			//
			if (!mom->markers[m].isKnown() && !dad->markers[m].isKnown())
				continue;
			//
			// if mother/father missing, substitute with uninformative marker
			// otherwise use as is
			//
			char gdad1, gdad2, gmom1, gmom2;
			if (!mom->markers[m].isKnown()) {
				gmom1 = 1; gmom2 = 2;
			} else {
				gmom1 = mom->markers[m].one; gmom2 = mom->markers[m].two;
			}
			if (!dad->markers[m].isKnown()) {
				gdad1 = 1; gdad2 = 2;
			} else {
				gdad1 = dad->markers[m].one; gdad2 = dad->markers[m].two;
			}
			//
			// check for mendelian error
			//
			// person missing data
			if (!ped[i].markers[m].isKnown()) {
				if (dad->markers[m].isHomozygous() && mom->markers[m].isHomozygous()) {
					ped[i].markers[m].one = gdad1; ped[i].markers[m].two = gmom1;
				}
				continue;
			}
			// no error
			if (((ped[i].markers[m].one == gdad1 || ped[i].markers[m].one == gdad2) &&
			     (ped[i].markers[m].two == gmom1 || ped[i].markers[m].two == gmom2)) ||
			    ((ped[i].markers[m].two == gdad1 || ped[i].markers[m].two == gdad2) &&
			     (ped[i].markers[m].one == gmom1 || ped[i].markers[m].one == gmom2)))
				continue;
			// error found, make missing
			else {
				// std::clog << "Marker index " << m << std::endl;
				// std::clog << "Father geno " << gdad1 << " " << gdad2 << std::endl;
				// std::clog << "Mather geno " << gmom1 << " " << gmom2 << std::endl;
				// std::clog << "Kid geno " << ped[i].markers[m].one << " " << ped[i].markers[m].two << std::endl;
				__errorCount += 1;
				ped[i].markers[m].one = ped[i].markers[m].two = 0;
			}
		}
	}
}


inline bool hasEnding(const std::string & fullString, const std::string & ending)
{
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}


inline unsigned adjustSize(unsigned n, unsigned a)
{
	if (a == 0) return n;
	if (a == 1) return a;
	div_t divresult = div(n, a);
	// reduce size by x such that rem + res * x = a - x
	return (unsigned)(a - ((a - divresult.rem) / (divresult.quot + 1)));
}


inline std::string collapse(const VecString & haplotype, unsigned start, unsigned end, unsigned size)
{
	if (end == 0) end = haplotype.size();
	if (start == end) return "?";
	std::string collapsed_haplotype = "";
	unsigned wsize = adjustSize(end - start, size);
	unsigned counter = 0;
	std::string code = "1";

	for (unsigned i = start; i < end; ++i) {
		counter += 1;
		if (haplotype[i] == "2") code = "2";
		else code = (code == "?" || code == "2") ? code : haplotype[i];
		if (counter == wsize) {
			collapsed_haplotype += code;
			counter = 0;
			code = "1";
		}
	}
	// make it missing data if "?" is in the haplotype
	if (collapsed_haplotype.find("?") != std::string::npos) return "?";
	else return collapsed_haplotype;
}


inline VecVecDouble calculate_cmaf(const std::vector<double> & maf,
                                   std::vector<unsigned> chunks, unsigned size)
{
	// append the ultimate end position
	chunks.push_back(maf.size() + 2);
	// return value
	VecVecDouble res(0);
	unsigned start = 0;
	for (unsigned c = 0; c < chunks.size(); ++c) {
		unsigned end = chunks[c] - 2;
		unsigned wsize = adjustSize(end - start, size);
		std::vector<double> cmaf(0);
		unsigned counter = 0;
		double icmaf = 1.0;
		for (unsigned i = start; i < end; ++i) {
			counter += 1;
			icmaf *= (1 - maf[i]);
			if (counter == wsize) {
				cmaf.push_back(1 - icmaf);
				counter = 0;
				icmaf = 1.0;
			}
		}
		res.push_back(cmaf);
		start = end;
	}
	// for (unsigned i = 0 ; i < res.size(); ++i) {
	//  for (unsigned j = 0; j < res[i].size(); ++j) {
	//      std::clog << res[i][j] << " ";
	//  }
	//  std::clog << std::endl;
	// }
	// std::clog << std::endl;
	return res;
}


void HaplotypeCoder::Execute(const VecVecVecString & haploVecsConst, const VecVecDouble & mafVecsConst)
{
	__data.resize(0);
	__freqs.clear();
	VecVecVecString haploVecs = haploVecsConst;
	if (!haploVecs.size()) return;
	for (unsigned f = 0; f < haploVecs.size(); f++) {
		//
		// record recombination and adjust data format
		//
		std::vector<unsigned> recombPositions(0);
		for (unsigned p = 0; p < haploVecs[f].size(); p++) {
			for (unsigned i = 2; i < haploVecs[f][p].size(); i++) {
				// recombination event detected
				if (!hasEnding(haploVecs[f][p][i], ":") &&
				    !hasEnding(haploVecs[f][p][i], "|")) {
					__recombCount += 1;
					if (std::find(recombPositions.begin(), recombPositions.end(), i) == recombPositions.end()) {
						recombPositions.push_back(i);
					}
				}
				// use one of the likely haplotype configuration
				// http://www.sph.umich.edu/csg/abecasis/merlin/tour/haplotyping.html
				haploVecs[f][p][i] = (isupper(haploVecs[f][p][i][0]))
				                     ? haploVecs[f][p][i].substr(1, 1) : haploVecs[f][p][i].substr(0, 1);
			}
		}
		std::sort(recombPositions.begin(), recombPositions.end());
		//
		// collapse haplotype vectors to haplotype Strings
		//
		VecVecString haploStrs(haploVecs[f].size());
		VecVecString patterns(recombPositions.size() + 1);
		for (unsigned p = 0; p < haploVecs[f].size(); p++) {
			for (unsigned r = 0; r <= recombPositions.size(); r++) {
				std::string haplotype = collapse(haploVecs[f][p],
					(r > 0) ? recombPositions[r - 1] : 2,
					(r == recombPositions.size()) ? haploVecs[f][p].size() : recombPositions[r],
					__size);
				if (haplotype != "?" &&
				    std::find(patterns[r].begin(), patterns[r].end(), haplotype) == patterns[r].end()) {
					patterns[r].push_back(haplotype);
				}
				haploStrs[p].push_back(haplotype);
			}
		}
		//
		// convert haplotype super strings to haplotype patterns
		//
		for (unsigned p = 0; p < haploVecs[f].size(); p++) {
			// convert one haplotype's all recomb segments to a single super marker
			for (unsigned r = 0; r <= recombPositions.size(); r++) {
				std::sort(patterns[r].begin(), patterns[r].end());
				if (haploStrs[p][r] == "?") haploStrs[p][r] = "0";
				else haploStrs[p][r] = std::to_string(std::find(patterns[r].begin(), patterns[r].end(),
							haploStrs[p][r]) - patterns[r].begin() + 1);
			}
			// push combined haplotype to data
			if (!__data.size() || (__data.back()[0] != haploVecs[f][p][0] ||
			                       __data.back()[1] != haploVecs[f][p][1])) {
				VecString newperson(haploVecs[f][p].begin(), haploVecs[f][p].begin() + 2);
				__data.push_back(newperson);
			}
			__data[__data.size() - 1].push_back(std::accumulate(haploStrs[p].begin(), haploStrs[p].end(),
					std::string("")));
		}
		//
		// calculate haplotype pattern frequency
		//
		// size of cmafs equals recombPositions.size() + 1
		VecVecDouble cmafs = calculate_cmaf(mafVecsConst[f], recombPositions, __size);
		__freqs[haploVecs[f][0][0]] = VecVecDouble();
		for (unsigned r = 0; r <= recombPositions.size(); r++) {
			std::vector<double> tmp(patterns[r].size());
			for (unsigned i = 0; i < patterns[r].size(); i++) {
				tmp[i] = 1.0;
				for (unsigned j = 0; j < cmafs[r].size(); j++) {
					tmp[i] *= (patterns[r][i][j] == '2') ? cmafs[r][j] : (1.0 - cmafs[r][j]);
				}
			}
			__freqs[haploVecs[f][0][0]].push_back(tmp);
		}
	}
}


void HaplotypeCoder::Print()
{
	for (unsigned p = 0; p < __data.size(); p++) {
		for (unsigned i = 0; i < __data[p].size(); i++) {
			std::clog << __data[p][i] << "\t";
		}
		std::clog << std::endl;
	}
	std::clog << std::endl;
}


}
