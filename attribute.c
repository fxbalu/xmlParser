/**
 * \file attribute.c
 * \brief XML attribute related functions
 *
 * Functions to use a XML_Tag structure.
 *
 * \author François-Xavier Balu \<fx.balu@gmail.com\>
 * \date 16 mars 2014
 */


#include <stdlib.h>        /* malloc(), realloc(), free() */
#include <stdio.h>         /* FILE, fgetc() */
#include <string.h>        /* strlen(), strcpy() */

#include "../log.h"     /* logError(), logMem() */
#include "attribute.h"


/**
 * \brief Create a XML attribute.
 * Allocate memory for a XML attribute and set its members to NULL.
 *
 * \return Created XML attribute.
 */
XML_Attribute* createXMLAttribute(void)
{
   XML_Attribute* attr;

   attr = NULL;
   attr = allocXMLAttribute(attr);
   initXMLAttribute(attr);

   return attr;
}


/**
 * \brief Destroy a XML attribute.
 * Free all memory allocated for a XML attribute, and accessible attributes
 * pointed by next member to prevent memory leaks.
 *
 * \param attr  Destroyed XML attribute.
 */
void destroyXMLAttribute(XML_Attribute* attr)
{
   if(attr == NULL) {
      logError("Trying to destroy a NULL attribute",  __FILE__ ,  __LINE__ );
   }
   else {
      if(attr->name != NULL){
         logMem(LOG_FREE, attr->name, "string", "attribute name",  __FILE__ ,  __LINE__ );
         free(attr->name);
      }
      if(attr->value != NULL){
         logMem(LOG_FREE, attr->value, "string", "attribute value",  __FILE__ ,  __LINE__ );
         free(attr->value);
      }
      if(attr->next != NULL){
         destroyXMLAttribute(attr->next);
      }
      logMem(LOG_FREE, attr, "XML_Attribute", "attribute",  __FILE__ ,  __LINE__ );
      free(attr);
   }
}


/**
 * \brief Allocate memory for a XM attribute.
 * Need more blah blah, like in allocXMLTag().
 *
 * \param attr  Tested XML attribute.
 * \return      Allocated XML attribute.
 */
XML_Attribute* allocXMLAttribute(XML_Attribute* attr)
{
   if(attr != NULL) {
      logError("Trying to allocate memory for a non NULL attribute",
                __FILE__ ,  __LINE__ );
   }
   else if((attr = malloc(sizeof(XML_Attribute))) == NULL) {
      logError("Can't allocate memory for an attribute",  __FILE__ ,  __LINE__ );
   }
   else {
      logMem(LOG_ALLOC, attr, "XML_Attribute", "attribute",  __FILE__ ,  __LINE__ );
   }

   return attr;
}


/**
 * \brief Free an initialized XML attribute.
 * Free memory from a XML attribute. If attribute isn't in an initialized state,
 * it won't be freed to prevent memory leaks.
 *
 * \param attr Freed XML attribute.
 */
void freeXMLAttribute(XML_Attribute* attr)
{
   if(attr == NULL) {
      logError("Trying to free a NULL attribute",  __FILE__ ,  __LINE__ );
   }
   else if((attr->name != NULL) ||
           (attr->value != NULL) ||
           (attr->next != NULL)) {
      logError("Trying to free a non initialized attribute",
                __FILE__ ,  __LINE__ );
   }
   else {
      logMem(LOG_FREE, attr, "XML_Attribute", "attribute",  __FILE__ ,  __LINE__ );
      free(attr);
   }
}


/**
 * \brief Initialize an allocated attribute.
 * Set attribute's members to NULL. It can't check if members are used or not,
 * so memory leaks can happen here. You should use it immediately after
 * allocXMLAttribute().
 *
 * \param attr  Initialized attribute.
 */
void initXMLAttribute(XML_Attribute* attr)
{
   if(attr == NULL) {
      logError("Trying to initialize a NULL attr",  __FILE__ ,  __LINE__ );
   }
   else {
      attr->name = NULL;
      attr->value = NULL;
      attr->next = NULL;
   }
}


/**
 * \brief Reset a XML attribute to its initial state.
 * Free memory allocated to attribute's members and initialize it. It does work
 * on already initialized attribute, but freeXMLAttribute() is better in this
 * case.
 *
 * Attributes are recursively freed to prevent memory leaks. Use
 * copyXMLAttribute() to pass an attribute to another structure, such as
 * XML_Node.
 *
 * Don't use this function on an allocated attribute, because free() has
 * undefined behavior on non allocated variable.
 *
 * \param attr  Reseted XML attribute.
 */
void resetXMLAttribute(XML_Attribute* attr)
{
   if(attr == NULL) {
      logError("Trying to reset a NULL attribute",  __FILE__ ,  __LINE__ );
   }
   else {
      if(attr->name != NULL) {
         logMem(LOG_FREE, attr->name, "string", "attribute's name",  __FILE__ ,  __LINE__ );
         free(attr->name);
      }
      if(attr->value != NULL) {
         logMem(LOG_FREE, attr->value, "string", "attribute's value",  __FILE__ ,  __LINE__ );
         free(attr->value);
      }
      if(attr->next != NULL) {
         destroyXMLAttribute(attr->next);
      }
      initXMLAttribute(attr);
   }
}


