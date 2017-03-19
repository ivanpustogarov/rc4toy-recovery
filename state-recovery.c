#include <stdint.h>
#include <stdlib.h>
#include <alloca.h>
#include <stdio.h>
#include <string.h>
#include "util.h" // convert from hex to binary
#include "rc4prga.h"

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
//# define DEBUG_PRINT(x) do {} while (0)
# define DEBUG_PRINT(x)
#endif

/* Data types */

struct candidate_struct
{
  int s[SIZE]; // Current permutation; changes each step
  int inv_s[SIZE]; // Inverse permutation; changes each step
  int guessed[SIZE]; // contains guessed indices and their values
  int guessed_si;
  int guessed_sj;
  int nguessed; // Number of guessed values (i.e. number of set elements in guessed
  int j; // counter j in RC4
  int i; // counter i in RC4
  int idx; // index in the permutation <s> the value of which we are guessing
           // If it is 0, then we are working with index j.
  int t; // keystream position
};

typedef struct candidate_struct candidate;

/* Forward declarations */

int bt(candidate c, uint8_t *z, int z_len);
void print_candidate(candidate *c);
void debug_print_candidate(candidate *c);

/* Function definitions */

/* Print candidate_struct fields
 *
 * Print partially filled permutation and inverse permutation
 *
 * @param c Struct to print
 * @return void
*/
void print_candidate(candidate *c)
{
  int l = 0;

  printf("Permuation:\n");
  for(l=0;l<SIZE;l++)
    printf("% 3d ", l);
  printf("\n");
  for(l=0;l<SIZE;l++)
    printf("% 3d ", c->s[l]);

  printf("\n");

  printf("Reverse permuation:\n");
  for(l=0;l<SIZE;l++)
    printf("% 3d ", l);
  printf("\n");
  for(l=0;l<SIZE;l++)
    printf("% 3d ", c->inv_s[l]);

  printf("\n");
  printf("t=%d; i=%d; j=%d\n", c->t, c->i, c->j);
  return;
}

/* Print candidate_struct fields in DEBUG mode
 *
 * Print partially filled permutation and inverse permutation
 * only if -DDEBUG was set during the compilation time
 *
 * @param c Struct to print
 * @return void
*/
void debug_print_candidate(candidate *c)
{
  int l = 0;

  DEBUG_PRINT(("Permuation:\n"));
  for(l=0;l<SIZE;l++)
    DEBUG_PRINT(("% 3d ", l));
  DEBUG_PRINT(("\n"));
  for(l=0;l<SIZE;l++)
    DEBUG_PRINT(("% 3d ", c->s[l]));

  DEBUG_PRINT(("\n"));

  DEBUG_PRINT(("Reverse permuation:\n"));
  for(l=0;l<SIZE;l++)
    DEBUG_PRINT(("% 3d ", l));
  DEBUG_PRINT(("\n"));
  for(l=0;l<SIZE;l++)
    DEBUG_PRINT(("% 3d ", c->inv_s[l]));

  DEBUG_PRINT(("\n"));
  DEBUG_PRINT(("t=%d; i=%d; j=%d\n", c->t, c->i, c->j));
  return;
}


/* Check if the candidate's permutation does not have dublicates
 *
 * @param c Candidate to check
 * @return void
*/
void sanity_check(candidate *c)
{
  //Extra array needed
  int *s = c->s;
  int numbers[SIZE];
  int l;

  for(l=0;l<SIZE;l++)
    numbers[l] = 0;

  for(l=0;l<SIZE;l++)
  {
    if(s[l] == -1)
      continue;
    //Check if that number is already present
    if((numbers[s[l]] == 0) )
      numbers[s[l]]++;
    else
    {
      //Duplicate found
      DEBUG_PRINT(("sanity_check(): Dublicate found: %d (%d times)",s[l], numbers[s[l]]));
#ifdef DEBUG
      debug_print_candidate(c);
#endif
      getchar();
    }
  }
  return; 
}

