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
	class ConstWeakNode;
	class WeakNode;

	class ConstNode
	{
	public:
		ConstNode(const ConstNode &other) = default;
		ConstNode &operator=(const ConstNode &other) = default;
		ConstNode(ConstNode &&other) noexcept = default;
		ConstNode &operator=(ConstNode &&other) noexcept = default;
		~ConstNode() = default;
		friend inline void swap(ConstNode &lhs, ConstNode &rhs) noexcept { lhs.d.swap(rhs.d); } // must be implemented inline because of the friend declaration

		explicit operator bool() const;
		bool operator!() const;
		bool operator==(const ConstNode &other) const;
		bool operator!=(const ConstNode &other) const;

		// value access functions
		bool hasValue() const;
		template <typename TDefault>
		TValue value(TDefault &&defaultValue = TValue{}) const;
		// value access operators
		const TValue &operator*() const;
		const TValue *operator->() const;

		// child access
		bool containsChild(const TKey &key) const;
		int childCount() const;
		bool hasChildren() const;
		QList<ConstNode> children() const;
		ConstNode child(const TKey &key) const;
		// child access operators
		ConstNode operator[](const TKey &key) const;

		// tree access
		int depth() const;
		QList<TKey> key() const;
		TKey subKey() const;
		ConstNode parent() const;
		ConstNode findChild(const QList<TKey> &keys) const;

		// other
		void detach();
		ConstNode clone() const;
		ConstWeakNode toWeakNode() const;
		void drop();

	protected:
		NodePtr d;

		inline ConstNode(NodePtr data);

	private:
		friend class QGenericTreeBase;
		friend class ConstWeakNode;
		ConstNode() = default;
	};

	class Node : public ConstNode
	{
	public:
		Node();

		Node(const Node &other) = default;
		Node &operator=(const Node &other) = default;
		Node(Node &&other) noexcept = default;
		Node &operator=(Node &&other) noexcept = default;
		~Node() = default;
		friend inline void swap(Node &lhs, Node &rhs) noexcept { lhs.d.swap(rhs.d); } // must be implemented inline because of the friend declaration

		using ConstNode::operator bool;
		using ConstNode::operator!;
		bool operator==(const Node &other) const;
		bool operator!=(const Node &other) const;

		// value access functions
		void setValue(TValue value);
		TValue takeValue();
		void clearValue();
		// value access operators
		template <typename TAssign>
		Node &operator=(TAssign &&value);
		using ConstNode::operator*;
		TValue &operator*();
		using ConstNode::operator->;
		TValue *operator->();

		// child access
		using ConstNode::children;
		QList<Node> children();
		using ConstNode::child;
		Node child(const TKey &key);
		void insertChild(const TKey &key, Node child);
		Node emplaceChild(const TKey &key);
		Node takeChild(const TKey &key);
		bool removeChild(const TKey &key);
		void clearChildren();
		// child access operators
		using ConstNode::operator[];
		Node operator[](const TKey &key);

		// tree access
		using ConstNode::parent;
		Node parent();
		using ConstNode::findChild;
		Node findChild(const QList<TKey> &keys);

		// other
		Node clone() const;
		WeakNode toWeakNode() const;

	private:
		friend class QGenericTreeBase;
		friend class WeakNode;

		inline Node(NodePtr data);
	};

	class ConstWeakNode
	{
	public:
		ConstWeakNode() = default;
		ConstWeakNode(const ConstNode &node);
		ConstWeakNode(const ConstWeakNode &other) = default;
		ConstWeakNode &operator=(const ConstWeakNode &other) = default;
		ConstWeakNode(ConstWeakNode &&other) noexcept = default;
		ConstWeakNode &operator=(ConstWeakNode &&other) noexcept = default;
		~ConstWeakNode() = default;
		friend inline void swap(ConstWeakNode &lhs, ConstWeakNode &rhs) noexcept { lhs.d.swap(rhs.d); } // must be implemented inline because of the friend declaration

		explicit operator bool() const;
		bool operator!() const;
		ConstNode toNode() const;

	protected:
		WeakNodePtr d;
	};

	class WeakNode : public ConstWeakNode
	{
	public:
		WeakNode() = default;
		WeakNode(const Node &node);
		WeakNode(const WeakNode &other) = default;
		WeakNode &operator=(const WeakNode &other) = default;
		WeakNode(WeakNode &&other) noexcept = default;
		WeakNode &operator=(WeakNode &&other) noexcept = default;
		~WeakNode() = default;
		friend inline void swap(WeakNode &lhs, WeakNode &rhs) noexcept { lhs.d.swap(rhs.d); } // must be implemented inline because of the friend declaration

		using ConstWeakNode::operator bool;
		using ConstWeakNode::operator!;
		Node toNode() const;
	};

	using iterator_category_const = std::bidirectional_iterator_tag;
	struct iterator_category_non_const : public iterator_category_const, public std::output_iterator_tag {};

	template <typename TIterValue>
	class iterator_base
	{
		friend class QGenericTreeBase;
	public:
		using value_type = TIterValue;
		using difference_type = int;
		using pointer = value_type*;
		using reference = value_type&;
		using iterator_category = std::conditional_t<std::is_const_v<value_type>, iterator_category_const, iterator_category_non_const>;

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
		template<typename SFINAE = value_type>
		std::enable_if_t<std::is_const_v<SFINAE>, ConstNode> node() const;
		template<typename SFINAE = value_type>
		std::enable_if_t<!std::is_const_v<SFINAE>, Node> node() const;

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

	ConstNode rootNode() const;
	Node rootNode();

	bool contains(const TKey &key) const;
	bool contains(const QList<TKey> &key) const;
	int countElements(bool valueOnly = false) const;
	ConstNode find(const QList<TKey> &keys) const;
	Node find(const QList<TKey> &keys);
	ConstNode operator[](const TKey &key) const;
	Node operator[](const TKey &key);
	ConstNode operator[](const QList<TKey> &key) const;
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
		int depth() const;
		QList<TKey> key() const;
	};

	Node _root;
};

