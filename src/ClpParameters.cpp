/*
  Copyright (C) 2007, Lou Hafer, International Business Machines Corporation
  and others.  All Rights Reserved.

  This code is licensed under the terms of the Eclipse Public License (EPL).

*/

#include <cassert>

#include "ClpParamUtils.hpp"
#include "ClpParameters.hpp"

/*
  Constructor for settings class.
*/

ClpParameters::ClpParameters() : parameters_(ClpParam::LASTPARAM), model_(0) {

  for (int i = 0; i < parameters_.size(); i++){
     parameters_[i] = new ClpParam();
  }
  char dirsep = CoinFindDirSeparator();
  dfltDirectory_ = (dirsep == '/' ? "./" : ".\\");

  addClpParams();
}

ClpParameters::~ClpParameters() {
  for (int i = 0; i < parameters_.size(); i++){
     delete parameters_[i];
  }
  if (model_) {
    delete model_;
  }
}

int ClpParameters::matches(std::string field, int &numberMatches) {
  int firstMatch = -1;
  for (int iParam = 0; iParam < (int)parameters_.size(); iParam++) {
    int match = parameters_[iParam]->matches(field);
    if (match == 1) {
      numberMatches = 1;
      return iParam;
    } else {
      if (match) {
        if (firstMatch < 0) {
          firstMatch = iParam;
        }
        numberMatches++;
      }
    }
  }
  return firstMatch < 0 ? ClpParam::INVALID : firstMatch;
}

void ClpParameters::addClpParams() {

  addClpStrParams();
  addClpHelpParams();
  addClpActionParams();
  addClpKwdParams();
  addClpDblParams();
  addClpIntParams();
  addClpBoolParams();

  for (int code = ClpParam::FIRSTPARAM + 1; code < ClpParam::LASTPARAM;
       code++) {
     getParam(code)->setParameters(this);
     getParam(code)->setParamCode(code);
  }

  return;
}

//###########################################################################
//###########################################################################

void ClpParameters::addClpHelpParams() {
  for (int code = ClpParam::FIRSTHELPPARAM + 1; code < ClpParam::LASTHELPPARAM;
       code++) {
    getParam(code)->setPushFunc(ClpParamUtils::doHelpParam);
  }
}

//###########################################################################
//###########################################################################

