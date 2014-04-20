/**
 * \file node.c
 * \brief Node related functions
 *
 * Definitions of functions to access a Node's members.
 *
 * \author François-Xavier Balu \<fx.balu@gmail.com\>
 * \date 11 mars 2014
 */

#include <stdio.h>      /* printf() */
#include <stdlib.h>     /* malloc(), realloc(), free() */
#include <string.h>     /* strlen(), strcpy() */

#include "../log.h"     /* logError() */
#include "attribute.h"  /* XML_Attribute */
#include "tag.h"        /* XML_Tag */
#include "node.h"


/**
 * \brief Create an initialized XML node.
 * Allocate memory for a XML node and initialize it.
 *
 * \return  Created XML node.
 */
XML_Node* createXMLNode(void)
{
   XML_Node* n;

   n = NULL;
   n = allocXMLNode(n);
   initXMLNode(n);

   return n;
}


/**
 * \brief Destroy a XML node.
 * Free all memory allocated for a XML node. Recursively destroy its attributes
 * and its children to prevent memory leaks. References in parent and siblings
 * node are deleted to prevent memory violations.
 *
 * \param n  Destroyed XML node.
 */
void destroyXMLNode(XML_Node* n)
{
   if(n == NULL) {
      logError("Trying to destroy a NULL node", __FILE__, __LINE__);
   }
   else {
      /* destroy children */
      while(n->cc > 0) {
         destroyXMLNode(n->last);
      }

      /* delete reference from parent and siblings nodes */
      if(n->parent != NULL) {
         deleteXMLNodeFromParent(n);
      }

      /* destroy other members */
      if(n->name != NULL) {
         logMem(LOG_FREE, n->name, "string", "node name", __FILE__, __LINE__);
         free(n->name);
      }
      if(n->value != NULL) {
         logMem(LOG_FREE, n->value, "string", "node value", __FILE__, __LINE__);
         free(n->value);
      }
      if(n->attr != NULL) {
         destroyXMLAttribute(n->attr);
      }

      /* free node */
      logMem(LOG_FREE, n, "XML_Node", "node", __FILE__, __LINE__);
      free(n);
   }
}


XML_Node* allocXMLNode(XML_Node* n)
{
   if(n != NULL) {
      logError("Try to allocate memory for an already used node",
               __FILE__, __LINE__);
   }
   else if((n = malloc(sizeof(XML_Node))) == NULL) {
      logError("Can't allocate memory for a XML node", __FILE__, __LINE__);
   }
   else {
      logMem(LOG_ALLOC, n, "XML_Node", "node", __FILE__, __LINE__);
   }

   return n;
}


void freeXMLNode(XML_Node* n)
{
   if(n == NULL) {
      logError("Trying to free a NULL node", __FILE__, __LINE__);
   }
   else if((n->name != NULL) ||
           (n->value != NULL) ||
           (n->attr != NULL) ||
           (n->parent != NULL) ||
           (n->previous != NULL) ||
           (n->next != NULL) ||
           (n->first != NULL) ||
           (n->current != NULL) ||
           (n->last != NULL) ||
           (n->cc != 0)) {
      logError("Trying to free a non initialized node", __FILE__, __LINE__);
   }
   else {
      logMem(LOG_FREE, n, "XML_Node", "node", __FILE__, __LINE__);
      free(n);
   }
}


/**
 * \brief Initialize a node.
 * Set a Node's members to NULL for the pointers and 0 for the children count.
 *
 * \param n  Initialized Node
 */
void initXMLNode(XML_Node* n)
{
   if(n == NULL) {
      logError("Trying to initialize a NULL node", __FILE__, __LINE__);
   }
   else {
      n->name = NULL;
      n->value = NULL;
      n->attr = NULL;
      n->parent = NULL;
      n->previous = NULL;
      n->next = NULL;
      n->first = NULL;
      n->current = NULL;
      n->last = NULL;
      n->cc = 0;
   }
}



/**
 * \brief Set a node's name.
 * Allocate memory for a \p node 's name, and copy \p name 's content in it.
 * If \p node already has a name, memory is reallocated instead.
 *
 * \param[in] name  Given name.
 * \param     node   Modified node.
 */
