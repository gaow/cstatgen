// $File: HaplotypingUtils.cpp $
// $LastChangedDate:  $
// $Rev:  $
// Copyright (c) 2014, Gao Wang <ewanggao@gmail.com>
// GNU General Public License (http://www.gnu.org/licenses/gpl.html)
#include "HaplotypingUtils.hpp"
#include "MerlinCluster.h"

#include <iostream>
#include <cstdlib>
#include <cctype>
#include <set>
#include <algorithm>
#include <sstream>

namespace cstatgen {

void DataLoader::LoadVariants(Pedigree * & ped,
                              const VecString & names,
                              const VecInt & positions,
                              const std::string & chrom,
                              double positionAdjustment)
{
	VecInt vs(names.size());

	for (unsigned i = 0; i < names.size(); ++i) {
		// marker already exists
		if (ped->LookupMarker(names[i].c_str()) != -1) continue;
		// add new marker
		int markerID = ped->GetMarkerID(names[i].c_str());
		ped->pd.AddMarkerColumn(names[i].c_str());
		MarkerInfo * info = ped->GetMarkerInfo(markerID);
		info->chromosome = (chrom == "X" || chrom == "x") ? 999 : atoi(chrom.c_str());
		// adjust input position to make sure every position is unique
		int position = positions[i];
		while (std::find(vs.begin(), vs.end(), position) != vs.end()) position++;
		vs[i] = position;
		info->positionFemale = info->positionMale = info->position = position * positionAdjustment;
		// std::clog << info->freq.dim << std::endl;
	}
}


void DataLoader::LoadSamples(Pedigree * & ped, const VecVecString & samples, const VecString & names)
{

	for (unsigned i = 0; i < samples.size(); ++i) {
		VecString fam_info(samples[i].begin(), samples[i].begin() + 5);
		VecString genotypes(samples[i].begin() + 5, samples[i].end());
		__AddPerson(ped, fam_info, genotypes, names);
	}

	ped->Sort();
	SortFamilies(*ped);
}


void DataLoader::__AddPerson(Pedigree * & ped, const VecString & fam_info,
                             const VecString & genotypes, const VecString & names)
{
	// add person info
	bool sex_failure = false;

	ped->AddPerson(fam_info[0].c_str(), fam_info[1].c_str(),
		fam_info[2].c_str(), fam_info[3].c_str(),
		ped->TranslateSexCode(fam_info[4].c_str(), sex_failure));
	// add person genotypes
	for (unsigned i = 0; i < names.size(); ++i) {
		int markerID = ped->GetMarkerID(names[i].c_str());
		// convert char's face value to int
		//ped->FindPerson(fam_info[0].c_str(), fam_info[1].c_str())->markers[markerID].one = (int)genotypes[i][0] - 48;
		//ped->FindPerson(fam_info[0].c_str(), fam_info[1].c_str())->markers[markerID].two = (int)genotypes[i][1] - 48;
		String c1(genotypes[i][0]);
		String c2(genotypes[i][1]);
		Alleles new_genotype;
		new_genotype[0] = ped->LoadAllele(ped->GetMarkerInfo(markerID), c1);
		new_genotype[1] = ped->LoadAllele(ped->GetMarkerInfo(markerID), c2);
		if (new_genotype.isKnown())
			ped->FindPerson(fam_info[0].c_str(), fam_info[1].c_str())->markers[markerID] = new_genotype;

	}
}


void GeneticHaplotyper::Apply(Pedigree * & ped)
{
	data.resize(0);

	if (__chrom == "X") ped->chromosomeX = true;
	ped->EstimateFrequencies(0, true);
	// recode alleles so more frequent alleles have lower allele numbers internally
	ped->LumpAlleles(0.0);
	// remove uninformative family or individuals
	// !! Do not trim here, because if a family is uninformative we can report as is
	// ped.Trim(true);

	FamilyAnalysis engine(*ped);
	// activate haplotyping options
	engine.bestHaplotype = true;
	engine.zeroRecombination = false;
	engine.SetupGlobals();
	String chrom = __chrom.c_str();
	engine.SetupMap(chrom);
	for (int i = 0; i < ped->familyCount; i++) {
		if (engine.SelectFamily(ped->families[i])) {
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


void MendelianErrorChecker::Apply(Pedigree * & ped)
{
	// check mendelian error for everyone's every marker in input ped object
	for (int i = 0; i < ped->count; i++) {
		// skip founder
		if (ped->persons[i]->isFounder()) continue;
		// identify founders for this person
		Person * mom = ped->persons[i]->mother;
		Person * dad = ped->persons[i]->father;
		for (int m = 0; m < ped->markerCount; m++) {
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
			if (!ped->persons[i]->markers[m].isKnown()) {
				if (dad->markers[m].isHomozygous() && mom->markers[m].isHomozygous()) {
					ped->persons[i]->markers[m].one = gdad1; ped->persons[i]->markers[m].two = gmom1;
				}
				continue;
			}
			// no error
			if (((ped->persons[i]->markers[m].one == gdad1 || ped->persons[i]->markers[m].one == gdad2) &&
			     (ped->persons[i]->markers[m].two == gmom1 || ped->persons[i]->markers[m].two == gmom2)) ||
			    ((ped->persons[i]->markers[m].two == gdad1 || ped->persons[i]->markers[m].two == gdad2) &&
			     (ped->persons[i]->markers[m].one == gmom1 || ped->persons[i]->markers[m].one == gmom2)))
				continue;
			// error found, make missing
			else {
				// std::clog << "Marker index " << m << std::endl;
				// std::clog << "Father geno " << gdad1 << " " << gdad2 << std::endl;
				// std::clog << "Mather geno " << gmom1 << " " << gmom2 << std::endl;
				// std::clog << "Kid geno " << ped[i].markers[m].one << " " << ped[i].markers[m].two << std::endl;
				__errorCount += 1;
				ped->persons[i]->markers[m].one = ped->persons[i]->markers[m].two = 0;
			}
		}
	}
}


inline std::string join(const std::vector<std::string> & vec, const char * delim = "")
{
	std::stringstream ss;

	std::for_each(vec.begin(), vec.end(), [&] (const std::string & s) { ss << ((ss.tellp()) ? delim : "") << s; });
	return ss.str();
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
	div_t divresult = div((int) n, a);
	// reduce size by x such that rem + res * x = a - x
	return (divresult.rem > 0) ? (unsigned)(a - ((a - divresult.rem) / (divresult.quot + 1))) : a;
}


inline std::string collapse_by_cluster(const VecString & haplotype, unsigned start, unsigned end, const VecVecInt & clusters)
{
	std::string collapsed_haplotype = "";
	std::vector<unsigned> clustered(0);
	for (unsigned i = start; i < end; ++i) {
		// previously being clustered with another position and was accounted for
		if (std::find(clustered.begin(), clustered.end(), i - 2) != clustered.end()) continue;
		// find positions that cluster with i
		std::vector<unsigned> icluster(0);
		for (unsigned c = 0; c < clusters.size(); ++c) {
			if (std::find(clusters[c].begin(), clusters[c].end(), i - 2) != clusters[c].end()) {
				for (unsigned j = 0; j < clusters[c].size(); ++j) {
					if (j >= (end - 2) ) continue;
					clustered.push_back(j);
					icluster.push_back(j + 2);
				}
			}
		}
		// collapse
		if (icluster.size() == 0) collapsed_haplotype += haplotype[i];
		else {
			std::string code = "1";
			for (unsigned j = 0; j < icluster.size(); ++j) {
				if (haplotype[icluster[j]] == "2") code = "2";
				else code = (code == "?" || code == "2") ? code : haplotype[icluster[j]];
			}
			collapsed_haplotype += code;
		}
	}
	return collapsed_haplotype;
}


inline std::string collapse_by_size(const VecString & haplotype, unsigned start, unsigned end, unsigned size)
{

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


inline VecVecDouble calculate_cmaf_by_size(const std::vector<double> & maf,
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


inline VecVecDouble calculate_cmaf_by_cluster(const std::vector<double> & maf,
                                              std::vector<unsigned> chunks, const VecVecInt & clusters)
{
	// append the ultimate end position
	chunks.push_back(maf.size() + 2);
	// return value
	VecVecDouble res(0);
	unsigned start = 0;
	for (unsigned c = 0; c < chunks.size(); ++c) {
		unsigned end = chunks[c] - 2;
		std::vector<unsigned> clustered(0);
		std::vector<double> cmaf(0);
		for (unsigned i = start; i < end; ++i) {
			// previously being clustered with another position and was accounted for
			if (std::find(clustered.begin(), clustered.end(), i - 2) != clustered.end()) continue;
			// find positions that cluster with i
			std::vector<unsigned> icluster(0);
			for (unsigned c = 0; c < clusters.size(); ++c) {
				if (std::find(clusters[c].begin(), clusters[c].end(), i - 2) != clusters[c].end()) {
					for (unsigned j = 0; j < clusters[c].size(); ++j) {
						if (j >= (end - 2) ) continue;
						clustered.push_back(j);
						icluster.push_back(j + 2);
					}
				}
			}
			// collapse
			if (icluster.size() == 0) cmaf.push_back(maf[i]);
			else {
				double icmaf = 1.0;
				for (unsigned j = 0; j < icluster.size(); ++j) {
					icmaf *= (1 - maf[j]);
				}
				cmaf.push_back(1 - icmaf);
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


inline std::string collapse(const VecString & haplotype, unsigned start, unsigned end, double size, const VecVecInt & clusters)
{
	if (end == 0) end = haplotype.size();
	if (start == end) return "?";
	if (size > 0 && size < 1) return collapse_by_cluster(haplotype, start, end, clusters);
	else return collapse_by_size(haplotype, start, end, unsigned(size));
}


inline VecVecDouble calculate_cmaf(const std::vector<double> & maf, std::vector<unsigned> chunks, double size, const VecVecInt & clusters)
{
	if (size > 0 && size < 1) return calculate_cmaf_by_cluster(maf, chunks, clusters);
	else return calculate_cmaf_by_size(maf, chunks, unsigned(size));
}


void HaplotypeCoder::Execute(const VecVecVecString & haploVecsConst, const VecVecDouble & mafVecsConst,
                             const VecVecVecInt & markerIdxClusters, bool recomb)
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
                if (recomb){
                    if (!hasEnding(haploVecs[f][p][i], ":") &&
                        !hasEnding(haploVecs[f][p][i], "|") &&
                        i != 2) {
                        __recombCount += 1;
                        if (std::find(recombPositions.begin(), recombPositions.end(), i) == recombPositions.end()) {
                            recombPositions.push_back(i);
                        }
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
		VecVecString found_patterns(haploVecs[f].size());
		for (unsigned p = 0; p < haploVecs[f].size(); p++) {
			for (unsigned r = 0; r <= recombPositions.size(); r++) {
				std::string haplotype = collapse(haploVecs[f][p],
					(r > 0) ? recombPositions[r - 1] : 2,
					(r == recombPositions.size()) ? haploVecs[f][p].size() : recombPositions[r],
					__size, markerIdxClusters[f]);
				//std::clog<<p<<'\t'<<r<<'\t'<<"hap:"<<haplotype<<std::endl;
				//if (haplotype != "?" &&
				if (haplotype.find("?") == std::string::npos &&
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
				//std::clog << p<<'\t'<<r<<'\t'<<haploStrs[p][r]<< std::endl;
				std::sort(patterns[r].begin(), patterns[r].end(),[](std::string a, std::string b){return std::count(a.begin(), a.end(), '2') < std::count(b.begin(), b.end(), '2');});
				if (haploStrs[p][r].find("?") != std::string::npos) {haploStrs[p][r] = "0"; found_patterns[p].push_back("NULL");}
				else {
					int pattern_idx=std::find(patterns[r].begin(), patterns[r].end(),haploStrs[p][r])-patterns[r].begin();
					haploStrs[p][r] = std::to_string(pattern_idx + 1);
				        found_patterns[p].push_back(patterns[r][pattern_idx]);
					}
			}
			// push combined haplotype to data
			if (!__data.size() || (__data.back()[0] != haploVecs[f][p][0] ||
			                       __data.back()[1] != haploVecs[f][p][1])) {
				VecString newperson(haploVecs[f][p].begin(), haploVecs[f][p].begin() + 2);
				__data.push_back(newperson);
			}
			__data[__data.size() - 1].push_back(join(haploStrs[p], ","));
			__data[__data.size() - 1].push_back(join(found_patterns[p], ","));
		}
		//
		// calculate haplotype pattern frequency
		//
		// size of cmafs equals recombPositions.size() + 1
		VecVecDouble cmafs = calculate_cmaf(mafVecsConst[f], recombPositions, __size, markerIdxClusters[f]);
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
