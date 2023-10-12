/*
 * tinyosc_match.c
 *
 *  Created on: May 17, 2022
 *      Author: master
 */

#include <stdio.h>
#include <stdbool.h>

#define _DEBUGBLOCK_ 0

/**
 * matching the {foo,bar,yeah} string list scenario
 *
 * returns true if name s (i.e the current character):
 *
 *     will match AT LEAST ONE of the strings in pattern p {a,b,c}
 *     s wont match ANY of the strings in pattern p {!a,b,c} (This is a deliberate extension to OSC 1.1 convenient though)
 *
 * if match is successful, both the entire pattern ({...}) and the matched name portion will be 'consumed', meaning that
 * the pointers will have advanced, the length information will be adjusted (decreased by that much too)
 * otherwise, the pointers wont be changed
 *
 * @param p		pointer to char pointer for the pattern to match, keeps advancing while matching (starting with '{')
 * @param plen	pointer to the pattern length, keeps decrementing while matching (including leading '{')
 * @param s		pointer to char pointer for the name to match, keeps advancing while matching (starting with '{')
 * @param slen	pointer to the name length, keeps decrementing while matching (including leading '{')
 *
 * @return true if pattern matches the name, false otherwise
 */
static bool osc_matchStringList(const char** const p, int* const plen, const char** const s, int* const slen)
{
	//{foo,bar,bla} contains a ',' separated list of strings, any of them can match

	//sanity checks
	if(*plen<=0) return false;
	if(**p != '{') return false;

#if _DEBUGBLOCK_

	const char* _p = *p;
	int _plen = *plen;
	const char* _s = *s;
	int _slen = *slen;

#endif

	const char* pp = *p+1; //pass '{'
	int pplen = (*plen)-1;

	bool inv = false; //!-logic
	if(pplen<=0) return false;
	if(*pp == '!') { inv = true; --pplen; ++pp; } //check and remember for inverse logic, consume the '!\ if present

	if(pplen<=0) return false;
	if(*slen<=0) return false;

	//from here on we need logic awareness
	bool m = inv; //definition of done

__res:
	while(pplen>0 && m==inv) //in inverted case, one mismatch doesnt prove anything, we need to scan all parts, also, while no match for the noninverted case
	{
		//for resets to compare to probable next part
		const char* ss = *s;
		int sslen = *slen;
		bool matched = true; //default for one new round only

		while(pplen>0 && m==inv)
			switch(*pp)
			{
				case '}':
				case ',':
				{
					//consumed what needed from p
					//at this point, we are either done with successfully comparing the last one or came from an mismatch in any of the strings

					if(matched)
					{
						//match found

						m = true;

						if(inv) goto __out1; //we matched one string, but shouldn't
						else
						{
							//we matched at least one string, leave gracefully
							//will leave due to m(true) != inv(false)

							//consume the matching part of the name
							*s = ss; *slen = sslen;
						}
					}
					else if(*pp == '}') //and also not matched
					{
						//no more elements to compare to
						m = false;

						//nothing matched so far
						if(inv)
						{
							//leave gracefully, we didnt find any matches
							//will leave due to m(false) != inv(true)

							//consuming the non matching part of the name must not be done here, as it can match additional conditions later
							//this can be a problem though,
							//example: /{!bt,dt}est --> /atest
							//successfully "not matches" bt neither dt, and while the
							//"/{!bt,dt}" portion will get consumed in the pattern, resulting in "est"
							//the name will remain the same, "atest
							//the only solution would be to somehow drop the "at" in the name
							//this requires usage of the skipping mechanisms, from * operator
							//dropping anything from name until the first match is encountered
							//or nothing to drop anymore
							//for now, this is to wild and probably totally unnecessary
						}
						else goto __out1; //if no more elements to scan, leave with error
					}
					else
					{
						//no match, and ','so still things to do
						goto __res; //this will never come here, just for clarity, is handled in the default case
					}
				}
					break;
				default:
				{
					if(sslen<=0) return false;
					if(*pp != *ss)
					{
						//mismatch
						matched = false;

						//forward to next string element to compare or skip to '}'
						while(pplen>0)
						{
							if(*pp == ',') { --pplen; ++pp; break; }
							else if(*pp == '}') { break; } //no more parts to compare to, nothing found
							--pplen; ++pp; //consume mismatching chars
						}
						if(*pp == '}') break; //we already have encountered the end ("-]" case), don't consume, just re-evaluate
						goto __res; //s params will be restored, we are either out of options pointing to '}' or to the next string
					}
					else
					{
						//match one char of the string only, we can remain matched=true
						//continue string compare by character, until we hit ',' or '}'
					}
					--pplen; ++pp; --sslen; ++ss; //compare next ch
				}
					break;
			} //switch
	} //while

	//success

	//make sure pp and pplen pass } to rest on next
	while(pplen>0) { if(*pp == '}') { break; } --pplen; ++pp;	}
	if(pplen <= 0) return false; //no '}' found

	--pplen; ++pp; //consume the '}'
	*p = pp; *plen = pplen;

	//consuming the matching string in name has already been done

__out1:

	//determine the result
	if(!inv && !m) return false; //normal case
	if(inv && m) return false; //inverted case

	return true;
}

