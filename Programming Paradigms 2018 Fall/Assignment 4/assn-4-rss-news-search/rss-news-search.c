#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "url.h"
#include "bool.h"
#include "vector.h"
#include "hashset.h"
#include "urlconnection.h"
#include "streamtokenizer.h"
#include "html-utils.h"

#define PRIME_BUCKETS 1009
#define MALLOC_SIZE 1024

typedef struct indice {
  char* word;
  vector* vec; 
} indice;

typedef struct info {
  int count;
  char* article;
  char* website;
} info;

static void Welcome(const char *welcomeTextFileName);
static void BuildIndices(const char *feedsFileName, hashset* map, hashset* stopWords);
static void ProcessFeed(const char *remoteDocumentName, hashset* map, hashset* stopWords);
static void PullAllNewsItems(urlconnection *urlconn, hashset* map, hashset* stopWords);
static bool GetNextItemTag(streamtokenizer *st);
static void ProcessSingleNewsItem(streamtokenizer *st, hashset* map, hashset* stopWords);
static void ExtractElement(streamtokenizer *st, const char *htmlTag, char dataBuffer[], int bufferLength);
static void ParseArticle(const char *articleTitle, const char *articleDescription, const char *articleURL,
                         hashset* map, hashset* stopWords);
static void ScanArticle(streamtokenizer *st, const char *articleTitle, const char *unused, const char *articleURL,
                         hashset* map, hashset* stopWords);
static void QueryIndices(hashset* map, hashset* stopWords);
static void ProcessResponse(const char *word, hashset* map, hashset* stopWords);
static bool WordIsWellFormed(const char *word);

static const char *const kTextDelimiters = " \t\n\r\b!@$%^*()_+={[}]|\\'\":;/?.>,<~`";
static const char *const kWelcomeTextFile = "data/welcome.txt";
static const char *const filePrefix = "file://";
static const char *const kDefaultFeedsFile = "data/test.txt";
static const char *const stopWordsData = "data/stop-words.txt";

void initStopWords(hashset* stopWords);
int strHash(const void *v, int numBuckets);
int strCmp(const void *v1, const void* v2);
void strFree(void* v);

int indHash(const void *v, int numBuckets);
int indCmp(const void *v1, const void* v2);
void indFree(void* v);

void infoFree(void *v);
int infoCmp(const void *v1, const void* v2);
int vectorSortCmp(const void *v1, const void* v2);

void ProcessCorrectResponce(hashset* map, const char* word);
void createInfo(info* newInfo, const char *articleTitle, const char *articleURL);
void updateInfo(void* search, info* inf, const char *articleTitle, const char *articleURL);
void addNewInfo(indice* ind, hashset* map, const char *articleTitle, const char *articleURL, char* word);
void scanCorrectArticle(hashset* map, hashset* stopWords, char* word, int* numWords, char* longestWord,
                        indice* lookupable, indice* ind, info* inf, const char* articleTitle,
                        const char* articleURL);

/**
 * Function: main
 * --------------
 * Serves as the entry point of the full application.
 * You'll want to update main to declare several hashsets--
 * one for stop words, another for previously seen urls, etc--
 * and pass them (by address) to BuildIndices and QueryIndices.
 * In fact, you'll need to extend many of the prototypes of the
 * supplied helpers functions to take one or more hashset *s.
 *
 * Think very carefully about how you're going to keep track of
 * all of the stop words, how you're going to keep track of
 * all the previously seen articles, and how you're going to 
 * map words to the collection of news articles where that
 * word appears.
 */

int main(int argc, char **argv) {
  setbuf(stdout, NULL);
  Welcome(kWelcomeTextFile);

  hashset stopWords;
  HashSetNew(&stopWords, sizeof(char*), PRIME_BUCKETS, strHash, strCmp, strFree);
  initStopWords(&stopWords);

  hashset map;
  HashSetNew(&map, sizeof(indice),PRIME_BUCKETS, indHash, indCmp, indFree);

  BuildIndices((argc == 1) ? kDefaultFeedsFile : argv[1], &map, &stopWords);
  QueryIndices(&map, &stopWords);

  HashSetDispose(&stopWords);
  HashSetDispose(&map);
  return 0;
}

