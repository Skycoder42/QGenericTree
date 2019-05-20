#ifndef QGENERICTREEBASE_H
#define QGENERICTREEBASE_H

#include <optional>

#include <QtCore/QSharedPointer>
#include <QtCore/QWeakPointer>

template <typename TKey, typename TValue, template<class, class> class TContainer>
class QGenericTreeBase
{
public:
	class Node {
	private:
		struct Data;
		using Container = TContainer<TKey, QSharedPointer<Data>>;

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
		QList<Node> children() const;
		Node child(const TKey &key) const;
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
		Node parent() const;
		Node findChild(const QList<TKey> &keys) const;

		// other
		Node clone() const;

	private:
		struct Data {
			inline Data(QWeakPointer<Data> parent = {});
			inline Data(const Data &) = default;
			inline Data(Data &&) noexcept = default;

			QWeakPointer<Data> parent;
			Container children;
			std::optional<TValue> value;

			QSharedPointer<Data> find(const QList<TKey> &keys, int index, const QSharedPointer<Data> &current) const;
			QSharedPointer<Data> clone() const;
		};

		friend class QGenericTreeBase;
		QSharedPointer<Data> d;

		inline Node(QSharedPointer<Data> data) :
			d{std::move(data)}
		{}
	};

	QGenericTreeBase() = default;
	QGenericTreeBase(const QGenericTreeBase &other) = delete;
	QGenericTreeBase &operator=(const QGenericTreeBase &other) = delete;
	QGenericTreeBase(QGenericTreeBase &&other) noexcept = default;
	QGenericTreeBase &operator=(QGenericTreeBase &&other) noexcept = default;

private:
	Node _root;
};

// GENERIC IMPLEMENTATION

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QGenericTreeBase<TKey, TValue, TContainer>::Node::Node() :
	d{QSharedPointer<Data>::create()}
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
QList<typename QGenericTreeBase<TKey, TValue, TContainer>::Node> QGenericTreeBase<TKey, TValue, TContainer>::Node::children() const {
	QList<Node> childList;
	childList.reserve(d->children.size());
	for (const auto &child : d->children)
		childList.append(child);
	return childList;
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::child(const TKey &key) const {
	return d->children.value(key, QSharedPointer<Data>{});
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
		dIter = d->children.insert(key, QSharedPointer<Data>::create(d.toWeakRef()));
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
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::parent() const {
	return d->parent.toStrongRef();
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::findChild(const QList<TKey> &keys) const {
	return d->find(keys, 0, d);
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
typename QGenericTreeBase<TKey, TValue, TContainer>::Node QGenericTreeBase<TKey, TValue, TContainer>::Node::clone() const {
	Node clone{d->clone()};
	clone.d->parent = nullptr;
	return clone;
}



template <typename TKey, typename TValue, template<class, class> typename TContainer>
inline QGenericTreeBase<TKey, TValue, TContainer>::Node::Data::Data(QWeakPointer<Data> parent) :
	parent{parent}
{}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QSharedPointer<typename QGenericTreeBase<TKey, TValue, TContainer>::Node::Data> QGenericTreeBase<TKey, TValue, TContainer>::Node::Data::find(const QList<TKey> &keys, int index, const QSharedPointer<Data> &current) const {
	if (index == keys.size())
		return current;
	else {
		auto fChild = children.value(keys[index], QSharedPointer<Data>{});
		return fChild ? fChild->find(keys, index + 1, fChild) : fChild;
	}
}

template <typename TKey, typename TValue, template<class, class> typename TContainer>
QSharedPointer<typename QGenericTreeBase<TKey, TValue, TContainer>::Node::Data> QGenericTreeBase<TKey, TValue, TContainer>::Node::Data::clone() const {
	auto cloned = QSharedPointer<Data>::create(*this);
	for (auto it = cloned->children.begin(), end = cloned->children.end(); it != end; ++it) {
		*it = it->clone();
		it->parent = cloned.toWeakRef();
	}
	return cloned;
}

#endif // QGENERICTREEBASE_H