/**
 * matching the [a-z] char list scenario
 *
 * returns true if name s (i.e the current character):
 *
 *     will match AT LEAST ONE of the characters in pattern p [a-z] or [abcDEg] or even [adef-xYu1-4] or [-z] or [a-] (doesn't make much sense though)
 *     wont match ANY of the characters in pattern p [!a-z] or the respectively negated examples above
 *
 * if match is successful, both the entire pattern ([...]) and the matched name portion (one char) will be 'consumed', meaning that
 * the pointers will have advanced, the length information will be adjusted (decreased by that much too)
 * otherwise, the pointers wont be changed
 *
 * @param p		pointer to char pointer for the pattern to match, keeps advancing while matching (starting with '[')
 * @param plen	pointer to the pattern length, keeps decrementing while matching (including leading '[')
 * @param s		pointer to char pointer for the name to match, keeps advancing while matching (starting with '[')
 * @param slen	pointer to the name length, keeps decrementing while matching (including leading '[')
 *
 * @return true if pattern matches the name, false otherwise
 */
static bool osc_matchCharList(const char** const p, int* const plen, const char** const s, int* const slen)
{
	//[abc], [!abc], [a-c], [!a-c] characters match, only one range indication is supported. [a-eC-F] is not possible
	//! indicates a negation of logic

	//sanity checks
	if(*plen<=0) return false;
	if(**p != '[') return false;

#if _DEBUGBLOCK_

	const char* _p = *p;
	int _plen = *plen;
	const char* _s = *s;
	int _slen = *slen;

#endif

	const char* pp = *p+1; //pass '['
	int pplen = (*plen)-1;

	bool inv = false; //!-logic
	if(pplen<=0) return false;
	if(*pp == '!') { inv = true; --pplen; ++pp; } //check and remember for inverse logic, consume the '!\ if present

	if(pplen<=0) return false;
	if(*slen<=0) return false;

	//from here on we need logic awareness
	bool m = inv; //default invalid value of match depends on expectation
	unsigned char cha = 0, chb = 0; //invalid range

	//case [abc] or [a-c] is done implicitly, analyzing the others chars
	while(pplen>0 && m==inv)
	{
		switch(*pp)
		{
			case ']': //consumed what needed from pattern, nothing matched
			{
				if(!inv) goto __out2; //nothing encountered up until now, this is a no match

				//in inverted logic, no error up to end means we haven't found any forbidden match
				//success, if no errors found up to now
				m = !inv;
			}
				break;
			case '-': //range encountered, could be first after, could be last before ]
			{
				int offs = inv?2:1;
				if (pplen+offs == *plen)
				{
					//'-' is first after [ or [!
					cha = 0; //open range to the left
				}
				else
				{
					//range spec found after previous character
					//leave the left side of the range to the previously set value
				}
				//automatically open the range to the far right, it will be corrected if a letter is present after that
				chb = 0xFFu;

				--pplen; ++pp; //consume '-'

				if(pplen <= 0) return false; //no ']' possible
				if(*pp !=']') continue; //case "-]" needs to fall through, otherwise restart, we already consumed the '-'

				//fall through for no more characters, so we get to check the last range
			}
			/* no break */
			default:
			{
				if(cha == chb)
				{
					//no range check, update with single char range for later check
					cha = *pp;
					chb = *pp;
				}
				else if(*pp ==']')
				{
					//range already set when encountered last bracket, from fall through of "-]"
				}
				else
				{
					//range opened, update only right side with the current value
					chb = *pp;
				}

				//check for out of range or != test char
				if(**s < cha || **s > chb)
				{
					//out of range or no match
					//for normal logic, a miss is not yet an error, so retry with next
					//for inverted logic, all possible ranges need to be tested to be successful, continue
				}
				//check for inside range
				else if (**s >= cha && **s <= chb)
				{
					//in range
					if(inv) goto __out2; //match with forbidden range

					//success, if no errors found up to now
					m = !inv;
				}
				else
				{
					//sanity, may never occur, but..
					goto __out2;
				}

				//close the range, so it can be updated with next char
				cha = chb;
			}
				break;
		}//switch

		if(*pp ==']') continue; //we already have encountered the end ("-]" case), don't consume, just re-evaluate
		--pplen; ++pp; //consume test char
	} //while next characters


	//a match has been found, pass closing bracket ']' consuming all the char list fuzz except ']'
	while(pplen>0) { if(*pp == ']') { break; } --pplen; ++pp; }
	if(pplen <= 0) return false; //no ']' found

	//success

	--pplen; ++pp; //consume the ']'
	*p = pp; *plen = pplen;

	--*slen; ++*s; //consuming the matching character in name

__out2:

	//determine the result
	if(!inv && !m) return false; //normal case
	if(inv && m) return false; //inverted case

	return true;
}