/** 
 * Function: Welcome
 * -----------------
 * Displays the contents of the specified file, which
 * holds the introductory remarks to be printed every time
 * the application launches.  This type of overhead may
 * seem silly, but by placing the text in an external file,
 * we can change the welcome text without forcing a recompilation and
 * build of the application.  It's as if welcomeTextFileName
 * is a configuration file that travels with the application.
 */
 
static const char *const kNewLineDelimiters = "\r\n";
static void Welcome(const char *welcomeTextFileName)
{
  FILE *infile;
  streamtokenizer st;
  char buffer[1024];
  
  infile = fopen(welcomeTextFileName, "r");
  assert(infile != NULL);    
  
  STNew(&st, infile, kNewLineDelimiters, true);
  while (STNextToken(&st, buffer, sizeof(buffer))) {
    printf("%s\n", buffer);
  }
  
  printf("\n");
  STDispose(&st); // remember that STDispose doesn't close the file, since STNew doesn't open one.. 
  fclose(infile);
}

/**
 * Function: BuildIndices
 * ----------------------
 * As far as the user is concerned, BuildIndices needs to read each and every
 * one of the feeds listed in the specied feedsFileName, and for each feed parse
 * content of all referenced articles and store the content in the hashset of indices.
 * Each line of the specified feeds file looks like this:
 *
 *   <feed name>: <URL of remore xml document>
 *
 * Each iteration of the supplied while loop parses and discards the feed name (it's
 * in the file for humans to read, but our aggregator doesn't care what the name is)
 * and then extracts the URL.  It then relies on ProcessFeed to pull the remote
 * document and index its content.
 */

static void BuildIndices(const char *feedsFileName, hashset* map, hashset* stopWords)
{
  FILE *infile;
  streamtokenizer st;
  char remoteFileName[1024];
  
  infile = fopen(feedsFileName, "r");
  //assert(infile != NULL);
  STNew(&st, infile, kNewLineDelimiters, true);
  while (STSkipUntil(&st, ":") != EOF) { // ignore everything up to the first selicolon of the line
    STSkipOver(&st, ": ");		 // now ignore the semicolon and any whitespace directly after it
    STNextToken(&st, remoteFileName, sizeof(remoteFileName));   
    ProcessFeed(remoteFileName, map, stopWords);
  }
  
  STDispose(&st);
  fclose(infile);
  printf("\n");
}

/**
 * Function: ProcessFeedFromFile
 * ---------------------
 * ProcessFeed locates the specified RSS document, from locally
 */

static void ProcessFeedFromFile(char *fileName, hashset* map, hashset* stopWords)
{
  FILE *infile;
  streamtokenizer st;
  char articleDescription[1024];
  articleDescription[0] = '\0';
  infile = fopen((const char *)fileName, "r");
  assert(infile != NULL);
  STNew(&st, infile, kTextDelimiters, true);
  ScanArticle(&st, (const char *)fileName, articleDescription, (const char *)fileName, map, stopWords);
  STDispose(&st); // remember that STDispose doesn't close the file, since STNew doesn't open one..
  fclose(infile);
}

/**
 * Function: ProcessFeed
 * ---------------------
 * ProcessFeed locates the specified RSS document, and if a (possibly redirected) connection to that remote
 * document can be established, then PullAllNewsItems is tapped to actually read the feed.  Check out the
 * documentation of the PullAllNewsItems function for more information, and inspect the documentation
 * for ParseArticle for information about what the different response codes mean.
 */

