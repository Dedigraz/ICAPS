#define BLANK_SKEW 0
#define DEFAULT_MOTOR_SPEED 23

typedef struct LatLng{
  double lat;
  double lng;
};

enum AnomalyType {
  POTHOLE,
  SPEEDBUMP,
  COLLISION,
  ACCIDENT_SCENE,
  CRACK,
  MANHOLE
};

typedef struct Anomaly{
float x; // x offset of the anomaly  
  float width;
  float height;
  enum AnomalyType type;
  float vibration;
  LatLng position;
};

typedef struct QNode{
  Anomaly* data;
  QNode* next;
};

typedef struct Queue{
  QNode *Head;
  QNode *Tail;
};


// Function to create a queue
Queue* createQueue()
{
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->Head = q->Tail = NULL;
    return q;
}

// Function to check if the queue is empty
int isEmpty(Queue* q)
{

    // If the front and Tail are null, then the queue is
    // empty, otherwise it's not
    if (q->Head == NULL && q->Tail == NULL) {
        return 1;
    }
    return 0;
}
QNode* createNode(Anomaly new_data){
  return nullptr;
}
// Function to add an element to the queue
void enqueue(Queue* q, Anomaly new_data)
{

    // Create a new linked list node
    QNode* new_node = createNode(new_data);

    // If queue is empty, the new node is both the front
    // and Tail
    if (q->Tail == NULL) {
        q->Head = q->Tail = new_node;
        return;
    }

    // Add the new node at the end of the queue and
    // change Tail
    q->Tail->next = new_node;
    q->Tail = new_node;
}

// Function to remove an element from the queue
void dequeue(Queue* q)
{

    // If queue is empty, return
    if (isEmpty(q)) {
        printf("Queue Underflow\n");
        return;
    }

    // Store previous front node and moves front one node
    // ahead
    QNode* temp = q->Head;
    q->Head = q->Head->next;

    // If front becomes null, then change Tail also
    // to null
    if (q->Head == NULL)
        q->Tail = NULL;

    // Deallocate memory of the old front node
    free(temp);
}

// Function to get the front element of the queue
Anomaly* getFront(Queue* q)
{

    // Checking if the queue is empty
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return {0};
    }
    return q->Head->data;
}

// Function to get the Tail element of the queue
Anomaly* getRear(Queue* q)
{

    // Checking if the queue is empty
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return {0};
    }
    return q->Tail->data;
}

bool operator>(LatLng a, LatLng b){
  return true;
}