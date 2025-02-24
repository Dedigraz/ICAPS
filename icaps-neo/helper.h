// Add these variables at the top with other defines
#define MOVE_NOT_STARTED 0
#define MOVE_IN_PROGRESS 1
#define MOVE_COMPLETED 2
#define BLANK_SKEW 0
#define DEFAULT_MOTOR_SPEED 50
#define USE_SERIAL Serial
#define MOVE_PAUSED 3
#define EMERGENCY_MOVE_DURATION 2000 // 2 seconds for emergency moves

const unsigned int max_input = 300;

#define LEFT_MOTOR_IN1 25
#define LEFT_MOTOR_IN2 26
#define RIGHT_MOTOR_IN1 18
#define RIGHT_MOTOR_IN2 5

// Example PWM pins (ESP32)
#define LEFT_MOTOR_PWM 14
#define RIGHT_MOTOR_PWM 12
#define MOTOR_FREQ 20000
#define MOTOR_RESOLUTION 8
#define LEFT_MOTOR_CHANNEL 0
#define RIGHT_MOTOR_CHANNEL 1

typedef struct LatLng
{
  double lat;
  double lng;
};

enum AnomalyType
{
  VIBRATION,
  CRACK,
  SPEEDBUMP,
  POTHOLE,
  FIRE
};

typedef struct Anomaly
{
  float x; // x offset of the anomaly
  float width;
  float height;
  enum AnomalyType type;
  float vibration;
  LatLng position;
};

struct Move
{
  float speed;
  float skew;
  bool reverse;
  long duration;
};

typedef struct QNode
{
  Move *data; // Changed from Anomaly* to Move*
  struct QNode *next;
} QNode;

typedef struct Queue
{
  QNode *Head;
  QNode *Tail;
} Queue;

// Function to create a queue
Queue *createQueue()
{
  Queue *q = (Queue *)malloc(sizeof(Queue));
  q->Head = q->Tail = NULL;
  return q;
}

// Function to check if the queue is empty
int isEmpty(Queue *q)
{

  // If the front and Tail are null, then the queue is
  // empty, otherwise it's not
  if (q->Head == NULL && q->Tail == NULL)
  {
    return 1;
  }
  return 0;
}

// Updated createNode function
QNode *createNode(Move *new_data)
{
  QNode *node = (QNode *)malloc(sizeof(QNode));
  if (node == NULL)
  {
    return NULL;
  }
  node->data = new_data;
  node->next = NULL;
  return node;
}

// Updated enqueue function
void enqueue(Queue *q, Move *new_data)
{
  QNode *new_node = createNode(new_data);

  if (new_node == NULL)
  {
    return;
  }

  if (q->Tail == NULL)
  {
    q->Head = q->Tail = new_node;
    return;
  }

  q->Tail->next = new_node;
  q->Tail = new_node;
}

// Function to remove an element from the queue
void dequeue(Queue *q)
{

  // If queue is empty, return
  if (isEmpty(q))
  {
    printf("Queue Underflow\n");
    return;
  }

  // Store previous front node and moves front one node
  // ahead
  QNode *temp = q->Head;
  q->Head = q->Head->next;

  // If front becomes null, then change Tail also
  // to null
  if (q->Head == NULL)
    q->Tail = NULL;

  // Deallocate memory of the old front node
  free(temp);
}

// Updated getFront function
Move *getFront(Queue *q)
{
  if (isEmpty(q))
  {
    printf("Queue is empty\n");
    return NULL;
  }
  return q->Head->data;
}

// Updated getRear function
Move *getRear(Queue *q)
{
  if (isEmpty(q))
  {
    printf("Queue is empty\n");
    return NULL;
  }
  return q->Tail->data;
}

bool operator>(LatLng a, LatLng b)
{
  return true;
}

// Add these function declarations after the existing Queue functions
void executeMove(Move *move);
void processCurrentMove(Queue *q, unsigned long currentTime);
void insertEmergencyMove(Queue *q, Move *emergencyMove);

void clearQueue(Queue *q)
{
  while (!isEmpty(q))
  {
    dequeue(q);
  }
}