//forward
static bool osc_matchPattern0(const char** const p, int* const plen, const char** const s, int* const slen, unsigned int ctx);
static bool osc_matchPatternImpl(const char** const p, int* const plen, const char** const s, int* const slen, unsigned int ctx);

/**
 * matching the '*' scenario
 *
 * we may skip characters in name while still trying to match, up until the respective section end
 *
 * PATTERN SECTION END REACHED
 *
 * case 1: "*" (mo further sections)
 *	A name has no further sections: match
 *	B name has further sections: no match

 * case 2: "* /" (more sections)
 *	A name has no further sections: no match
 *	B name has further sections: fully recurse into new section
 *
 * MORE PATTERN IN SECTION
 * case 3: "*cd"
 *	retry matching, if mismatch skip a name character and retry, otherwise restore normal process
 *	will eventually end in case 1
 *
 * case 4: "*cd/"
 *	retry matching, if mismatch skip a name character and retry, otherwise restore normal process
 *	will eventually end in case 2
 *
 * @param p		pointer to char pointer for the pattern to match, keeps advancing while matching (starting with '[')
 * @param plen	pointer to the pattern length, keeps decrementing while matching (including leading '[')
 * @param s		pointer to char pointer for the name to match, keeps advancing while matching (starting with '[')
 * @param slen	pointer to the name length, keeps decrementing while matching (including leading '[')
 *
 * @return true if pattern matches the name, false otherwise
*/
static bool osc_matchStar(const char** const p, int* const plen, const char** const s, int* const slen, unsigned int ctx)
{
	//ignore unmatched and retry with a char ahead, a match resets it to normal state

	//sanity checks
	if(*plen<=0) return false;
	if(**p != '*') return false;

	const char* pp = *p+1; //pass '*'
	int pplen = (*plen)-1;
	const char* ss = *s;
	int sslen = *slen;

	//if pattern section finished
	if((*pp == '/') || (pplen<=0))
	{
		//case 1 and 2

		//("*" pr "*/.." case)
		//pattern section is finished (or no more sections) right after the '*'
		//so anything in name, that might follow up until its own end of section is alright
		//so we need to check for next section in name
		//but if more sections available...recurse

		//skip current name section
		while(sslen>0) { if(*ss == '/') { break; } --sslen; ++ss; }

		if(sslen<=0)
		{
			//no section in name
			if(pplen<=0)
			{
				//no more name sections, no more pattern sections, match (CASE 1 A)
				*p = pp; *plen = pplen;
				*s = ss; *slen = sslen;
				return true;
			}
			else
			{
				return false;// CASE 2 A
			}
		}

		//still have a section in name

		if(pplen<=0) return false; //but no more pattern sections, this is a mismatch even with '*' (CASE 1 B)

		//still have a section in pattern

		//so recurse there together with the pattern (CASE 2 B)
		if(!osc_matchPatternImpl(&pp, &pplen, &ss, &sslen, ctx)) return false;
		//consume the matched stuff
		*p = pp; *plen = pplen;
		*s = ss; *slen = sslen;
		return true;
	}
	else
	{
		//case 3 and 4
		//more in pattern, so we need to match ("*abc" case)
		while(1)
		{
			//entering recursive star mode, saving previous conditions
			if(osc_matchPattern0(&pp, &pplen, &ss, &sslen, ctx))
			{
				//restore normal mode with new conditions
				*p = pp; *plen = pplen;
				*s = ss; *slen = sslen;
				return true; //continue normal evaluation
			}
			else
			{
				//match went wrong, try match further in name string
				--sslen; ++ss;

				if(sslen<=0) return false; //no name left but pattern goes on /?
			}
		} //while
	}
}

