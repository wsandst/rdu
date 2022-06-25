#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../src/file_node.h"

void test_file_node_simple();
void test_file_node_saving();
void test_file_node_validate_tree(FileNode* root, FileNode* child1, FileNode* child2,
                                  FileNode* child21);

void test_file_node() {
    printf("[UNIT-TEST] Running file node/tree tests...\n");

    test_file_node_simple();
    test_file_node_saving();

    printf("[UNIT-TEST] Passed file node/tree tests!\n");
}

void test_file_node_simple() {
    FileNode* root = file_node_new();
    FileNode* child1 = file_tree_add_child(root);
    FileNode* child2 = file_tree_add_child(root);
    FileNode* child21 = file_tree_add_child(child2);
    test_file_node_validate_tree(root, child1, child2, child21);
    file_node_free_all(root);
}

void test_file_node_saving() {
    FileNode* root = file_node_new();
    FileNode* child1 = file_tree_add_child(root);
    FileNode* child2 = file_tree_add_child(root);
    FileNode* child21 = file_tree_add_child(child2);
    root->file_size = 10;

    file_tree_save(root, "build/test/obj/tree.dat");
    file_node_free_all(root);

    root = file_tree_load("build/test/obj/tree.dat");
    child1 = root->first_child;
    child2 = root->last_child;
    child21 = child2->first_child;

    assert(root->file_size == 10);
    test_file_node_validate_tree(root, child1, child2, child21);
    file_node_free_all(root);
}

void test_file_node_validate_tree(FileNode* root, FileNode* child1, FileNode* child2,
                                  FileNode* child21) {
    // Validate structure
    assert(root->first_child == child1);
    assert(root->last_child == child2);
    assert(root->parent == NULL);
    assert(root->next_sibling == NULL);
    assert(child1->next_sibling == child2);
    assert(child2->previous_sibling == child1);
    assert(child1->parent == root);
    assert(child2->parent == root);
    assert(child1->first_child == NULL);
    assert(child2->first_child == child21);
    assert(child2->last_child == child21);
    assert(child21->parent == child2);
    assert(child21->first_child == NULL);

    assert(file_tree_count_nodes(root) == 4);
}