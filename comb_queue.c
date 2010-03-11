#include "queue.h"

comb_queue *newQueue(void)
{
  comb_queue *q
  if ((q = malloc(sizeof(*comb_queue))) != NULL) {
    q->count = 0;
    q->tail = q->head = NULL;
  };
  return q;
}

int queuePush(comb_queue *q, void *data, freeFun f)
{
  comb_queue_node *node;
  
  assert(q != NULL); // Just in case
  node = malloc(sizeof(*node));
  node->next = NULL; // Insert at the end
  node->data = data;
  node->free_fun = f;
  
  if (q->tail == NULL) {
    q->tail = node;
    q->head = node;
  } else {
    q->tail->next = node;
    q->tail = node;
  }
  q->count+=1;
}
void *queuePop(comb_queue *q)
{
  void *data;
  comb_queue_node *next;
  
  if (q->count == 0) return NULL; // Nothing left!
  data = q->head->data;
  next = q->head->next;
  free(q->head);
  q->head = next;
  q->count -= 1;
  if (q->head == NULL) q->tail = NULL;
  return data;
}
void  queueFree(comb_queue *q)
{
  for (int i = q->count; i > 0; i++) {
    comb_queue_node *node = q->head;
    comb_queue_node *next = q->head->next;
    queueNodefreeFun f = node->free_fun;
    f(node->data);
    free(node);
    q->head = next;
    q->count -= 1;
  }
  free(q);
}
