#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*Global definitions*/
#define LH +1
#define EH 0
#define RH -1

typedef struct node
{
	void*			day;
	void*			month;
	void*			year;
	void*			temp;
	struct node*	left;
	struct node*	right;
	int				bal;
} NODE;
typedef struct
{
	int		count;
	int		(*compare)	(void* argu1, void *argu2);
	NODE*	root;
} AVL_TREE;

/****************************************FUNCTION HEADERS**********************************/

/*functions marked with "CITE" were taken from
	Data Structures: A Pseudocode Approach with C, Second Edition*/

/***** prompts user for menu choice *****/
int printMenu ();

/***** tree print functions *****/
void printTree (NODE* tree);	//prints the tree in-order
void printLow (NODE* root);
void printHigh (NODE* root);	//prints the lowest and highest values

/***** Create a tree and allocate memory*****/
AVL_TREE* AVL_Create (int (*compare) (void* newTemp, void* storedTemp)); /*CITE*/
int compare (void *newTemp, void *storedTemp); 	//compare two integer values

/***** fill tree with data *****/
void fillTree(AVL_TREE *tree, char* line, int choice);
bool AVL_Insert (AVL_TREE* tree, void* day, void* month, void* year, void* temp);  /*CITE*/
NODE* _insert (AVL_TREE* tree, NODE* root, NODE* newPtr, bool *taller);  /*CITE*/

/***** rotate and balance after insertion******/
NODE *insLeftBal (NODE* root, bool* taller);  /*CITE*/
NODE *insRightBal (NODE* root, bool* taller);  /*CITE*/
NODE *rotateLeft (NODE* root);  /*CITE*/
NODE *rotateRight (NODE *root);  /*CITE*/
NODE* dltRightBal (NODE *root, bool*shorter);  /*CITE*/

/***** delete & free memory*****/
bool AVL_Delete (AVL_TREE* tree, void* dltKey);  /*CITE*/
NODE* _delete (AVL_TREE *tree, NODE *root, void* dltKey, bool* shorter, bool *success);  /*CITE*/
AVL_TREE *AVL_Destroy (AVL_TREE* tree);  /*CITE*/
void _destroy (NODE*root);   /*CITE*/

/***** other functions*****/
void* AVL_Retrieve (AVL_TREE* tree, void *keyPtr);  /*CITE*/
void* _retrieve (AVL_TREE *tree, void* keyPtr, NODE* root);  /*CITE*/
bool AVL_Empty (AVL_TREE* tree);  /*CITE*/

int main (void)
{
	setbuf(stdout, NULL); //for scanf

	/***** local declarations*****/
	FILE *fIn;
	int choice, month, day, year, temperature;	//Used to store the decision and values
	char temperatureArray[50], filename[50], getLine[50], dayArr[50], monthArr[50], yearArr[50];	//used to store data from the file
	AVL_TREE *tree;
	tree=AVL_Create (compare);
	bool fileLoaded=false; //used to check if file has already been loaded

	/***** loops until user terminates with 4 (quit)*****/
	while(1) //terminates when user enters 4 (quit)
	{
		choice = printMenu(); //get user option (1-4)

		switch (choice)
		{
		case 1:
			if(fileLoaded){
				printf("Error: Values from %s already stored.\n", filename);
				break; //prevents user from reading data twice and creating double entries
			}
			/***** get filename*****/
			puts("Enter the name of the file you would like to open: ");
			scanf("%s", filename);
			fIn = fopen( filename, "r" );
			if( fIn == NULL )
			{
				fprintf( stderr, "Error: cannot open %s\n", filename );
				exit(0);
			} //if NULL

			/***** get data *****/
			while(fgets(getLine, 50, fIn))
				fillTree(tree, getLine, choice);
			printf("%d temperatures were pulled from %s and stored in the AVL_Tree.\n", tree->count, filename);
			printf("\n***** Below is an in-order printout of the tree*****\n");
			printTree(tree->root);
			fileLoaded=true; //File has been loaded

			break;
		case 2:
			/***** get month *****/
			puts("Enter the month.");
			scanf("%d", &month);
			while (month<0 || month>12)
			{
				puts("Enter the month.");
				printf("Error: Month read as %d", month);
				scanf("%d", &month);			}

			/***** get day *****/
			puts("Enter the day.");
			scanf("%d", &day);
			while (day<0 || day >31)
			{
				printf("Error: day read as %d. Try again.", day);
				scanf("%d", &day);
			}

			/***** get year *****/
			puts("Enter the year.");
			scanf("%d", &year);
			while (year>2100 || year <1850) //arbitrary dates to ensure appropriate entries
			{
				printf("Error: year read as %d", year);
				puts("Enter the year.");
				scanf("%d", &year);
			}

			/***** get temperature *****/
			puts("Enter the temperature in F.");
			scanf("%d", &temperature);
			while (temperature>150 ||temperature< -460)
			{
				printf("Error: temperature read as %d. Try again\n", temperature);
				scanf("%d", &temperature);
			}

			/***** moves values to a string for fillTree function *****/
			itoa(day, dayArr, 10);
			itoa(month, monthArr, 10);
			itoa(year, yearArr, 10);
			itoa(temperature, temperatureArray, 10);
			strcpy(getLine, monthArr);
			strcat(getLine, "/");
			strcat(getLine, dayArr);
			strcat(getLine, "/");
			strcat(getLine, yearArr);
			strcat(getLine, ",");
			strcat(getLine, temperatureArray);
			fillTree(tree, getLine, choice);

			/***** print new values*****/
			printf("New temperature added to tree.\n");
			printf("\n***** Below is an in-order printout of the tree*****\n");
			printTree(tree->root);

			break;
		case 3:
			/***** prints lowest highest values*****/
			if (tree->count==0)
			{
				printf("Error no temperatures recorded yet.\n");
				break;
			}
			printf("\nThe lowest value found was:");
			printLow(tree->root);
			printf("The highest value found was:");
			printHigh(tree->root);

			break;
		case 4:
			/*****exits *****/
			puts("Thank you for using the program. Goodbye!");
			AVL_Destroy (tree);
			exit(1);

			break;
		default:
			printf("Error. limit your choice to 1-4. Try again:\n");
			scanf("%d", &choice);
		}//switch(choice)
	}//while (1)
	return 0;
}//main

