#ifndef QGENERICTREEBASE_H
#define QGENERICTREEBASE_H

#include <optional>
#include <iterator>
#include <type_traits>

#include <QtCore/QSharedPointer>
#include <QtCore/QWeakPointer>

template <typename TKey, typename TValue, template<class, class> class TContainer>
class QGenericTreeBase
{
private:
	struct NodeData;
	using NodePtr = QSharedPointer<NodeData>;
	using WeakNodePtr = QWeakPointer<NodeData>;
	using Container = TContainer<TKey, NodePtr>;

public:
	class Node
	{
	public:
		Node();

		Node(const Node &other) = default;
		Node &operator=(const Node &other) = default;
		Node(Node &&other) noexcept = default;
		Node &operator=(Node &&other) noexcept = default;
		~Node() = default;

		explicit operator bool() const;
		bool operator!() const;

		// value access functions
		bool hasValue() const;
		template <typename TDefault>
		TValue value(TDefault &&defaultValue = TValue{}) const;
		void setValue(TValue value);
		TValue takeValue();
		void clearValue();
		// value access operators
		template <typename TAssign>
		Node &operator=(TAssign &&value);
		const TValue &operator*() const &;
		TValue &operator*() &;
		const TValue &&operator*() const &&;
		TValue &&operator*() &&;
		const TValue *operator->() const;
		TValue *operator->();

		// child access
		bool containsChild(const TKey &key) const;
		int childCount() const;
		bool hasChildren();
		QList<Node> children();
		QList<const Node> children() const;
		Node child(const TKey &key);
		const Node child(const TKey &key) const;
		void insertChild(const TKey &key, Node child);
		Node emplaceChild(const TKey &key);
		Node takeChild(const TKey &key);
		bool removeChild(const TKey &key);
		void clearChildren();
		// child access operators
		const Node operator[](const TKey &key) const;
		Node operator[](const TKey &key);

		// tree access
		int depth() const;
		QList<TKey> key() const;
		TKey subKey() const;
		Node parent();
		const Node parent() const;
		Node findChild(const QList<TKey> &keys);
		const Node findChild(const QList<TKey> &keys) const;

		// other
		void detach();
		Node clone() const;

	private:
		friend class QGenericTreeBase;
		NodePtr d;

		inline Node(NodePtr data) :
			d{std::move(data)}
		{}
	};

	using iterator_category_const = std::bidirectional_iterator_tag;
	struct iterator_category_non_const : public iterator_category_const, public std::output_iterator_tag {};

	template <typename TIterValue>
	class iterator_base
	{
		friend class QGenericTreeBase;
	public:
		static constexpr auto is_const = std::is_const_v<TIterValue>;
		using iterator_category = std::conditional_t<is_const, iterator_category_const, iterator_category_non_const>;
		using value_type = TIterValue;
		using difference_type = int;
		using pointer = value_type*;
		using reference = value_type&;

		// LegacyIterator requirements
		iterator_base(const iterator_base &other) = default;
		iterator_base &operator=(const iterator_base &other) = default;
		friend inline void swap(iterator_base &lhs, iterator_base &rhs) noexcept { swap(lhs._node, rhs._node); } // must be implemented inline because of the friend declaration

		// LegacyInputIterator & LegacyOutputIterator requirements
		bool operator==(const iterator_base &other) const;
		bool operator!=(const iterator_base &other) const;
		reference operator*() const;
		pointer operator->() const;
		iterator_base &operator++();
		iterator_base operator++(int);

		// LegacyForwardIterator requirements
		iterator_base() = default;

		// LegacyBidirectionalIterator requirements
		iterator_base &operator--();
		iterator_base operator--(int);

		// non-STL
		iterator_base(iterator_base &&other) noexcept = default;
		iterator_base &operator=(iterator_base &&other) noexcept = default;
		explicit operator bool() const;
		bool operator!() const;
		QList<TKey> key() const;
		TKey subKey() const;
		template<typename = std::enable_if_t<!is_const, void*>>
		Node node() const;
		template<typename = std::enable_if_t<is_const, void*>>
		const Node node() const;

	protected:
		NodePtr _node;

		iterator_base(NodePtr data);
	};

	using iterator = iterator_base<TValue>;
	using const_iterator = iterator_base<const TValue>;

