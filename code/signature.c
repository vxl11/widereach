#include <math.h>

#include "widereach.h"

void set_signature(
        node_signature_t *signature,
        int level, 
        int primary,
        double bound,
        int seqno) {
    signature->level = level;
    signature->primary = primary;
    signature->bound = bound;
    signature->seqno = seqno;
}

void copy_signature(node_signature_t *dest, const node_signature_t *src) {
    set_signature(dest, src->level, src->primary, src->bound, src->seqno);
}

int compare_signature(const node_signature_t *a, const node_signature_t *b) {
    int difference;
    difference = a->primary - b->primary;
    // difference = b->primary - a->primary;
    if (difference) {
        return difference;
    }
    // difference = a->level - b->level;
    difference = b->level - a->level;
    if (difference) {
        return difference;
    }
    difference = (int) round(a->bound - b->bound);
    if (difference) {
        return difference;
    }
    return a->seqno - b->seqno;
    // return b->seqno - a->seqno;
}
    
    