static void ProcessFeed(const char *remoteDocumentName, hashset* map, hashset* stopWords)
{

  if(!strncmp(filePrefix, remoteDocumentName, strlen(filePrefix))){
    ProcessFeedFromFile((char *)remoteDocumentName + strlen(filePrefix), map, stopWords);
    return;
  }

  url u;
  urlconnection urlconn;
  
  URLNewAbsolute(&u, remoteDocumentName);
  URLConnectionNew(&urlconn, &u);
  
  switch (urlconn.responseCode) {
      case 0: printf("Unable to connect to \"%s\".  Ignoring...", u.serverName);
              break;
      case 200: PullAllNewsItems(&urlconn, map, stopWords);
                break;
      case 301: 
      case 302: ProcessFeed(urlconn.newUrl, map, stopWords);
                break;
      default: printf("Connection to \"%s\" was established, but unable to retrieve \"%s\". [response code: %d, response message:\"%s\"]\n",
		      u.serverName, u.fileName, urlconn.responseCode, urlconn.responseMessage);
	       break;
  };
  
  URLConnectionDispose(&urlconn);
  URLDispose(&u);
}

/**
 * Function: PullAllNewsItems
 * --------------------------
 * Steps though the data of what is assumed to be an RSS feed identifying the names and
 * URLs of online news articles.  Check out "datafiles/sample-rss-feed.txt" for an idea of what an
 * RSS feed from the www.nytimes.com (or anything other server that syndicates is stories).
 *
 * PullAllNewsItems views a typical RSS feed as a sequence of "items", where each item is detailed
 * using a generalization of HTML called XML.  A typical XML fragment for a single news item will certainly
 * adhere to the format of the following example:
 *
 * <item>
 *   <title>At Installation Mass, New Pope Strikes a Tone of Openness</title>
 *   <link>http://www.nytimes.com/2005/04/24/international/worldspecial2/24cnd-pope.html</link>
 *   <description>The Mass, which drew 350,000 spectators, marked an important moment in the transformation of Benedict XVI.</description>
 *   <author>By IAN FISHER and LAURIE GOODSTEIN</author>
 *   <pubDate>Sun, 24 Apr 2005 00:00:00 EDT</pubDate>
 *   <guid isPermaLink="false">http://www.nytimes.com/2005/04/24/international/worldspecial2/24cnd-pope.html</guid>
 * </item>
 *
 * PullAllNewsItems reads and discards all characters up through the opening <item> tag (discarding the <item> tag
 * as well, because once it's read and indentified, it's been pulled,) and then hands the state of the stream to
 * ProcessSingleNewsItem, which handles the job of pulling and analyzing everything up through and including the </item>
 * tag. PullAllNewsItems processes the entire RSS feed and repeatedly advancing to the next <item> tag and then allowing
 * ProcessSingleNewsItem do process everything up until </item>.
 */

static void PullAllNewsItems(urlconnection *urlconn, hashset* map, hashset* stopWords)
{
  streamtokenizer st;
  STNew(&st, urlconn->dataStream, kTextDelimiters, false);
  while (GetNextItemTag(&st)) { // if true is returned, then assume that <item ...> has just been read and pulled from the data stream
    ProcessSingleNewsItem(&st, map, stopWords);
  }
  
  STDispose(&st);
}

/**
 * Function: GetNextItemTag
 * ------------------------
 * Works more or less like GetNextTag below, but this time
 * we're searching for an <item> tag, since that marks the
 * beginning of a block of HTML that's relevant to us.  
 * 
 * Note that each tag is compared to "<item" and not "<item>".
 * That's because the item tag, though unlikely, could include
 * attributes and perhaps look like any one of these:
 *
 *   <item>
 *   <item rdf:about="Latin America reacts to the Vatican">
 *   <item requiresPassword=true>
 *
 * We're just trying to be as general as possible without
 * going overboard.  (Note that we use strncasecmp so that
 * string comparisons are case-insensitive.  That's the case
 * throughout the entire code base.)
 */

static const char *const kItemTagPrefix = "<item";
static bool GetNextItemTag(streamtokenizer *st)
{
  char htmlTag[1024];
  while (GetNextTag(st, htmlTag, sizeof(htmlTag))) {
    if (strncasecmp(htmlTag, kItemTagPrefix, strlen(kItemTagPrefix)) == 0) {
      return true;
    }
  }	 
  return false;
}

