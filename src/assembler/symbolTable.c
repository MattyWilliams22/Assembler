#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symbolTable.h"

/**
 * Adds a new node to the Symbol Table.
 *
 * @param table The symbol table to which to add the new node
 * @param key The key of the new node
 * @param value The value of the new node
 */
void add_to_table(Symbol_Table *table, Key key, Value value) {
  Node_t *new_node = make_list_node(key, value, 0, NULL);
  if (table->head == NULL) {
    table->head = new_node;
  } else {
    Node_t *current = table->head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = new_node;
  }
}

/**
 * Checks if a given key exists within the symbol table.
 *
 * @param table The symbol table in question
 * @param key The key that is to be checked if it exists within the table
 * @return True iff the given keys exists within the symbol table, false otherwise
 */
bool exists_in_table(Symbol_Table *table, Key key) {
  if (table == NULL) return false;
  Node_t *current = table->head;
  while (current != NULL) {
    if (strcmp(current->label, key) == 0) {
      return true;
    }
    current = current->next;
  }
  return false;
}

/**
 * Removes a node from the symbol table.
 *
 * @param table The symbol table from which to remove the node
 * @param key The key of the node that is to be removed
 */
void remove_from_table(Symbol_Table *table, Key key) {
  Node_t *current = table->head;
  Node_t *previous = NULL;

  while (current != NULL) {
    if (strcmp(current->label, key) == 0) {
      if (previous == NULL) {
        table->head = current->next;
      } else {
        previous->next = current->next;
      }
      free(current);
      return;
    }
    previous = current;
    current = current->next;
  }
}

/**
 * Finds the node with the given key in the symbol table.
 *
 * @param table The symbol table in which the node is to be found
 * @param key The key of the node that is to be found within the table
 * @return The node with the given key value if it exists, else NULL
 */
Node_t *find_in_table(Symbol_Table *table, Key key) {
  Node_t *current = table->head;
  while (current != NULL) {
    if (strcmp(current->label, key) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

/**
 * Frees the dynamically allocated symbol table.
 *
 * @param table The symbol table to free
 */
void free_table(Symbol_Table *table) {
  Node_t *current = table->head;
  Node_t *next = NULL;
  while (current != NULL) {
    next = current->next;
    free_list_node(current);
    current = next;
  }
  table->head = NULL;
  free(table);
}

/**
 * Sets the address of a single dependency.
 *
 * @param node 
 * @param index
 * @param lines
 */
void set_address(Node_t node, int index, token_line *lines) {
  int dependency_line = node.dependencies[index];
  int offset = (node.address - dependency_line);
  operand *dependent_operand = &lines[dependency_line].operands[lines[dependency_line].operand_count - 1];
  sprintf(dependent_operand->word, "%d", offset);
}

/**
 * Sets the address of all dependencies of a node.
 *
 * @param table 
 * @param node
 * @param lines
 */
void set_addresses(Symbol_Table *table, Node_t node, token_line *lines) {
  for (int i = 0; i < node.no_dependencies; i++) {
    set_address(node, i, lines);
  }
}

/**
 * Returns pointer to a new Node_t with the given values.
 *
 * @param label The key of the new node
 * @param address The value of the new node
 * @param no_dependencies
 * @param next The next node after the new node to which the new node should point to
 * @return A pointer to a new node with the given key and value
 */
Node_t *make_list_node(Key label, Value address, int no_dependencies, Node_t *next) {
  Node_t *new_node = malloc(sizeof(*new_node));
  if (new_node == NULL) {
    perror("Error allocating memory to new_node in make_list_node");
    return NULL;
  }
  new_node->label = strdup(label);
  new_node->address = address;
  new_node->no_dependencies = no_dependencies;
  new_node->next = next;
  new_node->dependencies = malloc(sizeof(dependency) * no_dependencies);
  if (new_node->dependencies == NULL) {
    perror("Error allocating memory to new_node->dependencies in make_list_node");
    return NULL;
  }
  return new_node;
}

/**
 * Free a given dynamically allocated node.
 *
 * @param node The node to be freed
 */
void free_list_node(Node_t *node) {
  free(node->label);
  free(node->dependencies);
  free(node);
}

/**
 * Adds a dependency to the symbol table.
 *
 * @param table
 * @param label
 * @param op
 * @param line_no
 * @param lines
 */
void add_dependency(Symbol_Table *table, Key label, operand op, int line_no, token_line *lines) {
  if (exists_in_table(table, label)) {
    Node_t *node = find_in_table(table, label);
    node->dependencies = realloc(node->dependencies, (node->no_dependencies + 1) * sizeof(int));
    node->dependencies[node->no_dependencies] = line_no;
    node->no_dependencies++;
    if (node->address != -1) {
      set_address(*node, (node->no_dependencies - 1), lines);
    }
  } else {
    Node_t *new_node = make_list_node(label, -1, 1, NULL);
    if (new_node == NULL) {
      perror("Error allocating memory to new_node in add_dependency");
      exit(EXIT_FAILURE);
    }
    new_node->dependencies[0] = line_no;
    if (table->head == NULL) {
      table->head = new_node;
    } else {
      Node_t *current = table->head;
      while (current->next != NULL) {
        current = current->next;
      }
      current->next = new_node;
    }
  }
}

/**
 * Adds an address to the symbol table.
 *
 * @param table
 * @param label
 * @param line_no
 * @param lines
 */
void add_address(Symbol_Table *table, Key label, int line_no, token_line *lines) {
  // Remove ':' from end of string
  int length = strlen(label);
  label[length - 1] = '\0';
  if (exists_in_table(table, label)) {
    Node_t *node = find_in_table(table, label);
    node->address = line_no;
    set_addresses(table, *node, lines);
  } else {
    Node_t *new_node = (Node_t *)malloc(sizeof(Node_t));
    new_node->label = strdup(label);
    new_node->address = line_no;
    new_node->no_dependencies = 0;
    new_node->next = NULL;
    if (table->head == NULL) {
      table->head = new_node;
    } else {
      Node_t *current = table->head;
      while (current->next != NULL) {
        current = current->next;
      }
      current->next = new_node;
    }
  }
}
