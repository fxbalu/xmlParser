/**
 * \file xml.c
 * \brief XML file related functions.
 *
 * Definitions of functions to read and parse a XML file.
 *
 * \author François-Xavier Balu \<fx.balu@gmail.com\>
 * \date 13 mars 2014
 */


#include <stdio.h>   /* printf(), fopen(), fclose(), fgets() */
#include <stdlib.h>  /* malloc(), free(), atoi(), strtod() */
#include <string.h>  /* strlen(), strcpy(), strcmp() */

#include "../log.h"  /* logError() */
#include "node.h"    /* XML_Node */
#include "xml.h"


/**
 * \brief Create an initialized XML_File.
 * Allocate memory for a XML_File and initialize it.
 *
 * \return  Created XML_File.
 */
XML_File* createXMLFile(void)
{
   XML_File* xml;

   if((xml = malloc(sizeof(XML_File))) == NULL) {
      logError("Can't allocate memory for XML_File", __FILE__, __LINE__);
   }
   else {
      logMem(LOG_ALLOC, xml, "XML_File", "xml file", __FILE__, __LINE__);
      xml->path = NULL;
      xml->file = NULL;
      xml->root = NULL;
   }

   return xml;
}


/**
 * \brief Destroy a XML_File.
 *
 * \param xml  Destroyed XML_File.
 */
void destroyXMLFile(XML_File* xml)
{
   if(xml == NULL) {
      logError("Trying to destroy a NULL XML_File", __FILE__, __LINE__);
   }
   else {
      /* free path */
      if(xml->path != NULL) {
         logMem(LOG_FREE, xml->path, "string", "xml path", __FILE__, __LINE__);
         free(xml->path);
      }
      /* close file */
      if(xml->file != NULL) {
         logMem(LOG_FREE, xml->file, "file", "xml file", __FILE__, __LINE__);
         fclose(xml->file);
      }
      /* destroy tree */
      if(xml->root != NULL) {
         destroyXMLNode(xml->root);
      }
      /* free XML_File */
      logMem(LOG_FREE, xml, "XML_File", "xml file", __FILE__, __LINE__);
      free(xml);
   }
}


/**
 * \brief Set a file path for a XML_File.
 * Allocate memory for a XML_File's path, and copy path's content in it.
 * If \p xml already has a name, memory is reallocated instead.
 *
 * \param[in] path  Given file path.
 * \param     xml   Modified XML_File.
 */
void setXMLFilePath(const char* path, XML_File* xml)
{
   /* NULL XML_File */
   if(xml == NULL) {
      logError("Giving a file path to a NULL XML_File", __FILE__, __LINE__);
   }
   /* NULL file path */
   else if(path == NULL) {
      logError("Giving a NULL file path to a XML_File", __FILE__, __LINE__);
   }
   /* XML_File already has a file path */
   else if(xml->path != NULL) {
      if((xml->path = realloc(xml->path, (strlen(path) + 1) * sizeof(char))) == NULL) {
         logError("Can't reallocate memory for file path", __FILE__, __LINE__);
      }
      else {
         strcpy(xml->path, path);
      }
   }
   /* XML_File doesn't have a file path */
   else {
      if((xml->path = malloc((strlen(path) + 1) * sizeof(char))) == NULL) {
         logError("can't allocate memory for file path", __FILE__, __LINE__);
      }
      else {
         logMem(LOG_ALLOC, xml->path, "string", "xml path", __FILE__, __LINE__);
         strcpy(xml->path, path);
      }
   }
}


void openXMLFile(XML_File* xml)
{
   if(xml == NULL) {
      logError("Can't open XML file of a NULL XML_File", __FILE__, __LINE__);
   }
   else if(xml->path == NULL) {
      logError("No path found in XML_File", __FILE__, __LINE__);
   }
   else if(xml->file != NULL) {
      logError("File already opened in XML_File", __FILE__, __LINE__);
   }
   else if((xml->file = fopen(xml->path, "r")) == NULL) {
      logError("Can't open file with XML_File's path", __FILE__, __LINE__);
   }
   else {
      logMem(LOG_ALLOC, xml->file, "file", "xml file", __FILE__, __LINE__);
   }
}


void closeXMLFile(XML_File* xml)
{
   if(xml == NULL) {
      logError("Can't close XML file of a NULL XML_File", __FILE__, __LINE__);
   }
   else if(xml->file == NULL) {
      logError("Can't close a NULL file in XML_File", __FILE__, __LINE__);
   }
   else {
      logMem(LOG_FREE, xml->file,"file", "xml file", __FILE__, __LINE__);
      fclose(xml->file);
      xml->file = NULL;
   }
}


int checkFirstLineXMLFile(XML_File* xml)
{
   char firstLine[XML_BUFFER_LENGTH];

   if(xml == NULL) {
      logError("Can't check first line of a NULL XML_File", __FILE__, __LINE__);
   }
   else if(xml->file == NULL) {
      logError("Can't read a NULL file in XML_File", __FILE__, __LINE__);
   }
   else if(fgets(firstLine, XML_BUFFER_LENGTH, xml->file) == NULL) {
      logError("Can't read first line of XML_File", __FILE__, __LINE__);
   }
   else {
      return (strcmp(firstLine, XML_FIRST_LINE) == 0);
   }

   return 0;
}