void setXMLNodeName(const char* name, XML_Node* n)
{
   /* NULL node */
   if(n == NULL) {
      logError("Giving a name to a NULL node", __FILE__, __LINE__);
   }
   /* NULL node */
   else if(n == NULL) {
      logError("Giving a NULL name to a node", __FILE__, __LINE__);
   }
   /* node already has a name */
   else if(n->name != NULL) {
      if((n->name = realloc(n->name, (strlen(name) + 1) * sizeof(char))) == NULL) {
         logError("Can't reallocate memory for node's name", __FILE__, __LINE__);
      }
      else {
         strcpy(n->name, name);
      }
   }
   /* node doesn't have a name */
   else {
      if((n->name = malloc((strlen(name) + 1) * sizeof(char))) == NULL) {
         logError("can't allocate memory for node's name", __FILE__, __LINE__);
      }
      else {
         logMem(LOG_ALLOC, n->name, "string", "node name", __FILE__, __LINE__);
         strcpy(n->name, name);
      }
   }
}


/**
 * \brief Set a node's value.
 * Allocate memory for a \p node 's value, and copy \p value 's content in it.
 * If \p node already has a value, memory is reallocated instead.
 *
 * \param[in] value  Given value.
 * \param     node   Modified node.
 */
void setXMLNodeValue(const char* value, XML_Node* n)
{
   /* NULL node */
   if(n == NULL) {
      logError("Giving a value to a NULL node", __FILE__, __LINE__);
   }
   /* NULL node */
   else if(n == NULL) {
      logError("Giving a NULL value to a node", __FILE__, __LINE__);
   }
   /* node already has a value */
   else if(n->value != NULL) {
      if((n->value = realloc(n->value, (strlen(value) + 1) * sizeof(char))) == NULL) {
         logError("Can't reallocate memory for node's value", __FILE__, __LINE__);
      }
      else {
         strcpy(n->value, value);
      }
   }
   /* node doesn't have a value */
   else {
      if((n->value = malloc((strlen(value) + 1) * sizeof(char))) == NULL) {
         logError("can't allocate memory for node's value", __FILE__, __LINE__);
      }
      else {
         logMem(LOG_ALLOC, n->value, "string", "node value", __FILE__, __LINE__);
         strcpy(n->value, value);
      }
   }
}


/**
 * \brief Add an attribute to a XML node.
 *
 * \param attr  Added attribute.
 * \param n     Modified node.
 */
void addAttributeToXMLNode(XML_Attribute* attr, XML_Node* n)
{
   if(n == NULL) {
      logError("Trying to add an attribute to a NULL tag", __FILE__, __LINE__);
   }
   else if(n == NULL) {
      logError("Trying to add a NULL attribute to a tag", __FILE__, __LINE__);
   }
   /* no attribute in node */
   else if(n->attr == NULL) {
      n->attr = attr;
   }
   /* one or more attributes in node */
   else {
      attr->next = n->attr;
      n->attr = attr;
   }
}


/**
 * \brief Delete an attribute form a XML node.
 * Remove reference of an attribute from a XML node, and return the deleted
 * attribute. The deleted attribute isn't freed from memory.
 *
 * \param n  Modified node.
 * \return     Deleted attribute.
 */
XML_Attribute* deleteAttributeFromXMLNode(XML_Node* n)
{
   XML_Attribute* deleted;

   deleted = NULL;
   if(n == NULL) {
      logError("Trying to delete an attribute from a NULL node",
               __FILE__, __LINE__);
   }
   else if(n->attr == NULL) {
      logError("Nothing to delete in node", __FILE__, __LINE__);
   }
   else {
      deleted = n->attr;
      n->attr = deleted->next;
      deleted->next = NULL;
   }

   return deleted;
}


void addXMLNodeToParent(XML_Node* parent, XML_Node* child)
{
   if(parent == NULL) {
      logError("Trying to add child node in a NULL parent", __FILE__, __LINE__);
   }
   else if(child == NULL) {
      logError("Trying to add NULL child node in a parent", __FILE__, __LINE__);
   }
   else if(child->parent != NULL) {
      logError("Child node already has a parent", __FILE__, __LINE__);
   }
   else if((child->previous != NULL) || (child->next != NULL)) {
      logError("Child node already has siblings", __FILE__, __LINE__);
   }
   else {
      child->parent = parent;
      parent->cc++;
      /* no child in parent node */
      if(parent->last == NULL) {
         parent->first = child;
         parent->current = child;
         parent->last = child;
      }
      /* other children in parent node */
      else {
         parent->last->next = child;
         child->previous = parent->last;
         parent->last = child;
      }
   }
}


