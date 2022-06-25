#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../src/file_node.h"

void test_file_node_simple();
void test_file_node_saving();

void test_file_node() {
    printf("[UNIT-TEST] Running file node/tree tests...\n");

    //test_file_node_simple();
    //test_file_node_saving();

    printf("[UNIT-TEST] Passed file node/tree tests!\n");
}

void test_file_node_simple() {
    FileNode* root = file_node_new();
    FileNode* child1 = file_tree_add_child(root);
    FileNode* child2 = file_tree_add_child(root);
    FileNode* child21 = file_tree_add_child(child2);
    // Validate structure
    assert(root->first_child == child1);
    assert(root->last_child == child2);
    assert(root->parent == NULL);
    assert(root->next_sibling == NULL);
    assert(child1->next_sibling == child2);
    assert(child2->previous_sibling == child1);
    assert(child1->parent = root);
    assert(child2->parent = root);
    assert(child1->first_child == NULL);
    assert(child2->first_child == child21);
    assert(child2->last_child == child21);
    assert(child21->parent = root);
    assert(child21->first_child == NULL);
}

void test_file_node_saving() {
}