/**
 * Function: ProcessSingleNewsItem
 * -------------------------------
 * Code which parses the contents of a single <item> node within an RSS/XML feed.
 * At the moment this function is called, we're to assume that the <item> tag was just
 * read and that the streamtokenizer is currently pointing to everything else, as with:
 *   
 *      <title>Carrie Underwood takes American Idol Crown</title>
 *      <description>Oklahoma farm girl beats out Alabama rocker Bo Bice and 100,000 other contestants to win competition.</description>
 *      <link>http://www.nytimes.com/frontpagenews/2841028302.html</link>
 *   </item>
 *
 * ProcessSingleNewsItem parses everything up through and including the </item>, storing the title, link, and article
 * description in local buffers long enough so that the online new article identified by the link can itself be parsed
 * and indexed.  We don't rely on <title>, <link>, and <description> coming in any particular order.  We do asssume that
 * the link field exists (although we can certainly proceed if the title and article descrption are missing.)  There
 * are often other tags inside an item, but we ignore them.
 */

static const char *const kItemEndTag = "</item>";
static const char *const kTitleTagPrefix = "<title";
static const char *const kDescriptionTagPrefix = "<description";
static const char *const kLinkTagPrefix = "<link";
static void ProcessSingleNewsItem(streamtokenizer *st, hashset* map, hashset* stopWords)
{
  char htmlTag[1024];
  char articleTitle[1024];
  char articleDescription[1024];
  char articleURL[1024];
  articleTitle[0] = articleDescription[0] = articleURL[0] = '\0';
  
  while (GetNextTag(st, htmlTag, sizeof(htmlTag)) && (strcasecmp(htmlTag, kItemEndTag) != 0)) {
    if (strncasecmp(htmlTag, kTitleTagPrefix, strlen(kTitleTagPrefix)) == 0) ExtractElement(st, htmlTag, articleTitle, sizeof(articleTitle));
    if (strncasecmp(htmlTag, kDescriptionTagPrefix, strlen(kDescriptionTagPrefix)) == 0) ExtractElement(st, htmlTag, articleDescription, sizeof(articleDescription));
    if (strncasecmp(htmlTag, kLinkTagPrefix, strlen(kLinkTagPrefix)) == 0) ExtractElement(st, htmlTag, articleURL, sizeof(articleURL));
  }
  
  if (strncmp(articleURL, "", sizeof(articleURL)) == 0) return;     // punt, since it's not going to take us anywhere
  ParseArticle(articleTitle, articleDescription, articleURL, map, stopWords);
}

/**
 * Function: ExtractElement
 * ------------------------
 * Potentially pulls text from the stream up through and including the matching end tag.  It assumes that
 * the most recently extracted HTML tag resides in the buffer addressed by htmlTag.  The implementation
 * populates the specified data buffer with all of the text up to but not including the opening '<' of the
 * closing tag, and then skips over all of the closing tag as irrelevant.  Assuming for illustration purposes
 * that htmlTag addresses a buffer containing "<description" followed by other text, these three scanarios are
 * handled:
 *
 *    Normal Situation:     <description>http://some.server.com/someRelativePath.html</description>
 *    Uncommon Situation:   <description></description>
 *    Uncommon Situation:   <description/>
 *
 * In each of the second and third scenarios, the document has omitted the data.  This is not uncommon
 * for the description data to be missing, so we need to cover all three scenarious (I've actually seen all three.)
 * It would be quite unusual for the title and/or link fields to be empty, but this handles those possibilities too.
 */
 
static void ExtractElement(streamtokenizer *st, const char *htmlTag, char dataBuffer[], int bufferLength)
{
  assert(htmlTag[strlen(htmlTag) - 1] == '>');
  if (htmlTag[strlen(htmlTag) - 2] == '/') return;    // e.g. <description/> would state that a description is not being supplied
  STNextTokenUsingDifferentDelimiters(st, dataBuffer, bufferLength, "<");
  RemoveEscapeCharacters(dataBuffer);
  if (dataBuffer[0] == '<') strcpy(dataBuffer, "");  // e.g. <description></description> also means there's no description
  STSkipUntil(st, ">");
  STSkipOver(st, ">");
}