void ClpParameters::addClpActionParams() {

  parameters_[ClpParam::ALLSLACK]->setup(
      "allS!lack", "Set basis back to all slack and reset solution",
      "Mainly useful for tuning purposes.  Normally the first dual or primal "
      "will be using an all slack basis anyway.",
      CoinParam::displayPriorityNone);

  parameters_[ClpParam::BARRIER]->setup(
      "barr!ier", "Solve using primal dual predictor corrector algorithm",
      "This command solves the current model using the  primal dual predictor "
      "corrector algorithm. You may want to link in an alternative ordering "
      "and factorization. It will also solve models with quadratic objectives.",
      CoinParam::displayPriorityHigh);

  // TODO What is this actually doing?
  parameters_[ClpParam::BAB]->setup("branch!AndCut", "Do Branch and Cut", "",
#if 0
     "This does branch and cut.  There are many parameters which can affect the performance. First just try with default settings and look carefully at the log file.  Did cuts help?  Did they take too long? Look at output to see which cuts were effective and then do some tuning.  You will see that the options for cuts are off, on, root and ifmove, forceon.  Off is obvious. " CUTS_LONGHELP " For probing, forceonbut just does fixing probing in tree - not strengthening etc. If pre-processing reduced the size of the problem or strengthened many coefficients then it is probably wise to leave it on.  Switch off heuristics which did not provide solutions.  The other major area to look at is the search.  Hopefully good solutions were obtained fairly early in the search so the important point is to select the best variable to branch on. See whether strong branching did a good job - or did it just take a lot of iterations.  Adjust the strongBranching and trustPseudoCosts parameters.  If cuts did a good job, then you may wish to have more rounds of cuts - see passC!uts and passT!ree.",
#endif
                                   CoinParam::displayPriorityHigh);

  parameters_[ClpParam::DUALSIMPLEX]->setup(
      "dualS!implex", "Do dual simplex algorithm",
      "This command solves the continuous relaxation of the current model "
      "using the dual steepest edge algorithm. The time and iterations may be "
      "affected by settings such as presolve, scaling, crash and also by dual "
      "pivot method, fake bound on variables and dual and primal tolerances.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::SOLVECONTINUOUS]->setup(
      "initialS!olve", "Solve to continuous",
      "This just solves the problem to continuous - without adding any cuts",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::EITHERSIMPLEX]->setup(
      "either!Simplex", "Do dual or primal simplex algorithm",
      "This command solves the continuous relaxation of the current model "
      "using the dual or primal algorithm, based on a dubious analysis of "
      "model.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::SOLVE]->setup(
      "solv!e", "Do dual or primal simplex algorithm", 
      "This command solves the continuous relaxation of the current model "
      "using the dual or primal algorithm, based on a dubious analysis of "
      "model.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::ENVIRONMENT]->setup(
      "environ!ment", "Read commands from environment",
      "This starts reading from environment variable CLP_ENVIRONMENT.",
      CoinParam::displayPriorityNone);

  parameters_[ClpParam::EXIT]->setup(
      "end", "Stops clp execution",
      "This stops execution ; end, exit, quit and stop are synonyms",
      CoinParam::displayPriorityHigh);

#if 0
    parameters_[ClpParam::EXIT]->setup("quit", "Stops clp execution",
      "This stops the execution of Clp, end, exit, quit and stop are synonyms", CoinParam::displayPriorityHigh);

    parameters_[ClpParam::EXIT]->setup("stop", "Stops clp execution",
      "This stops the execution of Clp, end, exit, quit and stop are synonyms", CoinParam::displayPriorityHigh);
#endif

  parameters_[ClpParam::GMPL_SOLUTION]->setup(
      "gsolu!tion", "Puts glpk solution to file",
      "Will write a glpk solution file to the given file name.  It will use "
      "the default directory given by 'directory'.  A name of '$' will use the "
      "previous value for the name.  This is initialized to 'stdout' (this "
      "defaults to ordinary solution if stdout). If problem created from gmpl "
      "model - will do any reports.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::GUESS]->setup(
      "guess", "Guesses at good parameters",
      "This looks at model statistics and does an initial solve setting some "
      "parameters which may help you to think of possibilities.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::MINIMIZE]->setup(
      "min!imize", "Set optimization direction to minimize",
      "The default is minimize - use 'maximize' for maximization.\n This "
      "should only be necessary if you have previously set maximization You "
      "can also use the parameters 'direction minimize'.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::NETLIB_EITHER]->setup(
      "netlib", "Solve entire netlib test set",
      "This exercises the unit test for clp and then solves the netlib test "
      "set using dual or primal. The user can set options before e.g. clp "
      "-presolve off -netlib",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::NETLIB_BARRIER]->setup(
      "netlibB!arrier", "Solve entire netlib test set with barrier",
      "This exercises the unit test for clp and then solves the netlib test "
      "set using barrier. The user can set options before e.g. clp -kkt on "
      "-netlib",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::NETLIB_DUAL]->setup(
      "netlibD!ual", "Solve entire netlib test set (dual)",
      "This exercises the unit test for clp and then solves the netlib test "
      "set using dual. The user can set options before e.g. clp -presolve off "
      "-netlib",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::NETLIB_PRIMAL]->setup(
      "netlibP!rimal", "Solve entire netlib test set (primal)",
      "This exercises the unit test for clp and then solves the netlib test "
      "set using primal. The user can set options before e.g. clp -presolve "
      "off -netlibp",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::NETLIB_TUNE]->setup(
      "netlibT!une", "Solve entire netlib test set with 'best' algorithm",
      "This exercises the unit test for clp and then solves the netlib test "
      "set using whatever works best. I know this is cheating but it also "
      "stresses the code better by doing a mixture of stuff. The best "
      "algorithm was chosen on a Linux ThinkPad using native cholesky with "
      "University of Florida ordering.",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::NETWORK]->setup(
      "network", "Tries to make network matrix",
      "Clp will go faster if the matrix can be converted to a network.  The "
      "matrix operations may be a bit faster with more efficient storage, but "
      "the main advantage comes from using a network factorization. It will "
      "probably not be as fast as a specialized network code.",
      CoinParam::displayPriorityNone);

  parameters_[ClpParam::OUTDUPROWS]->setup(
      "outDup!licates", "takes duplicate rows etc out of integer model", "",
      CoinParam::displayPriorityNone);

  parameters_[ClpParam::PARAMETRICS]->setup(
      "para!metrics", "Import data from file and do parametrics",
      "This will read a file with parametric data from the given file name and "
      "then do parametrics. It will use the default directory given by "
      "'directory'. A name of '$' will use the previous value for the name. "
      "This is initialized to '', i.e. it must be set.  This can not read from "
      "compressed files. File is in modified csv format - a line ROWS will be "
      "followed by rows data while a line COLUMNS will be followed by column "
      "data.  The last line should be ENDATA. The ROWS line must exist and is "
      "in the format ROWS, inital theta, final theta, interval theta, n where "
      "n is 0 to get CLPI0062 message at interval or at each change of theta "
      "and 1 to get CLPI0063 message at each iteration.  If interval theta is "
      "0.0 or >= final theta then no interval reporting.  n may be missed out "
      "when it is taken as 0.  If there is Row data then there is a headings "
      "line with allowed headings - name, number, lower(rhs change), upper(rhs "
      "change), rhs(change).  Either the lower and upper fields should be "
      "given or the rhs field. The optional COLUMNS line is followed by a "
      "headings line with allowed headings - name, number, objective(change), "
      "lower(change), upper(change). Exactly one of name and number must be "
      "given for either section and missing ones have value 0.0.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::PLUSMINUS]->setup(
      "plus!Minus", "Tries to make +- 1 matrix",
      "Clp will go slightly faster if the matrix can be converted so that the "
      "elements are not stored and are known to be unit.  The main advantage "
      "is memory use.  Clp may automatically see if it can convert the problem "
      "so you should not need to use this.",
      CoinParam::displayPriorityNone);

  parameters_[ClpParam::PRIMALSIMPLEX]->setup(
      "primalS!implex", "Do primal simplex algorithm",
      "This command solves the continuous relaxation of the current model "
      "using the primal algorithm. The default is to use exact devex. The time "
      "and iterations may be affected by settings such as presolve, scaling, "
      "crash and also by column selection  method, infeasibility weight and "
      "dual and primal tolerances.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::RESTORESOL]->setup(
      "restoreS!olution", "reads solution from file",
      "This will read a binary solution file from the given file name.  It "
      "will use the default directory given by 'directory'.  A name of '$' "
      "will use the previous value for the name. This is initialized to "
      "'solution.file'. This reads in a file from saveSolution",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::REALLY_SCALE]->setup("reallyS!cale",
                                            "Scales model in place", "",
                                            CoinParam::displayPriorityNone);

  parameters_[ClpParam::REVERSE]->setup(
      "reverse", "Reverses sign of objective",
      "Useful for testing if maximization works correctly",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::DUMMY]->setup(
      "sleep", "for debug",
      "If passed to solver fom ampl, then ampl will wait so that you can copy "
      ".nl file for debug.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::STATISTICS]->setup(
      "stat!istics", "Print some statistics",
      "This command prints some statistics for the current model. If log level "
      ">1 then more is printed. These are for presolved model if presolve on "
      "(and unscaled).",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::TIGHTEN]->setup("tightLP",
                                       "Poor person's preSolve for now", "",
                                       CoinParam::displayPriorityNone);

  parameters_[ClpParam::UNITTEST]->setup("unitTest", "Do unit test",
                                        "This exercises the unit test for clp",
                                        CoinParam::displayPriorityLow);

  parameters_[ClpParam::USERCLP]->setup(
      "userClp", "Hand coded Clp stuff",
      "There are times e.g. when using AMPL interface when you may wish to do "
      "something unusual. Look for USERCLP in main driver and modify sample "
      "code.",
      CoinParam::displayPriorityNone);
}

//###########################################################################
//###########################################################################

