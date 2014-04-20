/**
 * \file tag.c
 * \brief XML tag related functions
 *
 * Functions to use a XML_Tag structure.
 *
 * \author François-Xavier Balu \<fx.balu@gmail.com\>
 * \date 16 mars 2014
 */

#include <stdio.h>      /* FILE */
#include <stdlib.h>     /* malloc(), realloc(), free() */
#include <string.h>     /* strlen(), strcpy() */

#include "../log.h"     /* logError() */
#include "attribute.h"
#include "tag.h"


/**
 * \brief Create an initialized XML tag.
 * Allocate memory for a XML tag and initialize it.
 * \see initXMLTag
 *
 * \return  Created XML tag.
 */
XML_Tag* createXMLTag(void)
{
   XML_Tag* tag;

   tag = NULL;
   tag = allocXMLTag(tag);
   initXMLTag(tag);

   return tag;
}


/**
 * \brief Destroy a XML tag.
 * Free all memory allocated for a XML tag. Recursively destroy its attributes
 * to prevent memory leak.
 *
 * \param tag  Destroyed XML tag.
 */
void destroyXMLTag(XML_Tag* tag)
{
   if(tag == NULL) {
      logError("NULL tag can't be destroyed",  __FILE__ ,  __LINE__ );
   }
   else {
      resetXMLTag(tag);
      freeXMLTag(tag);
   }
}


/**
 * \brief Allocate memory for a XML tag.
 * Check if a tag is NULL, and allocate memory if so. Doesn't allocate memory if
 * the given tag isn't NULL to prevent memory leak.
 * Use initXMLTag() immediately after this.
 *
 * \param tag  Tested XML tag.
 * \return     Allocated tag, NULL if an error happened.
 */
XML_Tag* allocXMLTag(XML_Tag* tag)
{
   if(tag != NULL) {
      logError("Doesn't allocate memory for a non NULL tag to prevent memory leak",
                __FILE__ ,  __LINE__ );
      tag = NULL;
   }
   else if((tag = malloc(sizeof(XML_Tag))) == NULL) {
      logError("Can't allocate memory for XML_Tag",  __FILE__ ,  __LINE__ );
   }
   else {
      logMem(LOG_ALLOC, tag, "XML_Tag", "tag",  __FILE__ ,  __LINE__ );
   }

   return tag;
}


/**
 * \brief Free an initialized XML tag.
 * Free memory from a XML tag. If tag isn't in an initialized state, it won't
 * be freed to prevent memory leaks.
 *
 * \param tag Freed XML tag.
 */
void freeXMLTag(XML_Tag* tag)
{
   if(tag == NULL) {
      logError("Trying to free a NULL tag",  __FILE__ ,  __LINE__ );
   }
   else if((tag->name != NULL) || (tag->attr != NULL)) {
      logError("Trying to free a non initialized tag",  __FILE__ ,  __LINE__ );
   }
   else {
      logMem(LOG_FREE, tag, "XML_Tag", "tag",  __FILE__ ,  __LINE__ );
      free(tag);
   }
}


/**
 * \brief Initialize an allocated tag.
 * Set tag's members to NULL. It can't check if members are used or not, so
 * memory leaks can happen here. You should use it immediately after
 * allocXMLTag().
 *
 * \param tag  Initialized tag.
 */
void initXMLTag(XML_Tag* tag)
{
   if(tag == NULL) {
      logError("Trying to initialize a NULL tag",  __FILE__ ,  __LINE__ );
   }
   else {
      tag->name = NULL;
      tag->attr = NULL;
      tag->type = UNKNOWN;
   }
}


/**
 * \brief Reset a XML tag to its initial state.
 * Free memory allocated to tag's members and initialize it. It does work on
 * already initialized tags, but freeXMLTag() is better in this case.
 *
 * Attributes are recursively freed to prevent memory leaks. Use
 * copyXMLAttribute() to pass an attribute to another structure, such as
 * XML_Node.
 *
 * Don't use this function on an allocated tag, because free() has undefined
 * behavior on non allocated variable.
 *
 * \param tag  Reseted XML tag.
 */
