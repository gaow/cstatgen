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


namespace cstatgen {

class RVTester
{
public:
	RVTester (const std::vector<std::vector<double> > & genotype,
		const std::vector<std::vector<double> > & phenotype,
		const std::vector<std::vector<double> > & covariates)
	{
		// genotype matrix rows are variants, cols are people
		Matrix mg;

		mg.Dimension(genotype[0].size(), genotype.size());
		for (int row = 0; row < genotype[0].size(); ++row) {
			for (int col = 0; col < genotype.size(); ++col) {
				mg[row][col] = genotype[row][col];
			}
		}
		// phentype matrix has dimension nx1
		Matrix mp;
		mp.Dimension(phenotype[0].size(), phenotype.size());
		for (int row = 0; row < phenotype[0].size(); ++row) {
			for (int col = 0; col < phenotype.size(); ++col) {
				mp[row][col] = phenotype[col][row];
			}
		}
		//
		Matrix mc;
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
		m_dc.consolidate(mp, mc, mg);
	}


	~RVTester()
	{
		delete m_model;
	};
  // DataConsolidator does not allow for copying
	// RVTester * clone() const { return new RVTester(*this); }
	int fit(const std::string t, bool isBinary = false, int n = 1000, double a = 0.05)
	{
		// if (t == "AnalyticVT") m_model = new AnalyticVT();
		if (t == "CMATTest") m_model = new CMATTest(n, a);
		else if (t == "CMCFisherExactTest") m_model = new CMCFisherExactTest();
		else if (t == "CMCTest") m_model = new CMCTest();
		else if (t == "CMCWaldTest") m_model = new CMCWaldTest();
		else if (t == "MadsonBrowningTest") m_model = new MadsonBrowningTest(n, a);
		else if (t == "RareCoverTest") m_model = new RareCoverTest(n, a);
		else if (t == "KBACTest") m_model = new KBACTest(n, a);
		else if (t == "FpTest") m_model = new FpTest();
		else if (t == "SkatTest") m_model = new SkatTest(n, a, 1, 25);
		else if (t == "VariableThresholdPrice") m_model = new VariableThresholdPrice(n, a);
		else if (t == "VTCMC") m_model = new VTCMC();
		else if (t == "ZegginiWaldTest") m_model = new ZegginiWaldTest();
		else if (t == "ZegginiTest") m_model = new ZegginiTest();
		else m_model = new SingleVariantScoreTest();
		if (isBinary) m_model->setBinaryOutcome();
		int status = m_model->fit(&m_dc);
        m_model -> writeOutput(NULL, m_siteInfo);
        return status;
	}


	std::string pvalue()
	{
		return m_model->getResult()["Pvalue"];
	}


private:
	DataConsolidator m_dc;
	ModelFitter * m_model;
  Result m_siteInfo;
};
}
#endif
