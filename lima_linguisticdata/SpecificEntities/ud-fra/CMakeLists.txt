# SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# French ModEx rules for the deeplima-backed ud-fra media. Ported from the
# legacy fre/ rules with UD categories (NC->NOUN, NP->PROPN, V->VERB,
# PREP->ADP). Large category-free lists and gazetteers are not duplicated:
# the rules use them from ../../fre/.

add_subdirectory(resources)
add_subdirectory(Numex)
add_subdirectory(DateTime)
add_subdirectory(Organization)
add_subdirectory(Location)
add_subdirectory(Person)
add_subdirectory(Event)
add_subdirectory(Product)
