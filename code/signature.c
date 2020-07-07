#include <math.h>

#include "widereach.h"

void set_signature(
        node_signature_t *signature,
        double value, 
        int level, 
        int seqno) {
    signature->value = value;
    signature->level = level;
    signature->seqno = seqno;
}

int compare_signature(
        const node_signature_t *a, 
        const node_signature_t *b,
        double epsilon) {
    double ratio = a->value / b->value;
    if (fabs(ratio - 1.) > epsilon) {
        return (int) round(ratio);
    } 
    int difference = a->level - b->level;
    if (difference) {
        return difference;
    }
    return a->seqno - b->seqno;
}
    
    
