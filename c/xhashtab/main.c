#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xhashtab.h"

static DEFINE_HASHTABLE(phandle_ht, 3);

char test[] = "123456789abcdef";

int main()
{

	int i = 0;
	HASH_ELM *pelm = NULL;
	unsigned int bkt = 0;
	unsigned int count = 0;

	for (i = 0; i < 16; i++) {
		HASH_ELM *pelm = malloc(sizeof(HASH_ELM));

		pelm->val = (void *)&test[i];
		pelm->key = (void *)&test[i];

		HASH_ADD(phandle_ht, pelm);

	}

	HASH_ELM *pt = HASH_FIND(phandle_ht, &test[7]);
	printf("find test: \t %p, %c\n", pt->key, *(char *)pt->val);

/*
		HASH_FOREACH(phandle_ht, bkt, pelm){
			printf("bucket: %d, %p, %c, %d\n", bkt, pelm->key, *(char *)pelm->val, phandle_ht[bkt].count);
		}
		*/

	HASH_FOREACH_BKT(phandle_ht, bkt){
		count += phandle_ht[bkt].count;
		printf("bucket: %d, count: %d\n", bkt, phandle_ht[bkt].count);

		HASH_FOREACH_ELM(&phandle_ht[bkt], pelm){
			printf("\t %p, %c\n", pelm->key, *(char *)pelm->val);
			HASH_DEL(phandle_ht, pelm);
			free(pelm);
		}
	}

	printf("total count: %d\n", count);
	printf("%d----------------\n", HASH_EMPTY(phandle_ht));

	return 0;
}