// GENERIC IMPLEMENTATION

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::operator bool() const {
	return d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::operator!() const {
	return !d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::operator==(const ConstNode &other) const
{
	return d == other.d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::operator!=(const ConstNode &other) const
{
	return d != other.d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::hasValue() const {
	return d->value.has_value();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TDefault>
TValue QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::value(TDefault &&defaultValue) const {
	return d->value.value_or(std::forward<TDefault>(defaultValue));
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
const TValue &QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::operator*() const {
	return *(d->value);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
const TValue *QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::operator->() const {
	return d->value.operator->();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::containsChild(const TKey &key) const {
	return d->children.contains(key);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
int QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::childCount() const {
	return d->children.size();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::hasChildren() const {
	return !d->children.empty();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QList<typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode> QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::children() const {
	QList<ConstNode> childList;
	childList.reserve(d->children.size());
	for (const auto &child : d->children)
		childList.append(child);
	return childList;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::child(const TKey &key) const {
	return d->children.value(key, NodePtr{});
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::operator[](const TKey &key) const {
	return child(key);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
int QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::depth() const {
	return d->depth();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QList<TKey> QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::key() const {
	return d->key();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
TKey QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::subKey() const
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
typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::parent() const {
	return d->parent.toStrongRef();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::findChild(const QList<TKey> &keys) const {
	return d->find(keys, 0, d);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::detach()
{
	const auto parent = d->parent.toStrongRef();
	if (!parent)
		return;

	for (auto it = parent->children.begin(), end = parent->children.end(); it != end; ++it) {
		if (*it == d) {
			parent->children.erase(it);
			d->parent = nullptr;
			break;
		}
	}
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::clone() const {
	Node clone{d->clone()};
	clone.d->parent = nullptr;
	return clone;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::ConstWeakNode QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::toWeakNode() const
{
	return ConstWeakNode{*this};
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::drop()
{
	d.clear();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer>::ConstNode::ConstNode(QGenericTreeBase::NodePtr data) :
	d{std::move(data)}
{}




template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer>::Node::Node() :
	ConstNode{NodePtr::create()}
{}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::Node::operator==(const Node &other) const
{
	return this->d == other.d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::Node::operator!=(const Node &other) const
{
	return this->d != other.d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::Node::setValue(TValue value) {
	this->d->value = std::move(value);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
TValue QGenericTreeBase<TKey, TValue, TContainer>::Node::takeValue() {
	if (this->d->value) {
		auto tValue = *std::move(this->d->value);
		this->d->value = std::nullopt;
		return tValue;
	} else
		return {};
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::Node::clearValue() {
	this->d->value = std::nullopt;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TAssign>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node &QGenericTreeBase<TKey, TValue, TContainer>::Node::operator=(TAssign &&value) {
	this->d->value = std::forward<TAssign>(value);
	return *this;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
TValue &QGenericTreeBase<TKey, TValue, TContainer>::Node::operator*() {
	if (!this->d->value.has_value())
		return this->d->value.emplace();
	else
		return *(this->d->value);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
TValue *QGenericTreeBase<TKey, TValue, TContainer>::Node::operator->() {
	return this->d->value.operator->();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QList<typename QGenericTreeBase<TKey, TValue, TContainer>::Node> QGenericTreeBase<TKey, TValue, TContainer>::Node::children() {
	QList<Node> childList;
	childList.reserve(this->d->children.size());
	for (const auto &child : this->d->children)
		childList.append(child);
	return childList;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::child(const TKey &key) {
	return this->d->children.value(key, NodePtr{});
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::Node::insertChild(const TKey &key, Node child) {
	child.d->parent = this->d.toWeakRef();
	this->d->children.insert(key, child.d);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::emplaceChild(const TKey &key) {
	Node child;
	child.d->parent = this->d.toWeakRef();
	this->d->children.insert(key, child.d);
	return child;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::takeChild(const TKey &key) {
	Node child{this->d->children.take(key)};
	if (child.d)
		child.d->parent = nullptr;
	return child;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::Node::removeChild(const TKey &key) {
	return this->d->children.remove(key) > 0;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
void QGenericTreeBase<TKey, TValue, TContainer>::Node::clearChildren() {
	this->d->children.clear();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::operator[](const TKey &key) {
	auto dIter = this->d->children.find(key);
	if (dIter == this->d->children.end())
		dIter = this->d->children.insert(key, NodePtr::create(this->d.toWeakRef()));
	return *dIter;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::parent() {
	return this->d->parent.toStrongRef();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::findChild(const QList<TKey> &keys) {
	return this->d->find(keys, 0, this->d);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::clone() const {
	Node clone{this->d->clone()};
	clone.d->parent = nullptr;
	return clone;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::WeakNode QGenericTreeBase<TKey, TValue, TContainer>::Node::toWeakNode() const
{
	return WeakNode{*this};
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer>::Node::Node(QGenericTreeBase::NodePtr data) :
	ConstNode{std::move(data)}
{}



template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer>::ConstWeakNode::ConstWeakNode(const ConstNode &node) :
	d{node.d}
{}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer>::ConstWeakNode::operator bool() const
{
	return this->d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::ConstWeakNode::operator!() const
{
	return !this->d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode QGenericTreeBase<TKey, TValue, TContainer>::ConstWeakNode::toNode() const
{
	return Node{this->d.toStrongRef()};
}



template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer>::WeakNode::WeakNode(const Node &node) :
	ConstWeakNode{node}
{}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::WeakNode::toNode() const
{
	return Node{this->d.toStrongRef()};
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
	// first step: check if at root node -> cant advance over end
	if (!_node->parent)
		return *this;

	// second step: check for children -> if yes, advance to first child
	if (!_node->children.empty()) {
		_node = _node->children.first();
		return *this;
	}

	// third step: go back to parent and check for siblings, in a loop
	forever {
		const auto parent = _node->parent.toStrongRef();
		if (!parent) // no parent -> at root node -> cant advance over end
			return *this;

		// search myself within my parent
		for (auto it = parent->children.begin(), end = parent->children.end(); it != end; ++it) {
			if (*it == _node) {
				// if next element in child list still exists -> this one is next
				if (++it != end) {
					_node = *it;
					return *this;
				} else { // I am last element -> proceed one layer up
					_node = parent;
					break;
				}
			}
		}

		Q_ASSERT(_node == parent);
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
	// first step: get parent -> no parent means at end -> walk to last valid element
	const auto parent = _node->parent.toStrongRef();
	if (!parent) {
		// walk that one down to the outermost and deepest right element possible
		while (!_node->children.empty())
			_node = _node->children.last();
		return *this;
	}

	// second step: find self in parent list
	for (auto it = parent->children.begin(), end = parent->children.end(); it != end; ++it) {
		if (*it == _node) {
			if (it != parent->children.begin()) {
				// if previous element in child list still exists ->
				// walk that one down to the outermost and deepst right element possible
				_node = *--it;
				while (!_node->children.empty())
					_node = _node->children.last();
				return *this;
			} else { // I am first element -> proceed one layer up -> parent is next node
				if (parent->parent) // parent has at parent -> not at begin
					_node = parent;
				// else: is at beginnig, can't go back
				return *this;
			}
		}
	}

	Q_UNREACHABLE();
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
template<typename SFINAE>
std::enable_if_t<std::is_const_v<SFINAE>, typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode> QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::node() const
{
	return ConstNode{this->_node};
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template <typename TIterValue>
template<typename SFINAE>
std::enable_if_t<!std::is_const_v<SFINAE>, typename QGenericTreeBase<TKey, TValue, TContainer>::Node> QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::node() const
{
	return Node{this->_node};
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
template<typename TIterValue>
QGenericTreeBase<TKey, TValue, TContainer>::iterator_base<TIterValue>::iterator_base(NodePtr data) :
	_node{data}
{}



template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer> QGenericTreeBase<TKey, TValue, TContainer>::makeTree(QGenericTreeBase::Node node)
{
	Q_ASSERT_X(!node.parent(), Q_FUNC_INFO, "Cannot create trees from nodes with a parent. Call clone or detach first.");
	QGenericTreeBase<TKey, TValue, TContainer> tree;
	tree._root = node;
	return tree;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode QGenericTreeBase<TKey, TValue, TContainer>::rootNode() const
{
	return _root;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::rootNode()
{
	return _root;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::contains(const QList<TKey> &key) const
{
	return static_cast<bool>(_root.findChild(key));
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
bool QGenericTreeBase<TKey, TValue, TContainer>::contains(const TKey &key) const
{
	return _root.containsChild(key);
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
typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode QGenericTreeBase<TKey, TValue, TContainer>::find(const QList<TKey> &keys) const
{
	return _root.findChild(keys);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::find(const QList<TKey> &keys)
{
	return _root.findChild(keys);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode QGenericTreeBase<TKey, TValue, TContainer>::operator[](const TKey &key) const
{
	return _root[key];
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::operator[](const TKey &key)
{
	return _root[key];
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::ConstNode QGenericTreeBase<TKey, TValue, TContainer>::operator[](const QList<TKey> &key) const
{
	return _root->findChild(key);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::operator[](const QList<TKey> &key)
{
	auto cNode = _root;
	for (const auto &subKey : key)
		cNode = cNode[subKey];
	return cNode;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::iterator QGenericTreeBase<TKey, TValue, TContainer>::begin()
{
	return _root.hasChildren() ?
				_root.d->children.first() :
				_root.d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::iterator QGenericTreeBase<TKey, TValue, TContainer>::end()
{
	return _root.d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::const_iterator QGenericTreeBase<TKey, TValue, TContainer>::begin() const
{
	return _root.hasChildren() ?
				_root.d->children.first() :
				_root.d;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::const_iterator QGenericTreeBase<TKey, TValue, TContainer>::end() const
{
	return _root.d;
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
		*it = (*it)->clone();
		(*it)->parent = cloned.toWeakRef();
	}
	return cloned;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
int QGenericTreeBase<TKey, TValue, TContainer>::NodeData::depth() const
{
	const auto strParent = parent.toStrongRef();
	return strParent ? strParent->depth() + 1 : 0;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QList<TKey> QGenericTreeBase<TKey, TValue, TContainer>::NodeData::key() const
{
	const auto strParent = parent.toStrongRef();
	if (!strParent)
		return {};

	// search myself within my parent to get my key
	for (auto it = strParent->children.begin(), end = strParent->children.end(); it != end; ++it) {
		if (*it == this) {
			auto keyChain = strParent->key();
			keyChain.append(it.key());
			return keyChain;
		}
	}

	return {};
}

#endif // QGENERICTREEBASE_H
