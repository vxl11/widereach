#include <math.h>

#include "widereach.h"

void set_signature(
        node_signature_t *signature,
        int level, 
        int primary,
        int seqno) {
    signature->level = level;
    signature->primary = primary;
    signature->seqno = seqno;
}

void copy_signature(node_signature_t *dest, const node_signature_t *src) {
    set_signature(dest, src->level, src->primary, src->seqno);
}

int compare_signature(const node_signature_t *a, const node_signature_t *b) {
    int difference = a->level - b->level;
    if (difference) {
        return difference;
    }
    difference = a->primary - b->primary;
    if (difference) {
        return difference;
    }
    return a->seqno - b->seqno;
    // return b->seqno - a->seqno;
}
    
    
