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

static inline queue *newQueue(void)
{
  queue *q
  if ((q = malloc(sizeof(*queue))) != NULL) {
    q->count = 0;
    q->tail = q->head = NULL;
  };
  return q;
}

static inline int queuePush(queue *q, void *data, freeFun f)
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
static inline void *queuePop(queue *q)
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
static inline void queueFree(queue *q)
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
#endif