/** 
 * Function: ParseArticle
 * ----------------------
 * Attempts to establish a network connect to the news article identified by the three
 * parameters.  The network connection is either established of not.  The implementation
 * is prepared to handle a subset of possible (but by far the most common) scenarios,
 * and those scenarios are categorized by response code:
 *
 *    0 means that the server in the URL doesn't even exist or couldn't be contacted.
 *    200 means that the document exists and that a connection to that very document has
 *        been established.
 *    301 means that the document has moved to a new location
 *    302 also means that the document has moved to a new location
 *    4xx and 5xx (which are covered by the default case) means that either
 *        we didn't have access to the document (403), the document didn't exist (404),
 *        or that the server failed in some undocumented way (5xx).
 *
 * The are other response codes, but for the time being we're punting on them, since
 * no others appears all that often, and it'd be tedious to be fully exhaustive in our
 * enumeration of all possibilities.
 */

static void ParseArticle(const char *articleTitle, const char *articleDescription, const char *articleURL, 
                         hashset* map, hashset* stopWords)
{
  url u;
  urlconnection urlconn;
  streamtokenizer st;

  URLNewAbsolute(&u, articleURL);
  URLConnectionNew(&urlconn, &u);

  switch (urlconn.responseCode)
  {
  case 0:
    printf("Unable to connect to \"%s\".  Domain name or IP address is nonexistent.\n", articleURL);
    break;
  case 200:
    printf("Scanning \"%s\" from \"http://%s\"\n", articleTitle, u.serverName);
    STNew(&st, urlconn.dataStream, kTextDelimiters, false);
    ScanArticle(&st, articleTitle, articleDescription, articleURL, map, stopWords);
    STDispose(&st);
    break;
  case 301:
  case 302: // just pretend we have the redirected URL all along, though index using the new URL and not the old one...
    ParseArticle(articleTitle, articleDescription, urlconn.newUrl, map, stopWords);
    break;
  default:
    printf("Unable to pull \"%s\" from \"%s\". [Response code: %d] Punting...\n", articleTitle, u.serverName, urlconn.responseCode);
    break;
  }

  URLConnectionDispose(&urlconn);
  URLDispose(&u);
}

/**
 * Function: ScanArticle
 * ---------------------
 * Parses the specified article, skipping over all HTML tags, and counts the numbers
 * of well-formed words that could potentially serve as keys in the set of indices.
 * Once the full article has been scanned, the number of well-formed words is
 * printed, and the longest well-formed word we encountered along the way
 * is printed as well.
 *
 * This is really a placeholder implementation for what will ultimately be
 * code that indexes the specified content.
 */
static void ScanArticle(streamtokenizer *st, const char *articleTitle, const char *unused, const char *articleURL,
                        hashset* map, hashset* stopWords) {
  int numWords = 0;
  char* word = malloc(MALLOC_SIZE);
  char longestWord[MALLOC_SIZE] = {'\0'};

  indice* ind = malloc(sizeof(indice));
  indice* lookupable = malloc(sizeof(indice));
  info* inf = malloc(sizeof(info));

  while (STNextToken(st, word, MALLOC_SIZE)) {
    if (strcasecmp(word, "<") == 0){
      SkipIrrelevantContent(st); // in html-utls.h
    } else {
      RemoveEscapeCharacters(word);
      //This is the function i've written for decomposition purposes
      scanCorrectArticle(map,stopWords,word,&numWords,longestWord,lookupable,ind,inf,articleTitle,articleURL);
    }
  }
  free(inf); free(word); free(lookupable); free(ind); //frees allocatd memory

  printf("\tWe counted %d well-formed words [including duplicates].\n", numWords);
  printf("\tThe longest word scanned was \"%s\".", longestWord);
  if (strlen(longestWord) >= 15 && (strchr(longestWord, '-') == NULL)) printf(" [Ooooo... long word!]");
  printf("\n");
}

