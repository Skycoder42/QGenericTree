#ifndef QUNORDEREDTREE_H
#define QUNORDEREDTREE_H

#include "qgenerictreebase.h"

#include <QtCore/QHash>

template <typename TKey, typename TValue>
using QUnorderedTree = QGenericTreeBase<TKey, TValue, QHash>;

#endif // QUNORDEREDTREE_H
