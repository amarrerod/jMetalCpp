//  CMAES.cpp
//
//  Author:
//       Esteban López-Camacho <esteban@lcc.uma.es>
//
//  Copyright (c) 2011 Antonio J. Nebro, Juan J. Durillo
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include <CMAES.h>


/*
 * This class implements the CMA-ES algorithm.
 */


/**
 * Constructor
 * @param problem Problem to solve
 */
CMAES::CMAES(Problem *problem) : Algorithm(problem) {
} // CMAES


SolutionSet * CMAES::execute() {
  
  //int populationSize;
  int maxEvaluations;
  //int evaluations;
  
  SolutionSet * population;
  
  //Read the parameters
  populationSize_ = *(int *) getInputParameter("populationSize");
  maxEvaluations = *(int *) getInputParameter("maxEvaluations");
  
  //Initialize the variables
  //population = new SolutionSet(populationSize);
  counteval = 0;
  countiter = 0;
  
  Comparator * comparator = new ObjectiveComparator(0) ;
  
  cout << "PUA" << endl;

  // ESTO ES EL PROBLEMA DE JMETALCPP
  //IObjectiveFunction fitfun = new Rosenbrock();

  // new a CMA-ES and set some initial values
  
  // PONE state a -1 PARA PERMITIR INICIALIZACIONES
  //CMAEvolutionStrategy cma = new CMAEvolutionStrategy();
  
  // LEE FICHERO DE PROPIEDADES INICIALES ¿COPIAR?
  //cma.readProperties(); // read options, see file CMAEvolutionStrategy.properties
  
  // TAMAÑO DE N (NUMERO DE VARIABLES), NOSOTROS LO COGEMOS DE PROBLEM)
  //cma.setDimension(10); // overwrite some loaded properties
//  cma.setInitialX(0.05); // in each dimension, also setTypicalX can be used
//  cma.setInitialStandardDeviation(0.2); // also a mandatory setting 
//  cma.options.stopFitness = 1e-14;       // optional setting
  
  sp = new CMAParameters(problem_->getNumberOfVariables(), populationSize_);
  
  cout << "PUA2" << endl;

  // initialize cma and get fitness array to fill in later
  double * fitness = init();  // new double[cma.parameters.getPopulationSize()];

//  // initial output to files
//  cma.writeToDefaultFilesHeaders(0); // 0 == overwrites old files
//
//  // iteration loop
//  while(cma.stopConditions.getNumber() == 0) {
  while (counteval < maxEvaluations) {
//
    cout << "PUA3" << endl;
    // --- core iteration step ---
    SolutionSet * pop = samplePopulation(); // get a new population of solutions
    
    cout << "PUA4" << endl;
    
    for(int i = 0; i < pop->size(); i++) {    // for each candidate solution i
            // a simple way to handle constraints that define a convex feasible domain  
            // (like box constraints, i.e. variable boundaries) via "blind re-sampling" 
                                                   // assumes that the feasible domain is convex, the optimum is  
//      while (!fitfun.isFeasible(pop[i]))     //   not located on (or very close to) the domain boundary,  
//        pop[i] = cma.resampleSingle(i);    //   initialX is feasible and initialStandardDeviations are  
                                                     //   sufficiently small to prevent quasi-infinite looping here
              // compute fitness/objective value	
//      fitness[i] = fitfun.valueOf(pop[i]); // fitfun.valueOf() is to be minimized
      problem_->evaluate(pop->get(i));
    }
    
    cout << "PUA5" << endl;
    
//    cma.updateDistribution(fitness);         // pass fitness array to update search distribution
    updateDistribution();
//          // --- end core iteration step ---
//
//    // output to files and console 
//    cma.writeToDefaultFiles();
//    int outmod = 150;
//    if (cma.getCountIter() % (15*outmod) == 1)
//      cma.printlnAnnotation(); // might write file as well
//    if (cma.getCountIter() % outmod == 1)
//      cma.println(); 
  }
  
  cout << "PUA6" << endl;
  
  population_->sort(comparator) ;
  
  delete comparator;

  SolutionSet * resultPopulation  = new SolutionSet(1) ;
  resultPopulation->add(new Solution(population->get(0))) ;
  delete population;

  return resultPopulation ;
  
  
//  // evaluate mean value as it is the best estimator for the optimum
//  cma.setFitnessOfMeanX(fitfun.valueOf(cma.getMeanX())); // updates the best ever solution 
//
//  // final output
//  cma.writeToDefaultFiles(1);
//  cma.println();
//  cma.println("Terminated due to");
//  for (String s : cma.stopConditions.getMessages())
//    cma.println("  " + s);
//  cma.println("best function value " + cma.getBestFunctionValue() 
//      + " at evaluation " + cma.getBestEvaluationNumber());
//
//   we might return cma.getBestSolution() or cma.getBestX()
  
} // execute

