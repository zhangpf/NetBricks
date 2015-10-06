#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <dpdk.h>
#include <rte_config.h>
#include <rte_lcore.h>
#include <pthread.h>

struct node {
	int tid;
	int core;
};

uint64_t fib (uint64_t l)
{
	uint64_t a = 0, b = 1;
	while (b < l) {
		int temp = a;
		a = b;
		b = a + temp;
	}
	return b;
}

void *thr(void* arg)
{
	struct node* n = arg;
	struct rte_mbuf* restrict pkts[32];
	int i;
	init_thread(n->tid, n->core);
	for (int j = 0; j < 100000; j++) {
		i = mbuf_alloc_bulk(pkts, 512, 32);
		if (i != 0) {
			printf("Error allocating packets %d\n", i);
			break;
		} else {
			i = mbuf_free_bulk(pkts, 32);
			if (i != 0) {
				printf("Error freeing packets %d\n", i);
				break;
			}
		}
	}
	printf("Socket ID (%d) is %d. DONE\n", n->core, rte_socket_id());
	return NULL;
}

int main (int argc, char* argv[]) {

	pthread_t thread[20];
	struct node n[20];
	int ret = init_system(0);

	assert(ret == 0);
	for (int i = 0; i < 20; i++) {
		n[i].tid = 64 - i;
		n[i].core = i;
		pthread_create(&thread[i],
				NULL,
				&thr,
				&n[i]);
	}

	for (int i = 0; i < 20; i++) {
		pthread_join(thread[i], NULL);
	}
	return 0;
}