void resetXMLTag(XML_Tag* tag)
{
   if(tag == NULL) {
      logError("Trying to reset a NULL tag",  __FILE__ ,  __LINE__ );
   }
   else {
      if(tag->name != NULL) {
         logMem(LOG_FREE, tag->name, "string", "tag name", __FILE__ , __LINE__ );
         free(tag->name);
      }
      if(tag->attr != NULL) {
         destroyXMLAttribute(tag->attr);
      }
      initXMLTag(tag);
   }
}


/**
 * \brief Set a tag's name.
 * Allocate memory for a \p tag 's name, and copy \p name 's content in it.
 * If \p tag already has a name, memory is reallocated instead.
 *
 * \param[in] name  Given name.
 * \param     tag   Modified tag.
 */
void setXMLTagName(const char* name, XML_Tag* tag)
{
   /* NULL tag */
   if(tag == NULL) {
      logError("Giving a name to a NULL tag",  __FILE__ ,  __LINE__ );
   }
   /* NULL tag */
   else if(name == NULL) {
      logError("Giving a NULL name to a tag",  __FILE__ ,  __LINE__ );
   }
   /* tag already has a name */
   else if(tag->name != NULL) {
      if((tag->name = realloc(tag->name, (strlen(name) + 1) * sizeof(char))) == NULL) {
         logError("Can't reallocate memory for tag's name",  __FILE__ ,  __LINE__ );
      }
      else {
         printf("   >>> realloc tag.c:184 <<<\n");
         strcpy(tag->name, name);
      }
   }
   /* tag doesn't have a name */
   else {
      if((tag->name = malloc((strlen(name) + 1) * sizeof(char))) == NULL) {
         logError("can't allocate memory for tag's name",  __FILE__ ,  __LINE__ );
      }
      else {
         logMem(LOG_ALLOC, tag->name, "string", "tag name",  __FILE__  ,  __LINE__ );
         strcpy(tag->name, name);
      }
   }
}


/**
 * \brief Add an attribute to a XML tag.
 *
 * \param attr  Added attribute.
 * \param tag   Modified Tag.
 */
void addAttributeToXMLTag(XML_Attribute* attr, XML_Tag* tag)
{
   if(tag == NULL) {
      logError("Trying to add an attribute to a NULL tag",  __FILE__ ,  __LINE__ );
   }
   else if(attr == NULL) {
      logError("Trying to add a NULL attribute to a tag",  __FILE__ ,  __LINE__ );
   }
   /* no attribute in tag */
   else if(tag->attr == NULL) {
      tag->attr = attr;
   }
   /* one or more attributes in tag */
   else {
      attr->next = tag->attr;
      tag->attr = attr;
   }
}


/**
 * \brief Delete an attribute form a XML tag.
 * Remove reference of an attribute from a XML tag, and return the deleted
 * attribute. The deleted attribute isn't freed from memory.
 *
 * \param tag  Modified tag.
 * \return     Deleted attribute.
 */
XML_Attribute* deleteAttributeFromXMLTag(XML_Tag* tag)
{
   XML_Attribute* deleted;

   deleted = NULL;
   if(tag == NULL) {
      logError("Trying to delete an attribute from a NULL tag",
                __FILE__ ,  __LINE__ );
   }
   else if(tag->attr == NULL) {
      logError("Nothing to delete in tag",  __FILE__ ,  __LINE__ );
   }
   else {
      deleted = tag->attr;
      tag->attr = deleted->next;
      deleted->next = NULL;
   }

   return deleted;
}


/**
 * \brief Read and parse a tag in a XML file.
 * Read characters in a XML file until '>' is reached, and store informations in
 * a XMLTag structure.
 *
 * \param[in] file Read XML file. Need to be opened.
 * \return         Read and parsed XML_Tag, \c NULL if an error happened.
 */
