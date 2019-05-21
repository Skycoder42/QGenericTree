#ifndef QORDEREDTREE_H
#define QORDEREDTREE_H

#include "qgenerictreebase.h"

#include <QtCore/QMap>

template <typename TKey, typename TValue>
using QOrderedTree = QGenericTreeBase<TKey, TValue, QMap>;

#endif // QORDEREDTREE_H
