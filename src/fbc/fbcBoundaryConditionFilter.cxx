#include "fbcBoundaryConditionFilter.hxx"

#include "sbml/SBase.h"
#include "sbml/Species.h"

namespace fbc
{

/** Return true if the tested element is a Species instance with a
 * boundaryCondition attribute set to "true"; return false otherwise.
 * @param element An SBase instance to be tested.
 */
bool BoundaryConditionFilter::filter(const SBase* element)
{
  const Species* sp = dynamic_cast<const Species*>(element);
  if (sp == NULL)
  {
    return false;
  }
  else
  {
    if (sp->isSetBoundaryCondition())
    {
      return sp->getBoundaryCondition();
    }
    else
    {
      return false;
    }
  }
}

}
