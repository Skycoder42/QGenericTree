#include <QtTest>

#include "qunorderedtree.h"
#include "qorderedtree.h"

class QGenericTreeTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void testNodeValues();
	void testNodeChildren();
	void testNodeCildTrees();
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

void QGenericTreeTest::testNodeChildren()
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
	QCOMPARE(testNode.hasChildren(), false);
	QCOMPARE(static_cast<bool>(weakChild), false);
	QCOMPARE(!weakChild, true);
	QVERIFY(!weakChild.toNode());

	// add child via operator
	QCOMPARE(testNode.containsChild(7), false);
	QCOMPARE(static_cast<bool>(testNode[7]), true);
	QCOMPARE(!testNode[7], false);
	QCOMPARE(testNode[7].parent(), testNode);
	QCOMPARE(testNode[7].depth(), 1);
	QCOMPARE(testNode[7].subKey(), 7);
	QCOMPARE(testNode[7].key(), QList<int>{7});
	testNode.clearChildren();
	QCOMPARE(testNode.hasChildren(), false);
}

void QGenericTreeTest::testNodeCildTrees()
{
	// build a basic tree of form:
	// 0---1---3---5
	//   \-2 \-4 \-6
	QOrderedTree<int, int>::Node node0;
	QVERIFY(node0);
	auto node1 = node0[1];
	QVERIFY(node1);
	auto node2 = node0[2];
	QVERIFY(node2);
	auto node3 = node1[3];
	QVERIFY(node3);
	auto node4 = node1[4];
	QVERIFY(node4);
	auto node5 = node3[5];
	QVERIFY(node5);
	auto node6 = node3[6];
	QVERIFY(node6);

	// verify the tree structure
	QVERIFY(!node0.parent());
	QCOMPARE(node1.parent(), node0);
	QCOMPARE(node2.parent(), node0);
	QCOMPARE(node3.parent(), node1);
	QCOMPARE(node4.parent(), node1);
	QCOMPARE(node5.parent(), node3);
	QCOMPARE(node6.parent(), node3);

	// find children recursively
	QVERIFY(node0.findChild({1}));
	QVERIFY(node0.findChild({2}));
	QVERIFY(node0.findChild({1, 3}));
	QVERIFY(node0.findChild({1, 4}));
	QVERIFY(node0.findChild({1, 3, 5}));
	QVERIFY(node0.findChild({1, 3, 6}));
	QVERIFY(node1.findChild({3}));
	QVERIFY(node1.findChild({4}));
	QVERIFY(node1.findChild({3, 5}));
	QVERIFY(node1.findChild({3, 6}));
	QVERIFY(node3.findChild({5}));
	QVERIFY(node3.findChild({6}));
	QVERIFY(!node0.findChild({42}));
	QVERIFY(!node0.findChild({1, 42}));
	QVERIFY(!node0.findChild({1, 3, 42}));
	QVERIFY(!node0.findChild({1, 3, 5, 42}));

	// clone 3
	auto cloned = node3.clone();
	QVERIFY(cloned != node3);
	QCOMPARE(node3.parent(), node1);
	QVERIFY(!cloned.parent());
	QCOMPARE(cloned.containsChild(5), true);
	QCOMPARE(cloned.containsChild(6), true);
	QVERIFY(cloned[5] != node5);
	QVERIFY(cloned[6] != node6);

	// detach 3
	node3.detach();
	QVERIFY(!node3.parent());
	QCOMPARE(node5.parent(), node3);
	QCOMPARE(node6.parent(), node3);
	QCOMPARE(node1.containsChild(3), false);
	// drop node5
	node5.drop();
	QCOMPARE(node3.containsChild(5), true);
	node5 = node3[5];
}

QTEST_MAIN(QGenericTreeTest)

#include "main.moc"