/* Make a guess of an entry in the candidate's current permutation
 *
 * The guessed entry should not already be present in the permutation
 *
 * @param c candidate for which to guess entries
 * @param start The guessed value will be bigger than <start>
 * @return guess Newly guessed value
*/
int guess_entry(candidate *c, int start)
{
  int guess = start;
  while(c->inv_s[guess] != -1 && guess < SIZE)
    guess++;
  if(guess >= SIZE)
    return -1;
  return guess;
}

/* Try to make a RC4 step for the candidate (i.e. update the permutation)
 *
 * If the necessary entries in the permutation are not
 * defined, guess them. First guess S[i] (if neceseary) and
 * then guess S[j] (if necessary)
 *
 * @param c Candidate for which to make an RC4 step
 * @param si_start The guessed value for S[i] should be larger than <si_start>
 * @param sj_start The guessed value for S[j] should be larger than <sj_start>
 * @return  0 Successfully guessed both values (everything is alright)
 *         -1 Could not guess value for S[i] (for the curren value of <si_start>)
 *         -2 Could not guess value for S[j] (for the curren value of <sj_start>)
 *            and S[i] can be re-guessed
 *         -3 Could not guess value for S[j] (for the curren value of <sj_start>)
 *            and S[i] cannot be re-guessed (it was already set in previous steps)
 *         -4 Neither S[i] nor S[j] were guessed (they already were set in the permutation
 *            in the previous steps) (everything is alright)
 *	   -5 S[i] was guessed successfully, but S[j] could not be guessed
*/
int step(candidate *c, int si_start, int sj_start)
{
 
  int is_si_guessed = 0;
  int is_sj_guessed = 0;
   
  DEBUG_PRINT(("step(): starting step\n"));
  
  DEBUG_PRINT(("Updating i: %d -> ",c->i));
  c->i = ind(c->i+1); 
  DEBUG_PRINT(("%d\n",c->i));

  if (c->j == -1) // If we lost track of <j>
  {
    printf("Bug: we lost track of j\n");
    exit(-1);
  }

  // Guess s[i] if needed
  if (c->s[c->i] == -1)
  {
    int entry  = guess_entry(c, si_start);
    if (entry == -1)
    {
      DEBUG_PRINT(("WARNING: cannot guees a value for S[i], current value is %d\n", c->s[c->i]));
      return -1;
    }
    c->s[c->i] = entry;
    c->guessed_si = c->s[c->i];
    is_si_guessed = 1;
    DEBUG_PRINT(("step(): Guessing S[%d] = %d\n", c->i,c->s[c->i]));
  } else
  {
    DEBUG_PRINT(("step(): No guessing: S[%d]=%d is known\n", c->i,c->s[c->i]));
  }

  DEBUG_PRINT(("Updating j: %d -> ",c->j));
  c->j = ind(c->j+c->s[c->i]);
  DEBUG_PRINT(("%d\n",c->j));

  int tmp = c->inv_s[c->s[c->i]];
  c->inv_s[c->s[c->i]] = c->i;

  // Guess s[j] if needed
  if (c->s[c->j] == -1)
  {
    c->s[c->j] = guess_entry(c, sj_start);
    c->guessed_sj = c->s[c->j];
    is_sj_guessed = 1;
    if (c->s[c->j] == -1 && (is_si_guessed == 1 ))
    {
      DEBUG_PRINT(("step(): WARNING: cannot guees a value for S[j] (will try to increase s[i])\n"));
      return -2;
    }
    if (c->s[c->j] == -1 && (is_si_guessed == 0 ))
    {
      DEBUG_PRINT(("step(): WARNING: cannot guees a value for S[j] (an s[i] is fixed)\n"));
      return -3;
    }
    DEBUG_PRINT(("step(): Guessing S[%d] = %d\n", c->j,c->s[c->j]));
  } else
  {
    DEBUG_PRINT(("step(): No guessing: S[%d]=%d is known\n", c->j,c->s[c->j]));
  }

  c->inv_s[c->s[c->i]] = tmp;

  DEBUG_PRINT(("step(): swapping positions s[%d]=%d and s[%d]=%d\n", c->i,c->s[c->i],c->j,c->s[c->j]));
  int temp = c->s[c->j]; // can be undefined, i.e. -1
  c->s[c->j] = c->s[c->i];
  c->s[c->i] = temp; 

  DEBUG_PRINT(("step(): step completed\n"));
  if( (is_si_guessed == 0) && (is_sj_guessed == 0) ) // if no guessing was made
  {
    DEBUG_PRINT(("step(): No guessing at all\n"));
    return -4;
  }

  if( (is_si_guessed == 1) && (is_sj_guessed == 0) ) // if s[i] was guessed but s[j] was not
  {
    DEBUG_PRINT(("step(): S[i] was guessed but s[j] was not\n"));
    return -5;
  }
  return 0;
}


