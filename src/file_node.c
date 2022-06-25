#include "file_node.h"

static FileNode* file_tree_linearize(FileNode* root, size_t tree_node_count);
static void file_tree_linearize_recurse(FileNode* node, int* index,
                                        FileNode** node_pointers);
static FileNode* file_tree_remap_tree(FileNode* linear_mem, size_t tree_node_count);

// Create a new file tree, representing a filesystem with sizes
FileNode* file_node_new() {
    return calloc(1, sizeof(FileNode));
}

// Free the entire tree below, starting with this root node
void file_node_free_all(FileNode* node) {
    if (node->next_sibling) {
        file_node_free_all(node->next_sibling);
    }
    if (node->first_child) {
        file_node_free_all(node->first_child);
    }
    free(node);
}

// Add a child to the parent node.
FileNode* file_tree_add_child(FileNode* parent) {
    FileNode* node = file_node_new();
    if (parent->first_child == NULL) {
        parent->first_child = node;
        parent->last_child = node;
    }
    else {
        parent->last_child->next_sibling = node;
        node->previous_sibling = parent->last_child;
        parent->last_child = node;
    }
    node->parent = parent;
    return node;
}

// Save the file tree to file (used as cache)
void file_tree_save(FileNode* root, char* filename) {
    size_t tree_size = file_tree_count_nodes(root);
    FileNode* linear_mem = file_tree_linearize(root, tree_size);

    FILE* f = fopen(filename, "wb");
    fwrite(linear_mem, sizeof(FileNode), tree_size + 1, f);
    fclose(f);
    free(linear_mem);
}

// Convert the dynamic tree into a linear array, remap the pointers to indices
static FileNode* file_tree_linearize(FileNode* root, size_t tree_node_count) {
    tree_node_count += 1;
    // We start out creating a linear array to pointers, then we can copy the data properly afterwards
    FileNode** node_pointers = calloc(tree_node_count, sizeof(FileNode*));
    int index = 1;
    file_tree_linearize_recurse(root, &index, node_pointers);

    // Convert the array of pointers into actual
    FileNode* linear_mem = calloc(tree_node_count, sizeof(FileNode));
    for (size_t i = 1; i < tree_node_count; i++) {
        linear_mem[i] = *node_pointers[i];
        FileNode* new_node = &linear_mem[i];

        // Remap every pointer to the corresponding index
        if (new_node->first_child) {
            new_node->first_child = (FileNode*) new_node->first_child->_linear_index;
        }
        if (new_node->last_child) {
            new_node->last_child = (FileNode*) new_node->last_child->_linear_index;
        }
        if (new_node->next_sibling) {
            new_node->next_sibling = (FileNode*) new_node->next_sibling->_linear_index;
        }
        if (new_node->previous_sibling) {
            new_node->previous_sibling =
                (FileNode*) new_node->previous_sibling->_linear_index;
        }
        if (new_node->parent) {
            new_node->parent = (FileNode*) new_node->parent->_linear_index;
        }
    }
    free(node_pointers);

    return linear_mem;
}

// Count the amount of nodes in the tree, ie all decendents and neighbours of node
size_t file_tree_count_nodes(FileNode* node) {
    size_t total = 0;
    while (node) {
        if (node->first_child) {
            total += file_tree_count_nodes(node->first_child);
        }
        total += 1;
        node = node->next_sibling;
    }
    return total;
}

// Create a linear array of all the pointers to the nodes in the tree
static void file_tree_linearize_recurse(FileNode* node, int* index,
                                        FileNode** node_pointers) {
    // Every pointer needs to be mapped to an index, then copied to the array
    while (node) {
        if (node->first_child) {
            file_tree_linearize_recurse(node->first_child, index, node_pointers);
        }
        // Copy a pointer to this node into the position of the new node index
        node->_linear_index = *index;
        node_pointers[*index] = node;
        *index = *index + 1;
        node = node->next_sibling;
    }
}

// Remap index in an array to refer to a dynamic tree
static FileNode* file_tree_remap_tree(FileNode* linear_mem, size_t tree_node_count) {
    FileNode** nodes = calloc(tree_node_count, sizeof(FileNode*));
    // Allocate the nodes
    for (size_t i = 1; i < tree_node_count; i++) {
        FileNode node = linear_mem[i];
        nodes[i] = calloc(1, sizeof(FileNode));
        *nodes[i] = node;
    }
    // Remap the indices to pointers
    FileNode* root = nodes[tree_node_count - 1];
    for (size_t i = 1; i < tree_node_count; i++) {
        FileNode* node = nodes[i];
        if (!node->parent && !node->previous_sibling) {
            // Must be root
            root = node;
        }
        if (node->first_child) {
            node->first_child = nodes[(size_t) node->first_child];
        }
        if (node->last_child) {
            node->last_child = nodes[(size_t) node->last_child];
        }
        if (node->next_sibling) {
            node->next_sibling = nodes[(size_t) node->next_sibling];
        }
        if (node->previous_sibling) {
            node->previous_sibling = nodes[(size_t) node->previous_sibling];
        }
        if (node->parent) {
            node->parent = nodes[(size_t) node->parent];
        }
    }
    free(nodes);
    return root;
}

// Load the file tree from file (used as cache)
FileNode* file_tree_load(char* filename) {
    size_t file_bytes;
    // Load tree from file into linear mem
    FileNode* linear_mem = load_file(filename, &file_bytes);
    size_t tree_node_count = file_bytes / sizeof(FileNode);
    // Remap linear mem into dynamic tree
    FileNode* root = file_tree_remap_tree(linear_mem, tree_node_count);
    free(linear_mem);
    return root;
}