	QGenericTreeBase() = default;
	QGenericTreeBase(const QGenericTreeBase &other) = delete;
	QGenericTreeBase &operator=(const QGenericTreeBase &other) = delete;
	QGenericTreeBase(QGenericTreeBase &&other) noexcept = default;
	QGenericTreeBase &operator=(QGenericTreeBase &&other) noexcept = default;
	friend inline void swap(QGenericTreeBase &lhs, QGenericTreeBase &rhs) noexcept { swap(lhs._root, rhs._root); } // must be implemented inline because of the friend declaration

	static QGenericTreeBase makeTree(Node node);

	Node rootNode();
	const Node rootNode() const;

	bool contains(const QList<TKey> &key) const;
	int countElements(bool valueOnly = false) const;
	const Node operator[](const QList<TKey> &key) const;
	Node operator[](const QList<TKey> &key);

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	void clear();
	QGenericTreeBase clone() const;

private:
	struct NodeData {
		inline NodeData(WeakNodePtr parent = {});
		inline NodeData(const NodeData &) = default;
		inline NodeData &operator=(const NodeData &) = default;
		inline NodeData(NodeData &&) noexcept = default;
		inline NodeData &operator=(NodeData &&) noexcept = default;

		WeakNodePtr parent;
		Container children;
		std::optional<TValue> value;

		NodePtr find(const QList<TKey> &keys, int index, const NodePtr &current) const;
		NodePtr clone() const;
	};

	Node _root;
};

