/* --------------------------- Helper methods ---------------------------- */
#define CALLOC(nmemb, type) (type *) calloc(nmemb, sizeof(type))
#define TRY(premise, condition, value) \
  { premise; if (condition) { return value; } }



