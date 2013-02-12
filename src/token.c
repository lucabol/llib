#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "str.h"
#include "assert.h"

tokenizer_t tokenizer(char* s, const char* delimiters, unsigned empties) {
    tokenizer_t result ;
    assert(s);
    assert(delimiters);
    assert(empties == TOKENIZER_EMPTIES_OK || empties == TOKENIZER_NO_EMPTIES);
    assert(*delimiters != '\0');

    result.s                 = s;
    result.delimiters        = delimiters;
    result.current           = NULL;
    result.next              = result.s;
    result.is_ignore_empties = (empties != TOKENIZER_EMPTIES_OK);

    return result;
}

char* tokenize(tokenizer_t* tokenizer) {
    assert(tokenizer);
    assert(tokenizer->s);

    if (!tokenizer->next)   return NULL;

    tokenizer->current  = tokenizer->next;
    tokenizer->next     = strpbrk(tokenizer->current, tokenizer->delimiters );

    if (tokenizer->next) {
        *tokenizer->next = '\0';
        tokenizer->next += 1;

        if (tokenizer->is_ignore_empties) {
            tokenizer->next += strspn(tokenizer->next, tokenizer->delimiters);
            if (!(*tokenizer->current))
                return tokenize( tokenizer );
        }
    }
    else if (tokenizer->is_ignore_empties && !(*tokenizer->current))
        return NULL;

    return tokenizer->current;
}



