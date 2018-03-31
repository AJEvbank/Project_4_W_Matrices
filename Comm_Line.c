#ifndef COMM_LINE

#define COMM_LINE

#include "main.h"

void CommLineArgs(int argc,
                  char ** argv,
                  int *seed,
                  int *max_num,
                  int *n,
                  int *connectivity,
                  int *part,
                  int *full,
                  int *print
                 )
{
  *seed = SEED; *max_num = MAX_NUM; *n = N; *connectivity = CONNECTIVITY, *part = PART, *full = FULL, *print = PRINT;
  static char * SHORT = "";
	static struct option OPTS[] =
																{
                                  {"seed",required_argument,0,'s'},
																	{"max_num",required_argument,0,'m'},
                                  {"n",required_argument,0,'n'},
                                  {"connectivity",required_argument,0,'c'},
                                  {"part",required_argument,0,'t'},
                                  {"full",no_argument,0,'f'},
                                  {"print",no_argument,0,'p'},
																	{0,0,0,0}
																};
	int ch;
	int opt_index = 0;
	while(1)
	{
		ch = getopt_long_only(argc,argv,SHORT,OPTS,&opt_index);

    if (ch == -1)
		{
			break;
		}

    switch(ch)
    {
      case 's':
                if(isNumber(optarg))
                {
                  *seed = atoi(optarg);
                }
                else
                {
                  *seed = SEED;
                }
                break;
      case 'm':
                if(isNumber(optarg))
                {
                  *max_num = atoi(optarg);
                }
                else
                {
                  *max_num = MAX_NUM;
                }
                break;
      case 'n':
                if(isNumber(optarg))
                {
                  *n = atoi(optarg);
                }
                else
                {
                  *n = N;
                }
                break;
      case 'c':
                if(isNumber(optarg))
                {
                  *connectivity = atoi(optarg);
                }
                else
                {
                  *connectivity = CONNECTIVITY;
                }
                break;
      case 't':
                if(isNumber(optarg))
                {
                  *part = atoi(optarg);
                }
                else
                {
                  *part = PART;
                }
                break;
      case 'f':
                *full = 1;
      case 'p':
                *print = 1;
                break;
      default:
                printf("default case\n");
                break;
    }
  }

}

int isNumber(const char * str)
{
	int i;
	int len = strlen(str);
	char ch;
	enum isNumStates state = INITIAL;
	if (len > 0)
	{
		for (i = 0; i < len; i++)
		{
			ch = str[i];
			switch (state)
			{
			case INITIAL:
			{
				if (ch == '-' || ch == '+')
				{
					state = PLUSORMINUS;
				}
				else if (ch == '0')
				{
					state = ZERO;
				}
				else if (isdigit(ch))
				{
					state = NUMBER;
				}
				else
				{
					state = ERROR;
				}
				break;
			}

			case PLUSORMINUS:
			{
				if (ch == '0')
				{
					state = ZERO;
				}
				else if (isdigit(ch))
				{
					state = NUMBER;
				}
				else
				{
					state = ERROR;
				}
				break;
			}

			case ZERO:
			{
				if (ch == '.')
				{
					state = DECIMAL;
				}
				else
				{
					state = ERROR;
				}
				break;
			}

			case NUMBER:
			{
				if (isdigit(ch))
				{
					state = NUMBER;
				}
				else if (ch == '.')
				{
					state = DECIMAL;
				}
				else
				{
					state = ERROR;
				}
				break;
			}

			case DECIMAL:
			{
				if (isdigit(ch))
				{
					state = DECIMAL;
				}
				else
				{
					state = ERROR;
				}
				break;
			}

			case ERROR:
			{
				return 0;
				break;
			}

			default:
			{
				printf("default in isNumber: %d \n", (int)state);
				return 0;
				break;
			}

			}
		}
		return 1;
	}
	else
	{
		return 0;
	}
}



#endif
