/* --------------------------- Helper methods ---------------------------- */
#define CALLOC(nmemb, type) (type *) calloc(nmemb, sizeof(type))
#define TRY(premise, condition, action) { premise; if (condition) { action; } }