/**
 * main recursable osc matching function
 * matching an osc pattern to an osc name or path, both including leading '/'.
 * several sections of /.../.../... are allowed in both.
 *
 * rule of thumb for the code:
 * return false on error, switch break is continue to lookout for next matching conditions
 * if all the pattern matched and no more name to match, this is a match, returning true
 *
 * osc container and osc methods use same logic, no need to differentiate here
 *
 * this function uses recursion because of the '*' and the "//" operators,
 * hence it has to be called by the @osc_matchPatternImpl. It should NOT be called directly, because it skips the sanity checks.
 *
 * if match is successful, both the matched pattern (including possible [...], {...}, '*', "//" etc) and the matched name portion will be 'consumed', meaning that
 * the pointers will have advanced, the length information will be adjusted (decreased by that much too)
 * otherwise, the pointers will remain at the last matched position.
 *
 * @param p		pointer to char pointer for the pattern to match, keeps advancing while matching (starting with '/')
 * @param plen	pointer to the pattern length, keeps decrementing while matching (including leading '/')
 * @param s		pointer to char pointer for the name to match, keeps advancing while matching (starting with '/')
 * @param slen	pointer to the name length, keeps decrementing while matching (including leading '/')
 *
 * @return true if pattern matches the adress, false otherwise
 */
static bool osc_matchPattern0(const char** const p, int* const plen, const char** const s, int* const slen, unsigned int ctx)
{
	//no sanity checks, internal function block

	int plen_ = *plen; //to detect //

	//try get first char of pattern
	while(*plen>0)
	{
#if _DEBUGBLOCK_

	const char* _p = *p;
	int _plen = *plen;
	const char* _s = *s;
	int _slen = *slen;

#endif

		//*p is available, consume, don't forget to ++p and --plen after usage, same for s, slen
		switch(**p)
		{
			case '*': if(!osc_matchStar(p, plen, s, slen, ctx)) return false; //star match went wrong
				return true; //exception, recursions decide and consume for themselves
			case '?':
			{
				if(*slen<=0) return false; //no name left, but pattern left
				//matches any single char //TEST
				--*plen; ++*p; --*slen; ++*s; //consume
				//continue
			}
				break;

			case '{': if(!osc_matchStringList(p, plen, s, slen)) return false; //string match went wrong
				//all the matched characters have been consumed
				//continue
				break;

			case '[': if(!osc_matchCharList(p, plen, s, slen)) return false; //char list match went wrong
				//all the matched characters have been consumed
				//continue
				break;

			case '}':
			case ']':
				return false; //invalid, must have been consumed by the subroutines

			case '/': //need to be first, which we already checked, this means we are in a new section, OR this is the // case
			{
				if(*plen == plen_)
				{
					// "//" case
					ctx |= 1; //mark deep match

					//skip all sections until nothing left or first match found

					//store return conditions
					const char* pp = *p+1; //pass '/'
					int pplen = (*plen)-1;
					const char* ss = *s;
					int sslen = *slen;

					if(pplen<=0) return false; //if no more pattern follows the second '/', this is an error.. a pattern ending in "....//" is illegal
					if(sslen<=0) return false; //empty name is error

					while(1)
					{
						//entering recursive star mode, saving previous conditions
						if(osc_matchPattern0(&pp, &pplen, &ss, &sslen, ctx)) //may encounter yet another "//" (needt to use the Raw osc_matchPattern0 here, because the name already lacks the leading '/'
						{
							//restore normal mode with new conditions
							*p = pp; *plen = pplen;
							*s = ss; *slen = sslen;

							ctx &= ~1; //kill mark of deep match
							return true; //no break here, the recursed function will have resumed the matching itself
						}
						else
						{
							//match went wrong, try match deeper in the name sections by skipping one
							while(sslen>0) { if(*ss == '/') { break; } --sslen; ++ss; }
							if(sslen<=0) return false; //no name left but pattern goes on

							--sslen; ++ss; //pass the found section begin '/'

							//found another name section
							//retry
						}
					}

					break;
				}
				else
				{
					//end of a pattern section reached, which marks the start of a new one
					//recurse
					//return osc_matchPatternImpl(p, plen, s, slen, ctx);

					//consume and compare, to prevent recursion, technically, it is a
					//fallthrough
				}
			}
			/* no break */
			default: //match a single s character, if available
			{
				if(*slen<=0) return false;
				if(**p != **s) return false; //TEST
				--*plen; ++*p; --*slen; ++*s; //consume
				//continue
			}
				break;
		} //switch

	} //while pattern left to check

	//no more pattern left to check

	if(*slen>0) return false; //name left over unchecked

	return true;
}