void fillTree(AVL_TREE *tree, char* line, int choice)
{
	/***** declarations *****/
	int count, count2;	//used exclusively to count loops
	char temp[50];		//temporarily stores values for atoi function
	int *temperature, *day, *month, *year;

	/***** memory allocations *****/
	temperature=(int*) malloc (sizeof(int));
	day=(int*) malloc (sizeof(int));
	month=(int*) malloc (sizeof(int));
	year=(int*) malloc (sizeof(int));

	/***** reads month and stores in int month*****/
	for (count=0; line[count]!='/'; count++)
		temp[count]=line[count];
	*month = atoi(temp);
	count++; //move past the line to the next integer

	/***** reads day and stores in int day*****/
	for (count2=0;line[count]!='/'; count++, count2++)
		temp[count2]=line[count];
	temp[count2+1]='\0'; //delete any remaining values in temp this is necessary for dates with two digit month
						 //and one digit day Example: 12/1/2018.
	*day=atoi(temp);	 //store integer found as the day
	count++; // move past the second slash to the first integer in the year

	/***** reads year and stores in int year*****/
	for (count2=0;line[count]!=','; count++, count2++) //csv=comma separated values. Comma means new column
		temp[count2]=line[count];
	*year=atoi(temp);
	count++; //move past the comma to the temperature in the next column

	/***** reads temperature and stores in int temperature*****/
	for (count2=0;line[count]!='\0';count++, count2++)
		temp[count2]=line[count]; //store temperature in a temp array
	temp[count2]='\0'; //place an endpoint for the string
	*temperature=atoi(temp);

	/***** Insert values found into tree*****/
	AVL_Insert (tree, day, month, year, temperature);
}//fillTree

int printMenu ()
{
	/***** declarations*****/
	int choice; //used to store user decision

	/***** loop to read user decision*****/
	while (choice<1 || choice >4)
	{
		puts("1. Enter the name of the file.");
		puts("2. Enter a new date and a temperature into the AVL tree.");
		puts("3. Request the highest and the lowest temperatures in the tree.");
		puts("4. Quit.");
		scanf("%d", &choice);

		if (choice<1 || choice >4)
			printf("Your choice was read as %d. Please limit your choice to 1-4.\n", choice);
	}//loop choice

	return choice;
}//printMenu

AVL_TREE* AVL_Create (int (*compare) (void* argu1, void* argu2))
{
	/***** declarations *****/
	AVL_TREE *tree;

	/**** memory allocation*****/
	tree = (AVL_TREE*) malloc (sizeof(AVL_TREE));

	/***** set node values *****/
	if (tree){
		tree->root		= NULL;
		tree->count		= 0;
		tree->compare	= compare;
	} //end if NULL

	return tree;
}

