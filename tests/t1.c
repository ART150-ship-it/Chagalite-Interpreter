// ***************************************************
// * CS460: Programming Assignment 5: Test Program 1 *
// ***************************************************

function int print_something_else (int a)
{
  printf("Something Else\n");
  return a;
}

procedure print_something (void)
{
  printf("Something\n");
}

procedure main (void)
{
  printf("Hello, World!\n");
  int b;
  b = 3;
  if (b == 4) {
    b = print_something_else(b);
  }
  
}
