#include <math.h>

#include "widereach.h"

void set_signature(
        node_signature_t *signature,
        int level, 
        int seqno) {
    signature->level = level;
    signature->seqno = seqno;
}

void copy_signature(node_signature_t *dest, const node_signature_t *src) {
    set_signature(dest, src->level, src->seqno);
}

int compare_signature(const node_signature_t *a, const node_signature_t *b) {
    int difference = a->level - b->level;
    if (difference) {
        return difference;
    }
    return a->seqno - b->seqno;
}
    
    
