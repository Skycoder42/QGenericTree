#include <QtTest>

#include "qunorderedtree.h"
#include "qorderedtree.h"

class QGenericTreeTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void testNodeValues();
	void testChildren();
};

void QGenericTreeTest::testNodeValues()
{
	QUnorderedTree<QString, int>::Node testNode;

	// without value
	QCOMPARE(static_cast<bool>(testNode), true);
	QCOMPARE(!testNode, false);
	QCOMPARE(testNode.hasValue(), false);
	QCOMPARE(testNode.value(4711), 4711);
	QCOMPARE(testNode.takeValue(), 0);

	// set value
	testNode.setValue(42);
	QCOMPARE(testNode.hasValue(), true);
	QCOMPARE(testNode.value(4711), 42);
	QCOMPARE(*qAsConst(testNode), 42);
	QCOMPARE(*testNode, 42);

	// take value
	QCOMPARE(testNode.takeValue(), 42);
	QCOMPARE(testNode.hasValue(), false);

	// assign value
	testNode = 7;
	QCOMPARE(testNode.hasValue(), true);
	QCOMPARE(testNode.value(4711), 7);
	QCOMPARE(*qAsConst(testNode), 7);
	QCOMPARE(*testNode, 7);

	// clear value
	testNode.clearValue();
	QCOMPARE(testNode.hasValue(), false);

	// write value
	*testNode = 13;
	QCOMPARE(testNode.hasValue(), true);
	QCOMPARE(testNode.value(4711), 13);
	QCOMPARE(*qAsConst(testNode), 13);
	QCOMPARE(*testNode, 13);
	*testNode = 3;
	QCOMPARE(testNode.hasValue(), true);
	QCOMPARE(testNode.value(4711), 3);
	QCOMPARE(*qAsConst(testNode), 3);
	QCOMPARE(*testNode, 3);

}

void QGenericTreeTest::testChildren()
{
	QUnorderedTree<int, int>::Node testNode;

	// init state
	QCOMPARE(static_cast<bool>(testNode), true);
	QCOMPARE(!testNode, false);
	QVERIFY(!testNode.parent());
	QCOMPARE(testNode.depth(), 0);
	QCOMPARE(testNode.subKey(), 0);
	QVERIFY(testNode.key().isEmpty());
	// tree state
	QCOMPARE(testNode.childCount(), 0);
	QCOMPARE(testNode.hasChildren(), false);
	QCOMPARE(testNode.containsChild(42), false);
	QVERIFY(testNode.children().isEmpty());
	QVERIFY(!qAsConst(testNode).child(42));
	QVERIFY(!testNode.child(42));
	QVERIFY(!qAsConst(testNode)[42]);
	QVERIFY(!testNode.takeChild(42));

	// add a child
	auto childNode = testNode.emplaceChild(42);
	QCOMPARE(static_cast<bool>(childNode), true);
	QCOMPARE(!childNode, false);
	QCOMPARE(childNode.parent(), testNode);
	QCOMPARE(childNode.depth(), 1);
	QCOMPARE(childNode.subKey(), 42);
	QCOMPARE(childNode.key(), QList<int>{42});
	// tree state
	QList<QUnorderedTree<int, int>::Node> childList {childNode};
	QCOMPARE(testNode.hasChildren(), true);
	QCOMPARE(testNode.containsChild(42), true);
	QCOMPARE(testNode.children(), childList);
	QCOMPARE(qAsConst(testNode).child(42), childNode);
	QCOMPARE(testNode.child(42), childNode);
	QCOMPARE(qAsConst(testNode)[42], childNode);
	QCOMPARE(testNode[42], childNode);

	// take child
	{
		auto takenChild = testNode.takeChild(42);
		QCOMPARE(takenChild, childNode);
	}
	QVERIFY(!childNode.parent());
	QCOMPARE(childNode.depth(), 0);
	QCOMPARE(childNode.subKey(), 0);
	QVERIFY(childNode.key().isEmpty());
	// tree state
	QCOMPARE(testNode.hasChildren(), false);
	QCOMPARE(testNode.containsChild(42), false);

	// insert child
	testNode.insertChild(13, childNode);
	QCOMPARE(childNode.parent(), testNode);
	QCOMPARE(childNode.depth(), 1);
	QCOMPARE(childNode.subKey(), 13);
	QCOMPARE(childNode.key(), QList<int>{13});
	// tree state
	QCOMPARE(testNode.hasChildren(), true);
	QCOMPARE(testNode.containsChild(13), true);
	QCOMPARE(testNode.children(), childList);
	QCOMPARE(qAsConst(testNode).child(13), childNode);
	QCOMPARE(testNode.child(13), childNode);
	QCOMPARE(qAsConst(testNode)[13], childNode);
	QCOMPARE(testNode[13], childNode);

	// drop all references to child, but a weak ref
	auto weakChild = childNode.toWeakNode();
	QCOMPARE(static_cast<bool>(weakChild), true);
	QCOMPARE(!weakChild, false);
	QCOMPARE(weakChild.toNode(), childNode);
	childNode.drop();
	childList.clear();
	QCOMPARE(static_cast<bool>(weakChild), true);
	QCOMPARE(!weakChild, false);
	testNode.removeChild(13);
	QCOMPARE(static_cast<bool>(weakChild), false);
	QCOMPARE(!weakChild, true);
	QVERIFY(!weakChild.toNode());

}

QTEST_MAIN(QGenericTreeTest)

#include "main.moc"