// GENERIC IMPLEMENTATION

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer>::Node::Node() :
	d{NodePtr::create()}
{}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer>::Node::operator bool() const {
	return d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::Node::operator!() const {
	return !d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::Node::hasValue() const {
	return d->value.has_value();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TDefault>
TValue QGenericTreeBase<TKey, TValue, TContainer>::Node::value(TDefault &&defaultValue) const {
	return d->value.value_or(std::forward<TDefault>(defaultValue));
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::Node::setValue(TValue value) {
	d->value = std::move(value);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
TValue QGenericTreeBase<TKey, TValue, TContainer>::Node::takeValue() {
	if (d->value) {
		auto tValue = *std::move(d->value);
		d->value = std::nullopt;
		return tValue;
	} else
		return {};
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::Node::clearValue() {
	d->value = std::nullopt;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TAssign>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node &QGenericTreeBase<TKey, TValue, TContainer>::Node::operator=(TAssign &&value) {
	d->value = std::forward<TAssign>(value);
	return *this;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
const TValue &QGenericTreeBase<TKey, TValue, TContainer>::Node::operator*() const & {
	return *(d->value);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
TValue &QGenericTreeBase<TKey, TValue, TContainer>::Node::operator*() & {
	return *(d->value);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
const TValue &&QGenericTreeBase<TKey, TValue, TContainer>::Node::operator*() const && {
	return *(std::move(d->value));
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
TValue &&QGenericTreeBase<TKey, TValue, TContainer>::Node::operator*() && {
	return *(std::move(d->value));
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
const TValue *QGenericTreeBase<TKey, TValue, TContainer>::Node::operator->() const {
	return d->value.operator->();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
TValue *QGenericTreeBase<TKey, TValue, TContainer>::Node::operator->() {
	return d->value.operator->();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::Node::containsChild(const TKey &key) const {
	return d->children.contains(key);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
int QGenericTreeBase<TKey, TValue, TContainer>::Node::childCount() const {
	return d->children.size();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::Node::hasChildren() {
	return !d->children.empty();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QList<typename QGenericTreeBase<TKey, TValue, TContainer>::Node> QGenericTreeBase<TKey, TValue, TContainer>::Node::children() {
	QList<Node> childList;
	childList.reserve(d->children.size());
	for (const auto &child : d->children)
		childList.append(child);
	return childList;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QList<const typename QGenericTreeBase<TKey, TValue, TContainer>::Node> QGenericTreeBase<TKey, TValue, TContainer>::Node::children() const {
	QList<const Node> childList;
	childList.reserve(d->children.size());
	for (const auto &child : d->children)
		childList.append(child);
	return childList;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::child(const TKey &key) {
	return d->children.value(key, NodePtr{});
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
const typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::child(const TKey &key) const {
	return d->children.value(key, NodePtr{});
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::Node::insertChild(const TKey &key, Node child) {
	child.d->parent = d.toWeakRef();
	d->children.insert(key, child.d);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::emplaceChild(const TKey &key) {
	Node child;
	child.d->parent = d.toWeakRef();
	d->children.insert(key, child.d);
	return child;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::takeChild(const TKey &key) {
	Node child{d->children.take(key)};
	child.d->parent = nullptr;
	return child;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::Node::removeChild(const TKey &key) {
	return d->children.remove(key) > 0;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::Node::clearChildren() {
	d->children.clear();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
const typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::operator[](const TKey &key) const {
	return child(key);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::operator[](const TKey &key) {
	auto dIter = d->children.find(key);
	if (dIter == d->children.end())
		dIter = d->children.insert(key, NodePtr::create(d.toWeakRef()));
	return *dIter;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
int QGenericTreeBase<TKey, TValue, TContainer>::Node::depth() const {
	const auto parent = d->parent.toStrongRef();
	return parent ? parent->depth() + 1 : 0;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QList<TKey> QGenericTreeBase<TKey, TValue, TContainer>::Node::key() const {
	const auto parent = d->parent.toStrongRef();
	if (!parent)
		return {};

	// search myself within my parent to get my key
	for (auto it = parent->children.begin(), end = parent->children.end(); it != end; ++it) {
		if (*it == d) {
			auto keyChain = parent->key();
			keyChain.append(it.key());
			return keyChain;
		}
	}

	return {};
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
TKey QGenericTreeBase<TKey, TValue, TContainer>::Node::subKey() const
{
	const auto parent = d->parent.toStrongRef();
	if (!parent)
		return {};

	// search myself within my parent to get my key
	for (auto it = parent->children.begin(), end = parent->children.end(); it != end; ++it) {
		if (*it == d)
			return it.key();
	}

	return {};
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::parent() {
	return d->parent.toStrongRef();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
const typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::parent() const {
	return d->parent.toStrongRef();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::findChild(const QList<TKey> &keys) {
	return d->find(keys, 0, d);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
const typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::findChild(const QList<TKey> &keys) const {
	return d->find(keys, 0, d);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::Node::detach()
{
	const auto parent = d->parent.toStrongRef();
	if (!parent)
		return;

	for (auto it = parent->children.begin(), end = parent->children.end(); it != end; ++it) {
		if (*it == d) {
			parent->children.erase(it);
			d->parent = nullptr;
		}
	}
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::clone() const {
	Node clone{d->clone()};
	clone.d->parent = nullptr;
	return clone;
}



template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
bool QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::operator==(const iterator_base &other) const
{
	return _node == other._node;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
bool QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::operator!=(const QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue> &other) const
{
	return _node != other._node;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
typename QGenericTreeBase<TKey, TValue, TContainer>::template iterator_base<TIterValue>::reference QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::operator*() const
{
	return *(this->_node->value);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
typename QGenericTreeBase<TKey, TValue, TContainer>::template iterator_base<TIterValue>::pointer QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::operator->() const
{
	return this->_node->value.operator->();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
typename QGenericTreeBase<TKey, TValue, TContainer>::template iterator_base<TIterValue> &QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::operator++()
{
	// first step: check for children -> if yes, advance to first child
	if (!_node->children.empty()) {
		_node = _node->children.first();
		return *this;
	}

	// second step: go back to parent and check for siblings, in a loop
	auto currentNode = _node;
	forever {
		const auto parent = currentNode->parent.toStrongRef();
		if (!parent) { // no parent & no children -> go to end
			_node.reset();
			return *this;
		}

		// search myself within my parent
		for (auto it = parent->children.begin(), end = parent->children.end(); it != end; ++it) {
			if (*it == currentNode) {
				// if next element in child list still exists -> this one is next
				if (++it != end) {
					_node = *it;
					return *this;
				} else { // I am last element -> proceed one layer up
					currentNode = parent;
					break;
				}
			}
		}
	}
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
typename QGenericTreeBase<TKey, TValue, TContainer>::template iterator_base<TIterValue> QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::operator++(int)
{
	auto copy = *this;
	operator++();
	return copy;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
typename QGenericTreeBase<TKey, TValue, TContainer>::template iterator_base<TIterValue> &QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::operator--()
{
	// first step: check if first element in parent list -> inverse iterator search
	const auto parent = _node->parent.toStrongRef();
	if (!parent) // no parent -> can't go back -> do nothing
		return *this;

	// search myself within my parent, backwards
	for (auto it = std::make_reverse_iterator(parent->children.end()), end = std::make_reverse_iterator(parent->children.begin()); it != end; ++it) {
		if (*it == _node) {
			if (--it != end) {
				// if previous element in child list still exists ->
				// walk that one down to the outermost and deepst right element possible
				_node = *it;
				while (!_node->children.empty())
					_node = _node->children.last();
				return *this;
			} else { // I am first element -> proceed one layer up -> parent is next node
				_node = parent;
				return *this;
			}
		}
	}
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
typename QGenericTreeBase<TKey, TValue, TContainer>::template iterator_base<TIterValue> QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::operator--(int)
{
	auto copy = *this;
	operator--();
	return copy;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::operator bool() const
{
	return _node && _node->value;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
bool QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::operator!() const
{
	return !_node || !_node->value;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
QList<TKey> QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::key() const
{
	return Node{this->_node}.key();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
TKey QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::subKey() const
{
	return Node{this->_node}.subKey();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
template<typename>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::node() const
{
	return Node{this->_node};
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
template<typename>
const typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::node() const
{
	return Node{this->_node};
}



template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer> QGenericTreeBase<TKey, TValue, TContainer>::makeTree(QGenericTreeBase::Node node)
{
	Q_ASSERT_X(!node.parent(), Q_FUNC_INFO, "Cannot create trees from nodes with a parent. Call clone or detach first.");
	QGenericTreeBase<TKey, TValue, TContainer> tree;
	tree._root = node;
	return tree;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::rootNode()
{
	return _root;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
const typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::rootNode() const
{
	return _root;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::contains(const QList<TKey> &key) const
{
	return static_cast<bool>(_root.findChild(key));
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
int QGenericTreeBase<TKey, TValue, TContainer>::countElements(bool valueOnly) const
{
	auto cnt = 0;
	for (auto it = begin(), max = end(); it != max; ++it) {
		if (!valueOnly || it)
			++cnt;
	}
	return cnt;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
const typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::operator[](const QList<TKey> &key) const
{
	return _root.findChild(key);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::operator[](const QList<TKey> &key)
{
	return _root.findChild(key);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::iterator QGenericTreeBase<TKey, TValue, TContainer>::begin()
{
	return _root.d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::iterator QGenericTreeBase<TKey, TValue, TContainer>::end()
{
	return NodePtr{};
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::const_iterator QGenericTreeBase<TKey, TValue, TContainer>::begin() const
{
	return _root.d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::const_iterator QGenericTreeBase<TKey, TValue, TContainer>::end() const
{
	return NodePtr{};
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::clear()
{
	_root.clearValue();
	_root.clearChildren();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer> QGenericTreeBase<TKey, TValue, TContainer>::clone() const
{
	QGenericTreeBase<TKey, TValue, TContainer> cloned;
	cloned._root = _root.clone();
	return cloned;
}



template <typename TKey, typename TValue, template<class, class> typename TContainer>
inline QGenericTreeBase<TKey, TValue, TContainer>::NodeData::NodeData(WeakNodePtr parent) :
	parent{std::move(parent)}
{}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::NodePtr QGenericTreeBase<TKey, TValue, TContainer>::NodeData::find(const QList<TKey> &keys, int index, const NodePtr &current) const {
	if (index == keys.size())
		return current;
	else {
		auto fChild = children.value(keys[index], NodePtr{});
		return fChild ? fChild->find(keys, index + 1, fChild) : fChild;
	}
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::NodePtr QGenericTreeBase<TKey, TValue, TContainer>::NodeData::clone() const {
	auto cloned = NodePtr::create(*this);
	for (auto it = cloned->children.begin(), end = cloned->children.end(); it != end; ++it) {
		*it = it->clone();
		it->parent = cloned.toWeakRef();
	}
	return cloned;
}

#endif // QGENERICTREEBASE_H