int compare (void *newTemp, void *storedTemp)
{
	int result = *((int*)newTemp) - *((int*)storedTemp);
	return result;
}

bool AVL_Insert (AVL_TREE* tree, void* day, void* month, void* year, void* temp)
{
	/***** declarations *****/
	NODE *newPtr;
	bool forTaller;

	/***** memory allocation *****/
	newPtr = (NODE*) malloc(sizeof(NODE));
	if (!newPtr){
		printf("Error allocating memory in function AVL_Insert");
		exit(100);
	}//if malloc fail
	/***** set node values *****/
	newPtr->bal 	= EH;
	newPtr->right	= NULL;
	newPtr->left	= NULL;
	newPtr->temp	= temp;
	newPtr->day		= day;
	newPtr->month	= month;
	newPtr->year	= year;

	/*****call insert function *****/
	tree->root = _insert(tree, tree->root, newPtr, &forTaller);
	(tree->count)++;

	return true;
}//AVL_Insert

NODE* _insert (AVL_TREE* tree, NODE* root, NODE* newPtr, bool *taller)
{
	/***** leaf found insert values *****/
	if(!root)
	{
		root = newPtr;
		*taller = true;
		return root;
	}//!root

	/***** new temperature is less than root ... go left*****/
	if (tree->compare(newPtr->temp, root->temp)<0)
	{
		root->left = _insert(tree, root->left, newPtr, taller);
		if (*taller)
			//left subtree is now taller
			switch (root->bal)
			{
			case LH: //left high -- rotate
				root = insLeftBal (root, taller);
				break;
			case EH:
				root->bal = LH;
				break;
			case RH:
				root->bal = EH;
				*taller = false;
				break;
			} //end switch
		return root;
	}// go left
	/***** else go right *****/
	else
	{
		root->right = _insert(tree, root->right, newPtr, taller);
		if (*taller)
			switch (root->bal)
			{
			case LH:
				root->bal = EH;
				*taller = EH;
				*taller = false;
				break;
			case EH:
				root->bal = RH;
				break;
			case RH:
					root = insRightBal (root, taller);
					break;
			}
		return root;
	}
	return root;
}

NODE *insLeftBal (NODE* root, bool* taller)
{
	/***** declarations *****/
	NODE *rightTree;
	NODE *leftTree;

	leftTree = root->left;
	switch (leftTree->bal)
	{
	case LH:
		root->bal		= EH;
		leftTree->bal	= EH;

		root 		= rotateRight(root);
		*taller		= false;
		break; //if left high rotate right
	case EH:
		break; //do nothing
	case RH:
		rightTree = leftTree->right;
		switch (rightTree->bal)
		{
		case LH:
			root->bal		= RH;
			leftTree->bal	= LH;
			break;
		case RH:
			root->bal		= EH;
			leftTree->bal	= LH;
			break;
		} //if right high double rotate
		rightTree->bal = EH;
		root->left = rotateLeft (leftTree);
		root = rotateRight (root);
		*taller = false;
	}
	return root;
}

NODE *insRightBal (NODE* root, bool* taller){
	NODE *rightTree;
	NODE *leftTree;

	rightTree=root->right;
	switch (rightTree->bal)
	{
	case LH:
		leftTree = rightTree->left;
		switch (leftTree->bal)
		{
		case RH:
			root->bal = LH;
			rightTree->bal = EH;
			break;
		case EH:
			root->bal = EH;
			rightTree->bal = EH;
			break;
		case LH:
			root->bal = EH;
			rightTree->bal = RH;
			break;
		}
		leftTree->bal = EH;
		root->right = rotateRight(rightTree);
		root=rotateLeft(root);
		*taller = false;
		break;
	case EH:
		break;
	case RH:
		root->bal = EH;
		rightTree->bal = EH;
		root = rotateLeft (root);
		*taller = false;
		break;
	}
	return root;
}

NODE *rotateLeft (NODE* root){
	NODE *tempPtr;

	tempPtr			= root->right;
	root->right		= tempPtr->left;
	tempPtr->left	= root;
	return tempPtr;
}

NODE *rotateRight (NODE *root){
	NODE *tempPtr;

	tempPtr			= root->left;
	root->left		= tempPtr->right;
	tempPtr->right	= root;
	return tempPtr;
}