XML_Node* parseXMLFile(FILE* file)
{
   XML_Node *current, *child, *root;
   XML_Tag* tag;
   int endOfParsing;

   current = child = root = NULL;
   tag = NULL;
   endOfParsing = 0;

   /* read first tag */
   if((tag = readXMLTag(file)) == NULL) {
      logError("Nothing to parse", __FILE__, __LINE__);
      destroyXMLTag(tag);
      return NULL;
   }
   else if(tag->type == CLOSING) {
      logError("First tag is a closing tag", __FILE__, __LINE__);
      destroyXMLTag(tag);
      return NULL;
   }
   else if(tag->type == UNIQUE) {
      root = createXMLNode();
      initXMLNodeFromXMLTag(root, tag);
      destroyXMLTag(tag);
      return root;
   }
   else /* tag->type == OPENING */ {
      current = root = createXMLNode();
      initXMLNodeFromXMLTag(current, tag);
      destroyXMLTag(tag);
   }

   /* read following node's value or tags, if any */
   while(endOfParsing == 0) {
      //reachNextXMLTag(file);  // prevent parsing to read a node's value.
      readXMLNodeValue(current, file);
      tag = readXMLTag(file);

      if(tag == NULL) {
         logError("No tag remaining, and tree isn't finished",
                  __FILE__, __LINE__);
         destroyXMLNode(root);
         return NULL;
      }
      /* Tag opens a child node for current node */
      else if(tag->type == OPENING) {
         child = createXMLNode();
         initXMLNodeFromXMLTag(child, tag);
         addXMLNodeToParent(current, child);
         current = child;
      }
      else if(tag->type == UNIQUE) {
         child = createXMLNode();
         initXMLNodeFromXMLTag(child, tag);
         addXMLNodeToParent(current, child);
      }
      /* Tag close current node */
      else if(tag->type == CLOSING) {
         if(current->parent != NULL) {
            current = current->parent;
         }
         else {
            endOfParsing = 1;
         }
      }

      destroyXMLTag(tag);
   }

   if(root != current) {
      logError("Last closed node isn't root node", __FILE__, __LINE__);
      destroyXMLNode(root);
      return NULL;
   }

   return root;
}


XML_File* loadXMLFile(const char* path){
   XML_File* xml;

   if((xml = createXMLFile()) != NULL){
      setXMLFilePath(path, xml);
      openXMLFile(xml);
      checkFirstLineXMLFile(xml);
      xml->root = parseXMLFile(xml->file);
   }

   return xml;
}


/**
 * \brief Reads a value in a XML file.
 *
 * \param[in] path  Values path in the XML file.
 *                  To find a node's value, use "root/foo/bar$"
 *                  To find an attribute, use "root/foo/bar:attribute"
 * \param[in] xml   Searched XML file.
 */
char* getXMLValue(char* path, XML_File* xml){
   char strBuffer[XML_BUFFER_LENGTH];
   char charBuffer;
   char* value;
   XML_Node* n;
   XML_Attribute* attr;
   int iPath, iBuf;

   if((path == NULL) || (xml == NULL)){
      return NULL;
   }

   value = NULL;
   n = xml->root;
   attr = NULL;
   iPath = 0;

   /* reads path */
   while(value == NULL){
      iBuf = 0;
      charBuffer = path[iPath];
      while((charBuffer != '/') &&
            (charBuffer != ':') &&
            (charBuffer != '$') &&
            (charBuffer != '\0')){
         strBuffer[iBuf] = charBuffer;
         iBuf++;
         iPath++;
         charBuffer = path[iPath];
      }
      strBuffer[iBuf] = '\0';
      iPath++;

      /* found end of path */
      if(charBuffer == '\0'){
         logError("Reached end of path without ':' or '$'.", __FILE__, __LINE__);
         return NULL;
      }

      /* find child with this name */
      while((n != NULL) && (strcmp(strBuffer, n->name) != 0)){
         n = n->next;
      }
      if(n == NULL){
         logError("Didn't find a child with this name", __FILE__, __LINE__);
         return NULL;
      }

      /* found node character '/', checks child */
      if(charBuffer == '/'){
         n = n->first;
      }
      /* found value character '$', reads value */
      else if(charBuffer == '$'){
         value = n->value;
      }
      /* found attribute character ':', reads attribute */
      else if(charBuffer == ':'){

         /* read attribute's name */
         iBuf = 0;
         while(path[iPath] != '\0'){
            strBuffer[iBuf] = path[iPath];
            iBuf++;
            iPath++;
         }
         strBuffer[iBuf] = '\0';

         /* searches attribute */
         attr = n->attr;
         while((attr != NULL) && (strcmp(strBuffer, attr->name) != 0)){
            attr = attr->next;
         }
         if(attr == NULL){
            logError("Didn't find an attribute with this name", __FILE__, __LINE__);
            return NULL;
         }
         else{
            value = attr->value;
         }
      }
   }

   return value;
}