/* Update reverse permutation for current indices i and j
 *
 * @param c Candidate to update
 * @return void
*/
void finalize_step(candidate *c)
{
  int *s = c->s; 
  int i = c->i; 
  int j = c->j; 
  c->inv_s[s[i]] = i;
  c->inv_s[s[j]] = j;
  return;
}


/* Get the first candidate for backtracking
 * 
 * Makes a copy of candidate <c>, invokes RC4 step and returns with
 * a code which describes if the guesses durint the step went OK
 *
 * @param c Candidate from which to derive the one for recursion
 * @param z Keystream
 * @param ret Return code (we need it because we also need to return the new candidate)
 * @param s First candidate for backtracking
*/
candidate first(candidate c, uint8_t *z, int *ret)
{
  candidate s = c;
  int res = step(&s,0,0); // Make a step and guess permutation entries if necessary

  if(res == -1) // cannot guess s[i], end
    *ret = 1; // stop candidates cycle

 // if(res == -4) // no guessing is happenning at all (and corresponding s[i] and s[j] value were already considered in the first() function), end
 //   *ret = 1;

  if(res == -3) // cannot guess s[j], and cannot re-guess s[i] (it is fixed), end
    *ret = 1; // stop candidates cycle


  if(res == -2) // cannot guess s[j], but can re-guess s[i]
                // In this case we did not really assign any new values, so we should no go
		// deeper into recursion (nor update inverse permutation)
		// for this case; at the same time we should not interrupt
		// the current level of recursion
  {
    s.guessed_si++;// = 1;
    s.guessed_sj = -1;
    *ret = 2; // skip current candidate in the candidate cycle, and to next
  }

  if(res == -5) // guessed s[i], but s[j] was determinstic, so let's increase the counters for the next time
                // This case is valid in the sence that we should launch update_state
		// and go deeper into recursion
  {
    s.guessed_si++;// = 1;
    s.guessed_sj = -1;
  }

  // -4 means that S[i] and S[j] were fixed, so we did not do any guesses 
  if( (res == 0) || (res == -5) || (res == -4) )
  {
    *ret = 0;
    finalize_step(&s); // Update inverse permutation
  }
  return s;
}

/* Get the next candidate for backtracking
 * 
 * Makes a copy of candidate <c>, invokes RC4 step and returns with
 * a code which describes if the guesses during the step went OK
 *
 * @param c Candidate from which to derive the one for recursion
 * @param z Keystream
 * @param ret Return code (we need it because we also need to return the new candidate)
 * @param s Next candidate for backtracking
*/
candidate next(candidate c, candidate prev_s, uint8_t *z, int *ret)
{
  candidate s = c;

  // Returns 0 if everything is good
  int res = step(&s, prev_s.guessed_si, prev_s.guessed_sj+1); // Make a step and guess permutation entries if necessary

  if(res == -1) // cannot guess s[i], end
    *ret = 1;

  if(res == -4) // no guessing is happenning at all (and corresponding s[i] and s[j] value were already considered in the first() function), end
    *ret = 1;

  if(res == -3) // cannot guess s[j], and cannot re-guess s[i] (it's fixed), end
    *ret = 1;


  if(res == -2) // cannot guess s[j], but can re-guess s[i]
                // In this case we did not really assign any new values, so we should no go
		// deeper into recursion (nor update inverse permutation)
		// for this case; at the same time we should not interrupt
		// the current level of recursion
  {
    s.guessed_si++;
    s.guessed_sj = -1;
    *ret = 2; // skip current candidate in the candidate cycle, and to next
  }

  if(res == -5) // guessed s[i], but s[j] was determinstic, so let's increae the counters for the next time
  {
    s.guessed_si++;
    s.guessed_sj = -1;
  }
    
  if( (res == 0) || (res == -5) )
  {
    *ret = 0;
    finalize_step(&s); // Update inverse permutation
  }
  return s;
}


