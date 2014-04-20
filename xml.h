/**
 * \file xml.h
 * \brief XML files related definitions
 *
 * Definition of a XML_File structure and functions to use it.
 *
 * \author François-Xavier Balu \<fx.balu@gmail.com\>
 * \date 13 mars 2014
 */


#ifndef XML_H_INCLUDED
#define XML_H_INCLUDED


#include "node.h"    /* XML_Node member in XML_File structure */


/**
 * \brief Execute some testing code in function
 * Lines preceded by \code #ifdef DEBUG \endcode  and ended by \code #endif
 * \endcode will be executed.
 * \note Not used right now.
 */
#define DEBUG

/**
 * \brief Buffer length for XML file reading.
 * Number of characters read by fgets() while reading a XML file.
 * Need to be at least superior or equal to 39 char in order to read the first
 * line that contains xml version and character encoding.
 * \see XML_FIRST_LINE
 */
#ifndef XML_BUFFER_LENGTH
#define XML_BUFFER_LENGTH  200
#endif /* XML_BUFFER_LENGTH */

/**
 * \brief First line of a valid XML file.
 * First node of an XML file. It contains XML version and character encoding of
 * the file. There is 38 effective characters and a END OF STRING '\\0'
 * character at the end.
 * Quotes between values are escaped for C compatibility sake.
 */
#define XML_FIRST_LINE  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"


/**
 * \brief XML file structure
 * Contains informations about a XML file.
 */
typedef struct XML_File {
   char* path;      /**< Path of the XML file */
   FILE* file;      /**< Pointer to the file */
   XML_Node* root;  /**< Root of the generated tree after parsing */
} XML_File;


XML_File* loadXMLFile(const char* path);
char* getXMLString(char* path, XML_File* xml, char* defaultValue);
int getXMLInt(char* path, XML_File* xml, int defaultValue);
int getXMLBool(char* path, XML_File* xml, int defaultValue);
double getXMLDouble(char* path, XML_File* xml, double defaultValue);


XML_File* createXMLFile(void);
void destroyXMLFile(XML_File* xml);
void resetXMLFile(XML_File* xml);
void setXMLFilePath(const char* path, XML_File* xml);
void openXMLFile(XML_File* xml);
void closeXMLFile(XML_File* xml);
int checkFirstLineXMLFile(XML_File* xml);
XML_Node* parseXMLFile(FILE* file);
char* getXMLValue(char* path, XML_File* xml);
XML_Node* getXMLNode(char* path, XML_Node* root);

#endif /* XML_H_INCLUDED */
