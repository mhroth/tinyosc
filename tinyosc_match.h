/*
 * tinyosc_match.h
 *
 *  Created on: May 17, 2022
 *      Author: master
 */

#ifndef MAIN_TINYOSC_MATCH_H_
#define MAIN_TINYOSC_MATCH_H_

#include <stdbool.h>
#include "tinyosc.h"

bool tosc_matchAddress(tosc_message *o, const char* pcName);

#endif /* MAIN_TINYOSC_MATCH_H_ */