void ClpParameters::addClpStrParams() {

  parameters_[ClpParam::BASISIN]->setup(
      "basisI!n", "Import basis from bas file", std::string("default.bas"),
      "This will read an MPS format basis file from the given file name.  It "
      "will use the default directory given by 'directory'.  A name of '$' "
      "will use the previous value for the name. This is initialized to '', "
      "i.e. it must be set.  If you have libz then it can read compressed "
      "files 'xxxxxxxx.gz' or xxxxxxxx.bz2.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::BASISOUT]->setup(
      "basisO!ut", "Export basis as bas file", std::string("default.bas"),
      "This will write an MPS format basis file to the given file name.  It "
      "will use the default directory given by 'directory'.  A name of '$' "
      "will use the previous value for the name.  This is initialized to "
      "'default.bas'.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::DIRECTORY]->setup(
      "directory", "Set Default directory for import etc.", dfltDirectory_,
      "This sets the directory which import, export, saveModel, restoreModel "
      "etc. will use. It is initialized to the current directory.");

  parameters_[ClpParam::DIRSAMPLE]->setup(
      "dirSample", "Set directory where the COIN-OR sample problems are.",
      dfltDirectory_,
      "This sets the directory where the COIN-OR sample problems reside. It is "
      "used only when -unitTest is passed to clp. clp will pick up the test "
      "problems from this directory. It is initialized to '../../Data/Sample'",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::DIRNETLIB]->setup(
      "dirNetlib", "Set directory where the netlib problems are.",
      dfltDirectory_,
      "This sets the directory where the netlib problems reside. One can get "
      "the netlib problems from COIN-OR or from the main netlib site. This "
      "parameter is used only when -netlib is passed to cbc. cbc will pick up "
      "the netlib problems from this directory. If clp is built without zlib "
      "support then the problems must be uncompressed. It is initialized to "
      "'../../Data/Netlib'",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::DIRMIPLIB]->setup(
      "dirMiplib", "Set directory where the miplib 2003 problems are.",
      dfltDirectory_,
      "This sets the directory where the miplib 2003 problems reside. One can "
      "get the miplib problems from COIN-OR or from the main miplib site. This "
      "parameter is used only when -miplib is passed to cbc. cbc will pick up "
      "the miplib problems from this directory. If cbc is built without zlib "
      "support then the problems must be uncompressed. It is initialized to "
      "'../../Data/miplib3'",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::EXPORT]->setup(
      "export", "Export model as mps file", std::string("default.mps"),
      "This will write an MPS format file to the given file name.  It will use "
      "the default directory given by 'directory'.  A name of '$' will use the "
      "previous value for the name.  This is initialized to 'default.mps'. It "
      "can be useful to get rid of the original names and go over to using "
      "Rnnnnnnn and Cnnnnnnn.  This can be done by setting 'keepnames' off "
      "before importing mps file.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::IMPORT]->setup(
      "import", "Import model from mps file", std::string("default.mps"),
      "This will read an MPS format file from the given file name.  It will "
      "use the default directory given by 'directory'.  A name of '$' will use "
      "the previous value for the name. This is initialized to '', i.e. it "
      "must be set.  If you have libgz then it can read compressed files "
      "'xxxxxxxx.gz' or 'xxxxxxxx.bz2'. If 'keepnames' is off, then names are "
      "dropped -> Rnnnnnnn and Cnnnnnnn.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::PRINTMASK]->setup(
      "printM!ask", "Control printing of solution with a regular expression",
      "",
      "If set then only those names which match mask are printed in a "
      "solution. '?' matches any character and '*' matches any set of "
      "characters.  The default is '' (unset) so all variables are printed. "
      "This is only active if model has names.");

  parameters_[ClpParam::RESTORE]->setup(
      "restore!Model", "Restore model from binary file",
      std::string("default.prob"),
      "This reads data save by saveModel from the given file.  It will use the "
      "default directory given by 'directory'.  A name of '$' will use the "
      "previous value for the name.  This is initialized to 'default.prob'.",
      CoinParam::displayPriorityHigh);

  parameters_[ClpParam::SAVE]->setup(
      "saveM!odel", "Save model to binary file", std::string("default.prob"),
      "This will save the problem to the given file name for future use by restoreModel. It will use the default directory given by 'directory'.  A name of '$' will use the previous value for the name.  This is initialized to 'default.prob'.");
      
      parameters_[ClpParam::SAVESOL]->setup(
      "saveS!olution", "saves solution to file", std::string("solution.sln"),
      "This will write a binary solution file to the given file name.  It will "
      "use the default directory given by 'directory'.  A name of '$' will use "
      "the previous value for the name.  This is initialized to "
      "'solution.file'.  To read the file use fread(int) twice to pick up "
      "number of rows and columns, then fread(double) to pick up objective "
      "value, then pick up row activities, row duals, column activities and "
      "reduced costs - see bottom of parameters_[ClpParam::]->setup(.cpp for "
      "code that reads or writes file. If name contains '_fix_read_' then does "
      "not write but reads and will fix all variables");

  parameters_[ClpParam::SOLUTION]->setup(
      "solu!tion", "Prints solution to file", std::string("stdout"),
      "This will write a primitive solution file to the given file name.  It "
      "will use the default directory given by 'directory'.  A name of '$' "
      "will use the previous value for the name.  This is initialized to "
      "'stdout'.  The amount of output can be varied using printi!ngOptions or "
      "printMask.");
}

//###########################################################################
//###########################################################################

