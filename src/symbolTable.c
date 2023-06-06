#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"
#include "assembleutils.h"

// Adding to Symbol Table
void add_to_table(Symbol_Table *table, Key key, Value value) {
  new_node->key = key;
  new_node->value = value;
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

// Add dependency to symbol table
void add_dependency(Symbol_Table *table, Key label, operand op, int line_no) {
  dependency d;
  d.operand = op;
  d.line = line;
  if (exists_in_table(table, label)) {
    Node_t node = find_in_table(table, label);
    node->dependencies[node->no_dependencies] = d;
    node->no_dependencies++;
    if (node->address != NULL) {
      set_address(node, (node->no_dependencies - 1));
    }
  } else {
    new_node->key = key;
    new_node->value = NULL;
    new_node->dependencies[0] = d;
    new_node->no_dependencies = 1;
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

// Add address to symbol table
void add_address(Symbol_Table *table, Key label, int line_no) {
  // Remove ':' from end of string
  int length = strlen(label);
  label[length - 1] = '\0';


  if (exists_in_table(table, label)) {
    Node_t node = find_in_table(table, label);
    node->address = line_no;
    set_addresses(table, node);
  } else {
    new_node->key = key;
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

// Set address of all dependencies of a node
void set_addresses(Symbol_Table *table, Node_t node) {
  for (int i = 0; i < node->no_dependencies; i++) {
    set_address(node, i);
  }
}

// Sets address of a single dependency
void set_address(Node_t node, int dependency_no) {
  sprintf(node->dependencies[dependency_no].operand.word, "%d", (node->address - node->dependencies[dependency_no].line));
}

// Removing from Symbol Table
void remove_from_table(Symbol_Table *table, Key key) {
  Node_t *current = table->head;
  Node_t *previous = NULL;

  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
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

// Finding in Symbol Table
Node_t find_in_table(Symbol_Table *table, Key key) {
  Node_t *current = table->head;
  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      return current;
    }
    current = current->next;
  }
  return -1;
}

#include <stdbool.h>

// Freeing Symbol Table
void free_table(Symbol_Table *table) {
  Node_t *current = table->head;
  Node_t *next = NULL;
  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }
  table->head = NULL;
}

// Check existence of key in Symbol Table
bool exists_in_table(Symbol_Table *table, Key key) {
  Node_t *current = table->head;
  while (current != NULL) {
    if (strcmp(current->key, key) == 0) {
      return true;
    }
    current = current->next;
  }
  return false;
}