/**
 * \file attribute.h
 * \brief XML attribute related definitions
 *
 * Definition of a XML_Attribute structure and functions to use it.
 *
 * \author François-Xavier Balu \<fx.balu@gmail.com\>
 * \date 16 mars 2014
 */


#ifndef ATTRIBUTE_H_INCLUDED
#define ATTRIBUTE_H_INCLUDED


#include <stdio.h>   /* FILE */


#ifndef XML_BUFFER_LENGTH
#define XML_BUFFER_LENGTH  200
#endif /* XML_BUFFER_LENGTH */


/**
 * \struct XML_Attribute
 * \brief A XML node's attribute.
 */
typedef struct XML_Attribute
{
   char* name;    /**< attribute's name. */
   char* value;   /**< attribute's value. */
   struct XML_Attribute* next;   /**< Next attribute. */

} XML_Attribute;


XML_Attribute* createXMLAttribute(void);
void destroyXMLAttribute(XML_Attribute* attr);

XML_Attribute* allocXMLAttribute(XML_Attribute* attr);
void freeXMLAttribute(XML_Attribute* attr);

void initXMLAttribute(XML_Attribute* attr);
void resetXMLAttribute(XML_Attribute* attr);

void setXMLAttributeName(const char* name, XML_Attribute* attr);
void setXMLAttributeValue(const char* value, XML_Attribute* attr);

XML_Attribute* readXMLAttribute(FILE* file);

void copyXMLAttribute(XML_Attribute* dst, XML_Attribute* src);


#endif /* ATTRIBUTE_H_INCLUDED */
