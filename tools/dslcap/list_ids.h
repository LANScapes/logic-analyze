/* Read-only cached-registry identity listing, GPL-3.0-or-later (as dslcap). */
#ifndef DSLCAP_LIST_IDS_H
#define DSLCAP_LIST_IDS_H
/* Prints exactly one {"devices":[...]} object; diagnostics use stderr.
 * 0: complete inventory, 1: unavailable backend/incomplete identity/I/O error. */
int dslcap_list_ids(void);
#endif
