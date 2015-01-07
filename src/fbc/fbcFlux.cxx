#include "fbcFlux.hxx"

#include "sbml/Reaction.h"
#include "sbml/Species.h"
#include "sbml/util/List.h"

namespace fbc
{

/** \brief Constructor.
 *
 */
Flux::Flux()
{
}

/** \brief Constructor.
 * 
 * @param rt Pointer to a Reaction instance.
 * @param flx Value of the exchange flux.
 * @param bc List of boundary condition metabolites in the problem.
 */
Flux::Flux(Reaction* rt, double flx, List* bc)
{
  id = rt->getId();
  value = flx;
  exchange = false;
  bcProduct = false;
  bcReactant = false;
  // acquiring product identifiers
  products.clear();
  for (int p = 0; p < rt->getNumProducts(); p++)
  {
    products.push_back(rt->getProduct(p)->getSpecies());
  }
  // acquiring reactant identifiers
  reactants.clear();
  for (int r = 0; r < rt->getNumReactants(); r++)
  { 
    reactants.push_back(rt->getReactant(r)->getSpecies());
  }
  // is "rt" an exchange flux?
  for (int s = 0; s < bc->getSize(); s++)
  {
    Species* sp = (Species*) bc->get(s);
    if (rt->getReactant(sp->getId()) != NULL)
    {
      exchange = true;
      bcReactant = true;
    }
    else if (rt->getProduct(sp->getId()) != NULL)
    {
      exchange = true;
      bcProduct = true;
    }
  }
}

/** \brief Destructor.
 *
 */
Flux::~Flux()
{
}

/** \brief Getter.
 *
 * @return True if the boundary condition metabolite is a product of this,
 * false otherwise. If "false" is returned, it does not necessarily means that
 * the boundary condition metabolite is a reactant.
 */
bool Flux::isBCProduct()
{
  return bcProduct;
}

/** \brief Getter.
 * 
 * @return True if the boundary condition metabolite is a reactant of this,
 * false otherwise. If "false" is returned, it does not necessarily means that
 * the boundary condition metabolite is a product.
 */
bool Flux::isBCReactant()
{
  return bcReactant;
}

/** \brief Getter.
 *
 * @return exchange
 */
bool Flux::isExchange()
{
  return exchange;
}

/** \brief Getter.
 *
 * @return products
 */
std::vector<std::string> Flux::getProductIds()
{
  return products;
}

/** \brief Getter.
 *
 * @return reactants
 */
std::vector<std::string> Flux::getReactantIds()
{
  return reactants;
}

/** \brief Getter.
 *
 * @return id
 */
std::string Flux::getId()
{
  return id;
}

/** \brief Getter.
 *
 * @return value
 */
double Flux::getValue()
{
  return value;
}

/** \brief Setter for "value".
 *
 * @param flx A double.
 */
void Flux::setValue(double flx)
{
  value = flx;
}

}