void ClpParameters::addClpKwdParams() {
  for (int code = ClpParam::FIRSTKWDPARAM + 1; code < ClpParam::LASTKWDPARAM;
       code++) {
    parameters_[code]->setPushFunc(ClpParamUtils::pushClpKwdParam);
  }

#if CLP_HAS_ABC
  parameters_[ClpParam::ABCWANTED]->setup(
      "abc", "Whether to visit Aboca", "off", 0,
      "Decide whether to use A Basic Optimization Code (Accelerated?) and "
      "whether to try going parallel!");
  parameters_[ClpParam::ABCWANTED]->appendKwd("one");
  parameters_[ClpParam::ABCWANTED]->appendKwd("two");
  parameters_[ClpParam::ABCWANTED]->appendKwd("three");
  parameters_[ClpParam::ABCWANTED]->appendKwd("four");
  parameters_[ClpParam::ABCWANTED]->appendKwd("five");
  parameters_[ClpParam::ABCWANTED]->appendKwd("six");
  parameters_[ClpParam::ABCWANTED]->appendKwd("seven");
  parameters_[ClpParam::ABCWANTED]->appendKwd("eight");
  parameters_[ClpParam::ABCWANTED]->appendKwd("on");
  parameters_[ClpParam::ABCWANTED]->appendKwd("decide");
  paramAboca.setFakeKeyWord(10);
#endif

  parameters_[ClpParam::COMMANDPRINTLEVEL]->setup(
      "allC!ommands", "Whether to print less used commands", "no", 0,
      "For the sake of your sanity, only the more useful and simple commands "
      "are printed out on ?.");
  parameters_[ClpParam::COMMANDPRINTLEVEL]->appendKwd("more");
  parameters_[ClpParam::COMMANDPRINTLEVEL]->appendKwd("all");

  parameters_[ClpParam::BIASLU]->setup(
      "biasLU", "Whether factorization biased towards U", "LX", 0, "",
      CoinParam::displayPriorityNone);
  parameters_[ClpParam::BIASLU]->appendKwd("UX");
  parameters_[ClpParam::BIASLU]->appendKwd("UU");
  parameters_[ClpParam::BIASLU]->appendKwd("LL");

  parameters_[ClpParam::BARRIERSCALE]->setup(
      "bscale", "Whether to scale in barrier (and ordering speed)", "off", 0,
      "", CoinParam::displayPriorityNone);
  parameters_[ClpParam::BARRIERSCALE]->appendKwd("on");
  parameters_[ClpParam::BARRIERSCALE]->appendKwd("off1");
  parameters_[ClpParam::BARRIERSCALE]->appendKwd("on1");
  parameters_[ClpParam::BARRIERSCALE]->appendKwd("off2");
  parameters_[ClpParam::BARRIERSCALE]->appendKwd("on2");

  parameters_[ClpParam::CHOLESKY]->setup(
      "chol!esky", "Which cholesky algorithm", "native", 0,
      "For a barrier code to be effective it needs a good Cholesky ordering "
      "and factorization. The native ordering and factorization is not state "
      "of the art, although acceptable. You may want to link in one from "
      "another source.  See Makefile.locations for some possibilities.");
  parameters_[ClpParam::CHOLESKY]->appendKwd("dense");
#ifdef CLP_HAS_WSMP
  parameters_[ClpParam::CHOLESKY]->appendKwd("fudge!Long");
  parameters_[ClpParam::CHOLESKY]->appendKwd("wssmp");
#else
  parameters_[ClpParam::CHOLESKY]->appendKwd("fudge!Long_dummy");
  parameters_[ClpParam::CHOLESKY]->appendKwd("wssmp_dummy");
#endif
#if defined(CLP_HAS_AMD) || defined(CLP_HAS_CHOLMOD) || defined(CLP_HAS_GLPK)
  parameters_[ClpParam::CHOLESKY]->appendKwd("Uni!versityOfFlorida");
#else
  parameters_[ClpParam::CHOLESKY]->appendKwd("Uni!versityOfFlorida_dummy");
#endif
#ifdef TAUCS_BARRIER
  parameters_[ClpParam::CHOLESKY]->appendKwd("Taucs");
#else
  parameters_[ClpParam::CHOLESKY]->appendKwd("Taucs_dummy");
#endif
#ifdef CLP_HAS_MUMPS
  parameters_[ClpParam::CHOLESKY]->appendKwd("Mumps");
#else
  parameters_[ClpParam::CHOLESKY]->appendKwd("Mumps_dummy");
#endif
#ifdef PARDISO_BARRIER
  parameters_[ClpParam::CHOLESKY]->appendKwd("Pardiso");
#else
  parameters_[ClpParam::CHOLESKY]->appendKwd("Pardiso_dummy");
#endif

  parameters_[ClpParam::CRASH]->setup(
      "crash", "Whether to create basis for problem", "off", 0,
      "If crash is set to 'on' and there is an all slack basis then Clp will "
      "flip or put structural variables into the basis with the aim of getting "
      "dual feasible.  On average, dual simplex seems to perform better "
      "without it and there are alternative types of 'crash' for primal "
      "simplex, e.g. 'idiot' or 'sprint'. A variant due to Solow and Halim "
      "which is as 'on' but just flips is also available.");
  parameters_[ClpParam::CRASH]->appendKwd("on");
  parameters_[ClpParam::CRASH]->appendKwd("so!low_halim");
  parameters_[ClpParam::CRASH]->appendKwd("lots");
  parameters_[ClpParam::CRASH]->appendKwd("free");
  parameters_[ClpParam::CRASH]->appendKwd("zero");
  parameters_[ClpParam::CRASH]->appendKwd("single!ton");
#ifdef CLP_INHERIT_MODE
  parameters_[ClpParam::CRASH]->appendKwd("dual");
  parameters_[ClpParam::CRASH]->appendKwd("dw");
  parameters_[ClpParam::CRASH]->appendKwd("idiot");
#else
  parameters_[ClpParam::CRASH]->appendKwd("idiot1");
  parameters_[ClpParam::CRASH]->appendKwd("idiot2");
  parameters_[ClpParam::CRASH]->appendKwd("idiot3");
  parameters_[ClpParam::CRASH]->appendKwd("idiot4");
  parameters_[ClpParam::CRASH]->appendKwd("idiot5");
  parameters_[ClpParam::CRASH]->appendKwd("idiot6");
  parameters_[ClpParam::CRASH]->appendKwd("idiot7");
#endif

  parameters_[ClpParam::CROSSOVER]->setup(
      "cross!over",
      "Whether to get a basic solution with the simplex algorithm after the "
      "barrier algorithm finished",
      "on", 0,
      "Interior point algorithms do not obtain a basic solution. This option "
      "will crossover to a basic solution suitable for ranging or branch and "
      "cut. With the current state of the solver for quadratic programs it may "
      "be a good idea to switch off crossover for this case (and maybe "
      "presolve as well) - the option 'maybe' does this.");
  parameters_[ClpParam::CRASH]->appendKwd("off");
  parameters_[ClpParam::CRASH]->appendKwd("maybe");
  parameters_[ClpParam::CRASH]->appendKwd("presolve");

  parameters_[ClpParam::DIRECTION]->setup(
      "direction", "Minimize or Maximize", "min!imize", 0,
      "The default is minimize - use 'direction maximize' for maximization.\n "
      "You can also use the parameters 'maximize' or 'minimize'.");
  parameters_[ClpParam::DIRECTION]->appendKwd("max!imize");
  parameters_[ClpParam::DIRECTION]->appendKwd("zero");

  parameters_[ClpParam::DUALPIVOT]->setup(
      "dualP!ivot", "Dual pivot choice algorithm", "auto!matic", 0,
      "The Dantzig method is simple but its use is deprecated.  Steepest is "
      "the method of choice and there are two variants which keep all weights "
      "updated but only scan a subset each iteration. Partial switches this on "
      "while automatic decides at each iteration based on information about "
      "the factorization. The PE variants add the Positive Edge criterion. "
      "This selects incoming variables to try to avoid degenerate moves. See "
      "also option psi.",
      CoinParam::displayPriorityLow);
  parameters_[ClpParam::DUALPIVOT]->appendKwd("dant!zig");
  parameters_[ClpParam::DUALPIVOT]->appendKwd("partial");
  parameters_[ClpParam::DUALPIVOT]->appendKwd("steep!est");
  parameters_[ClpParam::DUALPIVOT]->appendKwd("PEsteep!est");
  parameters_[ClpParam::DUALPIVOT]->appendKwd("PEdantzig");

  parameters_[ClpParam::FACTORIZATION]->setup(
      "fact!orization", "Which factorization to use", "normal", 0,
#ifndef ABC_INHERIT
      "The default is to use the normal CoinFactorization, but other choices "
      "are a dense one, OSL's, or one designed for small problems."
#else
      "Normally the default is to use the normal CoinFactorization, but other "
      "choices are a dense one, OSL's, or one designed for small problems. "
      "However if at Aboca then the default is CoinAbcFactorization and other "
      "choices are a dense one, one designed for small problems or if enabled "
      "a long factorization."
#endif
  );
  parameters_[ClpParam::FACTORIZATION]->appendKwd("dense");
  parameters_[ClpParam::FACTORIZATION]->appendKwd("simple");
  parameters_[ClpParam::FACTORIZATION]->appendKwd("osl");

  parameters_[ClpParam::GAMMA]->setup("gamma!(Delta)",
                                     "Whether to regularize barrier", "off", 0,
                                     "", CoinParam::displayPriorityLow);
  parameters_[ClpParam::GAMMA]->appendKwd("on");
  parameters_[ClpParam::GAMMA]->appendKwd("gamma");
  parameters_[ClpParam::GAMMA]->appendKwd("delta");
  parameters_[ClpParam::GAMMA]->appendKwd("onstrong");
  parameters_[ClpParam::GAMMA]->appendKwd("gammastrong");
  parameters_[ClpParam::GAMMA]->appendKwd("deltastrong");

  parameters_[ClpParam::PRESOLVE]->setup(
      "presolve", "Whether to presolve problem", "on", 0,
      "Presolve analyzes the model to find such things as redundant equations, "
      "equations which fix some variables, equations which can be transformed "
      "into bounds, etc. For the initial solve of any problem this is worth "
      "doing unless one knows that it will have no effect. Option 'on' will "
      "normally do 5 passes, while using 'more' will do 10.  If the problem is "
      "very large one can let CLP write the original problem to file by using "
      "'file'.");
  parameters_[ClpParam::PRESOLVE]->appendKwd("off");
  parameters_[ClpParam::PRESOLVE]->appendKwd("more");
  parameters_[ClpParam::PRESOLVE]->appendKwd("file");

  parameters_[ClpParam::PRIMALPIVOT]->setup(
      "primalP!ivot", "Primal pivot choice algorithm", "auto!matic", 0,
      "The Dantzig method is simple but its use is deprecated.  Exact devex is "
      "the method of choice and there are two variants which keep all weights "
      "updated but only scan a subset each iteration. Partial switches this on "
      "while 'change' initially does 'dantzig' until the factorization becomes "
      "denser. This is still a work in progress. The PE variants add the "
      "Positive Edge criterion. This selects incoming variables to try to "
      "avoid degenerate moves. See also Towhidi, M., Desrosiers, J., Soumis, "
      "F., The positive edge criterion within COIN-OR's CLP; Omer, J., "
      "Towhidi, M., Soumis, F., The positive edge pricing rule for the dual "
      "simplex.",
      CoinParam::displayPriorityLow);
  parameters_[ClpParam::PRIMALPIVOT]->appendKwd("exa!ct");
  parameters_[ClpParam::PRIMALPIVOT]->appendKwd("dant!zig");
  parameters_[ClpParam::PRIMALPIVOT]->appendKwd("part!ial");
  parameters_[ClpParam::PRIMALPIVOT]->appendKwd("steep!est");
  parameters_[ClpParam::PRIMALPIVOT]->appendKwd("change");
  parameters_[ClpParam::PRIMALPIVOT]->appendKwd("sprint");
  parameters_[ClpParam::PRIMALPIVOT]->appendKwd("PEsteep!est");
  parameters_[ClpParam::PRIMALPIVOT]->appendKwd("PEdantzig");

  parameters_[ClpParam::INTPRINT]->setup(
      "printi!ngOptions", "Print options", "normal", 0,
      "This changes the amount and format of printing a solution:\nnormal - "
      "nonzero column variables \n integer - nonzero integer column "
      "variables\n special - in format suitable for OsiRowCutDebugger\n rows - "
      "nonzero column variables and row activities\n all - all column "
      "variables and row activities.\n\n For non-integer problems 'integer' "
      "and 'special' act like 'normal'. Also see printMask for controlling "
      "output.");
  parameters_[ClpParam::INTPRINT]->appendKwd("integer");
  parameters_[ClpParam::INTPRINT]->appendKwd("special");
  parameters_[ClpParam::INTPRINT]->appendKwd("rows");
  parameters_[ClpParam::INTPRINT]->appendKwd("all");
  parameters_[ClpParam::INTPRINT]->appendKwd("csv");
  parameters_[ClpParam::INTPRINT]->appendKwd("bound!ranging");
  parameters_[ClpParam::INTPRINT]->appendKwd("rhs!ranging");
  parameters_[ClpParam::INTPRINT]->appendKwd("objective!ranging");
  parameters_[ClpParam::INTPRINT]->appendKwd("stats");
  parameters_[ClpParam::INTPRINT]->appendKwd("boundsint");
  parameters_[ClpParam::INTPRINT]->appendKwd("boundsall");
  parameters_[ClpParam::INTPRINT]->appendKwd("fixint");
  parameters_[ClpParam::INTPRINT]->appendKwd("fixall");

  parameters_[ClpParam::SCALING]->setup("scal!ing", "Whether to scale problem",
                                       "auto!matic", 0, "",
                                       CoinParam::displayPriorityLow);
  parameters_[ClpParam::SCALING]->appendKwd("off");
  parameters_[ClpParam::SCALING]->appendKwd("equi!librium");
  parameters_[ClpParam::SCALING]->appendKwd("geo!metric");
  parameters_[ClpParam::SCALING]->appendKwd("dynamic");
  parameters_[ClpParam::SCALING]->appendKwd("rows!only");

#ifndef COIN_AVX2
  parameters_[ClpParam::VECTOR]->setup(
      "vector", "Whether to use vector? Form of matrix in simplex", "off", 0,
      "If this is on ClpPackedMatrix uses extra column copy in odd format.",
      CoinParam::displayPriorityNone);
  parameters_[ClpParam::VECTOR]->appendKwd("on");
#else
  parameters_[ClpParam::VECTOR]->setup(
      "vector", "Try and use vector instructions in simplex", "off", 0,
      "At present only for Intel architectures - but could be extended. Uses "
      "avx2 or avx512 instructions. Uses different storage for matrix - can be "
      "of benefit without instruction set on some problems. I may add pool to "
      "switch on a pool matrix",
      CoinParam::displayPriorityNone);
  parameters_[ClpParam::VECTOR]->appendKwd("on");
  parameters_[ClpParam::VECTOR]->appendKwd("ones");
#endif
}