/**
 * \brief Set an attribute's name.
 * Allocate memory for a attribute's name, and copy name's content in it.
 * If attribute already has a name, memory is reallocated instead.
 *
 * \param[in] name  Given name.
 * \param     attr  Modified attribute.
 */
void setXMLAttributeName(const char* name, XML_Attribute* attr)
{
   /* NULL attribute */
   if(attr == NULL) {
      logError("Giving a name to a NULL attribute",  __FILE__ ,  __LINE__ );
   }
   /* NULL name */
   else if(name == NULL) {
      logError("Giving a NULL name to an attribute",  __FILE__ ,  __LINE__ );
   }
   /* attribute already has a name, reallocate space */
   else if(attr->name != NULL) {
      if((attr->name = realloc(attr->name, (strlen(name) + 1) * sizeof(char))) == NULL) {
         logError("Can't reallocate memory for attribute's name",  __FILE__ ,  __LINE__ );
      }
      else {
         printf("   >>> realloc attribute.c:199 <<<\n");
         strcpy(attr->name, name);
      }
   }
   /* attribute doesn't have a name */
   else {
      if((attr->name = malloc((strlen(name) + 1) * sizeof(char))) == NULL) {
         logError("can't allocate memory for attribute's name",  __FILE__ ,  __LINE__ );
      }
      else {
         logMem(LOG_ALLOC, attr->name, "string", "attribute name", __FILE__ , __LINE__ );
         strcpy(attr->name, name);
      }
   }
}


/**
 * \brief Set an attribute's value.
 * Allocate memory for a attribute's value, and copy value's content in it.
 * If attribute already has a value, memory is reallocated instead.
 *
 * \param[in] value  Given value.
 * \param     attr  Modified attribute.
 */
void setXMLAttributeValue(const char* value, XML_Attribute* attr)
{
   /* NULL attribute */
   if(attr == NULL) {
      logError("Giving a value to a NULL attribute",  __FILE__ ,  __LINE__ );
   }
   /* NULL value */
   else if(value == NULL) {
      logError("Giving a NULL value to an attribute",  __FILE__ ,  __LINE__ );
   }
   /* attribute already has a value, reallocate space */
   else if(attr->value != NULL) {
      if((attr->value = realloc(attr->value, (strlen(value) + 1) * sizeof(char))) == NULL) {
         logError("Can't reallocate memory for attribute's value",  __FILE__ ,  __LINE__ );
      }
      else {
         strcpy(attr->value, value);
         printf("   >>> realloc attribute.c:241 <<<\n");
      }
   }
   /* attribute doesn't have a value */
   else {
      if((attr->value = malloc((strlen(value) + 1) * sizeof(char))) == NULL) {
         logError("can't allocate memory for attribute's value",  __FILE__ ,  __LINE__ );
      }
      else {
         logMem(LOG_ALLOC, attr->value, "string", "attribute value",  __FILE__ ,  __LINE__ );
         strcpy(attr->value, value);
      }
   }
}


/**
 * \brief Read a tag attribute in a XML file.
 *
 * \param file  Read XML file.
 * \return      Read tag's attribute.
 *
 * \todo Secure this function from buffer overflow with strBuffer.
 * \todo Secure this function from reading even when EOF is reached.
 */
XML_Attribute* readXMLAttribute(FILE* file)
{
   XML_Attribute* attr;
   char strBuffer[XML_BUFFER_LENGTH];
   int charBuffer, i;

   attr = createXMLAttribute();

   /* read attribute's name */
   i = 0;
   charBuffer = fgetc(file);
   while(charBuffer != (int)'=') {

      strBuffer[i] = (char)charBuffer;
      i++;
      charBuffer = fgetc(file);
   }
   strBuffer[i] = '\0';

   /* check implied following character '"' */
   if(fgetc(file) != (int)'"') {
      logError("Badly parsed XML file.",  __FILE__ ,  __LINE__ );
      freeXMLAttribute(attr);
      return NULL;
   }

   /* set attribute's name with read string */
   setXMLAttributeName(strBuffer, attr);

   /* read attribute's value */
   i = 0;
   charBuffer = fgetc(file);
   while(charBuffer != (int)'"') {
      strBuffer[i] = (char)charBuffer;
      i++;
      charBuffer = fgetc(file);
   }
   strBuffer[i] = '\0';

   /* set attribute's value with read string */
   setXMLAttributeValue(strBuffer, attr);

   return attr;
}


void copyXMLAttribute(XML_Attribute* dst, XML_Attribute* src)
{
   setXMLAttributeName(src->name, dst);
   setXMLAttributeValue(src->value, dst);
}