double * CMAES::init() {
  int i;
//  if (N <= 0)
//    error("dimension needs to be determined, use eg. setDimension() or setInitialX()");
//  if (state >= 0)
//    error("init() cannot be called twice");
//  if (state == 0) // less save variant 
//    return new double[sp.getLambda()]; 
//  if (state > 0)  
//    error("init() cannot be called after the first population was sampled");

//  sp = parameters; /* just in case the user assigned parameters */
//  if (sp.supplemented == 0) // a bit a hack
//    sp.supplementRemainders(N, options);
//  sp.locked = 1; // lambda cannot be changed anymore

  diagD = new double[problem_->getNumberOfVariables()];
  for (i = 0; i < problem_->getNumberOfVariables(); i++) {
    diagD[i] = 1;
  }
  
  cout << "INIT1" << endl;
  
  /* expand Boundaries */
//  LBound = expandToDimension(LBound, problem_->getNumberOfVariables());
//  if (LBound == NULL) {
  LBound = new double[problem_->getNumberOfVariables()];
  for (i = 0; i < problem_->getNumberOfVariables(); i++) {
    LBound[i] = std::numeric_limits<double>::min();
  }
//  }
  
  cout << "INIT2" << endl;

//  UBound = expandToDimension(UBound, problem_->getNumberOfVariables());
//  if (UBound == NULL) {
  UBound = new double[problem_->getNumberOfVariables()];
  for (i = 0; i < problem_->getNumberOfVariables(); i++) {
    UBound[i] = std::numeric_limits<double>::max();
  }
//  }
  
  cout << "INIT3" << endl;

  /* Initialization of sigmas */
//  if (startsigma != NULL) { // 
//    if (startsigma.length == 1) {
//      sigma = startsigma[0];
//    } else if (startsigma.length == N) {
//      sigma = math.max(startsigma);
//      if (sigma <= 0)
//        error("initial standard deviation sigma must be positive");
//      for (i = 0; i < N; ++i) {
//        diagD[i] = startsigma[i]/sigma;
//      }
//    } else
//      assert false;
//  } else {
//    // we might use boundaries here to find startsigma, but I prefer to have stddevs mandatory 
//    cerr << "no initial standard deviation specified, using sigma = 0.5" << endl;
  sigma = 0.5;
//  }
//
//  if (sigma <= 0 || math.min(diagD) <= 0) {
//    error("initial standard deviations not specified or non-positive, " + 
//    "use setInitialStandarddeviations()"); 
//    sigma = 1;
//  }
  /* save initial standard deviation */
//  if (startsigma == null || startsigma.length == 1) { 
  startsigma = new double[problem_->getNumberOfVariables()];
  for (i = 0; i < problem_->getNumberOfVariables(); ++i) {
    startsigma[i] = sigma * diagD[i];
  }
//  }
  maxstartsigma = startsigma[0];
  minstartsigma = startsigma[0];
  for (i = 1; i < problem_->getNumberOfVariables(); ++i) {
    if (maxstartsigma < startsigma[i]) {
      maxstartsigma = startsigma[i];
    }
    if (minstartsigma > startsigma[i]) {
      minstartsigma = startsigma[i];
    }
  }
  axisratio = maxstartsigma / minstartsigma; // axis parallel distribution
  
  cout << "INIT4" << endl;

  /* expand typicalX, might still be null afterwards */
//  typicalX = expandToDimension(typicalX, problem_->getNumberOfVariables());

  /* Initialization of xmean */
//  xmean = expandToDimension(xmean, problem_->getNumberOfVariables());
//  if (xmean == NULL) { 
    /* set via typicalX */
//    if (typicalX != NULL) {
//      xmean = typicalX.clone(); //TODO: REVISAR
  xmean = new double[problem_->getNumberOfVariables()];
  for (i = 0; i < problem_->getNumberOfVariables(); i++) {
    
//    cout << "RANDOM=" << PseudoRandom::randDouble(-1, 1) << endl;
//    cout << "i=" << i << endl;
//    cout << "xmean[i]=" << xmean[i] << endl;
//    cout << "sigma=" << sigma << endl;
//    cout << "diagD[i]=" << diagD[i] << endl;
//    
//    xmean[i] += sigma*diagD[i] * PseudoRandom::randDouble(-1, 1);
    xmean[i] = PseudoRandom::randDouble();
  }
  
  cout << "INIT5" << endl;
  
//      /* set via boundaries, is depriciated */
//    } else if (math.max(UBound) < Double.MAX_VALUE
//        && math.min(LBound) > -Double.MAX_VALUE) {
//      error("no initial search point (solution) X or typical X specified");
//      xmean = new double[problem_->getNumberOfVariables()];
//      for (i = 0; i < problem_->getNumberOfVariables(); ++i) { /* TODO: reconsider this algorithm to set X0 */
//        double offset = sigma*diagD[i];
//        double range = (UBound[i] - LBound[i] - 2*sigma*diagD[i]); 
//        if (offset > 0.4 * (UBound[i] - LBound[i])) {
//          offset = 0.4 * (UBound[i] - LBound[i]);
//          range = 0.2 * (UBound[i] - LBound[i]);
//        }
//        xmean[i] = LBound[i] + offset + rand.nextDouble() * range;
//      }
//    } else {
//      error("no initial search point (solution) X or typical X specified");
//      xmean = new double[problem_->getNumberOfVariables()];
//      for (i = 0; i < problem_->getNumberOfVariables(); ++i)
//        xmean[i] = rand.nextDouble();
//    }
//  }

  
  
  if (xmean == NULL) {
    cout << "Error: xmean not initialized." << endl;
    exit(-1);
  };
  if (sigma <= 0) {
    cout << "Error: sigma not correctly initialized." << endl;
    exit(-1);
  };

  /* interpret missing option value */
  diagonalCovarianceMatrix = 0;
//  if (options.diagonalCovarianceMatrix < 0) // necessary for hello world message
//    options.diagonalCovarianceMatrix = 1 * 150 * N / sp.lambda; // cave: duplication below

  /* non-settable parameters */
  pc = new double[problem_->getNumberOfVariables()];
  ps = new double[problem_->getNumberOfVariables()];
  B = new double*[problem_->getNumberOfVariables()];
  for (i = 0; i < problem_->getNumberOfVariables(); i++) {
    B[i] = new double[problem_->getNumberOfVariables()];
  }
  C = new double*[problem_->getNumberOfVariables()];
  for (i = 0; i < problem_->getNumberOfVariables(); i++) {
    C[i] = new double[problem_->getNumberOfVariables()];
  }
  
  xold = new double[problem_->getNumberOfVariables()];
  BDz = new double[problem_->getNumberOfVariables()];
//  bestever_x = xmean.clone();
  // bestever = new CMASolution(xmean);
  artmp = new double[problem_->getNumberOfVariables()];


  fit = new FitnessCollector();
  //fit->deltaFitHist = new double[5];
  fit->idxDeltaFitHist = -1;
  for (i = 0; i < 5; i++) {
    fit->deltaFitHist[i] = 1.;
  }

  // code to be duplicated in reSizeLambda
  fit->fitness = new IntDouble*[populationSize_];   // including penalties, used yet
  for (i = 0; i < populationSize_; i++) {
    fit->fitness[i] = new IntDouble();
  }
  fit->raw = new IntDouble*[populationSize_];       // raw function values
  for (i = 0; i < populationSize_; i++) {
    fit->raw[i] = new IntDouble();
  }
  fit->historyLength = 10+30*problem_->getNumberOfVariables()/populationSize_;
  fit->history = new double[fit->historyLength];	

  arx = new double*[populationSize_];
  for (i = 0; i < populationSize_; i++) {
    arx[i] = new double[problem_->getNumberOfVariables()];
  }
//  arx = new SolutionSet(populationSize_);
//  for (i = 0; i < populationSize_; i++) {
//    arx[i] = new Solution(problem_->getNumberOfVariables()]);
//  }
//  population_ = new double*[populationSize_];
//  for (i = 0; i < populationSize_; i++) {
//    population_[i] = new double[problem_->getNumberOfVariables()];
//  }
  population_ = new SolutionSet(populationSize_);
  for (i = 0; i < populationSize_; i++) {
    Solution * newIndividual = new Solution(problem_);
    population_->add(newIndividual);
  }

//  for (i = 0; i < sp.getLambda(); ++i) {
//    fit.fitness[i] = new IntDouble();
//    fit.raw[i] = new IntDouble();
//  }

  // initialization
  for (i = 0; i < problem_->getNumberOfVariables(); i++) {
    pc[i] = 0;
    ps[i] = 0;
    for (int j = 0; j < problem_->getNumberOfVariables(); j++) {
      B[i][j] = 0;
    }
    for (int j = 0; j < i; j++) {
      C[i][j] = 0;
    }
    B[i][i] = 1;
    C[i][i] = diagD[i] * diagD[i];
  }
//  maxsqrtdiagC = Math.sqrt(math.max(math.diag(C)));
//  minsqrtdiagC = Math.sqrt(math.min(math.diag(C)));
  countCupdatesSinceEigenupdate = 0;
  iniphase = false; // obsolete

  /* Some consistency check */
  for (i = 0; i < problem_->getNumberOfVariables(); i++) {
    if (LBound[i] > UBound[i]) {
      cout << "Error: lower bound is greater than upper bound." << endl;
      exit(-1);
    }
//    if (typicalX != null) {
//      if (LBound[i] > typicalX[i])
//        error("lower bound '" + LBound[i] + "'is greater than typicalX" + typicalX[i]);
//      if (UBound[i] < typicalX[i])
//        error("upper bound '" + UBound[i] + "' is smaller than typicalX " + typicalX[i]);
//    }
  }
//  String[] s = stopConditions.getMessages();
//  if(!s[0].equals(""))
//    warning("termination condition satisfied at initialization: \n  " + s[0]);

//  initialX = xmean.clone(); // keep finally chosen initialX

//  timings.start = System.currentTimeMillis();
//  timings.starteigen = System.currentTimeMillis();
//
//  state = 0;
//  if(options.verbosity > -1)
//    printlnHelloWorld();

  return new double[populationSize_];

} // CMAES::init()