//###########################################################################
//###########################################################################

void ClpParameters::addClpDblParams() {
  for (int code = ClpParam::FIRSTDBLPARAM + 1; code < ClpParam::LASTDBLPARAM;
       code++) {
    parameters_[code]->setPushFunc(ClpParamUtils::pushClpDblParam);
  }

  parameters_[ClpParam::DUALBOUND]->setup(
      "dualB!ound",
      "Initially algorithm acts as if no gap between bounds exceeds this value",
      1.0e-20, 1.0e20, 0.0,
      "The dual algorithm in Clp is a single phase algorithm as opposed to a "
      "two phase algorithm where you first get feasible then optimal.  If a "
      "problem has both upper and lower bounds then it is trivial to get dual "
      "feasible by setting non basic variables to correct bound.  If the gap "
      "between the upper and lower bounds of a variable is more than the value "
      "of dualBound Clp introduces fake bounds so that it can make the problem "
      "dual feasible.  This has the same effect as a composite objective "
      "function in the primal algorithm.  Too high a value may mean more "
      "iterations, while too low a bound means the code may go all the way and "
      "then have to increase the bounds.  OSL had a heuristic to adjust "
      "bounds, maybe we need that here.");

  parameters_[ClpParam::DUALTOLERANCE]->setup("dualT!olerance", "For an optimal solution no dual infeasibility may exceed this value", 1.0e-20, COIN_DBL_MAX, 0.0, "Normally the default tolerance is fine, but one may want to increase it a bit if the dual simplex algorithm seems to be having a hard time. One method which can be faster is to use a large tolerance e.g. 1.0e-4 and the dual simplex algorithm and then to clean up the problem using the primal simplex algorithm with the correct tolerance (remembering to switch off presolve for this final short clean up phase).");
                                         
  parameters_[ClpParam::FAKEBOUND]->setup(
      "fakeB!ound", "All bounds <= this value - DEBUG", 1.0, 1.0e15);

  parameters_[ClpParam::OBJSCALE]->setup(
      "objective!Scale", "Scale factor to apply to objective", -COIN_DBL_MAX,
      COIN_DBL_MAX, 1.0,
      "If the objective function has some very large values, you may wish to "
      "scale them internally by this amount.  It can also be set by autoscale. "
      "It is applied after scaling.  You are unlikely to need this.",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::PRESOLVETOLERANCE]->setup(
      "preT!olerance", "Tolerance to use in presolve", 1.0e-20, COIN_DBL_MAX,
      0.0,
      "One may want to increase this tolerance if presolve says the problem is "
      "infeasible and one has awkward numbers and is sure that the problem is "
      "really feasible.");

  parameters_[ClpParam::PRIMALTOLERANCE]->setup(
      "primalT!olerance",
      "For a feasible solution no primal infeasibility, i.e., constraint "
      "violation, may exceed this value",
      1.0e-20, COIN_DBL_MAX, 0.0,
      "Normally the default tolerance is fine, but one may want to increase it "
      "a bit if the primal simplex algorithm seems to be having a hard time.");

  parameters_[ClpParam::PRIMALWEIGHT]->setup(
      "primalW!eight",
      "Initially algorithm acts as if it costs this much to be infeasible",
      1.0e-20, COIN_DBL_MAX, 0.0,
      "The primal algorithm in Clp is a single phase algorithm as opposed to a "
      "two phase algorithm where you first get feasible then optimal.  So Clp "
      "is minimizing this weight times the sum of primal infeasibilities plus "
      "the true objective function (in minimization sense). Too high a value "
      "may mean more iterations, while too low a value means the algorithm may "
      "iterate into the wrong directory for long and then has to increase the "
      "weight in order to get feasible."); // OSL had a heuristic to adjust
                                           // bounds, maybe we need that here.

  parameters_[ClpParam::PSI]->setup(
      "psi", "Two-dimension pricing factor for Positive Edge criterion", -1.1,
      1.1, -0.5,
      "The Positive Edge criterion has been added to select incoming variables to try and avoid degenerate moves. Variables not in the promising set have their infeasibility weight multiplied by psi, so 0.01 would mean that if there were any promising variables, then they would always be chosen, while 1.0 effectively switches the algorithm off. There are two ways of switching this feature on.  One way is to set psi to a positive value and then the Positive Edge criterion will be used for both primal and dual simplex.  The other way is to select PEsteepest in dualpivot choice (for example), then the absolute value of psi is used. \
Code donated by Jeremy Omer.  See Towhidi, M., Desrosiers, J., Soumis, F., The positive edge criterion within COIN-OR's CLP; Omer, J., Towhidi, M., Soumis, F., The positive edge pricing rule for the dual simplex."); // Until this settles down it is only implemented in CLP.

  parameters_[ClpParam::PROGRESS]->setup(
      "progress!(Interval)", "Time interval for printing progress",
      -COIN_DBL_MAX, COIN_DBL_MAX, 0.7,
      "This sets a minimum interval for some printing - elapsed seconds");

  parameters_[ClpParam::OBJSCALE2]->setup(
      "reallyO!bjectiveScale", "Scale factor to apply to objective in place",
      -COIN_DBL_MAX, COIN_DBL_MAX, 1.0,
      "You can set this to -1.0 to test maximization or other to stress code",
      CoinParam::displayPriorityNone);

  parameters_[ClpParam::RHSSCALE]->setup(
      "rhs!Scale", "Scale factor to apply to rhs and bounds", -COIN_DBL_MAX,
      COIN_DBL_MAX, 1.0,
      "If the rhs or bounds have some very large meaningful values, you may "
      "wish to scale them internally by this amount.  It can also be set by "
      "autoscale.  This should not be needed.",
      CoinParam::displayPriorityNone);

  parameters_[ClpParam::TIMELIMIT]->setup(
      "sec!onds", "Maximum seconds", -1.0, COIN_DBL_MAX, -1.0,
      "After this many seconds clp will act as if maximum iterations had been "
      "reached (if value >=0).");

  parameters_[ClpParam::ZEROTOLERANCE]->setup(
      "zeroT!olerance",
      "Kill all coefficients whose absolute value is less than this value",
      1.0e-100, 1.0e-5, 1.0e-20,
      "This applies to reading mps files (and also lp files if "
      "KILL_ZERO_READLP defined)");
}