void makeSquarePattern(Queue *q, float sideLength, float speed)
{
  // Clear existing moves
  clearQueue(q);

  // Create moves for each side of the square
  for (int i = 0; i < 4; i++)
  {
    Move *straightMove = (Move *)malloc(sizeof(Move));
    Move *turnMove = (Move *)malloc(sizeof(Move));

    // Straight movement
    straightMove->speed = speed;
    straightMove->skew = BLANK_SKEW;
    straightMove->reverse = false;
    straightMove->duration = sideLength * 1000; // Convert to milliseconds

    // 90-degree turn
    turnMove->speed = speed / 2; // Slower for turning
    turnMove->skew = 90;         // Full turn
    turnMove->reverse = false;
    turnMove->duration = 1000; // 1 second for turn

    // Add moves to queue
    enqueue(q, straightMove);
    enqueue(q, turnMove);
  }
}

// Add these implementations before the end of the file
unsigned long moveStartTime = 0;
int moveStatus = MOVE_NOT_STARTED;

void executeMove(Move *move)
{
  USE_SERIAL.print("Executing move: Speed=");
  USE_SERIAL.print(move->speed);
  USE_SERIAL.print(" Skew=");
  USE_SERIAL.print(move->skew);
  USE_SERIAL.print(" Duration=");
  USE_SERIAL.println(move->duration);

  // Convert Move's speed to 8-bit duty
  int duty = (int)constrain(move->speed, 0, 255);

  // Basic left/right turn handling based on skew sign
  bool goingLeft = (move->skew < 0);
  bool goingRight = (move->skew > 0);

  // Determine forward/reverse logic
  if (!move->reverse) {
    // Forward
    digitalWrite(LEFT_MOTOR_IN1, HIGH);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_MOTOR_IN1, HIGH);
    digitalWrite(RIGHT_MOTOR_IN2, LOW);
  } else {
    // Reverse
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, HIGH);
    digitalWrite(RIGHT_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  }

  // Example: lower duty on one motor to steer
  if (goingLeft) {
    ledcWrite(LEFT_MOTOR_CHANNEL, duty / 3);  // reduce left motor
    ledcWrite(RIGHT_MOTOR_CHANNEL, duty);
  } else if (goingRight) {
    ledcWrite(LEFT_MOTOR_CHANNEL, duty);
    ledcWrite(RIGHT_MOTOR_CHANNEL, duty / 3); // reduce right motor
  } else {
    // Straight
    ledcWrite(LEFT_MOTOR_CHANNEL, duty);
    ledcWrite(RIGHT_MOTOR_CHANNEL, duty);
  }
}

void processCurrentMove(Queue *q, unsigned long currentTime)
{
  if (isEmpty(q))
  {
    // If queue is empty, restart the square pattern
    makeSquarePattern(q, 5.0, DEFAULT_MOTOR_SPEED);
    return;
  }

  Move *currentMove = getFront(q);

  switch (moveStatus)
  {
  case MOVE_NOT_STARTED:
    moveStartTime = currentTime;
    executeMove(currentMove);
    moveStatus = MOVE_IN_PROGRESS;
    break;

  case MOVE_IN_PROGRESS:
    if (currentTime - moveStartTime >= currentMove->duration)
    {
      dequeue(q);
      moveStatus = MOVE_NOT_STARTED;
      USE_SERIAL.println("Move completed");
      
      // If this was the last move, restart the pattern
      if (isEmpty(q))
      {
        USE_SERIAL.println("Restarting square pattern");
        makeSquarePattern(q, 5.0, DEFAULT_MOTOR_SPEED);
      }
    }
    break;

  case MOVE_PAUSED:
    // Do nothing while paused
    break;
  }
}

void insertEmergencyMove(Queue *q, Move *emergencyMove)
{
  // Save current head
  QNode *oldHead = q->Head;

  // Create new node for emergency move
  QNode *emergencyNode = createNode(emergencyMove);

  if (emergencyNode == NULL)
  {
    return;
  }

  // Insert at head
  emergencyNode->next = oldHead;
  q->Head = emergencyNode;

  // If queue was empty, set tail
  if (q->Tail == NULL)
  {
    q->Tail = emergencyNode;
  }

  // Set move status to not started to trigger immediate execution
  moveStatus = MOVE_NOT_STARTED;
}
