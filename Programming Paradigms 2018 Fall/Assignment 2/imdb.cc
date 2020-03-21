using namespace std;
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

//Struct that stores actorFile and movieFile pointers, also void* data meaning char* or a film*
struct Pointers {
  const void* actorPtr;
  const void* moviePtr;
  const void* data;
};

//compares actors by matching their strings, v1 being a Ponters*
int cmpActors(const void* v1, const void* v2) {
  Pointers p = *(Pointers*) v1;
  return strcmp((char*)p.data,(char*)p.actorPtr + *(int*)v2);
}

//compares movies, v1 being a pointer*, v2 being an int offset towards a film
int cmpMovies(const void* v1, const void* v2) {
  Pointers info = *(Pointers*) v1;
  char* name = (char*)info.moviePtr + *(int*)v2;
  int year = 1900 + *(char*)(name + strlen(name) + 1);

  film a = *(film*)(info.data);
  film b;
  b.title = name;
  b.year = year;

  // < and = operators exist within films
  if(a == b) return 0;
  else if(a < b) return -1;
  else return 1;
}

//pushes films into a vector
void getFilms(char* base, vector<film>& films, Pointers* info) {
  int length = strlen(base) + 1;
  if(length % 2 == 1) length++; //Pointer arithmetic based on padded bytes
  short numMovies = *(short*)(base + length);
  if((length + sizeof(short)) % 4 != 0) length += 2; // Same arithmetic here
  length += sizeof(short);

  //Calculates base of the int array, array++ being moving forward one int (4bytes) in the memory
  int * array = (int*)(base + length);
  film cur;
  for (int i = 0; i < numMovies; i++, array++) {
    char* movieName = (char*)info->moviePtr + *array;
    cur.year = 1900 + *(char*)(movieName + strlen(movieName) + 1);
    cur.title = movieName;
    films.push_back(cur);
  }
}

//pushes actor strings in a vector
void getActors(char* base, vector<string> &players, Pointers* info) {
  int length = strlen(base) + 2 * sizeof(char);
  if(length % 2 == 1) length++;
  short numActors = *(short*)(base + length);
  length += sizeof(short);
  if(length % 4 != 0) length += 2; //More pointer arithmetic, same logic as before 

  //Like upper method, calculates base of the array, iterating over it again with array++
  int* array = (int*)(base + length);
  for (short i = 0; i < numActors; i++, array++) {
    char* name = (char*)info->actorPtr + *array;
    players.push_back(name);
  }
}

//get credits for an actor
bool imdb::getCredits(const string& player, vector<film>& films) const { 
  Pointers info;
  info.actorPtr = actorFile;
  info.moviePtr = movieFile;
  info.data = player.c_str();

  int numActors = *(int*)actorFile;
  //bsearch for an actor pointer
  void* ptr = bsearch (&info, (char*)actorFile + sizeof(int), numActors, sizeof(int), cmpActors);
  if(ptr == NULL) return false;

  char* base = (char*)actorFile + *(int*)ptr;
  getFilms(base,films,&info);  
  return true;
}

//gets cast for a movie
bool imdb::getCast(const film& movie, vector<string>& players) const { 
  Pointers info;
  info.actorPtr = actorFile;
  info.moviePtr = movieFile;
  info.data = &movie;

  int numMovies = *(int*)movieFile;
  //bsearch for a movie pointer
  void* ptr = bsearch(&info, (char*)movieFile + sizeof(int), numMovies, sizeof(int), cmpMovies);
  if(ptr == NULL) return false;

  char* base = (char*)movieFile + *(int*)ptr;
  getActors(base,players,&info);
  return true;
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