//###########################################################################
//###########################################################################

void ClpParameters::addClpIntParams() {
  for (int code = ClpParam::FIRSTINTPARAM + 1; code < ClpParam::LASTINTPARAM;
       code++) {
    parameters_[code]->setPushFunc(ClpParamUtils::pushClpIntParam);
  }

  parameters_[ClpParam::CPP]->setup(
      "cpp!Generate", "Generates C++ code", -1, 50000, 0,
      "Once you like what the stand-alone solver does then this allows you to "
      "generate user_driver.cpp which approximates the code. 0 gives simplest "
      "driver, 1 generates saves and restores, 2 generates saves and restores "
      "even for variables at default value. 4 bit in cbc generates size "
      "dependent code rather than computed values. This is now deprecated as "
      "you can call stand-alone solver - see Cbc/examples/driver4.cpp.",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::DECOMPOSE_BLOCKS]->setup(
      "decomp!ose", "Whether to try decomposition", -COIN_INT_MAX, COIN_INT_MAX,
      0,
      "0 - off, 1 choose blocks >1 use as blocks Dantzig Wolfe if primal, "
      "Benders if dual - uses sprint pass for number of passes",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::DENSE]->setup(
      "dense!Threshold", "Threshold for using dense factorization", -1, 10000,
      -1,
      "If processed problem <= this use dense factorization",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::DUALIZE]->setup("dualize", "Solves dual reformulation",
                                       0, 4, 0, "Don't even think about it.",
                                       CoinParam::displayPriorityLow);

  parameters_[ClpParam::IDIOT]->setup(
      "idiot!Crash", "Whether to try idiot crash", -1, COIN_INT_MAX, 0,
      "This is a type of 'crash' which works well on some homogeneous "
      "problems. It works best on problems with unit elements and rhs but will "
      "do something to any model.  It should only be used before the primal "
      "simplex algorithm.  It can be set to -1 when the code decides for "
      "itself whether to use it, 0 to switch off, or n > 0 to do n passes.");

  parameters_[ClpParam::LOGLEVEL]->setup(
      "log!Level", "Level of detail in Coin branch and Cut output", -63, 63, 1,
      "If 0 then there should be no output in normal circumstances.  1 is "
      "probably the best value for most uses, while 2 and 3 give more "
      "information.");

  parameters_[ClpParam::MAXFACTOR]->setup(
      "maxF!actor", "Maximum number of iterations between refactorizations", 1,
      COIN_INT_MAX, 0,
      "If this is left at its default value of 200 then CLP will guess a  "
      "value to use.  CLP may decide to re-factorize earlier for accuracy.");

  parameters_[ClpParam::MAXITERATION]->setup(
      "maxIt!erations", "Maximum number of iterations before stopping", 0,
      COIN_INT_MAX, 0,
      "This can be used for testing purposes.  The corresponding library "
      "call\n \tsetMaximumIterations(value)\n can be useful.  If the code "
      "stops on seconds or by an interrupt this will be treated as stopping on "
      "maximum iterations. This is ignored in branchAndCut - use maxN!odes.");

  parameters_[ClpParam::OUTPUTFORMAT]->setup(
      "output!Format", "Which output format to use", 1, 6, 0,
      "Normally export will be done using normal representation for numbers "
      "and two values per line.  You may want to do just one per line (for "
      "grep or suchlike) and you may wish to save with absolute accuracy using "
      "a coded version of the IEEE value. A value of 2 is normal. otherwise "
      "odd values gives one value per line, even two.  Values 1,2 give normal "
      "format, 3,4 gives greater precision, while 5,6 give IEEE values.  When "
      "used for exporting a basis 1 does not save values, 2 saves values, 3 "
      "with greater accuracy and 4 in IEEE.");

  parameters_[ClpParam::PRESOLVEPASS]->setup(
      "passP!resolve", "How many passes in presolve", -200, 100, 0,
      "Normally Presolve does 10 passes but you may want to do less to make it "
      "more lightweight or do more if improvements are still being made.  As "
      "Presolve will return if nothing is being taken out, you should not "
      "normally need to use this fine tuning.",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::PERTVALUE]->setup("pertV!alue", "Method of perturbation",
                                         -5000, 102, 0, "",
                                         CoinParam::displayPriorityLow);

  parameters_[ClpParam::PRINTOPTIONS]->setup(
      "pO!ptions", "Dubious print options", 0, COIN_INT_MAX, 0,
      "If this is > 0 then presolve will give more information and branch and "
      "cut will give statistics",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::RANDOMSEED]->setup(
      "randomS!eed", "Random seed for Clp", 0, COIN_INT_MAX, 1234567,
      "Initialization of the random seed for pseudo-random numbers used to "
      "break ties in degenerate problems. This may yield a different "
      "continuous optimum and, in the context of Cbc, different cuts and "
      "heuristic solutions. The special value of 0 lets CLP use the time of "
      "the day for the initial seed.");

  parameters_[ClpParam::SLPVALUE]->setup(
      "slp!Value", "Number of slp passes before primal", -50000, 50000, 0,
      "If you are solving a quadratic problem using primal then it may be "
      "helpful to do some sequential Lps to get a good approximate solution.",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::SMALLFACT]->setup(
      "small!Factorization", "Threshold for using small factorization", -1,
      10000, -1, "If processed problem <= this use small factorization",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::SPECIALOPTIONS]->setup(
      "special!Options", "Dubious options for Simplex - see ClpSimplex.hpp", 0,
      COIN_INT_MAX, 0, "", CoinParam::displayPriorityNone);

  parameters_[ClpParam::SPRINT]->setup(
      "sprint!Crash", "Whether to try sprint crash", -1, COIN_INT_MAX, 0,
      "For long and thin problems this method may solve a series of small "
      "problems created by taking a subset of the columns.  The idea as "
      "'Sprint' was introduced by J. Forrest after an LP code of that name of "
      "the 60's which tried the same tactic (not totally successfully). CPLEX "
      "calls it 'sifting'.  -1 lets CLP automatically choose the number of "
      "passes, 0 is off, n is number of passes");

  parameters_[ClpParam::SUBSTITUTION]->setup(
      "subs!titution", "How long a column to substitute for in presolve", 0,
      10000, 3,
      "Normally Presolve gets rid of 'free' variables when there are no more "
      "than 3 coefficients in a row.  If you increase this, the number of rows "
      "may decrease but the number of coefficients may increase.",
      CoinParam::displayPriorityNone);

#ifdef CLP_THREAD
  parameters_[ClpParam::THREADS]->setup(
      "thread!s", "Number of threads to try and use", -100, 100000, 0,
      "To use multiple threads, set threads to number wanted.  It may be "
      "better to use one or two more than number of cpus available.  If 100+n "
      "then n threads and search is repeatable (maybe be somewhat slower), if "
      "200+n use threads for root cuts, 400+n threads used in sub-trees.");
#endif

  parameters_[ClpParam::VERBOSE]->setup(
      "verbose", "Switches on longer help on single ?", 0, 31, 0,
      "Set to 1 to get short help with ? list, 2 to get long help, 3 for both. "
      " (add 4 to just get ampl ones).",
      CoinParam::displayPriorityNone);
}