void deleteXMLNodeFromParent(XML_Node* child)
{
   if(child == NULL) {
      logError("Trying to delete a NULL node from its parent",
               __FILE__, __LINE__);
   }
   else if(child->parent == NULL) {
      logError("Node doesn't have parent and can't be deleted",
               __FILE__, __LINE__);
   }
   else {
      /* decrement parent's child count */
      (child->parent->cc)--;
      /* remove reference from parent first node */
      if(child->parent->first == child) {
         child->parent->first = child->next;
      }
      /* remove reference from parent last node */
      if(child->parent->last == child) {
         child->parent->last = child->previous;
      }
      /* remove reference from parent current node */
      if(child->parent->current == child) {
         if(child->next == NULL) {
            child->parent->current = child->previous;
         }
         else {
            child->parent->current = child->next;
         }
      }
      /* remove reference to parent */
      child->parent = NULL;
      /* remove reference from previous node */
      if(child->previous != NULL) {
         child->previous->next = child->next;
      }
      /* remove reference from next node */
      if(child->next != NULL) {
         child->next->previous = child->previous;
      }
      /* remove references to siblings */
      child->previous = NULL;
      child->next = NULL;
   }
}


void initXMLNodeFromXMLTag(XML_Node* n, XML_Tag* tag)
{
   if(n == NULL) {
      logError("Trying to initialize a NULL node", __FILE__, __LINE__);
   }
   else if(tag == NULL) {
      logError("Trying to initialize a node from a NULL tag", __FILE__, __LINE__);
   }
   else {
      initXMLNode(n);
      setXMLNodeName(tag->name, n);
      while(tag->attr != NULL) {
         addAttributeToXMLNode(deleteAttributeFromXMLTag(tag), n);
      }
   }
}


/**
 * \brief Display a node's data in a terminal
 * Display name and attributes of a node. If complete mode is chosen, this node
 * and its descendants will also be displayed.
 *
 * \param n     Displayed node.
 * \param mode  Quantity of informations displayed. 1 is normal mode, and only
 *              the given node is displayed. 2 is complete mode, and descendants
 *              are displayed too.
 */
void printXMLNode(XML_Node* n, int mode)
{
   XML_Attribute* current;
   XML_Node* child;

   if(n == NULL) {
      logError("Trying to print a NULL node", __FILE__, __LINE__);
   }
   else {
      printf("<%s", n->name);
      /* printing attributes */
      current = n->attr;
      while(current != NULL) {
         printf(" %s=\"%s\"", current->name, current->value);
         current = current->next;
      }
      /* normal display mode, without descendants */
      if(mode != 2) {
         if(n->value != NULL){
            printf("%s</%s>" ,n->value , n->name);
         }
         else{
            printf("/>");
         }

      }
      /* complete display mode, with descendants */
      else {
         if(n->value != NULL){
            printf(">%s\n" ,n->value);
         }
         else{
            printf(">\n");
         }
         child = n->first;
         while(child != NULL) {
            printXMLNode(child, 2);
            child = child->next;
         }
         printf("</%s>\n", n->name);
      }
   }
}


void readXMLNodeValue(XML_Node* n, FILE* file){
   char strBuffer[XML_BUFFER_LENGTH];
   int charBuffer;
   int i, reading;

   /* reaches first useful character */
   i = 0;
   do{
      charBuffer = fgetc(file);

      /* reached end of file, that's not good */
      if(charBuffer == EOF){
         logError("Reached EOF while reading a node's value", __FILE__, __LINE__);
         return;
      }
      /* found a tag, stop reading */
      else if((char)charBuffer == '<'){
         return;
      }
      /* found a compatible character */
      else if(((char)charBuffer >= '!') && ((char)charBuffer <= '~')){
         strBuffer[0] = charBuffer;
         i = 1;
      }
   }while(i == 0);

   /* same thing, but now spaces ' ' are read as well */
   reading = 1;
   do{
      charBuffer = fgetc(file);

      /* reached end of file, that's not good */
      if(charBuffer == EOF){
         logError("Reached EOF while reading a node's value", __FILE__, __LINE__);
         return;
      }
      /* end of value, stop reading */
      else if(((char)charBuffer == '<') ||
              ((char)charBuffer == '\n') ||
              ((char)charBuffer == '\r')){
         reading = 0;
      }
      /* found a compatible character */
      else if(((char)charBuffer >= ' ') && ((char)charBuffer <= '~')){
         strBuffer[i] = charBuffer;
         i++;
      }
   }while(reading == 1);

   /* stop reading and copy string */
   strBuffer[i] = '\0';
   setXMLNodeValue(strBuffer, n);
}