SolutionSet * CMAES::samplePopulation() {
//double ** CMAES::samplePopulation() {
  int i, j, iNk;
  double sum;

//  if (state < 0) {
//    init();
//  } else if (state < 3 && state > 2) {
//    error("mixing of calls to updateSingle() and samplePopulation() is not possible");
//  } else {
//    eigendecomposition(0); // latest possibility to generate B and diagD
//  }

  // NECESARIO CONTAR LAS ITERACIONES?? Y EL JODIDO STATE?
//  if (state != 1) {
    countiter++;
//  }
//  state = 1; // can be repeatedly called without problem
  idxRecentOffspring = populationSize_ - 1; // not really necessary at the moment

  // ensure maximal and minimal standard deviations
  // OPCIONES NO UTILES ¿?
//  if (options.lowerStandardDeviations != null && options.lowerStandardDeviations.length > 0)
//      for (i = 0; i < N; ++i) {
//          double d = options.lowerStandardDeviations[Math.min(i,options.lowerStandardDeviations.length-1)]; 
//          if(d > sigma * minsqrtdiagC) 
//              sigma = d / minsqrtdiagC;
//      }
//  if (options.upperStandardDeviations != null && options.upperStandardDeviations.length > 0)
//      for (i = 0; i < N; ++i) {
//          double d = options.upperStandardDeviations[Math.min(i,options.upperStandardDeviations.length-1)]; 
//          if (d < sigma * maxsqrtdiagC) 
//              sigma = d / maxsqrtdiagC;
//      }

//  testAndCorrectNumerics();

  /* sample the distribution */
  for (iNk = 0; iNk < populationSize_; iNk++) {
     /*
      * generate scaled
      * random vector (D * z)
      */

      // code duplication from resampleSingle because of possible future resampling before GenoPheno
      /* generate scaled random vector (D * z) */
    if (flgdiag) {
      for (i = 0; i < problem_->getNumberOfVariables(); i++) {
        arx[iNk][i] = xmean[i] + sigma * diagD[i] * PseudoRandom::randDouble(-1, 1);
      }
    } else {
      for (i = 0; i < problem_->getNumberOfVariables(); i++) { 
        artmp[i] = diagD[i] * PseudoRandom::randDouble(-1, 1);
      }

      /* add mutation (sigma * B * (D*z)) */
      for (i = 0; i < problem_->getNumberOfVariables(); i++) {
        for (j = 0, sum = 0; j < problem_->getNumberOfVariables(); j++) {
          sum += B[i][j] * artmp[j];
        }
        arx[iNk][i] = xmean[i] + sigma * sum;
      }
    }
    // redo this while isOutOfBounds(arx[iNk])
  }

  // I am desperately missing a const/readonly/visible qualifier. 
//  return population_ = genoPhenoTransformation(arx, population_);
  return genoPhenoTransformation(arx);

} // CMAES::samplePopulation()

