#include "stdafx.h"
#include "pop.h"
#include "params.h"
#include "state.h"
#include "rnd.h"
#include "data.h"
#include "FitnessEstimator.h"
#include "Fitness.h"
#include "Generationfns.h"
#include "InitPop.h"

void Generation(vector<ind>& pop,params& p,vector<Randclass>& r,data& d,state& s,FitnessEstimator& FE)
{
	
	//ind (*parloc)[p.popsize-1] = &pop;

	switch (p.sel)
	{
	case 1: // tournament selection
		{
		//if (p.loud) boost::progress_timer timer;
		// return pop ids for parents 
		vector<unsigned int> parloc(pop.size());
		//if (p.loud ) fcout << "     Tournament...";
		Tournament(pop,parloc,p,r);		
		//if (p.loud ) fcout << "     Apply Genetics...";
		try
		{
			ApplyGenetics(pop,parloc,p,r,d,s,FE);
		}
		catch(...)
				{
					cout<<"error in ApplyGenetics\n";
					throw;
				}
		//if (p.loud ) fcout << "     Gen 2 Phen...";
		//if (p.loud ) fcout << "     Fitness...";
		Fitness(pop,p,d,s,FE);
		//get mutation/crossover stats
		s.setCrossPct(pop);

		/*if (p.pHC_on && p.ERC)
		{
				for(int i=0; i<pop.size(); i++)
					HillClimb(pop.at(i),p,r,d,s);
		}
		if (p.eHC_on) 
		{
				for(int i=0; i<pop.size(); i++)
					EpiHC(pop.at(i),p,r,d,s);
		}*/
		break;
		}
	case 2: // deterministic crowding
		{	
			for (int j=0; j<p.popsize/omp_get_max_threads();j++) 
				DC(pop,p,r,d,s,FE);
			break;
		}
	case 3: // lexicase
		{
			vector<unsigned int> parloc(pop.size());
			LexicaseSelect(pop,parloc,p,r);
			//if (p.lex_age) vector<ind> tmppop(pop);

			ApplyGenetics(pop,parloc,p,r,d,s,FE);
			//FitnessLex(pop,parloc,p,r,d);
			if (!p.lexage)
			{
				Fitness(pop,p,d,s,FE);
				//get mutation/crossover stats
				s.setCrossPct(pop);
			}
			else
			{
				// add one new individual
				vector<ind> tmppop(1);
				tmppop[0].age=0;
				InitPop(tmppop,p,r);
				Fitness(tmppop,p,d,s,FE);
				pop.push_back(tmppop[0]);
				// select new population with tournament size 2, based on pareto age-fitness
				AgeFitSelect(pop,p,r);
			}
			break;
		}
	case 4: //age-fitness pareto front
		{   //scheme:
			// produce a new population equal in size to the old
			// pool all individuals from both populations
			
			AgeBreed(pop,p,r,d,s,FE);
			// add one new individual
			vector<ind> tmppop(1);
			tmppop[0].age=0;
			InitPop(tmppop,p,r);
			
			Fitness(tmppop,p,d,s,FE);
			pop.push_back(tmppop[0]);
			// select new population with tournament size 2, based on pareto age-fitness
			AgeFitSelect(pop,p,r);
		break;

		}
	default:
		cout << "Bad p.sel parameter. " << endl;
		break;
	}
	//if (p.loud ) fcout << "  Gentime...";
}