//Function that searches for an article and updates info if finds it in data, else creates a new one
void scanCorrectArticle(hashset* map, hashset* stopWords, char* word, int* numWords, char* longestWord,
                        indice* lookupable, indice* ind, info* inf, const char* articleTitle,
                        const char* articleURL) {

  //Searches for a word in a stopWord hashset
  void * stopSearch = HashSetLookup(stopWords,&word);
  if(WordIsWellFormed(word)) {
    if(stopSearch != NULL) return; //If finds, we dont deal with this word and method is over, else, works with data
    *numWords += 1; 
    if(strlen(word) > strlen(longestWord))strcpy(longestWord, word);
    lookupable->word = strdup(word);

    //Searches for a correct word in map, checks if we have info related to this article
    void * search = HashSetLookup(map,lookupable);
    if(search == NULL) addNewInfo(ind, map, articleTitle, articleURL, word);
    else updateInfo(search, inf, articleTitle, articleURL);
    free(lookupable->word);
  }
}

//Creates info, for decomposition purposes
void createInfo(info* newInfo, const char *articleTitle, const char *articleURL) {
  newInfo->count = 1;
  newInfo->article = strdup(articleTitle);
  newInfo->website = strdup(articleURL);
}

// If we have data about an article, we update its count, or add new info completely
void updateInfo(void* search, info* inf, const char *articleTitle, const char *articleURL) {
  vector* vec = (*(indice*)search).vec;
  createInfo(inf,articleTitle,articleURL);
  int position = VectorSearch(vec, inf, infoCmp, 0, false);

  if(position == -1) VectorAppend(vec,inf);
  else {
    ((info*)VectorNth(vec,position))-> count++;
    //We need to free website and arcitle here so we don't cause memory leak
    //Which i actually did and it took me 3 hours to find this bug :') :()
    free(inf->website);
    free(inf->article);
  }
}

//If we don't have data about it, we create new info and indice and put it in vector, then hashset
void addNewInfo(indice* ind, hashset* map, const char *articleTitle, const char *articleURL, char* word) {
  ind->word = strdup(word);
  ind->vec = malloc(sizeof(vector));
  VectorNew(ind->vec, sizeof(info), infoFree, 4);
  info *newInfo = malloc(sizeof(info));
  createInfo(newInfo, articleTitle, articleURL);
  VectorAppend(ind->vec, newInfo);
  HashSetEnter(map, ind);
  free(newInfo);
}

/** 
 * Function: QueryIndices
 * ----------------------
 * Standard query loop that allows the user to specify a single search term, and
 * then proceeds (via ProcessResponse) to list up to 10 articles (sorted by relevance)
 * that contain that word.
 */

static void QueryIndices(hashset* map, hashset* stopWords)
{
  char response[1024];
  while (true) {
    printf("Please enter a single search term [enter to break]: ");
    fgets(response, sizeof(response), stdin);
    response[strlen(response) - 1] = '\0';
    if (strcasecmp(response, "") == 0) break;
    ProcessResponse(response, map, stopWords);
  }
}

/** 
 * Function: ProcessResponse
 * -------------------------
 * Placeholder implementation for what will become the search of a set of indices
 * for a list of web documents containing the specified word.
 */

static void ProcessResponse(const char *word, hashset* map, hashset* stopWords)
{
  if (WordIsWellFormed(word)) {
    if(HashSetLookup(stopWords,&word) == NULL) {
      ProcessCorrectResponce(map, word);
    } else printf("Too common a word to be taken seriously. Try something more specific.\n");
  } else {
    printf("\tWe won't be allowing words like \"%s\" into our set of indices.\n", word);
  }
}

//We may need to print less than 10 articles, this method handles that case
int indexCount(vector * vec) {
  int res = 10;
  if(VectorLength(vec) < res) res = VectorLength(vec);
  return res;
}