Solution * CMAES::resampleSingle(int index) {
  int i,j;
  double sum;
//  if (state != 1)
//    error("call samplePopulation before calling resampleSingle(int index)");

  /* sample the distribution */
  /* generate scaled random vector (D * z) */
  if (flgdiag) {
    for (i = 0; i < problem_->getNumberOfVariables(); i++) {
      arx[index][i] = xmean[i] + sigma * diagD[i] * PseudoRandom::randDouble(-1, 1);
    }
  } else {
    for (i = 0; i < problem_->getNumberOfVariables(); i++) {
      artmp[i] = diagD[i] * PseudoRandom::randDouble(-1, 1);
    }
    
    /* add mutation (sigma * B * (D*z)) */
    for (i = 0; i < problem_->getNumberOfVariables(); i++) {
      for (j = 0, sum = 0; j < problem_->getNumberOfVariables(); j++) {
        sum += B[i][j] * artmp[j];
      }
      arx[index][i] = xmean[i] + sigma * sum;
    }
  }
//  return population_[index] = genoPhenoTransformation(arx[index], population_[index]);
  return genoPhenoTransformation(arx[index], index);
} // resampleSingle

SolutionSet * CMAES::genoPhenoTransformation(double ** popx) {

//  for (int i = 0; i < populationSize_; i++) {
//    delete [] popy;
//  }
//  delete popy;
  
  delete population_;
  
//  double ** res = new double*[populationSize_];
  population_ = new SolutionSet(populationSize_);
  for (int i = 0; i < populationSize_; i++) {
    Solution * solution = new Solution(problem_);
//    res[i] = new double[problem_->getNumberOfVariables()];
    for (int j = 0; j < problem_->getNumberOfVariables(); j++) {
      solution->getDecisionVariables()[j]->setValue(popx[i][j]);
//      res[i][j] = popx[i][j];
    }
    population_->add(solution);
  }
  return population_;
  
}