XML_Tag* readXMLTag(FILE* file)
{
   XML_Tag* tag;
   int charBuffer, i;
   char strBuffer[XML_BUFFER_LENGTH];

   /* create a tag structure where informations will be stored */
   tag = createXMLTag();

   /* pre name parsing, check the closing tag character '/' */
   i = 0;
   charBuffer = fgetc(file);
   /* ignore opening chevron '<' */
   if(charBuffer == (char)'<') {
      charBuffer = fgetc(file);
   }
   /* detect closing tag character '/' */
   if(charBuffer == (char)'/') {
      tag->type = CLOSING;
   }
   /* not a closing tag, put read character in name */
   else {
      strBuffer[0] = (char)charBuffer;
      i++;
   }

   /* get tag's name */
   charBuffer = fgetc(file);
   while((charBuffer != (int)' ') &&
         (charBuffer != (int)'>') &&
         (charBuffer != (int)'/') &&
         (charBuffer != EOF))
   {
      strBuffer[i] = (char)charBuffer;
      charBuffer = fgetc(file);
      i++;
      if(i >= XML_BUFFER_LENGTH) {
         logError("XML reading buffer strBuffer is full",  __FILE__ ,  __LINE__ );
         freeXMLTag(tag);
         return NULL;
      }
   }
   strBuffer[i] = '\0';

   /* put read name in tag structure XML_Tag */
   setXMLTagName(strBuffer, tag);

   /* check character after name */
   switch(charBuffer)
   {
      /* tag closing character '>' */
      case (int)'>':
         if(tag->type == UNKNOWN) {
            tag->type = OPENING;
         }
         break;

      /* unique tag character '/' */
      case (int)'/':
         if(tag->type == UNKNOWN) {
            tag->type = UNIQUE;
            /* check implied following '>' */
            if((charBuffer = fgetc(file)) != (int)'>') {
               logError("Badly parsed XML file.",  __FILE__ ,  __LINE__ );
               destroyXMLTag(tag);
               return NULL;
            }
         }
         else {
            logError("XML parser found a closing unique tag !",
                      __FILE__ ,  __LINE__ );
            destroyXMLTag(tag);
            return NULL;
         }
         break;

      /* attribute separation character ' ' */
      case (int)' ':
         /* tag has attribute, and can be opening or unique */
         break;

      /* End Of File character EOF, who shouldn't be here */
      case EOF:
         logError("Reached EOF while reading XML tag",  __FILE__ ,  __LINE__ );
         destroyXMLTag(tag);
         return NULL;

      /* Any other character, who shouldn't be here either */
      default:
         logError("Unknown character after tag's name.",  __FILE__ ,  __LINE__ );
         destroyXMLTag(tag);
         return NULL;
   }

   /* try reading attribute if tag isn't a closing one or a closed unique one */
   if(tag->type == UNKNOWN) {
      while(charBuffer == (int)' ') {
         addAttributeToXMLTag(readXMLAttribute(file), tag);
         charBuffer = fgetc(file);
      }
      /* check character after attributes */
      if(charBuffer == (int)'>') {
         tag->type = OPENING;
      }
      else if(charBuffer == (int)'/') {
         tag->type = UNIQUE;
         charBuffer = fgetc(file);
      }
   }

   /* check tag closing character '>' */
   if(charBuffer != (int)'>') {
      logError("Badly parsed XML file.",  __FILE__ ,  __LINE__ );
      destroyXMLTag(tag);
      return NULL;
   }

   return tag;
}


void reachNextXMLTag(FILE* file)
{
   int charBuffer;

   do {
      charBuffer = fgetc(file);
   } while((charBuffer != (char)'<') && (charBuffer != EOF));

   if(charBuffer == EOF) {
      logError("Reached End Of File while searching for next tag",
                __FILE__ ,  __LINE__ );
   }
}