//###########################################################################
//###########################################################################

void ClpParameters::addClpBoolParams() {
  for (int code = ClpParam::FIRSTBOOLPARAM + 1; code < ClpParam::LASTBOOLPARAM;
       code++) {
    parameters_[code]->appendKwd("off", ClpParameters::ParamOff);
    parameters_[code]->appendKwd("on", ClpParameters::ParamOn);
    parameters_[code]->setPushFunc(ClpParamUtils::pushClpKwdParam);
  }

  parameters_[ClpParam::AUTOSCALE]->setup(
      "auto!Scale",
      "Whether to scale objective, rhs and bounds of problem if they look odd",
      "off", 0,
      "If you think you may get odd objective values or large equality rows "
      "etc then it may be worth setting this true.  It is still experimental "
      "and you may prefer to use objective!Scale and rhs!Scale.",
      CoinParam::displayPriorityNone);

  parameters_[ClpParam::BUFFER_MODE]->setup(
      "buff!eredMode", "Whether to flush print buffer", "on", 1,
      "Default is on, off switches on unbuffered output");

  parameters_[ClpParam::ERRORSALLOWED]->setup(
      "error!sAllowed", "Whether to allow import errors", "off", 0,
      "The default is not to use any model which had errors when reading the "
      "mps file. Setting this to 'on' will allow all errors from which the "
      "code can recover simply by ignoring the error.  There are some errors "
      "from which the code can not recover e.g. no ENDATA.  This has to be set "
      "before import i.e. -errorsAllowed on -import xxxxxx.mps.");

  parameters_[ClpParam::KEEPNAMES]->setup(
      "keepN!ames", "Whether to keep names from import", "on", 1,
      "It saves space to get rid of names so if you need to you can set this "
      "to off. This needs to be set before the import of model - so -keepnames "
      "off -import xxxxx.mps.");

  parameters_[ClpParam::KKT]->setup(
      "KKT", "Whether to use KKT factorization in barrier", "off", 0, "",
      CoinParam::displayPriorityLow);

  parameters_[ClpParam::MESSAGES]->setup(
      "mess!ages", "Controls if Clpnnnn is printed", "off", 0,
      "The default behavior is to put out messages such as:\n Clp0005 2261  "
      "Objective 109.024 Primal infeas 944413 (758)\n but this program turns "
      "this off to make it look more friendly.  It can be useful to turn them "
      "back on if you want to be able to 'grep' for particular messages or if "
      "you intend to override the behavior of a particular message.");

  parameters_[ClpParam::PERTURBATION]->setup(
      "perturb!ation", "Whether to perturb the problem", "on", 1,
      "Perturbation helps to stop cycling, but CLP uses other measures for "
      "this. However, large problems and especially ones with unit elements "
      "and unit right hand sides or costs benefit from perturbation.  Normally "
      "CLP tries to be intelligent, but one can switch this off.");

  parameters_[ClpParam::PFI]->setup(
      "PFI", "Whether to use Product Form of Inverse in simplex", "off", 0,
      "By default clp uses Forrest-Tomlin L-U update.  If you are masochistic "
      "you can switch it off.",
      CoinParam::displayPriorityNone);

  parameters_[ClpParam::SPARSEFACTOR]->setup(
      "spars!eFactor", "Whether factorization treated as sparse", "on", 1, "",
      CoinParam::displayPriorityNone);
}

//###########################################################################
//###########################################################################