/* Return empty candidate at the root of the search tree for backtracking.
 *
 * The permutation's entries are set to -1
 * The reverse permutation's entries are set to -1
 * i and j are set to 0
 * t (current step) is set to -1
*/
candidate root()
{
  candidate c;
  int l = 0;
  for(l = 0; l < SIZE; l++)
    c.s[l] = -1;
  for(l = 0; l < SIZE; l++)
    c.inv_s[l] = -1;
  //c.s = s;
  c.i = 0;
  c.j = 0;
  c.nguessed = 0;
  c.idx = -1;
  c.t = -1;
  c.guessed_si = 0;
  c.guessed_sj = 0;
  return c;
}

/* Update the permutation and inverse permutation based on the
 * keystream. Return -1 if there is a contradiction:
 * Each new value we add to the permutation <s> should be either different from
 * the existing elements or should have the same index.
 *
 * @param c Candidate to update
 * @param z Keystream
 * @return  0 No contradictions
 *         -1 There was a contradiction with the current state
*/
int update_state(candidate *c, uint8_t *z)
{
    int i = c->i;
    int j = c->j;
    int t = c->t;
    int *s = c->s;
    int *inv_s = c->inv_s;
    int zt = (int)z[t];

    DEBUG_PRINT(("update_state(): zt=%d\n",zt));
    DEBUG_PRINT(("update_state(): inv_s[zt]=%d\n",inv_s[zt]));
    DEBUG_PRINT(("update_state(): j=%d\n",j));
    if (j != -1)
    {
      DEBUG_PRINT(("update_state(): s[j=%d] is %s\n",j, (s[j] != -1) ? "known" : "not known"));
    }
    DEBUG_PRINT(("update_state(): s[i=%d] is %s\n",i, (s[i] != -1) ? "known" : "not known"));

    // If S[i_t], S[j_t], and j_t are known, add Z[t] to the permutation
    if( (s[i] != -1) && (j != -1 ) && (s[j] != -1) )
    {
      int idx = ind(s[i]+s[j]);

      if ( ((s[idx] != -1) &&  (s[idx] != zt)) )
      {
        DEBUG_PRINT(("Was trying to update s[s[i]+s[j]=%d] with zt=%d (already occupied with %d)\n", idx, zt, s[idx]));
        return -1;
      }

      if ( (inv_s[zt] != -1) && (inv_s[zt] != idx) )
      {
        DEBUG_PRINT(("Was trying to update s[s[i]+s[j]=%d] with zt=%d (zt already appear at index %d)\n", idx, zt, inv_s[zt]));
        return -1;
      }
      DEBUG_PRINT(("Updating s[s[i]+s[j]=%d] with zt=%d\n", idx, zt));
      s[idx] = zt;
      inv_s[zt] = idx;
    }

    // If S⁻¹[zₜ], jₜ, and S[iₜ] are known, determine S[jₜ]
    if( (inv_s[zt] != -1) && (j != -1 ) && (s[i] != -1) )
    {
      int entry = ind(inv_s[zt]-s[i]); 
      if( (s[j] != entry) && (s[j] != -1) )  // if the entry already appears in the permutation with a different index
      {
        DEBUG_PRINT(("Entry %d already appears in the permutation with index different from j\n", entry));
        return -1;
      }
      DEBUG_PRINT(("Updating s[j=%d] with inv_s[zt]-s[i]=%d\n", j, entry));
      s[j] = entry;
      inv_s[entry] = j;
    }

    // If S⁻¹[Z_t], j_t, and S[j_t] are known, determine S[i_t]
    if( (inv_s[zt] != -1) && (j != -1 ) && (s[j] != -1) )
    {
      int entry = ind(inv_s[zt]-s[j]); 
      if( (s[i] != entry) && (s[i] != -1) ) // if the entry already appears in the permutation with a different index
      {
        DEBUG_PRINT(("Entry %d appears in the permutation with index different from i\n", entry));
        return -1;
      }
      DEBUG_PRINT(("Updating s[i=%d] with inv_s[zt]-s[j]=%d\n", i, entry));
      s[i] = entry;
      inv_s[entry] = i;
    }

    if( (s[i] != -1) && (inv_s[zt] != -1) && (inv_s[ind(inv_s[zt]-s[i])] != -1) )
    {
      j = inv_s[ind(inv_s[zt]-s[i])];
      if(c->j != j) // the computed value of <j> does not coincide with the one set before => contradicition
      {
        DEBUG_PRINT(("the computed value of <j> does not coincide with the one set before\n"));
        return -1;
      }
    }

    return 0; // no contradiction
}