/**
 * \brief Finds a particular node in a XML tree.
 *
 * \param[in] path  Node path in the tree.
 *                  eg. "foo/bar", "foo/bar?attr=value/lel"
 * \param[in] root  Tree's root.
 * \return          A pointer to found node, NULL if such a node wasn't found.
 */
XML_Node* getXMLNode(char* path, XML_Node* root){
   char nameBuffer[XML_BUFFER_LENGTH];
   char attrBuffer[XML_BUFFER_LENGTH];
   char valueBuffer[XML_BUFFER_LENGTH];
   int iPath, iNaBuf, iAtBuf, iVaBuf;
   char charBuffer;
   XML_Node* n;
   XML_Attribute* attr;
   int nodeFound;

   /* checks parameters */
   if((path == NULL) || (root == NULL)){
      return NULL;
   }

   n = root;

   /* reads node's name in path */
   iPath = iNaBuf = 0;
   charBuffer = path[iPath];
   while((charBuffer != '/') &&
         (charBuffer != '?') &&
         (charBuffer != '\0') &&
         (iNaBuf < XML_BUFFER_LENGTH)){
      nameBuffer[iNaBuf] = charBuffer;
      iNaBuf++;
      iPath++;
      charBuffer = path[iPath];
   }

   nameBuffer[iNaBuf] = '\0';
   iPath++;

   /* reads attribute's name and value if necessary */
   if(charBuffer == '?'){

      /* reads attribute's name in path */
      iAtBuf = iVaBuf = 0;
      do{
         charBuffer = path[iPath];
         attrBuffer[iAtBuf] = charBuffer;
         iAtBuf++;
         iPath++;
      }
      while((charBuffer != '=') &&
            (charBuffer != '\0') &&
            (iNaBuf < XML_BUFFER_LENGTH));
      attrBuffer[iAtBuf] = '\0';
      iPath++;

      /* checks if attribute's name is followed by a value */
      if(charBuffer != '='){
         logError("Attribute's name is not followed by a value.", __FILE__, __LINE__);
         return NULL;
      }

      /* reads attribute's value */
      else{
         do{
            charBuffer = path[iPath];
            valueBuffer[iVaBuf] = charBuffer;
            iVaBuf++;
            iPath++;
         }
         while((charBuffer != '/') &&
               (charBuffer != '\0') &&
               (iVaBuf < XML_BUFFER_LENGTH));
         valueBuffer[iVaBuf] = '\0';
         iPath++;
      }
   }

   /* finds a matching node */
   nodeFound = 0;
   do{
      /* checks node's name */
      if(strcmp(nameBuffer, n->name) != 0){
         n = n->next;
      }
      /* found a node with this name, and no need to check attribute */
      else if(!iAtBuf){
         nodeFound = 1;
      }
      /* also checks attribute's name and value */
      else{
         attr = n->attr;
         while((attr != NULL) && (nodeFound == 0)){
            if((strcmp(attr->name, attrBuffer) == 0) &&
               (strcmp(attr->value, valueBuffer) == 0)){
               nodeFound = 1;
            }
            attr = attr->next;
         }
         /* Didn't found a matching attribute, select next node */
         if(attr == NULL){
            n = n->next;
         }
      }
   }
   while((!nodeFound) && (n != NULL));

   /* Didn't found a matching node, return NULL */
   if(!nodeFound){
      n = NULL;
   }
   /* found node character '/', checks children */
   else if(charBuffer == '/'){
      n = getXMLNode(path + iPath, n->first);
   }
   /* Didn't find end of string character '/0', return NULL  */
   else if(charBuffer != '\0'){
      n = NULL;
   }

   return n;
}

char* getXMLString(char* path, XML_File* xml, char* defaultValue){
   char* value;

   if((value = getXMLValue(path, xml)) == NULL){
      value = defaultValue;
   }

   return value;
}

int getXMLInt(char* path, XML_File* xml, int defaultValue){
   char* temp;
   int value;

   if((temp = getXMLValue(path, xml)) == NULL){
      value = defaultValue;
   }
   else{
      value  = atoi(temp);
   }

   return value;
}

int getXMLBool(char* path, XML_File* xml, int defaultValue){
   char* temp;
   int value;

   if((temp = getXMLValue(path, xml)) == NULL){
      value = defaultValue;
   }
   else{
      if(strcmp(temp, "true") == 0){
         value = 1;
      }
      else if(strcmp(temp, "false") == 0){
         value = 0;
      }
      else{
         value = defaultValue;
      }
   }

   return value;
}

double getXMLDouble(char* path, XML_File* xml, double defaultValue){
   char* temp;
   double value;

   if((temp = getXMLValue(path, xml)) == NULL){
      value = defaultValue;
   }
   else{
      value  = strtod(temp, NULL);
   }

   return value;
}
