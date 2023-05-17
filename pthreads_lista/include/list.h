
// Define the max size of the string
enum {
	MAX_SIZE_STRING = 256
};

// ------------- Creat -----------------
struct Creat {
	long long id;		// Acts as key
	char specie[MAX_SIZE_STRING];
	int height;
	int weight;
	char color[MAX_SIZE_STRING];

	struct Creat *next;
	struct Creat *prev;
};

/**
 * @brief Creates a new instance of Creat
 * 
 * @param id long long 
 * @param specie string of max size 256
 * @param height int
 * @param weight int
 * @param color string of max size 256
 * @return struct Creat 
 */
struct Creat *newCreat(long long id, char *specie, int height,
		       int weight, char *color);

/**
 * @brief Prints the creat struct
 * 
 * @param creat Pointer to creat 
 */
void printCreat(struct Creat *creat);

// -------------------------------------

// -------------- List -----------------
struct List {
	struct Creat *head;
	struct Creat *last;
	pthread_mutex_t lock;
};

/**
 * @brief Create an empty List struct
 * 
 * @return struct List 
 */
struct List *createEmptyList();

/**
 * @brief Frees the memory of the list
 * 
 * @param list List to free
 * @return 0 = empty / 1 = Succesful
 */
int freeList(struct List *list);

/**
 * @brief Returns if the list is empty
 * 
 * @param list 
 * @return 0 = false / 1 = true
 */
int isEmptyList(struct List *list);

/**
 * @brief Prints the contents of the list
 * 
 * @param list 
 */
void printList(struct List *list);

/**
 * @brief Inserts Creat at the beginnning of the list
 * 
 * @param list 
 * @param data Of type Creat *
 * @return -1 = Malloc failed / 1 = Success
 */
int insertFirst(struct List *list, struct Creat *data);

/**
 * @brief Inserts Creat at the end of the list
 * 
 * @param list 
 * @param data Of type Creat *
 * @return -1 = Malloc failed / 1 = Success
 */
int insertLast(struct List *list, struct Creat *data);

/**
 * @brief Get the First Creat of the list
 * 
 * @param list 
 * @return struct Creat* 
 */
struct Creat *getFirst(struct List *list);

/**
 * @brief Get the Last Creat of the list
 * 
 * @param list 
 * @return struct Creat* 
 */
struct Creat *getLast(struct List *list);

/**
 * @brief Get the First Creat of the list
 * 
 * @param list 
 * @return struct Creat* 
 */
struct Creat *extractfirst(struct List *list);

/**
 * @brief Get the Last Creat of the list
 * 
 * @param list 
 * @return struct Creat* 
 */
struct Creat *extractlast(struct List *list);

/**
 * @brief Gets the length of the list
 * 
 * @param list 
 * @return int: Length
 */
int lengthList(struct List *list);

/**
 * @brief Finds the Creat in the list
 * 
 * @param list 
 * @param key Also know as Creat id
 * @return struct Creat* = Succes / NULL = Failed
 *
 * @note If found more than 1 match return NULL
 */
struct Creat *find(struct List *list, long long key);

/**
 * @brief Deletes the Creat in list that matches the key
 * 
 * @param list 
 * @param key Also know as Creat id
 * @return 0 = Failed / 1 = Success 
 */
int delete(struct List *list, long long key);

// -------------------------------------