/* Main backtracking procedure

   Takes a solution candidate, updates/checks for contradictions of
   its permutation <s> based on the current byte in the key stream.
   If there are not contradiction, make another step (i.e. read the next
   keystream byte), and go deeper into recursion.
   If the end of the keystream is reached, print a success message,
   and print the current candidate.

   @param c Current candidate with partially filled permutation
   @param z Keystream
   @param z_len lenght of the keystream
   @return 0
*/
int bt(candidate c, uint8_t *z, int z_len)
{
  int ret = 0;
  DEBUG_PRINT(("\n============================================\n"));
  DEBUG_PRINT((" ==> Invoking bt (t=%d).\n", c.t));
  DEBUG_PRINT((" => Update and check.\n"));
  if(update_state(&c, z) < 0)  // check for contradiction
  {
    DEBUG_PRINT((" ==> Dead candidate\n"));
    return 0;
  }
  DEBUG_PRINT(("The updated candidate is:\n"));
#ifdef DEBUG
  debug_print_candidate(&c);
#endif
  DEBUG_PRINT(("\n\n\n => Getting first candidate (t=%d).\n", c.t));
  if(c.t >= z_len-1)
  {
    printf(" ** Success (t=%d) Press any key ** \n", c.t);
    print_candidate(&c);
    printf("Print any key to continue search or CTRL-C to interrupt\n");
    //getchar();
    exit(0);
  }
  candidate s = first(c, z, &ret); // Do step here
#ifdef DEBUG
  debug_print_candidate(&s);
#endif
  //sanity_check(&s);

  int num = 1;
  while( (ret == 0) || (ret == 2) )
  {
    s.t++;
    if (ret == 0)
    {
      DEBUG_PRINT(("bt(): going deeper to level %d.\n",s.t));
      bt(s, z, z_len);
    }
    else
    {
      DEBUG_PRINT(("bt(): skipping candidate (ret=%d).\n",ret));
    }
    DEBUG_PRINT(("\n\n\n => Choosing next (%d) candidate (t=%d). The origin (c) is:\n",num, c.t));
#ifdef DEBUG
    debug_print_candidate(&c);
#endif
    num++;
    s = next(c,s,z,&ret); // next will be derived from c, previously guessed values are stored in s
#ifdef DEBUG
    debug_print_candidate(&s);
    //sanity_check(&s);
#endif
  }
  DEBUG_PRINT(("bt(): Parsed all children, none worked out. Going one level up.\n"));
  return 0;
}

int main(int argc, char *argv[])
{
  if(argc < 2 || argc > 2)
  {
    printf("Recover RC4 internal state from a keystream\n");
    printf("Use ./rc4test to generate a keystream\n");
    printf("Word size is defined in Makefile (ALPHA)\n\n");
    printf("Usage: state-recovery KEYSTREAMHEX\n");
#ifdef DEBUG
    printf("\nDEBUG_PRINT is ENABLED\n");
#endif
    exit(0);
  }

  // Parse hex keystream from the command line, convert it to binary and put to <z>
  uint8_t *stream_str = (uint8_t *)argv[1];
  int stream_len = strlen((char *)stream_str)/2;
  uint8_t *z = (uint8_t*)alloca(stream_len); // keystream
  fromHex(z, stream_str, stream_len, 0);

  printf("Starting state recovery of RC4-%d...\n",SIZE);

  candidate c = root();
  DEBUG_PRINT(("Root candidate:\n"));
#ifdef DEBUG
  debug_print_candidate(&c);
#endif
  bt(c, z, stream_len);
  
  return 0;
}