Solution * CMAES::genoPhenoTransformation(double * popx, int index) {
//  if (popy == NULL || popy == popx) || popy.length != popx.length) 
//    popy = new double[popx.length][];
//  
//  for (int i = 0; i < popy.length; ++i)
//    popy[i] = genoPhenoTransformation(popx[i], popy[i]);
//    
//  return popy;
  
  delete population_->get(index);
  
//  double * res = new double[problem_->getNumberOfVariables()];
  Solution * solution = new Solution(problem_);
  for (int i = 0; i < problem_->getNumberOfVariables(); i++) {
//    res[i] = popx[i];
    solution->getDecisionVariables()[i]->setValue(popx[i]);
  }
  population_->replace(index,solution);
  return solution;
  
}

void CMAES::updateDistribution() {
//  if (state == 3) {
//    error("updateDistribution() was already called");
//  }
//  if (functionValues.length != sp.getLambda())
//    error("argument double[] funcionValues.length=" + functionValues.length 
//        + "!=" + "lambda=" + sp.getLambda());

  cout << "UPDATE1" << endl;
  
  /* pass input argument */
  for (int i = 0; i < populationSize_; i++) {
    fit->raw[i]->setVal(population_->get(i)->getObjective(0));
    //fit->raw[i]->setVal(functionValues[i]);
    fit->raw[i]->setI(i);
  }
  
  cout << "UPDATE2" << endl;

  counteval += populationSize_;
  //recentFunctionValue = math.min(fit.raw).val;
  recentFunctionValue = fit->minRaw(populationSize_)->getVal();
  //recentMaxFunctionValue = math.max(fit.raw).val;
  recentMaxFunctionValue = fit->maxRaw(populationSize_)->getVal();
  //recentMinFunctionValue = math.min(fit.raw).val;
  recentMinFunctionValue = recentFunctionValue;
//  updateDistribution2();
//}
//
//void CMAES::updateDistribution2() {
  
  cout << "UPDATE3" << endl;
        
  int i, j, k, iNk, hsig;
  double sum;
  double psxps;

//  if (state == 3) {
//      error("updateDistribution() was already called");
//  }
  
//  cout << "BEFORE:" << endl;
//  for (int idx = 0; idx < populationSize_; idx++) {
//    cout << (idx+1) << "[" << fit->raw[idx]->getVal() << "," << fit->raw[idx]->getI() << "]" << endl;
//  }

  /* sort function values */
  UtilsCMAES::minFastSort(fit->raw, populationSize_);
  //Arrays.sort(fit.raw, fit.raw[0]);
  
//  cout << "AFTER:" << endl;
//  for (int idx = 0; idx < populationSize_; idx++) {
//    cout << (idx+1) << "[" << fit->raw[idx]->getVal() << "," << fit->raw[idx]->getI() << "]" << endl;
//  }
  
  cout << "UPDATE3b" << endl;

  for (iNk = 0; iNk < populationSize_; iNk++) {
    fit->fitness[iNk]->setVal(fit->raw[iNk]->getVal()); // superfluous at time
    fit->fitness[iNk]->setI(fit->raw[iNk]->getI());
  }
  
  cout << "UPDATE4" << endl;

  /* update fitness history */ 
  for (i = fit->historyLength - 1; i > 0; i--) {
    fit->history[i] = fit->history[i - 1];
  }
  fit->history[0] = fit->raw[0]->getVal();
  
  cout << "UPDATE5" << endl;

  /* save/update bestever-value */
//  updateBestEver(arx[fit->raw[0]->getI()], fit->raw[0]->getVal(), 
//      counteval - populationSize_ + fit->raw[0]->getI() + 1);

  /* re-calculate diagonal flag */
  flgdiag = (diagonalCovarianceMatrix == 1);// || diagonalCovarianceMatrix >= countiter); 
//  if (options.diagonalCovarianceMatrix == -1) // options might have been re-read
//    flgdiag = (countiter <= 1 * 150 * N / sp.lambda);  // CAVE: duplication of "default"
  
  cout << "UPDATE6" << endl;

  /* calculate xmean and BDz~N(0,C) */
  for (i = 0; i < problem_->getNumberOfVariables(); i++) {
    xold[i] = xmean[i];
    xmean[i] = 0.;
    for (iNk = 0; iNk < sp->getMu(); iNk++) {
      xmean[i] += sp->getWeights()[iNk] * arx[fit->fitness[iNk]->getI()][i];
    }
    BDz[i] = sqrt(sp->getMueff()) * (xmean[i] - xold[i]) / sigma;
  }
  
  cout << "UPDATE7" << endl;
  
  /* cumulation for sigma (ps) using B*z */
  if (flgdiag) {
    /* given B=I we have B*z = z = D^-1 BDz  */
    for (i = 0; i < problem_->getNumberOfVariables(); i++) {
      ps[i] = (1. - sp->getCs()) * ps[i]
          + sqrt(sp->getCs() * (2. - sp->getCs())) 
          * BDz[i] / diagD[i];
    }
  } else {
    /* calculate z := D^(-1) * B^(-1) * BDz into artmp, we could have stored z instead */
    for (i = 0; i < problem_->getNumberOfVariables(); i++) {
      for (j = 0, sum = 0.; j < problem_->getNumberOfVariables(); j++) {
        sum += B[j][i] * BDz[j];
      }
      artmp[i] = sum / diagD[i];
    }
    /* cumulation for sigma (ps) using B*z */
    for (i = 0; i < problem_->getNumberOfVariables(); i++) {
      for (j = 0, sum = 0.; j < problem_->getNumberOfVariables(); j++) {
        sum += B[i][j] * artmp[j];
      }
      ps[i] = (1. - sp->getCs()) * ps[i]
          + sqrt(sp->getCs() * (2. - sp->getCs())) * sum;
    }
  }
  
  cout << "UPDATE8" << endl;

  /* calculate norm(ps)^2 */
  psxps = 0;
  for (i = 0; i < problem_->getNumberOfVariables(); i++) {
    psxps += ps[i] * ps[i];
  }
  
  cout << "UPDATE9" << endl;

  /* cumulation for covariance matrix (pc) using B*D*z~N(0,C) */
  hsig = 0;
  if (sqrt(psxps)
        / sqrt(1. - pow(1. - sp->getCs(), 2. * countiter))
        / sp->getChiN() < 1.4 + 2. / (problem_->getNumberOfVariables() + 1.)) {
    hsig = 1;
  }
  for (i = 0; i < problem_->getNumberOfVariables(); i++) {
    pc[i] = (1. - sp->getCc()) * pc[i] + hsig
      * sqrt(sp->getCc() * (2. - sp->getCc())) * BDz[i];
  }
  
  cout << "UPDATE10" << endl;

  /* stop initial phase, not in use anymore as hsig does the job */
  if (iniphase && countiter > min(1 / sp->getCs(), 1 + problem_->getNumberOfVariables() / sp->getMucov())) {
    if (psxps / sp->getDamps()
            / (1. - pow((1. - sp->getCs()), countiter))
            < problem_->getNumberOfVariables() * 1.05) {
      iniphase = false;
    }
  }
  
  cout << "UPDATE11" << endl;

  /* this, it is harmful in a dynamic environment
   * remove momentum in ps, if ps is large and fitness is getting worse */
//        if (1 < 3 && psxps / N > 1.5 + 10 * Math.sqrt(2. / N)
//        		&& fit.history[0] > fit.history[1] && fit.history[0] > fit.history[2]) {
//          double tfac;
// 
//        	infoVerbose(countiter + ": remove momentum " + psxps / N + " "
//        			+ ps[0] + " " + sigma);
//
//        	tfac = Math.sqrt((1 + Math.max(0, Math.log(psxps / N))) * N / psxps);
//        	for (i = 0; i < N; ++i)
//        		ps[i] *= tfac;
//        	psxps *= tfac * tfac;
//        }
  
  cout << "UPDATE12" << endl;

  /* update of C */
  if (sp->getCcov() > 0 && iniphase == false) {
  
    countCupdatesSinceEigenupdate++;
    
    /* update covariance matrix */
    for (i = 0; i < problem_->getNumberOfVariables(); i++) {
      for (j = (flgdiag ? i : 0); j <= i; j++) {
        C[i][j] = (1 - sp->getCcov(flgdiag))
                * C[i][j]
                + sp->getCcov()
                * (1. / sp->getMucov())
                * (pc[i] * pc[j] + (1 - hsig) * sp->getCc()
                * (2. - sp->getCc()) * C[i][j]);
        for (k = 0; k < sp->getMu(); k++) {
          /*
           * additional rank mu
           * update
           */
          C[i][j] += sp->getCcov() * (1 - 1. / sp->getMucov())
                * sp->getWeights()[k]
                * (arx[fit->fitness[k]->getI()][i] - xold[i])
                * (arx[fit->fitness[k]->getI()][j] - xold[j]) / sigma
                / sigma;
        }
      }
    }
//    maxsqrtdiagC = sqrt(max(math.diag(C)));
//    minsqrtdiagC = sqrt(min(math.diag(C)));
  } // update of C
  
  cout << "UPDATE13" << endl;

  /* update of sigma */
  sigma *= exp(((sqrt(psxps) / sp->getChiN()) - 1) * sp->getCs()
          / sp->getDamps());
  
  cout << "UPDATE14" << endl;

//  state = 3;

} // updateDistribution()

//void CMAES::updateBestEver(double * x, double fitness, int eval) {
//    if (fitness < bestever_fit || Double.isNaN(bestever_fit)) {  // countiter == 1 not needed anymore
//        bestever_fit = fitness;
//        bestever_eval = eval;
//        bestever_x = assignNew(x, bestever_x); // save (hopefully) efficient assignment
//    }
//}
