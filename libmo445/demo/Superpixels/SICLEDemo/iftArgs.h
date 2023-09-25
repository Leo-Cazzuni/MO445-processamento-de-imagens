/*****************************************************************************\
* iftArgs.h
*
* AUTHOR  : Felipe Belem
* DATE    : 2021-02-24
* LICENSE : MIT License
* EMAIL   : felipe.belem@ic.unicamp.br
\*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "ift.h"

//############################################################################|
// 
//  STRUCTS, ENUMS, UNIONS & TYPEDEFS
//
//############################################################################|
typedef struct ift_args iftArgs;

//############################################################################|
// 
//  PUBLIC METHODS
//
//############################################################################|
//============================================================================|
// Constructors & Destructors
//============================================================================|
/*
 * Creates an instance containing a reference to the array of arguments given
 * in parameter.
 *
 * PARAMETERS:
 *		argc[in] - REQUIRED: Number of arguments
 *		argv[in] - REQUIRED: Argument array from command-line
 *
 * RETURNS: Instance of the object
 */
iftArgs *iftCreateArgs
(const int argc, const char **argv);

/*
 * Deallocates the respective object 
 *
 * PARAMETERS:
 *		args[in/out] - REQUIRED: Pointer to the object to be free'd
 */
void iftDestroyArgs
(iftArgs **args);

//============================================================================|
// Getters & Setters & Verifiers
//============================================================================|
/*
 * Gets the string value associated to the argument token provided. That is,
 * the string at i+1 given the token at i. The token MUST NOT contain the
 * default prefix "--".
 *
 * PARAMETERS:
 *		args[in] - REQUIRED: Program's arguments 
 *		token[in] - REQUIRED: Token (sans prefix)
 *
 * RETURNS: A string, if the token was found; NULL, otherwise. 
 */
const char *iftGetArg
(const iftArgs *args, const char *token);

/*
 * Verifies if the token exists within the program's arguments. The token MUST
 * NOT contain the default prefix "--".
 *
 * PARAMETERS:
 *		args[in] - REQUIRED: Program's arguments 
 *		token[in] - REQUIRED: Token (sans prefix)
 *
 * RETURNS: True, if the token exists; false, otherwise.
 */
bool iftExistArg
(const iftArgs *args, const char *token);

/*
 * Verifies whether the token provided has a value associated to it. That is,
 * if exists a string at i+1 given the token at i. The token MUST NOT contain
 * the default prefix "--".
 *
 * PARAMETERS:
 *		args[in] - REQUIRED: Program's arguments 
 *		token[in] - REQUIRED: Token (sans prefix)
 *
 * RETURNS: True, if the token has a value; false, otherwise.
 */
bool iftHasArgVal
(const iftArgs *args, const char *token);

#ifdef __cplusplus
}
#endif

// 
//  MACROS
//
//############################################################################|
#define _IFT_ARGS_PREFIX "--" // Default prefix for any token

//############################################################################|
// 
//  STRUCTS, ENUMS, UNIONS & TYPEDEFS
//
//############################################################################|
struct ift_args
{
  int argc; // Number of arguments given by the user
  const char **REF_ARGV; // Reference to the array of argument values
};

//############################################################################|
// 
//  PRIVATE METHODS
//
//############################################################################|
//============================================================================|
// General & Auxiliary
//============================================================================|
/*
* Gets the argument index within the container, if it exists. If not, a 
* negative number indicates the inexistence of such token in the array.
*
* PARAMETERS:
*   args[in] - REQUIRED: Program's arguments 
*   token[in] - REQUIRED: Token (sans prefix)
*
* RETURNS: [0, #args[, if it exists ; -1, otherwise
*/
int _iftGetArgIdx
(const iftArgs *args, const char *token)
{
  #ifdef IFT_DEBUG //---------------------------------------------------------|
  assert(args != NULL);
  assert(token != NULL);
  #endif //-------------------------------------------------------------------|
  bool found;
  int i, idx;
  char *full_token;

  full_token = iftConcatStrings(2, _IFT_ARGS_PREFIX, token);

  i = 0; found = false;
  while(i < args->argc && found == false)
  {
    found = iftCompareStrings(args->REF_ARGV[i], full_token);
    
    if(found != true) ++i;
  }
  free(full_token);

  if(i == args->argc) idx = -1;
  else idx = i;

  return idx;
}

//############################################################################|
// 
//  PUBLIC METHODS
//
//############################################################################|
//============================================================================|
// Constructor & Destructor
//============================================================================|
iftArgs *iftCreateArgs
(const int argc, const char **argv)
{
  #ifdef IFT_DEBUG //---------------------------------------------------------|
  assert(argc >= 0);
  #endif //-------------------------------------------------------------------|
  iftArgs *args;

  args = malloc(sizeof(iftArgs));
  assert(args != NULL);

  args->argc = argc;
  args->REF_ARGV = argv;

  return args;
}

void iftDestroyArgs
(iftArgs **args)
{
  #ifdef IFT_DEBUG //---------------------------------------------------------|
  assert(args != NULL && *args != NULL);
  #endif //-------------------------------------------------------------------|
  free(*args);

  *args = NULL;
}

//============================================================================|
// Getters & Setters & Verifiers
//============================================================================|
inline const char *iftGetArg
(const iftArgs *args, const char *token)
{
  #ifdef IFT_DEBUG //---------------------------------------------------------|
  assert(args != NULL);
  assert(token != NULL);
  assert(iftExistArg(args, token));
  assert(iftHasArgVal(args, token));
  #endif //-------------------------------------------------------------------|

  return args->REF_ARGV[_iftGetArgIdx(args, token) + 1];
}

inline bool iftExistArg
(const iftArgs *args, const char *token)
{
  #ifdef IFT_DEBUG //---------------------------------------------------------|
  assert(args != NULL);
  assert(token != NULL);
  #endif //-------------------------------------------------------------------|
  
  return _iftGetArgIdx(args, token) >= 0;
}

bool iftHasArgVal
(const iftArgs *args, const char *token)
{
  #ifdef IFT_DEBUG //---------------------------------------------------------|
  assert(args != NULL);
  assert(token != NULL);
  assert(iftExistArg(args, token) == true);
  #endif //-------------------------------------------------------------------|
  const char *VAL;
  bool has_val;
  int idx;

  idx = _iftGetArgIdx(args, token);
  VAL = args->REF_ARGV[idx + 1]; // The subsequent must be the value
  
  // If it starts with the prefix, it is a token, not a value
  if(idx == args->argc - 1 || iftStartsWith(VAL, _IFT_ARGS_PREFIX) == true) 
  {
    has_val = false;
  }
  else has_val = true;

  return has_val;
}
