#include <cstdio>
#include <set>
#include <random>
#include <gtest/gtest.h>
#include <stack>
#include "../data_structures/trees/avl.h"

// Ignore the warning about statement expressions in macros (the 'container_of' macro)
#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-statement-expression-from-macro-expansion"
#pragma clang diagnostic ignored "-Wlanguage-extension-token"

#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type, member) );})

struct Data {
    AVLNode node{};
    uint32_t val = 0;
};

struct Container {
    AVLNode *root = nullptr;
};

static void add(Container &c, uint32_t val) {
    Data *data = new Data();
    avl_init(&data->node);
    data->val = val;

    AVLNode *cur = nullptr;
    AVLNode **from = &c.root;
    while (*from) {
        cur = *from;
        uint32_t node_val = container_of(cur, Data, node)->val;
        from = (val < node_val) ? &cur->left : &cur->right;
    }
    *from = &data->node;        // attach the new node
    data->node.parent = cur;
    c.root = avl_fix(&data->node);
}

static bool del(Container &c, uint32_t val) {
    AVLNode *cur = c.root;
    while (cur) {
        uint32_t node_val = container_of(cur, Data, node)->val;
        if (val == node_val) break;
        cur = val < node_val ? cur->left : cur->right;
    }
    if (!cur) return false;

    c.root = avl_del(cur);
    delete container_of(cur, Data, node);
    return true;
}

static void avl_verify(AVLNode *root) {
    std::stack<std::pair<AVLNode *, uint32_t>> stack;
    AVLNode *node = root;
    uint32_t depth = 0;
    uint32_t max_depth = 0;

    while (!stack.empty() || node) {
        if (node) {
            stack.emplace(node, ++depth);
            max_depth = std::max(max_depth, depth);

            if (node->left) {
                if (node->left->parent != node)
                    FAIL() << "Parent mismatch in left child.";
                if (container_of(node->left, Data, node)->val > container_of(node, Data, node)->val)
                    FAIL() << "Left child's value is greater than parent's value.";
            }
            node = node->left;
        } else {
            auto p = stack.top();
            stack.pop();
            node = p.first->right;
            depth = p.second;
        }
    }

    ASSERT_EQ(max_depth, avl_height(root)) << "Tree height does not match.";
}

static void extract(AVLNode *root, std::multiset<uint32_t> &extracted) {
    std::stack<AVLNode *> stack;
    AVLNode *node = root;

    while (!stack.empty() || node) {
        if (node) {
            stack.push(node);
            node = node->left;
        } else {
            node = stack.top();
            stack.pop();
            extracted.insert(container_of(node, Data, node)->val);
            node = node->right;
        }
    }
}

static void container_verify(Container &c, const std::multiset<uint32_t> &ref) {
    avl_verify(c.root);
    ASSERT_EQ(avl_count(c.root), ref.size());
    std::multiset<uint32_t> extracted;
    extract(c.root, extracted);
    ASSERT_EQ(extracted, ref);
}

static void dispose(Container &c) {
    while (c.root) {
        AVLNode *node = c.root;
        c.root = avl_del(c.root);
        delete container_of(node, Data, node);
    }
}

static void test_insert(uint32_t sz) {
    for (uint32_t val = 0; val < sz; ++val) {
        Container c;
        std::multiset<uint32_t> ref;
        for (uint32_t i = 0; i < sz; ++i) {
            if (i == val) continue;
            add(c, i);
            ref.insert(i);
        }
        container_verify(c, ref);

        add(c, val);
        ref.insert(val);
        container_verify(c, ref);
        dispose(c);
    }
}

static void test_insert_dup(uint32_t sz) {
    for (uint32_t val = 0; val < sz; ++val) {
        Container c;
        std::multiset<uint32_t> ref;
        for (uint32_t i = 0; i < sz; ++i) {
            add(c, i);
            ref.insert(i);
        }
        container_verify(c, ref);

        add(c, val);
        ref.insert(val);
        container_verify(c, ref);
        dispose(c);
    }
}

static void test_remove(uint32_t sz) {
    for (uint32_t val = 0; val < sz; ++val) {
        Container c;
        std::multiset<uint32_t> ref;
        for (uint32_t i = 0; i < sz; ++i) {
            add(c, i);
            ref.insert(i);
        }
        container_verify(c, ref);

        ASSERT_TRUE(del(c, val));
        ref.erase(val);
        container_verify(c, ref);
        dispose(c);
    }
}

class AVLTreeTest : public ::testing::Test {
protected:
    Container c_{};
    std::multiset<uint32_t> ref_{};

    void SetUp() override {
        ref_.clear();
    }

    void TearDown() override {
        dispose(c_);
    }
};

TEST_F(AVLTreeTest, QuickTests) {
    container_verify(c_, {});
    add(c_, 123);
    container_verify(c_, {123});
    ASSERT_FALSE(del(c_, 124));
    ASSERT_TRUE(del(c_, 123));
    container_verify(c_, {});
}

TEST_F(AVLTreeTest, SequentialInsertion) {
    for (uint32_t i = 0; i < 1000; i += 3) {
        add(c_, i);
        ref_.insert(i);
        container_verify(c_, ref_);
    }
}

TEST_F(AVLTreeTest, RandomInsertion) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint32_t> dist(0, 999);

    for (uint32_t i = 0; i < 100; ++i) {
        uint32_t val = dist(rng);
        add(c_, val);
        ref_.insert(val);
        container_verify(c_, ref_);
    }
}

TEST_F(AVLTreeTest, RandomDeletion) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint32_t> dist(0, 999);
    for (uint32_t i = 0; i < 200; ++i) {
        uint32_t val = dist(rng);
        auto it = ref_.find(val);
        if (it == ref_.end()) {
            ASSERT_FALSE(del(c_, val));
        } else {
            ASSERT_TRUE(del(c_, val));
            ref_.erase(it);
        }
        container_verify(c_, ref_);
    }
}

TEST_F(AVLTreeTest, InsertionDeletionAtVariousPositions) {
    for (uint32_t i = 0; i < 200; ++i) {
        test_insert(i);
        test_insert_dup(i);
        test_remove(i);
    }
}

#if __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif
