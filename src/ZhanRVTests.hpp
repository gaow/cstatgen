// $File: ZhanRVTests.hpp $
// $LastChangedDate:  $
// $Rev:  $
// Copyright (c) 2014, Gao Wang <ewanggao@gmail.com>
// GNU General Public License (http://www.gnu.org/licenses/gpl.html)

#ifndef _ZHANRV_HPP_
#define _ZHANRV_HPP_

#include "ModelFitter.h"
#include "DataConsolidator.h"
#include "MathMatrix.h"
#include "Exception.hpp"

#include <vector>
#include <string>
#include <numeric>      // std::accumulate
#include <memory>

namespace cstatgen {

class RVTester
{
public:
	RVTester (const std::vector<std::vector<double> > & genotype_all,
		const std::vector<std::vector<double> > & phenotype,
		const std::vector<std::vector<double> > & covariates,
		double maf_lower = 0.0, double maf_upper = 1.0)
	{
		// input genotype is like VCF format rows are variants cols are samples
		// genotype matrix rows are samples, cols are variants
		std::vector<std::vector<double> > genotype = m_PruneByMAF(genotype_all, maf_lower, maf_upper);
		if (genotype.size() <= 1) {
			throw RuntimeError("Too few variants to analyze after pruning by MAF");
		}
		mg.Dimension(genotype[0].size(), genotype.size());
		for (int row = 0; row < genotype[0].size(); ++row) {
			for (int col = 0; col < genotype.size(); ++col) {
				mg[row][col] = genotype[col][row];
			}
		}
		// phentype matrix has dimension nx1
		mp.Dimension(phenotype[0].size(), phenotype.size());
		for (int row = 0; row < phenotype[0].size(); ++row) {
			for (int col = 0; col < phenotype.size(); ++col) {
				mp[row][col] = phenotype[col][row];
			}
		}
		//
		if (covariates.size() == 0) mc.Dimension(0, 0);
		else {
			mc.Dimension(covariates[0].size(), covariates.size());
			for (int row = 0; row < covariates[0].size(); ++row) {
				for (int col = 0; col < covariates.size(); ++col) {
					mc[row][col] = covariates[col][row];
				}
			}
		}
		m_dc.setStrategy(DataConsolidator::IMPUTE_MEAN);
	}


	~RVTester()
	{
		// if (m_model) delete m_model;
		// Use shared_ptr, no need to delete
	};
	// DataConsolidator does not allow for copying
	// RVTester * clone() const { return new RVTester(*this); }
	int fit(const std::string t, bool is_binary, int n = 1000, double a = 0.05)
	{
		m_dc.consolidate(mp, mc, mg);
		// if (t == "AnalyticVT") m_model = new AnalyticVT();
		if (t == "CMATTest") m_model = std::make_shared<CMATTest>(n, a);
		else if (t == "CMCFisherExactTest") m_model = std::make_shared<CMCFisherExactTest>();
		else if (t == "CMCTest") m_model = std::make_shared<CMCTest>();
		else if (t == "CMCWaldTest") m_model = std::make_shared<CMCWaldTest>();
		else if (t == "MadsonBrowningTest") m_model = std::make_shared<MadsonBrowningTest>(n, a);
		else if (t == "RareCoverTest") m_model = std::make_shared<RareCoverTest>(n, a);
		else if (t == "KBACTest") m_model = std::make_shared<KBACTest>(n, a);
		else if (t == "FpTest") m_model = std::make_shared<FpTest>();
		else if (t == "SkatTest") m_model = std::make_shared<SkatTest>(0, a, 1, 25);
		else if (t == "VariableThresholdPrice") m_model = std::make_shared<VariableThresholdPrice>(n, a);
		// else if (t == "VTCMC") m_model = std::make_shared<VTCMC>();
		else if (t == "ZegginiWaldTest") m_model = std::make_shared<ZegginiWaldTest>();
		else if (t == "ZegginiTest") m_model = std::make_shared<ZegginiTest>();
		else m_model = std::make_shared<SingleVariantScoreTest>();
		if (is_binary) m_model->setBinaryOutcome();
		int status = 0;
		try {
			status = m_model->fit(&m_dc);
		} catch (...) {
			return -1;
		}
		m_model->writeOutput(NULL, m_siteInfo);
		return status;
	}


	std::string pvalue()
	{
		if (m_model->getResult()["PvalueTwoSide"] != "NA")
			return m_model->getResult()["PvalueTwoSide"];
		else
			return m_model->getResult()["Pvalue"];
	}


private:
	Matrix mg;
	Matrix mp;
	Matrix mc;
	DataConsolidator m_dc;
	std::shared_ptr<ModelFitter> m_model;
	Result m_siteInfo;

	std::vector<std::vector<double> > m_PruneByMAF(const std::vector<std::vector<double> > & genotype_all,
	                                               double maf_lower, double maf_upper)
	{
		if (maf_lower > 0.0 || maf_upper < 0.5) {
			// remove by cutoff
			std::vector<std::vector<double> > genotype(0);
			double maf = 0.0;
			for (unsigned i = 0; i < genotype_all.size(); ++i) {
				maf = std::accumulate(genotype_all[i].begin(), genotype_all[i].end(), 0.0) /
				      float(genotype_all[i].size());
				maf = (maf > 0.5) ? 1 - maf : maf;
				if (maf > maf_lower && maf < maf_upper) genotype.push_back(genotype_all[i]);
			}
			return genotype;
		} else return genotype_all;
	}


};
}
#endif