/**
 * main osc matching function
 * matching an osc pattern to an osc name or path, both including leading '/'.
 * several sections of /.../.../... are allowed in both.
 *
 * it is recommended to call this function via @osc_matchPattern
 *
 * if match is successful, both the matched pattern (including possible [...], {...}, '*', "//" etc) and the matched name portion will be 'consumed', meaning that
 * the pointers will have advanced, the length information will be adjusted (decreased by that much too)
 * otherwise, the pointers will remain at the last matched position.
 *
 * @param p		pointer to char pointer for the pattern to match, keeps advancing while matching (starting with '/')
 * @param plen	pointer to the pattern length, keeps decrementing while matching (including leading '/')
 * @param s		pointer to char pointer for the name to match, keeps advancing while matching (starting with '/')
 * @param slen	pointer to the name length, keeps decrementing while matching (including leading '/')
 *
 * @return true if pattern matches the adress, false otherwise
 */
static bool osc_matchPatternImpl(const char** const p, int* const plen, const char** const s, int* const slen, unsigned int ctx)
{
	//make sure we start with '/' on both sides
	if(*plen<=0) return false;
	if(*slen<=0) return false;
	if(**p != '/') return false;
	if(**p != **s) return false; //TEST
	--*plen; ++*p; --*slen; ++*s; //consume
	//continue

	return osc_matchPattern0(p, plen, s, slen, ctx);
}

/**
 * main osc matching function
 * matching an osc pattern to an osc name or path, both including leading '/'.
 * several sections of /.../.../... are allowed in both.
 *
 * @param p		pointer to char pointer for the pattern to match, keeps advancing while matching (starting with '/')
 * @param plen	pointer to the pattern length, keeps decrementing while matching (including leading '/')
 * @param s		pointer to char pointer for the name to match, keeps advancing while matching (starting with '/')
 * @param slen	pointer to the name length, keeps decrementing while matching (including leading '/')
 *
 * @return true if pattern matches the adress, false otherwise
 */
bool osc_matchPattern(const char *pPatt, int pPattLen, const char *pName, int pNameLen)
{
	unsigned int ctx = 0; //for future use
	return osc_matchPatternImpl(&pPatt, &pPattLen, &pName, &pNameLen, ctx);
}

//// call in / glue code for tinyosc

#include <string.h>
#include "tinyosc_match.h"

bool tosc_matchAddress(tosc_message *o, const char* pcName) {
	const char* pcPatt = tosc_getAddress(o);
	int pLen = (int)strlen(pcPatt);
	int nLen = (int)strlen(pcName);

//	printf("OSC Matching> p: %s (%d) with %s (%d)\n", pcPatt, pLen, pcName, nLen);
	return osc_matchPattern(pcPatt, pLen, pcName, nLen);
}