bool AVL_Delete (AVL_TREE* tree, void* dltKey){
	bool shorter;
	bool success;
	NODE* newRoot;

	newRoot = _delete (tree, tree->root, dltKey, &shorter, &success);

	if (success)
	{
		tree->root = newRoot;
		(tree->count)--;
		return true;
	}
	else
		return false;
}

NODE* _delete (AVL_TREE *tree, NODE *root, void* dltKey, bool* shorter, bool *success){
	NODE* dltPtr;
	NODE* exchPtr;
	NODE* newRoot;

	if(!root)
	{
		*shorter = false;
		*success = false;
		return NULL;
	}

	if (tree->compare(dltKey, root->temp) <0)
	{
		root->left = _delete (tree, root->left, dltKey, shorter, success);
		if (*shorter)
			root = dltRightBal (root, shorter);
	}

	else
	{
		dltPtr = root;
		if (!root->right){
			newRoot = root->left;
			*success = true;
			*shorter = true;
			free (dltPtr);
			return newRoot;
		}
		else if (!root->left)
		{
			newRoot = root->right;
			*success = true;
			*shorter = true;
			free (dltPtr);
			return newRoot;
		}
		else
		{
			exchPtr = root->left;
			while (exchPtr->right)
				exchPtr = exchPtr->right;
			root->temp = exchPtr->temp;
			root->left = _delete(tree, root->left, exchPtr->temp, shorter, success);
			if (*shorter)
				root=dltRightBal(root, shorter);
		}
	}
	return root;
}
NODE* dltRightBal (NODE *root, bool*shorter)
{
	NODE *rightTree;
	NODE *leftTree;

	switch (root->bal)
		{
		case LH:
			root->bal = EH;
			break;
		case EH:
			root->bal = RH;
			*shorter = false;
			break;
		case RH:
			rightTree = root->right;
			if (rightTree->bal == LH)
			{
				leftTree	= rightTree->left;

				switch (leftTree->bal)
					{
					case LH:
						rightTree->bal 	= RH;
						root->bal		= EH;
						break;
					case EH:
						root->bal		= EH;
						rightTree->bal	= EH;
						break;
					case RH:
						root->bal		= LH;
						rightTree->bal	= EH;
						break;
					}

				leftTree->bal = EH;

				root->right = rotateRight (rightTree);
				root 		= rotateLeft (root);
			}
			else
			{
				switch (rightTree->bal)
					{
					case LH:
					case RH:
						root->bal		= EH;
						rightTree->bal	= EH;
					case EH:
						root->bal		= RH;
						rightTree->bal	= LH;
						*shorter		= false;
						break;
					}
				root = rotateLeft (root);
			}
		}
	return root;
}
void* AVL_Retrieve (AVL_TREE* tree, void *keyPtr){
	if (tree->root)
		return _retrieve (tree, keyPtr, tree->root);
	else
		return NULL;
}
void *_retrieve (AVL_TREE *tree, void* keyPtr, NODE* root){
	if (root)
	{
		if (tree->compare(keyPtr, root->temp) < 0)
			return _retrieve(tree, keyPtr, root->left);
		else if (tree->compare(keyPtr, root->temp)>0)
			return _retrieve(tree, keyPtr, root->right);
		else
			return root->temp;
	}
	else
		return NULL;
}
bool AVL_Empty (AVL_TREE* tree){
	return(tree->count==0);
}
void printTree (NODE* root){
	if (root)
	{
		printTree(root->left);
		printf("%d/%d/%d: %d\n",*((int*)root->month), *((int*)root->day), *((int*)root->year), *((int*)root->temp));
		printTree(root->right);
	}
}
void printLow (NODE* root){
	if (root)
	{
		printLow(root->left);
		if (!root->left)
			printf("%d/%d/%d: %d\n",*((int*)root->month), *((int*)root->day), *((int*)root->year), *((int*)root->temp));
	}
}
void printHigh (NODE* root){
	if (root)
	{
		printHigh(root->right);
		if (!root->right)
			printf("%d/%d/%d: %d\n",*((int*)root->month), *((int*)root->day), *((int*)root->year), *((int*)root->temp));
	}
}
AVL_TREE *AVL_Destroy (AVL_TREE* tree){
	if (tree)
		_destroy (tree->root);
	free(tree);
	return NULL;
}
void _destroy (NODE*root){
	if (root)
	{
		_destroy(root->left);
		free(root->temp);
		free(root->day);
		free(root->month);
		free(root->year);
		_destroy (root->right);
		free(root);
	}
	return;
}
