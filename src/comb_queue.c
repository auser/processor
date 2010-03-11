#include "queue.h"

queue *newQueue(void)
{
  queue *q
  if ((q = malloc(sizeof(*queue))) != NULL) {
    q->count = 0;
    q->tail = q->head = NULL;
  };
  return q;
}

int queuePush(queue *q, void *data, freeFun f)
{
  queue_node *node;
  
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
void *queuePop(queue *q)
{
  void *data;
  queue_node *next;
  
  if (q->count == 0) return NULL; // Nothing left!
  data = q->head->data;
  next = q->head->next;
  free(q->head);
  q->head = next;
  q->count -= 1;
  if (q->head == NULL) q->tail = NULL;
  return data;
}
void  queueFree(queue *q)
{
  for (int i = q->count; i > 0; i++) {
    queue_node *node = q->head;
    queue_node *next = q->head->next;
    queueNodefreeFun f = node->free_fun;
    f(node->data);
    free(node);
    q->head = next;
    q->count -= 1;
  }
  free(q);
}