//Prints final output, formatted like test data
void printStuff(info* cur, int i) {
  if(cur->count == 1) printf("%d.) \"%s\" [search term occurs %d time]\n",i+1, cur->article, 1);
  else printf("%d.) \"%s\" [search term occurs %d times]\n",i+1, cur->article, cur->count);
  printf("\"%s\"\n",cur->website);
}

//Works with a response, if is not a stopWord
void ProcessCorrectResponce(hashset* map, const char* word) {
  char * curWord = strdup(word);
  void * search = HashSetLookup(map,&curWord); 
  if(search == NULL) {
    printf("None of today's news articles contain the word \"%s\".\n", word);
    return;
  }
  indice * ind = (indice*)search;
  vector * vec = ind->vec;
  VectorSort(vec,vectorSortCmp);

  for (int i=0; i < indexCount(vec); i++) {
    info* cur = (info*)(VectorNth(vec,i));
    printStuff(cur,i);
  }
  free(curWord);
}

/**
 * Predicate Function: WordIsWellFormed
 * ------------------------------------
 * Before we allow a word to be inserted into our map
 * of indices, we'd like to confirm that it's a good search term.
 * One could generalize this function to allow different criteria, but
 * this version hard codes the requirement that a word begin with 
 * a letter of the alphabet and that all letters are either letters, numbers,
 * or the '-' character.  
 */

static bool WordIsWellFormed(const char *word)
{
  int i;
  if (strlen(word) == 0) return true;
  if (!isalpha((int) word[0])) return false;
  for (i = 1; i < strlen(word); i++)
    if (!isalnum((int) word[i]) && (word[i] != '-')) return false; 

  return true;
}

// * * * * * * Implementations of my functions (not for decomposition) * * * * * *

//Adds stopWords to hashset
void initStopWords(hashset* stopWords) {
  FILE *infile;
  streamtokenizer st;
  char arr[1024];
  char* cur;
  
  infile = fopen(stopWordsData, "r");
  assert(infile != NULL);
  STNew(&st, infile, kNewLineDelimiters, true);
  while (STNextToken(&st, arr, sizeof(arr))) { 
    cur = strdup(arr);
    HashSetEnter(stopWords,&cur);
  }
  
  STDispose(&st);
  fclose(infile);
  printf("\n");
}

//Copied this function from readme file, didn't write it myself
int strHash(const void *v,int numBuckets) {
  char* s = *(char**)v;
  unsigned long hashcode = 0;
  
  for (int i = 0; i < strlen(s); i++) hashcode = hashcode * -1664117991L + tolower(s[i]);  
  return hashcode % numBuckets; 
}

//Compares char*s
int strCmp(const void *v1, const void* v2) { 
  return strcasecmp(*(char**)v1,*(char**)v2);
}

//frees char*
void strFree(void* v) { 
  free(*(char**)v); 
}

//HashFn for indices, same logic as string one
int indHash(const void *v, int numBuckets) {
  indice* i = (indice*)v;
  char* str = i->word;
  return strHash(&str, numBuckets);
}

//Compares indices
int indCmp(const void *v1, const void* v2) {
  return strcasecmp(((indice*)v1)->word, ((indice*)v2)->word);
}

//Frees and indice, first disposes vector, then frees its pointer
void indFree(void* v) {
  indice* i = (indice*)v;
  free(i->word);
  VectorDispose(i->vec);
  free(i->vec);
}

//Frees info, char* only, int doesn't need to be freed
void infoFree(void* v) {
  info* inf = (info*)v;
  free(inf->website);
  free(inf->article);
}

//Compares infos, with char*s
int infoCmp(const void *v1, const void* v2) {
  info* a = (info*) v1; 
  info* b = (info*) v2;
  int titles = strcasecmp(a->article,b->article);
  if(titles == 0) return strcasecmp(a->website,b->website);
  else return titles;
}

//Compares infos with ints
int vectorSortCmp(const void* v1, const void* v2) {
  info* a = (info*)v1;
  info* b = (info*)v2;
  if(a->count == b->count) return 0;
  if(a->count < b->count) return 1;
  return -1;
}