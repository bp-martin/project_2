/* Project 2 - Multithreaded calculator */
// Name: Martin Garza
// Worked with Raul Villasana

#include "calc.h"

pthread_t adderThread;
pthread_t degrouperThread;
pthread_t multiplierThread;
pthread_t readerThread;
pthread_t sentinelThread;

char buffer[BUF_SIZE];
int num_ops;

/* Step 3: add mutual exclusion */
int b_len;

/* Step 6: add condition flag varaibles */
int checkProgress = 1;
int checkAddition = 1;
int checkMultiplication = 1;
int checkDegrouper = 1;

/* Step 7: use a semaphore */
static sem_t lockBuffer;

/* Utiltity functions provided for your convenience */

/* int2string converts an integer into a string and writes it in the
   passed char array s, which should be of reasonable size (e.g., 20
   characters).  */
char *int2string(int i, char *s)
{
    sprintf(s, "%d", i);
    return s;
}

/* string2int just calls atoi() */
int string2int(const char *s)
{
    return atoi(s);
}

/* isNumeric just calls isdigit() */
int isNumeric(char c)
{
    return isdigit(c);
}

/* End utility functions */


void printErrorAndExit(char *msg)
{
    msg = msg ? msg : "An unspecified error occured!";
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

int timeToFinish()
{
    /* be careful: timeToFinish() also accesses buffer */
    return buffer[0] == '.';
}

/* Looks for an addition symbol "+" surrounded by two numbers, e.g. "5+6"
   and, if found, adds the two numbers and replaces the addition subexpression
   with the result ("(5+6)*8" becomes "(11)*8")--remember, you don't have
   to worry about associativity! */
void *adder(void *arg)
{
    int bufferlen;
    int value1, value2;		// Variables that will store the input numbers that the user will give
    int startOffset, remainderOffset;
    int i;
    int addResult;		// Addition of variables
    char storeString[50];	// Amount of accepted characters for an input/output string

    while (1)
    {

	/* Step 3: add mutual exclusion */
	checkAddition = 1;
	startOffset = remainderOffset = -1;
	value1 = value2 = -1;
	sem_wait(&lockBuffer);

	if (timeToFinish())
	{
		sem_post(&lockBuffer);
		return NULL;
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = (int) strlen(buffer);

	/* Step 2: implement adder */
	for (i = 0; i < bufferlen; i++)
	{
		b_len = bufferlen;

		// Separates each operation with a ';' so the buffer can look at the
		// first expression and work with it
		if (buffer[i] == ';')
		{
			break;
		}

		// Will check the starting digit
		if (isdigit(buffer[i]))
		{
			// Checks the corresponding operators for the operation
			if (buffer[i] == '+' && buffer[i + 1] == '(')
			{
				i = i + 2;
			}

			startOffset = i;

			// Analyze and check the first operand
			value1 = string2int(buffer + i);

			// Increments the index that came from the first operand
			while (isdigit(buffer[i]))
			{
				i++;
			}

			// Checks if the expression that is being analyzed is a "+"
			if (buffer[i] != '+' || !isdigit(buffer[i + 1]))
			{
				// If is not a "+" expression, it will go to the next one
				continue;
			}

			// Analyze and check the second operand
			value2 = string2int(buffer + i + 1);

			// Performing the addition and storing the result in a variable
			addResult = value1 + value2;

			// Increments the index that came from the second operand
			do
			{
				i++;
			} while (isdigit(buffer[i]));

			remainderOffset = i;

			// Transforms the result of the operation into a string
			int2string(addResult, storeString);

			// Re-arrange the buffer
			strcpy(buffer + startOffset, storeString);
			strcpy((buffer + startOffset + strlen(storeString)), (buffer + remainderOffset));

			bufferlen = strlen(buffer);
			i = startOffset + (strlen(storeString)) - 1;
		}

		// Increment the number of operations
		num_ops++;
	}

	// something missing?

	/* Step 6: check progress */
	if (strlen(storeString) == 0 && bufferlen > 0)
	{
		checkAddition = 0;
	}

	/* Step 3: free the lock  */
	sem_post(&lockBuffer);

	/* Step 5: let others play */
	sched_yield();
    }
}

/* Looks for a multiplication symbol "*" surrounded by two numbers, e.g.
   "5*6" and, if found, multiplies the two numbers and replaces the
   mulitplication subexpression with the result ("1+(5*6)+8" becomes
   "1+(30)+8"). */
void *multiplier(void *arg)
{
    int bufferlen;
    int value1, value2;		// Variables that will store the input numbers that the user will give
    int startOffset, remainderOffset;
    int i;
    int multiplyResult;		// Multiplication of variables
    char storeString[50];	// Amount of accepted characters for an input/output string

    while (1)
    {

	/* Step 3: add mutual exclusion */
	checkMultiplication = 1;
	startOffset = remainderOffset = -1;
	value1 = value2 = -1;
	sem_wait(&lockBuffer);

	if (timeToFinish())
	{
		sem_post(&lockBuffer);
		return NULL;
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = (int) strlen(buffer);

	/* Step 2: implement multiplier */
	for (i = 0; i < bufferlen; i++)
	{
		b_len = bufferlen;

		// Separates each operation with a ';' so the buffer can look at the
		// first expression and work with it
		if (buffer[i] == ';')
		{
			break;
		}

		// Will check the starting digit
		if (isdigit(buffer[i]))
		{
			// Checks the corresponding operators for the operation
			if (buffer[i] == '*' && buffer[i + 1] == '(')
			{
				i = i + 2;
			}

			startOffset = i;

			// Analyze and check the first operand
			value1 = atoi(buffer + i);

			// Increment the index that came from the first operand
			while (isdigit(buffer[i]))
			{
				i++;
			}

			// Checks if the expression that is being analyzed is a "*"
			if (buffer[i] != '*' || !isdigit(buffer[i + 1]))
			{
				//If is not a "*" expression, it will go to the next one
				continue;
			}

			// Analyze and check the second operand
			value2 = atoi(buffer + i + 1);

			// Performing the multiplication and storing the result in a variable
			multiplyResult = value1 * value2;

			// Increments the index that came from the second operand
			do
			{
				i++;
			} while (isdigit(buffer[i]));

			remainderOffset = i;

			// Transforms the result of the operand into a string
			sprintf(storeString, "%d", multiplyResult);

			// Re-arrange the buffer
			strcpy(buffer + startOffset, storeString);
			strcpy((buffer + startOffset + strlen(storeString)), (buffer + remainderOffset));

			bufferlen = (int) strlen(buffer);
			i = startOffset + ((int) strlen(storeString)) - 1;
		}

		// Increments the number of operations
		num_ops++;
	}

	// something missing?
	/* Step 6: check progress */
	if (strlen(storeString) == 0 && bufferlen > 0)
	{
		checkMultiplication = 0;
	}

	/* Step 3: free the lock */
	sem_post(&lockBuffer);

	/* Step 5: let others play */
	sched_yield();
    }
}


/* Looks for a number immediately surrounded by parentheses [e.g.
   "(56)"] in the buffer and, if found, removes the parentheses leaving
   only the surrounded number. */
void *degrouper(void *arg)
{
    int bufferlen;
    int i;
    int startOffset = 0;

    while (1)
    {

	/* Step 3: add mutual exclusion */
	checkDegrouper = 1;
	sem_wait(&lockBuffer);

	if (timeToFinish())
	{
		sem_post(&lockBuffer);
		return NULL;
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = (int) strlen(buffer);
	int nakedNumber = 1;

	/* Step 2: implement degrouper */
	for (i = 0; i < bufferlen; i++)
	{
		b_len = bufferlen;

		// Checks if there's no naked number on the operation
		if (nakedNumber == 0)
		{
			//If there aren't any, will stop the operation
			break;
		}

		// Separates each operation with a ';' so the buffer can look
		// at the expression and work with it
		if (buffer[i] == ';')
		{
			break;
		}

		int x = i;

		if (bufferlen > 0)
		{
			while (x < bufferlen)
			{
				if (buffer[x] == '(')
				{
					i = x;
				}

				x++;
			}
		}

		// Checks for the parenthesis in the operation and if there's any naked number inside
		if (buffer[i] == '(' && isdigit(buffer[i + 1]))
		{
			startOffset = i;

			// Increments the index that came from all the digits
			while (buffer[i] != ')')
			{
				i++;

				if (buffer[i] == '+' || buffer[i] == '*')
				{
					nakedNumber = 0;
					break;
				}
			}

			if (nakedNumber == 0)
			{
				continue;
			}

			// Delete the second parenthesis ')' by moving the tail end of the expression
			strcpy((buffer + i), (buffer + i + 1));

			// Delete the first parenthesis '(' by moving the very first part of the expression
			strcpy((buffer + startOffset), (buffer + startOffset + 1));

			bufferlen -= 2;
			i = startOffset;
		}

		// Increments the number of operations
		num_ops++;
	}

	// something missing?
	/* Step 6: check progress */
	if (b_len == bufferlen && bufferlen > 0)
	{
		checkDegrouper = 0;
	}

	/* Step 3: free the lock */
	sem_post(&lockBuffer);

	/* Step 5: let others play */
	sched_yield();
    }
}


/* sentinel waits for a number followed by a ; (e.g. "453;") to appear
   at the beginning of the buffer, indicating that the current
   expression has been fully reduced by the other threads and can now be
   output.  It then "dequeues" that expression (and trailing ;) so work can
   proceed on the next (if available). */
void *sentinel(void *arg)
{
    char numberBuffer[20];
    int bufferlen;
    int i;

    while (1)
    {

	/* Step 3: add mutual exclusion */
	sem_wait(&lockBuffer);

	if (timeToFinish())
	{
		sem_post(&lockBuffer);
		return NULL;
	}

	// Check the progress of each operation
	if (checkAddition == 0 && checkMultiplication == 0 && checkDegrouper == 0)
	{
		printf("No progress can be made\n");
		exit(EXIT_FAILURE);
	}

	/* storing this prevents having to recalculate it in the loop */
	bufferlen = strlen(buffer);

	for (i = 0; i < bufferlen; i++)
	{
		if (buffer[i] == ';')
		{
			if (i == 0)
			{
		    		printErrorAndExit("Sentinel found empty expression!");
			}
			else
			{
		    		/* null terminate the string */
		    		numberBuffer[i] = '\0';

				/* print out the number we've found */
		    		fprintf(stdout, "%s\n", numberBuffer);

				/* shift the remainder of the string to the left */
		    		strcpy(buffer, &buffer[i + 1]);
		    		break;
			}
	    	}
		else if (!isNumeric(buffer[i]))
		{
			break;
	    	}
		else
		{
			numberBuffer[i] = buffer[i];
	    	}
	}

	sem_post(&lockBuffer);

	// something missing?
	/* Step 6: check for progress */
	/*if (checkAddition == 0 && checkMultiplication == 0 && checkDegrouper == 0)
	{
		printf("There's no further progress that can be done\n");
		exit(EXIT_FAILURE);
	}*/

	/* Step 5: let others play, too */
	sched_yield();
    }
}

/* reader reads in lines of input from stdin and writes them to the
   buffer */
void *reader(void *arg)
{
    while (1)
    {
	char tBuffer[100];
	int currentlen;
	int newlen;
	int free;

	fgets(tBuffer, sizeof(tBuffer), stdin);

	/* Sychronization bugs in remainder of function need to be fixed */
	newlen = strlen(tBuffer);
	currentlen = strlen(buffer);

	/* if tBuffer comes back with a newline from fgets, remove it */
	if (tBuffer[newlen - 1] == '\n')
	{
		/* shift null terminator left */
		tBuffer[newlen - 1] = tBuffer[newlen];
		newlen--;
	}

	/* -1 for null terminator, -1 for ; separator */
	free = sizeof(buffer) - currentlen - 2;

	while (free < newlen)
	{
		sem_wait(&lockBuffer);

		currentlen = strlen(buffer);
		free = sizeof(buffer) - currentlen - 2;

		sem_post(&lockBuffer);
		sched_yield();
	}

	/* Step 3: add mutual exclusion */
	sem_wait(&lockBuffer);

	/* we can add another expression now */
	strcat(buffer, tBuffer);
	strcat(buffer, ";");

	/* Step 6: reset flag variables indicating progress */
	sem_post(&lockBuffer);
	sched_yield();

	/* Stop when user enters '.' */
	if (tBuffer[0] == '.')
	{
		return NULL;
	}
    }
}


/* Where it all begins */
int smp3_main(int argc, char **argv)
{
    void *arg = 0;		/* dummy value */

    /* Step 7: use a semaphore */
    if (sem_init(&lockBuffer, 0, 1) != 0)
    {
	printErrorAndExit("Failed to create mutex");
    }

    /* let's create our threads */
       if (pthread_create(&degrouperThread, NULL, degrouper, arg)
	|| pthread_create(&adderThread, NULL, adder, arg)
	|| pthread_create(&multiplierThread, NULL, multiplier, arg)
	|| pthread_create(&sentinelThread, NULL, sentinel, arg)
	|| pthread_create(&readerThread, NULL, reader, arg)) {
	printErrorAndExit("Failed trying to create threads");
    }

    /* you need to join one of these threads... but which one? */
    pthread_join(sentinelThread, NULL);
    pthread_detach(multiplierThread);
    pthread_detach(adderThread);
    pthread_detach(degrouperThread);
    pthread_detach(sentinelThread);
    pthread_detach(readerThread);

    /* Step 1: we have to join on the sentinel thread. */

    /* everything is finished, print out the number of operations performed */
    fprintf(stdout, "Performed a total of %d operations\n", num_ops);

    // TODO destroy semaphores and mutex
    sem_destroy(&lockBuffer);

    return EXIT_SUCCESS;
}
