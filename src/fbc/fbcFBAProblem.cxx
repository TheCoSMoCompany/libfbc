#include "fbcFBAProblem.hxx"

namespace fbc
{

/** \brief Constructor.
 *
 */
FBAProblem::FBAProblem()
{
}


/** \brief Destructor.
 *
 */
FBAProblem::~FBAProblem()
{
  delete_lp(lpModel);
}

/** \brief Getter.
 * @return lpModel
 */
lprec* FBAProblem::getLpModel()
{
  return lpModel;
}

/** \brief Getter.
 * @return solution
 */
fbc::Solution FBAProblem::getSolution()
{
  return solution;
}

/** \brief Initializes this with the content of a LP file.
 * The lp-format is a file format for describing linear programming problems in
 * lp_solve; its complete description can be found in the lp_solve reference
 * guide at http://lpsolve.sourceforge.net/.
 * @param file Address of a LP file.
 */
void FBAProblem::initFromLPFile(const char* file)
{
  lpModel = read_LP(const_cast<char*>(file), NORMAL, "");
}

/** \brief Initializes this with the content of a SBML file.
 * The Systems Biology Markup Language (SBML) is an XML-based descriptive
 * language for systems biology. Specifications of its latest version can be
 * found at http://sbml.org.
 * @param file Address of a SBML file.
 */
void FBAProblem::initFromSBMLFile(const char* file)
{
  SBMLReader reader = SBMLReader();
  SBMLDocument* doc = reader.readSBMLFromFile(file);
  if (doc->getNumErrors() > 0)
  {
    std::cout << "Error " << doc->getError(0)->getErrorId() << " at line " <<
      doc->getError(0)->getLine() << " when opening file " << file << ": " <<
      doc->getError(0)->getMessage() << "\n";
    exit(EXIT_FAILURE);
  }
  else
  {
    Model* sb_model = doc->getModel();
    // acquire FBC plugin - mandatory
    FbcModelPlugin* pl =
      dynamic_cast<FbcModelPlugin*>(sb_model->getPlugin("fbc"));
    if (pl == NULL)
    {
      std::cout << "Error: no FBC plugin found in " << file;
      exit(EXIT_FAILURE);
    }
    populateMatrix(sb_model, pl);
  }
}

/** \brief Initializes this with a SBML string.
 * The Systems Biology Markup Language (SBML) is an XML-based descriptive
 * language for systems biology. Specifications of its latest version can be
 * found at http://sbml.org.
 * @param string A SBML string.
 */
void FBAProblem::initFromSBMLString(const char* string)
{
  SBMLReader reader = SBMLReader();
  SBMLDocument* doc = reader.readSBMLFromString(string);
  if (doc->getNumErrors() > 0)
  {
    std::cout << "Error " << doc->getError(0)->getErrorId() << " at line " <<
      doc->getError(0)->getLine() << " when opening input string";
  }
  else
  {
    Model* sb_model = doc->getModel();
    // acquire FBC plugin - mandatory
    FbcModelPlugin* pl =
      dynamic_cast<FbcModelPlugin*>(sb_model->getPlugin("fbc"));
    if (pl == NULL)
    {
      std::cout << "Error: no FBC plugin found.";
      exit(EXIT_FAILURE);
    }
    populateMatrix(sb_model, pl);
  }
}

/** \brief Fill the matrix of lpModel.
 * @param sb_model A Model object describing the problem to be solved.
 * @param pl A FbcModelPlugin object corresponding to sb_model.
 */
void FBAProblem::populateMatrix(Model* sb_model, FbcModelPlugin* pl)
{
  const Objective* obj = pl->getActiveObjective();
  const int num_species = sb_model->getNumSpecies();
  lpModel = make_lp(num_species, 0);
  // set the objective direction
  if (
    strcmp(const_cast<Objective*>(obj)->getType().c_str(), "maximize") == 0)
  {
    set_maxim(lpModel);
  }
  else
  {
    set_minim(lpModel);
  }
  // names rows after the species ids - row 0 is the objective function
  for (int i = 0; i < num_species; i++)
  {
    const std::string id = sb_model->getSpecies(i)->getId();
    set_row_name(lpModel, i+1, const_cast<char*>(id.c_str()));
  }
  // populate problem matrix
  for (int r = 0; r < sb_model->getNumReactions(); r++)
  {
    REAL col[num_species + 1];
    Reaction* rt = sb_model->getReaction(r);
    // objective function
    for (int o = 0; o < obj->getNumFluxObjectives(); o++)
    {
      const FluxObjective* fl_obj = obj->getFluxObjective(o);
      if (strcmp(rt->getId().c_str(), fl_obj->getReaction().c_str()) == 0)
      {
        col[0] = fl_obj->getCoefficient();
      }
      else
      {
        col[0] = 0.0;
      }
    }
    // stoichiometry matrix
    for (int s = 1; s <= num_species; s++)
    {
      char* current = get_row_name(lpModel, s);
      if (rt->getReactant(current) != NULL)
      {
        col[s] = - rt->getReactant(current)->getStoichiometry();
      }
      else if (rt->getProduct(current) != NULL)
      {
        col[s] = rt->getProduct(current)->getStoichiometry();
      }
      else
      {
        col[s] = 0.0;
      }
    }
    add_column(lpModel, col);
    // name column after reaction id
    const std::string r_id = rt->getId();
    set_col_name(lpModel, r+1, const_cast<char*>(r_id.c_str()));
  }
}

/** \brief Displays the problem encoded in lpModel.
 */
void FBAProblem::print()
{
  print_lp(lpModel);
}

/** \brief Solves the linear problem encoded by this.
 * Solution will be stored in this->solution.
 */
void FBAProblem::solveProblem()
{
  solve(lpModel);
}

}