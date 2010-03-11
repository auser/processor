#ifndef COMB_QUEUE_H
#define COMB_QUEUE_H

typedef void (*queueNodefreeFun)(void *);

typedef struct _comb_queue_node_t {
  void *data;
  struct comb_queue *next;
  queueNodefreeFun free_fun;
} comb_queue_node;

typedef struct _comb_queue_t {
  size_t count;
  comb_queue_node *head;
  comb_queue_node *tail;
} comb_queue;

comb_queue *newQueue(void);
int   queuePush(comb_queue *q, void *data, queueNodefreeFun f);
void *queuePop(comb_queue *q);
void  queueFree(comb_queue *q);

#endif